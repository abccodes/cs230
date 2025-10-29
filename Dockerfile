# syntax=docker/dockerfile:1
FROM ubuntu:24.04

# Basic tools & AFL++
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential clang make git curl python3 python3-pip vim pkg-config \
    afl++ \
 && rm -rf /var/lib/apt/lists/*

# Optional: create a non-root user so files you create map cleanly on host (UID 1000 typical on macOS/Linux)
RUN useradd -ms /bin/bash dev && usermod -aG sudo dev || true
USER dev
WORKDIR /workspace

# Tip: copy only when you want to build artifacts in image; otherwise rely on volume mount at runtime
# COPY . /workspace

