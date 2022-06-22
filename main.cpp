#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>

#include <Eigen/Dense>
#include <boost/lexical_cast.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>

template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
  std::string_view comma = "";
  os << '[';
  for (const auto &z : v) {
    os << comma << z;
    comma = ", ";
  }
  os << ']';
  return os;
}
#define PRINT(x)                                                               \
  do {                                                                         \
    std::cout << #x " = " << (x) << std::endl;                                 \
  } while (false)

std::vector<int> divide_size(const int N, const int m) {
  if (m == N) {
    std::vector<int> ret;
    ret.resize(m, 1);
    return ret;
  } else if (m < N) {
    const int q = N / m;
    const int r = N % m;
    std::vector<int> ret;
    ret.resize(m, q);
    std::fill(ret.begin(), ret.begin() + r, q + 1);
    return ret;
  } else {
    std::vector<int> ret;
    ret.resize(m, 0);
    std::fill(ret.begin(), ret.begin() + N, 1);
    return ret;
  }
}

using Matrix = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic,
                             Eigen::StorageOptions::RowMajor>;

void set_random(Matrix &m) {
  thread_local std::random_device rd;
  thread_local std::mt19937_64 gen{rd()};
  const auto dist = [](auto a, auto b) {
    return std::generate_canonical<Matrix::Scalar,
                                   std::numeric_limits<Matrix::Scalar>::digits>(
               gen) *
               (b - a) +
           a;
  };
  for (auto &x : m.reshaped()) {
    x = dist(-1, 1);
  }
}

int main(int argc, char *argv[]) {
  boost::mpi::environment env{argc, argv};
  boost::mpi::communicator world;

  int N = 100;
  if (argc >= 2) {
    N = boost::lexical_cast<int>(argv[1]);
  }
  if (N <= 0) {
    throw std::runtime_error("Input size must be positive integer!");
  }

  Matrix A;
  Matrix B(N, N);
  Matrix R(N, N);
  std::chrono::steady_clock::time_point t1;
  if (world.rank() == 0) {
    A.resize(N, N);
    set_random(A);
    set_random(B);
    PRINT(world.size());
    t1 = std::chrono::steady_clock::now();
  }
  // array describing how many elements to send to each process
  std::vector<int> sendcounts = divide_size(N, world.size());
  std::for_each(sendcounts.begin(), sendcounts.end(), [N](int &x) { x *= N; });
  // array describing the displacements where each segment begins
  std::vector<int> displs;
  displs.reserve(world.size());
  displs.push_back(0);
  std::partial_sum(sendcounts.begin(), sendcounts.end() - 1,
                   std::back_inserter(displs));
  if (world.rank() == 0) {
    PRINT(sendcounts);
    PRINT(displs);
  }

  // отправляем всем процессам строки A
  Matrix localA(sendcounts[world.rank()] / N, N);
  if (world.rank() == 0)
    boost::mpi::scatterv(world, A.data(), sendcounts, localA.data(), 0);
  else
    boost::mpi::scatterv(world, localA.data(), localA.size(), 0);
  /* boost::mpi::scatterv(world, A.data(), sendcounts, localA.data(), 0); */
  if (world.rank() == 0) {
    std::cout << "Process " << world.rank() << " Scatterv matrix A"
              << std::endl;
  }
  std::cout << "Process " << world.rank() << " recieved localA; shape = ("
            << localA.rows() << ", " << localA.cols() << ")" << std::endl;

  // отправляем всем процессам целую B
  boost::mpi::broadcast(world, B.data(), B.size(), 0);
  if (world.rank() == 0) {
    std::cout << "Process " << world.rank() << " Bcast matrix B" << std::endl;
  }
  std::cout << "Process " << world.rank() << " recieved matrix B" << std::endl;

  std::cout << "Process " << world.rank() << " calculating localR..."
            << std::endl;
  const Matrix localR = localA * B;

  // сохраняем результат от всех процессов
  boost::mpi::gatherv(world, localR.data(), localR.size(), R.data(), sendcounts,
                      displs, 0);

  if (world.rank() == 0) {
    const auto t2 = std::chrono::steady_clock::now();
    std::cout << "Elapsed: " << std::chrono::duration<double>(t2 - t1).count()
              << " s" << std::endl;
  }

  return 0;
}
