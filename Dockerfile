FROM ubuntu

COPY . .

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update \
  && apt install -y \
  ninja-build \
  build-essential \
  cmake \
  && rm -rf /var/lib/apt/lists/*

RUN mkdir build \
  && cd build \
  && mkdir artifacts \
  && cmake .. -G "Ninja" -DCMAKE_INSTALL_PREFIX=artifacts \
  && ninja -j$(nproc) \
  && ninja install
