function(dualsync_enable_sanitizers target)
    if(NOT DUALSYNC_ENABLE_SANITIZERS)
        return()
    endif()

    if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(
            "${target}"
            PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
                -fno-pie
                -fno-sanitize-recover=all
        )
        target_link_options(
            "${target}"
            PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
                -no-pie
                -fno-sanitize-recover=all
        )
    else()
        message(FATAL_ERROR "Sanitizers are unsupported by ${CMAKE_C_COMPILER_ID}")
    endif()
endfunction()
