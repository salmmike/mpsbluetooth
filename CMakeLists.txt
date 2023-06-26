if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(mpsbluetooth VERSION 0.0.1)

cmake_minimum_required(VERSION 3.22)

enable_testing()

add_subdirectory(lib)
add_subdirectory(test)
