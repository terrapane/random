add_executable(test_random_generator test_random_generator.cpp)

target_link_libraries(test_random_generator Terra::random Terra::stf)

add_test(NAME test_random_generator
         COMMAND test_random_generator)

# If requesting clang-tidy, try to look for it
if(random_CLANG_TIDY)
    find_program(CLANG_TIDY_COMMAND NAMES "clang-tidy")
    if(CLANG_TIDY_COMMAND)
        set_target_properties(test_random_generator PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
    else()
        message(WARNING "Could not find clang-tidy")
    endif()
endif()

# Specify the C++ standard to observe
set_target_properties(test_random_generator
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

target_compile_options(test_random_generator
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wpedantic -Wextra -Wall -Werror -Wconversion -Wshadow>
        $<$<CXX_COMPILER_ID:GNU>:-Wpedantic -Wextra -Wall -Werror -Wconversion -Wshadow=local>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>)
