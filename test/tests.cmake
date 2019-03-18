add_test(test_help_message ${PROJECT_SOURCE_DIR}/test/test_help.sh)
add_test(test_password_string ${PROJECT_SOURCE_DIR}/test/test_password_string.sh)
add_test(test_empty_password_string ${PROJECT_SOURCE_DIR}/test/test_empty_password_string.sh)
add_test(test_empty_password_string_fail ${PROJECT_SOURCE_DIR}/test/test_empty_password_string_fail.sh)
add_test(test_password_file ${PROJECT_SOURCE_DIR}/test/test_password_file.sh)
add_test(test_ciphertext_corruption ${PROJECT_SOURCE_DIR}/test/test_ciphertext_corruption.sh)
add_test(test_large_file ${PROJECT_SOURCE_DIR}/test/test_large_file.sh)

set_tests_properties(
    test_help_message
    test_password_string
    test_empty_password_string
    test_empty_password_string_fail
    test_password_file
    test_ciphertext_corruption
    test_large_file
    PROPERTIES
    ENVIRONMENT SFC=${CMAKE_CURRENT_BINARY_DIR}/sfc
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test
)