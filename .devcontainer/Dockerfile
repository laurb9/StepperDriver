FROM mcr.microsoft.com/devcontainers/base:jammy

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    python3-pip \
    python3-venv \
    udev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN pip3 install -U platformio intelhex
