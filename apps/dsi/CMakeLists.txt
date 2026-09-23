set(CURL_DIR "${BLOCKSDSEXT}/libcurl/lib/cmake/CURL")
find_package(CURL CONFIG REQUIRED)

find_library(
    DSWIFI_LIBRARY
    NAMES dswifi9
    HINTS "${BLOCKSDS}/libs/dswifi/lib"
    REQUIRED
    NO_DEFAULT_PATH
)
find_library(
    MBEDCRYPTO_LIBRARY
    NAMES mbedcrypto
    HINTS "${BLOCKSDSEXT}/mbedtls/lib"
    REQUIRED
    NO_DEFAULT_PATH
)
find_library(
    MBEDTLS_LIBRARY
    NAMES mbedtls
    HINTS "${BLOCKSDSEXT}/mbedtls/lib"
    REQUIRED
    NO_DEFAULT_PATH
)
find_library(
    MBEDX509_LIBRARY
    NAMES mbedx509
    HINTS "${BLOCKSDSEXT}/mbedtls/lib"
    REQUIRED
    NO_DEFAULT_PATH
)
find_library(
    ZLIB_LIBRARY
    NAMES z
    HINTS "${WONDERFUL_TOOLCHAIN}/toolchain/gcc-arm-none-eabi/arm-none-eabi/lib"
    REQUIRED
    NO_DEFAULT_PATH
)

add_executable(
    dualsync_dsi
    source/main.c
)

set_target_properties(
    dualsync_dsi
    PROPERTIES
        OUTPUT_NAME "dualsync-dsi"
)

target_link_libraries(
    dualsync_dsi
    PRIVATE
        dualsync_core
        "${DSWIFI_LIBRARY}"
        CURL::libcurl
        "${MBEDTLS_LIBRARY}"
        "${MBEDX509_LIBRARY}"
        "${MBEDCRYPTO_LIBRARY}"
        "${ZLIB_LIBRARY}"
)

dualsync_enable_warnings(dualsync_dsi)

nds_create_rom(
    dualsync_dsi
    NAME "DualSync"
    SUBTITLE "RomM client"
    AUTHOR "DualSync contributors"
    ARM7 "${BLOCKSDS}/sys/arm7/main_core/arm7_dswifi_maxmod.elf"
)

set(DUALSYNC_DSI_DIST_DIR "${PROJECT_SOURCE_DIR}/build/dist/dsi")
set(DUALSYNC_DSI_ROM "${CMAKE_CURRENT_BINARY_DIR}/dualsync-dsi.nds")

add_custom_target(
    dualsync_dsi_dist
    ALL
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${DUALSYNC_DSI_DIST_DIR}"
    COMMAND
        "${CMAKE_COMMAND}" -E copy_if_different
        "${DUALSYNC_DSI_ROM}"
        "${DUALSYNC_DSI_DIST_DIR}/dualsync-dsi.nds"
    DEPENDS dualsync_dsi_nds
    COMMENT "Copying DSi ROM to distribution directory"
    VERBATIM
)
