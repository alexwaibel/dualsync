#include <stdio.h>

#include <nds.h>

#include "dualsync/version.h"

int main(void)
{
    defaultExceptionHandler();
    consoleDemoInit();

    printf("DualSync %s\n\n", dualsync_version());
    printf("Shared core linked successfully.\n");
    printf("Runtime mode: %s\n\n", isDSiMode() ? "DSi" : "DS");
    printf("Press START to exit.\n");

    while (1)
    {
        swiWaitForVBlank();
        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}

