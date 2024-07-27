add_executable(test_random_generator test_random_generator.cpp)

target_link_libraries(test_random_generator Terra::random Terra::stf)

add_test(NAME test_random_generator
         COMMAND test_random_generator)

# Specify the C++ standard to observe
set_target_properties(test_random_generator
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

target_compile_options(test_random_generator PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
    $<$<CXX_COMPILER_ID:MSVC>: >)
