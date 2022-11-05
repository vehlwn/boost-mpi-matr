cmake_minimum_required(VERSION 3.5)

project(a LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED COMPONENTS mpi)
find_package(Eigen3 REQUIRED)
find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)

add_executable(
  main-mpi random_matrix.hpp
  main-mpi.cpp
)
target_include_directories(
  main-mpi PUBLIC
  ${Boost_INCLUDE_DIR}
  ${MPI_CXX_INCLUDE_DIRS}
)
target_link_libraries(
  main-mpi PUBLIC
  ${Boost_LIBRARIES}
  ${MPI_CXX_LIBRARIES}
  Eigen3::Eigen
)

add_executable(
  main-openmp random_matrix.hpp
  main-openmp.cpp
)
target_include_directories(
  main-openmp PUBLIC
  ${Boost_INCLUDE_DIR}
  ${MPI_CXX_INCLUDE_DIRS}
)
target_link_libraries(
  main-openmp PUBLIC
  ${MPI_CXX_LIBRARIES}
  Eigen3::Eigen
  OpenMP::OpenMP_CXX
)
