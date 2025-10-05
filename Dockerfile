# Base image
FROM nvidia/cuda:12.9.1-cudnn-devel-ubuntu24.04

# Update packages and install essentials
RUN apt-get update && apt-get install -y \
    build-essential \
    ninja-build \
    cmake \
    git \
    pkg-config \
    libgtk-3-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev \
    && rm -rf /var/lib/apt/lists/*

# -----------------------------
# OpenCV 4.12.0
# -----------------------------
WORKDIR /opt/opencv
RUN git clone https://github.com/opencv/opencv.git . && \
    git clone https://github.com/opencv/opencv_contrib.git && \
    git checkout 4.12.0 && \
    cd opencv_contrib && git checkout 4.12.0 && cd .. && \
    mkdir -p build && cd build && \
    cmake -D CMAKE_BUILD_TYPE=RELEASE \
          -D CMAKE_INSTALL_PREFIX=/usr/local \
          -D OPENCV_EXTRA_MODULES_PATH=/opt/opencv/opencv_contrib/modules \
          -D WITH_CUDA=ON \
          -D WITH_CUDNN=ON \
          -D OPENCV_DNN_CUDA=ON \
          -D ENABLE_FAST_MATH=ON \
          -D CUDA_FAST_MATH=ON \
          -D WITH_CUBLAS=ON \
          -D BUILD_EXAMPLES=OFF \
          -D BUILD_TESTS=OFF \
          -D BUILD_DOCS=OFF \
          -D BUILD_PERF_TESTS=OFF \
          -D WITH_TBB=ON .. && \
    make -j$(nproc) && \
    make install && \
    ln -s /usr/local/include/opencv4/opencv2 /usr/local/include/opencv2 && \
    ldconfig

# -----------------------------
# ZeroMQ 4.3.5
# -----------------------------
WORKDIR /opt/libzmq
RUN git clone https://github.com/zeromq/libzmq.git . && \
    git checkout v4.3.5 && \
    mkdir -p build && cd build && \
    cmake -DWITH_PERF_TOOL=OFF \
          -DZMQ_BUILD_TESTS=OFF \
          -DBUILD_TESTS=OFF \
          -DWITH_DOCS=OFF \
          -DENABLE_CPACK=OFF \
          -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# -----------------------------
# CPPZeroMQ 4.11.0
# -----------------------------
WORKDIR /opt/cppzmq
RUN git clone https://github.com/zeromq/cppzmq.git . && \
    git checkout v4.11.0 && \
    mkdir -p build && cd build && \
    cmake -DCPPZMQ_BUILD_TESTS=OFF .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# -----------------------------
# nlohmann/json 3.12.0
# -----------------------------
WORKDIR /opt/json
RUN git clone https://github.com/nlohmann/json.git . && \
    git checkout v3.12.0 && \
    mkdir -p build && cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# -----------------------------
# StreamVision-Server
# -----------------------------
COPY . /workspace/StreamVision-Server

RUN apt-get update && apt-get install -y doxygen && rm -rf /var/lib/apt/lists/*

RUN cd /workspace/StreamVision-Server && \
	mkdir build && cd build && \
	cmake .. -DBUILD_DOCS=ON && \
	make -j$(nproc)

CMD ["bash", "-c", "python3 -m http.server 8000 --directory /workspace/StreamVision-Server/docs/html >/dev/null 2>&1 & exec /bin/bash"]

WORKDIR /workspace/StreamVision-Server/build
