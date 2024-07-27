cmake_minimum_required(VERSION 3.21)

# Define the Random Number Library project
project(random
        VERSION 1.0.0.0
        DESCRIPTION "Random Number Library"
        LANGUAGES CXX)

# Tests are built by default when this is a top-level project
if(PROJECT_IS_TOP_LEVEL)
    # Option to control whether tests are built
    option(random_BUILD_TESTS "Build Tests for the Random Number Library" ON)
else()
    # Option to control whether tests are built
    option(random_BUILD_TESTS "Build Tests for the Random Number Library" OFF)
endif()

# Option to control ability to install the library
option(random_INSTALL "Install the Random Number Library" ON)

add_subdirectory(dependencies)
add_subdirectory(src)

include(CTest)

if(BUILD_TESTING AND random_BUILD_TESTS)
    add_subdirectory(test)
endif()
