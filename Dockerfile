
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# Dockerfile for MRNIU/cmake-kernel.

FROM ubuntu:latest

RUN DEBIAN_FRONTEND=noninteractive \
    && apt update \
    && apt install --fix-missing -y \
        zip \
        openssh-server \
        rsync \
        tar \
        git \
        vim \
        doxygen \
        graphviz \
        make \
        cmake \
        clang-format \
        qemu-system \
        build-essential \
        binutils \
        valgrind \
        gdb-multiarch \
        gcc \
        g++ \
        gcc-riscv64-linux-gnu \
        g++-riscv64-linux-gnu \
    && apt clean

# configure SSH for communication with Visual Studio
RUN mkdir -p /var/run/sshd \
    && echo 'PasswordAuthentication yes' >> /etc/ssh/sshd_config \
    && ssh-keygen -A

#没有mount mac下的目录到容器中，因为这样git的性能会非常糟糕。
# 所以不要轻易删除docker container，特别是如果你还有代码没提交时，删除container会导致在container中修改的代码丢失。
#VOLUME /projects

# expose port 22
EXPOSE 22

ENTRYPOINT service ssh restart && bash
