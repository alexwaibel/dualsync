FROM debian:bookworm-slim

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        clang-16 \
        clang-format-16 \
        cmake \
        cppcheck \
        libc6-dev \
        libclang-rt-16-dev \
        ninja-build \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work
