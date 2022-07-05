cmake_minimum_required(VERSION 3.5)

project(a LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED COMPONENTS mpi)
find_package(Eigen3 REQUIRED)
find_package(MPI REQUIRED)
add_executable(
  ${PROJECT_NAME}
  main.cpp
)
target_include_directories(
  ${PROJECT_NAME} PUBLIC
  ${Boost_INCLUDE_DIR}
  ${MPI_CXX_INCLUDE_DIRS}
)
target_link_libraries(
  ${PROJECT_NAME} PUBLIC
  ${Boost_LIBRARIES}
  ${MPI_CXX_LIBRARIES}
  Eigen3::Eigen
)
