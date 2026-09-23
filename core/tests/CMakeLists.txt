add_executable(
    dualsync_core_tests
    test_version.c
)

target_link_libraries(
    dualsync_core_tests
    PRIVATE
        dualsync_core
)

dualsync_enable_warnings(dualsync_core_tests CONVERSION)
dualsync_enable_sanitizers(dualsync_core_tests)

add_test(
    NAME dualsync_core_tests
    COMMAND dualsync_core_tests
)

set_tests_properties(
    dualsync_core_tests
    PROPERTIES
        ENVIRONMENT
            "ASAN_OPTIONS=detect_leaks=1:detect_stack_use_after_return=1:strict_string_checks=1;UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
        TIMEOUT 30
)
