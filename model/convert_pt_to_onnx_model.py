import sys
from ultralytics import YOLO

# Load a model
model = YOLO(sys.argv[1])
# Export the model
model.export(format="onnx")
