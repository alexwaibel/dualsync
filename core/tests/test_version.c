#include "dualsync/version.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    assert(strcmp(dualsync_version(), "0.1.0") == 0);
    return 0;
}
