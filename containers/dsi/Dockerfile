FROM skylyrac/blocksds:slim-latest

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        cmake \
        ninja-build \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && wf-pacman -Syu --noconfirm \
        toolchain-gcc-arm-none-eabi-zlib \
        blocksds-mbedtls \
        blocksds-libcurl \
    && wf-config clean-caches --all

WORKDIR /work
