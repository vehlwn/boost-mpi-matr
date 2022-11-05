#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <stdexcept>

#include <Eigen/Dense>
#include <boost/lexical_cast.hpp>

#include "random_matrix.hpp"

using Matrix = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic,
                             Eigen::StorageOptions::RowMajor>;

int main(int argc, char *argv[]) {
  int world = 1;
  int N = 100;
  if (argc >= 3) {
    world = boost::lexical_cast<int>(argv[1]);
    N = boost::lexical_cast<int>(argv[2]);
  }
  if (world <= 0) {
    throw std::runtime_error("world size must be positive integer!");
  }
  if (N <= 0) {
    throw std::runtime_error("Input size must be positive integer!");
  }
  Eigen::setNbThreads(world);
  std::clog << "Eigen::nbThreads = " << Eigen::nbThreads() << std::endl;

  Matrix A(N, N);
  Matrix B(N, N);
  set_random(A);
  set_random(B);

  using Clock = std::chrono::system_clock;
  Clock::time_point t1;
  t1 = Clock::now();

  const Matrix R = A * B;
  if (R.rows() != A.rows() || R.cols() != B.cols()) {
    throw std::runtime_error("Unreachable!");
  }

  const auto t2 = Clock::now();
  const double interval = std::chrono::duration<double>(t2 - t1).count();
  std::clog << "Elapsed: " << interval << " s" << std::endl;
  if (std::ofstream of{"times-openmp.txt", std::ios::app}) {
    of << world << '\t' << N << '\t' << interval << '\n';
  } else {
    std::cerr << "Failed to open output file" << std::endl;
  }
  return 0;
}
