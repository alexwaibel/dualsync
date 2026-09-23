#include "dualsync/version.h"

// cmocka requires these standard library types to be declared first.
// clang-format off
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
// clang-format on

static void version_matches_project_version(void **state)
{
    (void)state;

    assert_string_equal(dualsync_version(), "0.1.0");
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(version_matches_project_version),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
