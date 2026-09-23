FROM debian:bookworm-slim

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        clang \
        clang-format \
        cppcheck \
        libc6-dev \
        libclang-rt-dev \
        make \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work
