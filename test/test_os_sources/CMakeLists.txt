add_executable(test_os_sources test_os_sources.cpp)

target_link_libraries(test_os_sources Terra::random Terra::stf)

add_test(NAME test_os_sources
         COMMAND test_os_sources)

# Specify the C++ standard to observe
set_target_properties(test_os_sources
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

target_compile_options(test_os_sources PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
    $<$<CXX_COMPILER_ID:MSVC>: >)
