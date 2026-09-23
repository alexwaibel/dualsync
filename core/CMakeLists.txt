add_library(
    dualsync_core
    STATIC
    source/version.c
)

target_include_directories(
    dualsync_core
    PUBLIC
        include
)

dualsync_enable_warnings(dualsync_core CONVERSION)
dualsync_enable_sanitizers(dualsync_core)

