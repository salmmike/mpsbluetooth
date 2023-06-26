
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(mpsbluetooth_test mpsbluetooth_test.cpp)
target_link_libraries(mpsbluetooth_test
                      PRIVATE
                      GTest::GTest
                      ${PROJECT_NAME}
                      )
add_test(mpsbluetooth_test mpsbluetooth_test)