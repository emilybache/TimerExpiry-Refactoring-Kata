set(TEST_NAME TimerExpiryCatch2ApprovalTests)
add_executable(${TEST_NAME})
target_sources(${TEST_NAME} PRIVATE TimerExpiryCatch2ApprovalTests.cpp)
target_link_libraries(${TEST_NAME} src printers Catch2::Catch2 ApprovalTests::ApprovalTests)
set_property(TARGET ${TEST_NAME} PROPERTY CXX_STANDARD 11)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

# Set compiler option /FC for Visual Studio to to make the __FILE__ macro expand to full path.
# The __FILE__ macro is used by Catch2 to get the path to current test file.
# Links:
# * https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019
# * https://docs.microsoft.com/en-us/cpp/build/reference/fc-full-path-of-source-code-file-in-diagnostics?view=vs-2019
if (MSVC)
    target_compile_options(${TEST_NAME} PRIVATE "/FC")
endif()
