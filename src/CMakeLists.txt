# Create the library
add_library(random STATIC random_generator.cpp)
add_library(Terra::random ALIAS random)

# Specify the internal and public include directories
target_include_directories(random
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
if(WIN32)
    target_link_libraries(random PUBLIC Bcrypt)
endif()

# Specify the C++ standard to observe
set_target_properties(random
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# If requesting clang-tidy, try to look for it
if(random_CLANG_TIDY)
    find_program(CLANG_TIDY_COMMAND NAMES "clang-tidy")
    if(CLANG_TIDY_COMMAND)
        set_target_properties(random PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
    else()
        message(WARNING "Could not find clang-tidy")
    endif()
endif()

# Use the following compile options
target_compile_options(random
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -O2 -Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>: >)

# Install target and associated include files
if(random_INSTALL)
    include(GNUInstallDirs)
    install(TARGETS random EXPORT randomTargets ARCHIVE)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ TYPE INCLUDE)
    install(EXPORT randomTargets
            FILE randomConfig.cmake
            NAMESPACE Terra::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/random)
endif()
