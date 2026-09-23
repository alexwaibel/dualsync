FROM debian:bookworm-slim

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        clang \
        clang-format \
        cmake \
        cppcheck \
        libc6-dev \
        libclang-rt-dev \
        ninja-build \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work
