FROM skylyrac/blocksds:slim-latest@sha256:779f6193d1030c62e60d1f0f39c3e32cf4045c0bd224a45c0ceadf9d9ab01a51

LABEL org.opencontainers.image.description="DualSync development and build environment"
LABEL org.opencontainers.image.source="https://github.com/alexwaibel/dualsync"

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        clang-16=1:16.0.6-23ubuntu4 \
        clang-format-16=1:16.0.6-23ubuntu4 \
        clang-tidy-16=1:16.0.6-23ubuntu4 \
        clangd-16=1:16.0.6-23ubuntu4 \
        cmake=3.28.3-1build7 \
        cppcheck=2.13.0-2ubuntu3 \
        git=1:2.43.0-1ubuntu7.3 \
        libc6-dev=2.39-0ubuntu8.9 \
        libcmocka-dev=1.1.7-3 \
        libclang-rt-16-dev=1:16.0.6-23ubuntu4 \
        ninja-build=1.11.1-2 \
        openssh-client=1:9.6p1-3ubuntu13.19 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && for attempt in 1 2 3; do \
        wf-pacman -S --noconfirm \
            toolchain-gcc-arm-none-eabi-zlib=1.3.2-1 \
            blocksds-mbedtls=3.6.4-2 \
            blocksds-libcurl=8.16.0-2 \
        && break; \
        if [ "$attempt" -eq 3 ]; then exit 1; fi; \
        sleep $((attempt * 10)); \
    done \
    && wf-config clean-caches --all

WORKDIR /work

USER ubuntu
