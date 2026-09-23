#ifndef DUALSYNC_DSI_NETWORK_PROBE_H
#define DUALSYNC_DSI_NETWORK_PROBE_H

#include <stdbool.h>
#include <stddef.h>

size_t dualsync_heap_headroom(void);
bool dualsync_run_https_probe(const char *url);

#endif
