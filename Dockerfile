FROM ubuntu

COPY . .

ARG slurm_version=slurm-21-08-3-1

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update \
  && apt install -y \
  ninja-build \
  build-essential \
  cmake \
  git \
  python3 \
  && rm -rf /var/lib/apt/lists/*

RUN git clone -b ${slurm_version} --depth=1 https://github.com/SchedMD/slurm.git \
  && cd slurm \
  && ./configure \
  --disable-gtktest \
  --disable-debug \
  --disable-slurmrestd \
  && make -j$(nproc) \
  && make install

RUN mkdir build \
  && cd build \
  && mkdir artifacts \
  && cmake .. -G "Ninja" -DCMAKE_INSTALL_PREFIX=artifacts \
  && ninja -j$(nproc) \
  && ninja install
