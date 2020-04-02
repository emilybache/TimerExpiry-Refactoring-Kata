project(tests)

add_executable(${PROJECT_NAME}
        main.cpp timers_test.cpp ../timers.h ../timers.c)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ../third_party ..)

enable_testing()
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME})