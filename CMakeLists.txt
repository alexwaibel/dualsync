cmake_minimum_required(VERSION 3.20)

project(DualSync VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NINTENDO_DS)
    set(CMAKE_C_EXTENSIONS ON)
else()
    set(CMAKE_C_EXTENSIONS OFF)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(DualSyncSanitizers)
include(DualSyncWarnings)

option(DUALSYNC_ENABLE_SANITIZERS "Enable host address and undefined behavior sanitizers" OFF)

add_subdirectory(core)

if(NINTENDO_DS)
    add_subdirectory(apps/dsi)
else()
    include(CTest)
    add_subdirectory(core/tests)

    find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format-16 clang-format REQUIRED)
    find_program(CPPCHECK_EXECUTABLE NAMES cppcheck REQUIRED)

    set(DUALSYNC_C_SOURCES
        apps/dsi/source/main.c
        core/include/dualsync/version.h
        core/source/version.c
        core/tests/test_version.c
    )

    add_custom_target(
        format
        COMMAND "${CLANG_FORMAT_EXECUTABLE}" -i ${DUALSYNC_C_SOURCES}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Formatting C sources"
        VERBATIM
    )

    add_custom_target(
        format-check
        COMMAND "${CLANG_FORMAT_EXECUTABLE}" --dry-run --Werror ${DUALSYNC_C_SOURCES}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Checking C formatting"
        VERBATIM
    )

    add_custom_target(
        lint
        COMMAND
            "${CPPCHECK_EXECUTABLE}"
            --enable=warning,style,performance,portability
            --error-exitcode=1
            --inline-suppr
            --std=c11
            --suppress=missingIncludeSystem
            -Icore/include
            apps/dsi/source
            core/source
            core/tests
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Running static analysis"
        VERBATIM
    )
endif()
