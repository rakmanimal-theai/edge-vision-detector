# Real-Time Object Detection Pipeline — C++ · OpenCV · YOLOv5 · Edge Deployment

> A production-minded computer vision system that runs YOLOv5 inference on a live webcam feed, built from scratch in C++ for edge-deployable, real-time performance.

---

## Executive Summary

Built a real-time object detection pipeline in C++ using OpenCV's DNN module and a YOLOv5m ONNX model. The system captures a live webcam feed, runs inference at 15–25 FPS on CPU, applies Non-Maximum Suppression to filter overlapping detections, and overlays labelled bounding boxes on the live stream. Detections are logged to CSV with timestamps for downstream analysis. The application is fully configurable via YAML and containerised with Docker for one-command deployment.

---

## Business Problem

Edge AI systems — security cameras, industrial inspection, autonomous devices — need object detection pipelines that run efficiently on constrained hardware without cloud dependency. Latency, resource usage, and deployment consistency are critical. This project demonstrates a production-minded approach to that problem: a self-contained C++ pipeline that can be configured, deployed, and monitored without recompilation.

---

## Methodology

The pipeline follows a standard computer vision inference pattern:

1. **Capture** — OpenCV `VideoCapture` grabs frames from the camera at native resolution
2. **Preprocess** — frames are normalised and resized to 640×640 for YOLOv5 input
3. **Inference** — YOLOv5m ONNX model runs via OpenCV's DNN module (CPU backend)
4. **Postprocess** — raw detections are filtered by confidence score, then deduplicated using Non-Maximum Suppression (IoU threshold)
5. **Annotate** — surviving detections are drawn onto the frame with class labels and confidence percentages
6. **Log** — each detection is written to a CSV file with timestamp and FPS

YOLOv5 was chosen for its strong balance of speed and accuracy on the 80-class COCO dataset. The ONNX format allows the model to run without a PyTorch dependency at runtime, keeping the deployment footprint minimal.

---

## Technical Skills

| Category | Tools |
|---|---|
| Language | C++17 |
| Computer Vision | OpenCV 4.13, YOLOv5m ONNX |
| Build System | CMake |
| Containerisation | Docker, Docker Compose |
| Config | OpenCV FileStorage (YAML) |
| Logging | CSV via std::ofstream, std::chrono |
| Architecture | OOP — `Stream` and `Detector` classes, separation of concerns |

**Notable C++ patterns used:**
- Initialiser lists for member construction
- Pass-by-reference (`cv::Mat&`) for zero-copy frame processing
- Default constructor arguments for configurable thresholds
- `std::vector<Detection>` return type decoupling inference from rendering

---

## Results

- **15–25 FPS** on CPU (MacBook Air M2) with YOLOv5m
- **80 object classes** detected from the COCO dataset (people, vehicles, animals, household objects)
- **NMS** reduces duplicate bounding boxes per object from ~10–20 candidates to 1
- **CSV log** captures every detection with timestamp, FPS, class, confidence, and bounding box coordinates for downstream analysis

---

## How to Build and Run

### Requirements

- macOS or Linux
- CMake 3.10+
- OpenCV 4.x (`brew install opencv` on Mac)
- YOLOv5m ONNX model ([download here](https://github.com/ultralytics/yolov5/releases))

### Build

```bash
git clone https://github.com/Rakmanimal-theAI/edge-vision-detector
cd edge-vision-detector
mkdir build && cd build
cmake ..
make
./edge-vision-detector
```

### Docker (Linux)

```bash
docker compose up --build
```

### Configuration

Edit `config.yaml` at the project root to change model, labels, or thresholds — no recompilation needed:

```yaml
%YAML:1.0
---
model_path: "../yolov5m.onnx"
names_path: "../coco.names"
confidence_threshold: 0.5
iou_threshold: 0.4
```

### Controls

| Key | Action |
|---|---|
| `ESC` | Exit |

---

## CSV Log Format

Detections are written to `detections.log`:

```
timestamp,fps,class,confidence,x,y,width,height
2026-05-28 14:30:01,22,person,87,312,140,98,210
2026-05-28 14:30:01,22,laptop,91,540,300,120,90
```

---

## Project Structure

```
edge-vision-detector/
├── include/
│   ├── Stream.h
│   ├── Detector.h
│   └── Detection.h
├── src/
│   ├── main.cpp
│   ├── Stream.cpp
│   └── Detector.cpp
├── config.yaml
├── coco.names
├── CMakeLists.txt
└── Dockerfile
```

---

## Next Steps & Limitations

**Limitations**
- Inference runs on CPU only — no CUDA acceleration on Apple Silicon (NVIDIA GPU required for CUDA)
- YOLOv5m is a general-purpose model; accuracy on domain-specific tasks (industrial inspection, medical) would require fine-tuning
- No support for RTSP streams or video file input yet — webcam only

**Planned improvements**
- **CUDA / TensorRT acceleration** for GPU-accelerated inference on NVIDIA edge hardware — target 60+ FPS
- **RTSP stream input** to support IP cameras and remote deployments
- **Python bindings via pybind11** to expose the C++ detector to Python-based ML pipelines
- **SRE additions** — Prometheus metrics endpoint, alerting on detection anomalies, health check endpoint
- **Multi-camera support** via threaded `Stream` instances
