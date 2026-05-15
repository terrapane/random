# Change Log

v1.0.6

- Addressed recommendations from clang-tidy
- Removed pointer arithmetic
- Updated dependencies

v1.0.5

- CMake changes
- Updated dependencies

v1.0.4

- Removed some code that was redundant to reduce size and risk of maintenance
  mistakes
- Explicitly check the entropy value of the random\_device to ensure there
  is entropy before choosing to use it
- Employ a Chi-Squared test to verify uniformity of random numbers in
  the test suite
- Updated dependencies

v1.0.3

- Changed `ifdef` statements to allow building on more UNIX-like systems
- Addressed clang-tidy complaints

v1.0.2

- Updated to use v1.0.2 of the Simple Test Framework
- Revised build options for Windows to enable additional warnings
- Enable warnings to be treated as errors during builds
- Addressed a warning produced with the revised warning level
- Added explicit support for FreeBSD

v1.0.1

- Updated to use v1.0.1 of the Simple Test Framework

v1.0.0

- Initial Release
