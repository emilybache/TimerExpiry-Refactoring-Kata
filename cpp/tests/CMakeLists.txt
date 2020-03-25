project(tests)

add_executable(${PROJECT_NAME}
        main.cpp alarm_clock_test.cpp ../alarm_clock.h ../alarm_clock.c)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ../third_party ..)

enable_testing()
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME})