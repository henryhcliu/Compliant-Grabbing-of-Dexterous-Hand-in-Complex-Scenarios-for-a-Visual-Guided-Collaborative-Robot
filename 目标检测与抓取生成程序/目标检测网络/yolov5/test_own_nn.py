import torch

model = torch.hub.load('ultralytics/yolov5', 'custom', path='last.pt')  # local model

img = "..\datasets\HITGrasp_v1\\train\images\HITGrasp3_jpg.rf.8f61a3c58a90cc9179af89cac1f333a2.jpg"

result = model(img)

print(result)