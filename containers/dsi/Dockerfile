FROM skylyrac/blocksds:slim-latest

RUN wf-pacman -Syu --noconfirm \
        toolchain-gcc-arm-none-eabi-zlib \
        blocksds-mbedtls \
        blocksds-libcurl \
    && wf-config clean-caches --all

WORKDIR /work

