FROM debian:bookworm-slim@sha256:3783cc01769c7b2b1b83a5c5ad96c815348e28ed7da68e2e3687004faa906251

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        clang-16=1:16.0.6-15~deb12u1 \
        clang-format-16=1:16.0.6-15~deb12u1 \
        clang-tidy-16=1:16.0.6-15~deb12u1 \
        cmake=3.25.1-1 \
        cppcheck=2.10-2 \
        libc6-dev=2.36-9+deb12u14 \
        libcmocka-dev=1.1.5-2.1 \
        libclang-rt-16-dev=1:16.0.6-15~deb12u1 \
        ninja-build=1.11.1-2~deb12u1 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work
