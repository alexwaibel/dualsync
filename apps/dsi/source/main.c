#include "dualsync/version.h"
#include "network_probe.h"

#include <dswifi9.h>
#include <nds.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef DUALSYNC_PROBE_URL
#define DUALSYNC_PROBE_URL ""
#endif

enum
{
    WIFI_TIMEOUT_FRAMES = 30 * 60,
};

static const char *wifi_status_name(int status)
{
    switch (status)
    {
    case ASSOCSTATUS_DISCONNECTED:
        return "Disconnected";
    case ASSOCSTATUS_SEARCHING:
        return "Searching";
    case ASSOCSTATUS_AUTHENTICATING:
        return "Authenticating";
    case ASSOCSTATUS_ASSOCIATING:
        return "Associating";
    case ASSOCSTATUS_ACQUIRINGDHCP:
        return "Acquiring DHCP";
    case ASSOCSTATUS_ASSOCIATED:
        return "Associated";
    case ASSOCSTATUS_CANNOTCONNECT:
        return "Cannot connect";
    default:
        return "Unknown";
    }
}

static bool connect_wifi(void)
{
    printf("Connecting to firmware AP...\n");
    printf("B: cancel\n");

    Wifi_EnableWifi();
    Wifi_AutoConnect();

    int previous_status = -1;

    for (unsigned int frame = 0; frame < WIFI_TIMEOUT_FRAMES; frame++)
    {
        cothread_yield_irq(IRQ_VBLANK);
        scanKeys();

        if (keysDown() & KEY_B)
        {
            Wifi_DisconnectAP();
            printf("Connection cancelled.\n");
            return false;
        }

        const int status = Wifi_AssocStatus();
        if (status != previous_status)
        {
            printf("%s\n", wifi_status_name(status));
            previous_status = status;
        }

        if (status == ASSOCSTATUS_ASSOCIATED)
            return true;

        if (status == ASSOCSTATUS_CANNOTCONNECT)
            return false;
    }

    Wifi_DisconnectAP();
    printf("Connection timed out.\n");
    return false;
}

static bool wait_for_retry(void)
{
    printf("\nA: retry  START: exit\n");

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);
        scanKeys();

        const uint16_t keys = keysDown();
        if (keys & KEY_A)
            return true;

        if (keys & KEY_START)
            return false;
    }
}

int main(void)
{
    defaultExceptionHandler();
    consoleDemoInit();

    printf("DualSync %s\n\n", dualsync_version());
    printf("Phase 0 transport probe\n");
    printf("Runtime: %s\n", isDSiMode() ? "DSi" : "DS");
    printf("Heap headroom: %lu KiB\n\n", (unsigned long)(dualsync_heap_headroom() / 1024));

    if (!isDSiMode())
    {
        printf("DS mode is not supported by\n");
        printf("this probe. Relaunch in DSi mode.\n");
        printf("\nPress START to exit.\n");

        while (1)
        {
            cothread_yield_irq(IRQ_VBLANK);
            scanKeys();

            if (keysDown() & KEY_START)
                return 0;
        }
    }

    printf("Initializing WiFi...\n");
    if (!Wifi_InitDefault(INIT_ONLY | WIFI_ATTEMPT_DSI_MODE))
    {
        printf("WiFi initialization failed.\n");
        printf("\nPress START to exit.\n");

        while (1)
        {
            cothread_yield_irq(IRQ_VBLANK);
            scanKeys();

            if (keysDown() & KEY_START)
                return 1;
        }
    }

    while (1)
    {
        consoleClear();
        printf("DualSync transport probe\n\n");
        printf("Heap: %lu KiB\n\n", (unsigned long)(dualsync_heap_headroom() / 1024));

        if (connect_wifi())
        {
            printf("\nConnected.\n");

            if (DUALSYNC_PROBE_URL[0] == '\0')
            {
                printf("No heartbeat URL configured.\n");
                printf("Set DUALSYNC_PROBE_URL when\n");
                printf("building the ROM.\n");
            }
            else
            {
                printf("URL: %s\n\n", DUALSYNC_PROBE_URL);
                (void)dualsync_run_https_probe(DUALSYNC_PROBE_URL);
            }
        }
        else
        {
            printf("WiFi connection failed.\n");
        }

        if (!wait_for_retry())
            break;

        Wifi_DisconnectAP();
        cothread_yield_irq(IRQ_VBLANK);
    }

    Wifi_DisconnectAP();
    Wifi_DisableWifi();
    cothread_yield_irq(IRQ_VBLANK);
    cothread_yield_irq(IRQ_VBLANK);

    return 0;
}
