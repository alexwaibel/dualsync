FROM skylyrac/blocksds:slim-latest@sha256:779f6193d1030c62e60d1f0f39c3e32cf4045c0bd224a45c0ceadf9d9ab01a51

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        cmake=3.28.3-1build7 \
        ninja-build=1.11.1-2 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && wf-pacman -S --noconfirm \
        toolchain-gcc-arm-none-eabi-zlib=1.3.2-1 \
        blocksds-mbedtls=3.6.4-2 \
        blocksds-libcurl=8.16.0-2 \
    && wf-config clean-caches --all

WORKDIR /work
