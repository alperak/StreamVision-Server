from ultralytics import YOLO

# Load a model
model = YOLO("yolo11s.pt")
# Export the model
model.export(format="onnx", device=0, half=True)

