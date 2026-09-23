#include "network_probe.h"

#include <curl/curl.h>
#include <nds.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// https://letsencrypt.org/certificates/
static const char LETS_ENCRYPT_ROOTS[] =
    "-----BEGIN CERTIFICATE-----\n"
    "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\n"
    "-----END CERTIFICATE-----\n"
    "-----BEGIN CERTIFICATE-----\n"
    "MIICGzCCAaGgAwIBAgIQQdKd0XLq7qeAwSxs6S+HUjAKBggqhkjOPQQDAzBPMQsw\n"
    "CQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJuZXQgU2VjdXJpdHkgUmVzZWFyY2gg\n"
    "R3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBYMjAeFw0yMDA5MDQwMDAwMDBaFw00\n"
    "MDA5MTcxNjAwMDBaME8xCzAJBgNVBAYTAlVTMSkwJwYDVQQKEyBJbnRlcm5ldCBT\n"
    "ZWN1cml0eSBSZXNlYXJjaCBHcm91cDEVMBMGA1UEAxMMSVNSRyBSb290IFgyMHYw\n"
    "EAYHKoZIzj0CAQYFK4EEACIDYgAEzZvVn4CDCuwJSvMWSj5cz3es3mcFDR0HttwW\n"
    "+1qLFNvicWDEukWVEYmO6gbf9yoWHKS5xcUy4APgHoIYOIvXRdgKam7mAHf7AlF9\n"
    "ItgKbppbd9/w+kHsOdx1ymgHDB/qo0IwQDAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0T\n"
    "AQH/BAUwAwEB/zAdBgNVHQ4EFgQUfEKWrt5LSDv6kviejM9ti6lyN5UwCgYIKoZI\n"
    "zj0EAwMDaAAwZQIwe3lORlCEwkSHRhtFcP9Ymd70/aTSVaYgLXTWNLxBo1BfASdW\n"
    "tL4ndQavEi51mI38AjEAi/V3bNTIZargCyzuFJ0nN6T5U6VR5CmD1/iQMVtCnwr1\n"
    "/q4AaOeMSQ+2b1tbFfLn\n"
    "-----END CERTIFICATE-----\n";

typedef struct
{
    size_t bytes_received;
    size_t minimum_headroom;
    bool cancelled;
} ProbeContext;

size_t dualsync_heap_headroom(void)
{
    const uintptr_t heap_end = (uintptr_t)getHeapEnd();
    const uintptr_t heap_limit = (uintptr_t)getHeapLimit();

    if (heap_limit < heap_end)
        return 0;

    return (size_t)(heap_limit - heap_end);
}

static void sample_heap(ProbeContext *context)
{
    const size_t headroom = dualsync_heap_headroom();

    if (headroom < context->minimum_headroom)
        context->minimum_headroom = headroom;
}

static size_t discard_response(char *data, size_t size, size_t count, void *user_data)
{
    ProbeContext *context = user_data;

    (void)data;

    if ((size != 0) && (count > (SIZE_MAX / size)))
        return 0;

    const size_t bytes = size * count;

    if (bytes > (SIZE_MAX - context->bytes_received))
        return 0;

    context->bytes_received += bytes;
    sample_heap(context);
    return bytes;
}

static int report_progress(void *user_data, curl_off_t download_total, curl_off_t downloaded,
                           curl_off_t upload_total, curl_off_t uploaded)
{
    ProbeContext *context = user_data;

    (void)download_total;
    (void)downloaded;
    (void)upload_total;
    (void)uploaded;

    sample_heap(context);
    cothread_yield();
    scanKeys();

    if (keysDown() & KEY_B)
    {
        context->cancelled = true;
        return 1;
    }

    return 0;
}

static CURLcode set_options(CURL *curl, const char *url, ProbeContext *context,
                            char *error_buffer)
{
    struct curl_blob certificate = {
        .data = (void *)LETS_ENCRYPT_ROOTS,
        .len = sizeof(LETS_ENCRYPT_ROOTS) - 1,
        .flags = CURL_BLOB_NOCOPY,
    };

#define SET_OPTION(option, value)                                                                  \
    do                                                                                             \
    {                                                                                              \
        const CURLcode option_result = curl_easy_setopt(curl, option, value);                      \
        if (option_result != CURLE_OK)                                                             \
            return option_result;                                                                 \
    } while (0)

    SET_OPTION(CURLOPT_ERRORBUFFER, error_buffer);
    SET_OPTION(CURLOPT_URL, url);
    SET_OPTION(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
    SET_OPTION(CURLOPT_USERAGENT, "DualSync/0.1.0");
    SET_OPTION(CURLOPT_FOLLOWLOCATION, 1L);
    SET_OPTION(CURLOPT_MAXREDIRS, 3L);
    SET_OPTION(CURLOPT_CONNECTTIMEOUT, 15L);
    SET_OPTION(CURLOPT_TIMEOUT, 30L);
    SET_OPTION(CURLOPT_NOSIGNAL, 1L);
    SET_OPTION(CURLOPT_SSL_VERIFYPEER, 1L);
    SET_OPTION(CURLOPT_SSL_VERIFYHOST, 2L);
    SET_OPTION(CURLOPT_CAINFO_BLOB, &certificate);
    SET_OPTION(CURLOPT_WRITEFUNCTION, discard_response);
    SET_OPTION(CURLOPT_WRITEDATA, context);
    SET_OPTION(CURLOPT_NOPROGRESS, 0L);
    SET_OPTION(CURLOPT_XFERINFOFUNCTION, report_progress);
    SET_OPTION(CURLOPT_XFERINFODATA, context);

#undef SET_OPTION

    return CURLE_OK;
}

bool dualsync_run_https_probe(const char *url)
{
    const size_t starting_headroom = dualsync_heap_headroom();
    ProbeContext context = {
        .bytes_received = 0,
        .minimum_headroom = starting_headroom,
        .cancelled = false,
    };
    char error_buffer[CURL_ERROR_SIZE] = { 0 };
    long response_code = 0;
    curl_off_t total_time_us = 0;

    printf("HTTPS heartbeat\n");
    printf("B: cancel\n\n");

    CURLcode result = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (result != CURLE_OK)
    {
        printf("curl init: %s\n", curl_easy_strerror(result));
        return false;
    }

    CURL *curl = curl_easy_init();
    if (curl == NULL)
    {
        printf("curl handle allocation failed\n");
        curl_global_cleanup();
        return false;
    }

    sample_heap(&context);

    result = set_options(curl, url, &context, error_buffer);
    if (result != CURLE_OK)
    {
        printf("curl option setup failed: %s\n", curl_easy_strerror(result));
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return false;
    }

    result = curl_easy_perform(curl);
    sample_heap(&context);

    if (result == CURLE_OK)
    {
        result = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (result == CURLE_OK)
            result = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME_T, &total_time_us);
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    printf("\n");
    if (context.cancelled)
    {
        printf("Request cancelled.\n");
    }
    else if (result == CURLE_PEER_FAILED_VERIFICATION)
    {
        printf("TLS verification failed.\n");
        printf("Check certificate and clock.\n");
    }
    else if (result != CURLE_OK)
    {
        printf("Request failed: %s\n", curl_easy_strerror(result));
        if (error_buffer[0] != '\0')
            printf("%s\n", error_buffer);
    }
    else
    {
        printf("HTTP status: %ld\n", response_code);
        printf("Response bytes: %lu\n", (unsigned long)context.bytes_received);
        printf("Time: %lld ms\n", (long long)(total_time_us / 1000));
    }

    printf("Heap before: %lu KiB\n", (unsigned long)(starting_headroom / 1024));
    printf("Heap minimum: %lu KiB\n", (unsigned long)(context.minimum_headroom / 1024));

    return (result == CURLE_OK) && (response_code == 200);
}
