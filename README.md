# StreamVision-Server

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![OpenCV](https://img.shields.io/badge/OpenCV-4.12.0-5C3EE8?style=for-the-badge&logo=opencv&logoColor=white)
![CUDA](https://img.shields.io/badge/CUDA-12.9.1-76B900?style=for-the-badge&logo=nvidia&logoColor=white)
![ZeroMQ](https://img.shields.io/badge/ZeroMQ-4.3.5-DF0000?style=for-the-badge&logoColor=white)
![cppzmq](https://img.shields.io/badge/cppzmq-4.11.0-DF0000?style=for-the-badge&logoColor=white)
![nlohmann-json](https://img.shields.io/badge/nlohmann--json-3.12.0-00599C?style=for-the-badge&logoColor=white)
![TinyXML2](https://img.shields.io/badge/TinyXML2-11.0.0-00599C?style=for-the-badge&logoColor=white)
![YOLOv11](https://img.shields.io/badge/YOLO-v11/v12-00FFFF?style=for-the-badge)
![CMake](https://img.shields.io/badge/CMake-3.10+-064F8C?style=for-the-badge&logo=cmake&logoColor=white)
![Docker](https://img.shields.io/badge/Docker-Ready-2496ED?style=for-the-badge&logo=docker&logoColor=white)
![Doxygen](https://img.shields.io/badge/Docs-Doxygen-2C4AA8?style=for-the-badge&logo=doxygen&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-Ubuntu_24.04-E95420?style=for-the-badge&logo=ubuntu&logoColor=white)

---

## Table of Contents

- [Project Overview](#overview)
- [Architecture](#architecture)
- [Dependencies](#dependencies)
- [Installation](#installation)
  - [Docker (Recommended)](#docker-recommended)
  - [Manual Build](#manual-build)
- [Usage](#usage)
- [Configuration](#configuration)

---

## Overview

StreamVision is a distributed object detection system consisting of two components:

- **StreamVision-Server** (this repository): C++ application that receives encoded frames, decodes them, runs YOLOv11/v12 inference with GPU acceleration, and returns detection results to client.
- **[StreamVision-Client](https://github.com/alperak/StreamVision-Client)** (separate repository): C++ application that captures camera frames, sends them to a detection server, receives results, draws bounding boxes on frames, and displays live video stream in web browsers.

---

## Architecture

> **Note:** Application settings (Server Bind Address, Port, Model Configuration, Input Dimensions, Detection Thresholds, and Inference Target) are managed by `ConfigXML` singleton class, loaded once at startup.

```
┌─────────────────┐
│ FrameHandler    │ → Receives encoded frames/sends results via ZeroMQ (REP socket)
└────────┬────────┘
         ↓
┌─────────────────┐
│ FrameDecoder    │ → Decodes JPEG frames
└────────┬────────┘
         ↓
┌─────────────────┐
│ InferenceEngine │ → YOLOv11/v12 object detection (GPU/CPU)
└────────┬────────┘
         ↓
┌─────────────────┐
│ ResultSerializer│ → Converts detections to JSON
└────────┬────────┘
         ↓
┌─────────────────┐
│ FrameHandler    │ → Sends JSON results back to client
└─────────────────┘
```

---

## Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| [OpenCV](https://opencv.org/) | 4.12.0 | Image processing and DNN inference (with CUDA support) |
| [CUDA](https://developer.nvidia.com/cuda-toolkit) | 12.9.1 | GPU acceleration for neural network inference |
| [cuDNN](https://developer.nvidia.com/cudnn) | Latest | CUDA Deep Neural Network library |
| [ZeroMQ (libzmq)](https://zeromq.org/) | 4.3.5 | High-performance asynchronous messaging library |
| [cppzmq](https://github.com/zeromq/cppzmq) | 4.11.0 | Header-only C++ bindings for ZeroMQ |
| [nlohmann/json](https://github.com/nlohmann/json) | 3.12.0 | Modern C++ JSON serialization |
| [TinyXML2](https://github.com/leethomason/tinyxml2) | 11.0.0 | XML configuration parsing |
| [Doxygen](https://www.doxygen.nl/) | Latest | API documentation generator (optional) |

---

## Installation

### Docker (Recommended)

The easiest way to build and run StreamVision-Server with GPU support:

```bash
# Clone the repository
git clone https://github.com/alperak/StreamVision-Server.git
cd StreamVision-Server

# Build Docker image
sudo docker build -t streamvision-server .

# Run container with GPU support
sudo docker run -it --rm \
  --gpus all \
  --network host \
  streamvision-server
```

### Manual Build

For manual installation, follow the dependency build steps in the [`Dockerfile`](Dockerfile).

---

## Usage

### Quick Start

1. **Configure the application** by editing [config/config.xml](config/config.xml):

```xml
<Config>
    <Model>
        <Path>../model/yolo11s.onnx</Path>              <!-- ONNX model file path -->
        <InputWidth>640</InputWidth>                    <!-- Model input width -->
        <InputHeight>640</InputHeight>                  <!-- Model input height -->
        <LabelsPath>../model/labels.txt</LabelsPath>    <!-- Class labels file path -->
        <ScoreThreshold>0.45</ScoreThreshold>           <!-- Detection confidence threshold [0.0 - 1.0] -->
        <NMSThreshold>0.50</NMSThreshold>               <!-- NMS threshold [0.0 - 1.0] -->
        <!-- InferenceTarget: CPU or GPU -->
        <InferenceTarget>GPU</InferenceTarget>          <!-- Inference execution target -->
    </Model>
    <Server>
        <IP>0.0.0.0</IP>         <!-- Server bind IP address -->
        <Port>5555</Port>        <!-- Server bind port number -->
    </Server>
</Config>
```

> **Note:** 
> - If you don't configure, It will start with the current default values in the config.
> - If changing server IP/port, update StreamVision-Client config accordingly.
> - Adjust ScoreThreshold and NMSThreshold based on your detection accuracy requirements.
> - Set InferenceTarget to CPU or GPU depending on your hardware availability.

2. **Run the server**:

```bash
./streamvision-server
```

3. **Start the client** (see [StreamVision-Client](https://github.com/alperak/StreamVision-Client) repository)

4. **View the doxygen documentation** in your browser (Docker only):

```
http://localhost:8000
```

---

## Configuration

YOLOv11s already converted and available in the project but if you want to use another models, you can convert by following the steps:

#### Convert PyTorch Models to ONNX

`YOLOv11s` already converted and available in the project but if you want to use another models, you can convert by following the steps:

```bash
# Install Ultralytics package
pip install ultralytics

# For example, we want to try `YOLOv11x` so need to download the `YOLOv11x.pt` into our model directory
# and convert model to `ONNX` format like this:

python convert_pt_to_onnx_model.py yolo11x.pt 
```

You should see this output when you try YOLOv11x:

```
python convert_pt_to_onnx_model.py yolo11x.pt
Ultralytics 8.3.202 🚀 Python-3.12.3 torch-2.8.0+cu128 CPU (Intel Core i7-8700K 3.70GHz)
YOLO11x summary (fused): 190 layers, 56,919,424 parameters, 0 gradients, 194.9 GFLOPs

PyTorch: starting from 'yolo11x.pt' with input shape (1, 3, 640, 640) BCHW and output shape(s) (1, 84, 8400) (109.3 MB)

ONNX: starting export with onnx 1.19.0 opset 19...
ONNX: slimming with onnxslim 0.1.68...
ONNX: export success ✅ 3.7s, saved as 'yolo11x.onnx' (217.5 MB)

Export complete (5.5s)
Results saved to /home/alper/cpp-projects/YOLOv11-OpenCV-CUDA-Cpp/model
Predict:         yolo predict task=detect model=yolo11x.onnx imgsz=640  
Validate:        yolo val task=detect model=yolo11x.onnx imgsz=640 data=/ultralytics/ultralytics/cfg/datasets/coco.yaml  
Visualize:       https://netron.app
```

---

### Download YOLOv11/12 Models

If you want to try models other than the YOLOv11s in the project, download pretrained YOLOv11/12 models from [Ultralytics](https://docs.ultralytics.com/tasks/detect/#models):

| Model | Size<br>(pixels) | mAP<sup>val</sup><br>50-95 | Speed<br>CPU ONNX<br>(ms) | Speed<br>T4 TensorRT10<br>(ms) | Params<br>(M) | FLOPs<br>(B) |
|---------|:----------------:|:---------------------------:|:--------------------------:|:-------------------------------:|:-------------:|:------------:|
| [YOLOv11n](https://github.com/ultralytics/assets/releases/download/v8.3.0/yolo11n.pt) | 640 | 39.5 | 56.1 ± 0.8 | 1.5 ± 0.0 | 2.6 | 6.5 |
| [YOLOv11s](https://github.com/ultralytics/assets/releases/download/v8.3.0/yolo11s.pt) | 640 | 47.0 | 90.0 ± 1.2 | 2.5 ± 0.0 | 9.4 | 21.5 |
| [YOLOv11m](https://github.com/ultralytics/assets/releases/download/v8.3.0/yolo11m.pt) | 640 | 51.5 | 183.2 ± 2.0 | 4.7 ± 0.1 | 20.1 | 68.0 |
| [YOLOv11l](https://github.com/ultralytics/assets/releases/download/v8.3.0/yolo11l.pt) | 640 | 53.4 | 238.6 ± 1.4 | 6.2 ± 0.1 | 25.3 | 86.9 |
| [YOLOv11x](https://github.com/ultralytics/assets/releases/download/v8.3.0/yolo11x.pt) | 640 | 54.7 | 462.8 ± 6.7 | 11.3 ± 0.2 | 56.9 | 194.9 |


| Model | Size<br>(pixels) | mAP<sup>val</sup><br>50-95 | Speed<br>CPU ONNX<br>(ms) | Speed<br>T4 TensorRT<br>(ms) | Params<br>(M) | FLOPs<br>(B) | Comparison<br>(mAP/Speed) |
|---------|:----------------:|:---------------------------:|:--------------------------:|:-----------------------------:|:-------------:|:------------:|:------------------------:|
| [YOLO12n](https://github.com/ultralytics/assets/releases/download/v8.3.0/yolo12n.pt) | 640 | 40.6 | - | 1.64 | 2.6 | 6.5 | +2.1%/-9%<br>(vs. YOLOv10n) |
| [YOLO12s](https://github.com/ultralytics/assets/releases/download/v8.3.0/yolo12s.pt) | 640 | 48.0 | - | 2.61 | 9.3 | 21.4 | +0.1%/+42%<br>(vs. RT-DETRv2) |
| [YOLO12m](https://github.com/ultralytics/assets/releases/download/v8.3.0/yolo12m.pt) | 640 | 52.5 | - | 4.86 | 20.2 | 67.5 | +1.0%/-3%<br>(vs. YOLOv11m) |
| [YOLO12l](https://github.com/ultralytics/assets/releases/download/v8.3.0/yolo12l.pt) | 640 | 53.7 | - | 6.77 | 26.4 | 88.9 | +0.4%/-8%<br>(vs. YOLOv11l) |
| [YOLO12x](https://github.com/ultralytics/assets/releases/download/v8.3.0/yolo12x.pt) | 640 | 55.2 | - | 11.79 | 59.1 | 199.0 | +0.6%/-4%<br>(vs. YOLOv11x) |
