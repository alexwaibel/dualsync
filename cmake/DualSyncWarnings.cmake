function(dualsync_enable_warnings target)
    set(options CONVERSION)
    cmake_parse_arguments(DUALSYNC_WARNINGS "${options}" "" "" ${ARGN})

    if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(
            "${target}"
            PRIVATE
                -Wall
                -Werror
                -Wextra
                -Wpedantic
                -Wshadow
                -Wstrict-prototypes
        )

        if(DUALSYNC_WARNINGS_CONVERSION)
            target_compile_options("${target}" PRIVATE -Wconversion)
        endif()
    endif()
endfunction()

