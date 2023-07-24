
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# Dockerfile for MRNIU/cmake-kernel.

FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update \
    && apt install --fix-missing -y build-essential binutils \
       gdb-multiarch gcc-riscv64-linux-gnu g++-riscv64-linux-gnu \
       make cmake \
       clang-format
