#include <random>

template <class Matrix> void set_random(Matrix &m) {
  thread_local std::random_device rd;
  thread_local std::mt19937_64 gen{rd()};
  const auto dist = [](auto a, auto b) {
    return std::generate_canonical<
               typename Matrix::Scalar,
               std::numeric_limits<typename Matrix::Scalar>::digits>(gen) *
               (b - a) +
           a;
  };
  for (int row = 0; row != m.rows(); row++)
    for (int col = 0; col != m.cols(); col++)
      m(row, col) = dist(-1, 1);
}
