project(
  'boost-mpi-matr',
  'cpp',
  default_options: ['cpp_std=c++17', 'warning_level=3'],
  license: 'MIT',
  meson_version: '>=0.53.2'
)

boost_dep = dependency('boost', modules: ['mpi'], version: '>=1.71.0')
mpi_dep = dependency('mpi', language: 'cpp')
eigen_dep = dependency('eigen3')

executable('a', 'main.cpp', dependencies: [boost_dep, mpi_dep, eigen_dep])
