CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(Wujin)

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -Wall -Wextra -ggdb3")

FIND_PACKAGE(Boost 1.58.0 COMPONENTS system unit_test_framework REQUIRED)

FILE(GLOB SRCFILES src/*.cpp)

IF (Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  ADD_EXECUTABLE(AgeOfWujin ${SRCFILES})
  TARGET_LINK_LIBRARIES(AgeOfWujin ${Boost_LIBRARIES})

  # Add each unit test here
  ADD_EXECUTABLE(UnitTestItems test/TestItems.cpp src/Items.cpp)
  TARGET_LINK_LIBRARIES(UnitTestItems ${Boost_LIBRARIES})
  TARGET_INCLUDE_DIRECTORIES(UnitTestItems PRIVATE "src/")
  ADD_TEST(NAME UnitTestItems COMMAND UnitTestItems)
ELSE()
  MESSAGE("Boost not found: stopping CMake!")
ENDIF()
