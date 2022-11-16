# import winsound
# import pyrealsense2 as rs
# import time
#
#
# duration = 800  # millisecond
# freq = 440  # Hz
# winsound.Beep(freq, duration)
#
# # Create a pipeline
# pipeline = rs.pipeline()
#
# # Create a config and configure the pipeline to stream
# #  different resolutions of color and depth streams
# config = rs.config()
#
# # Get device product line for setting a supporting resolution
# pipeline_wrapper = rs.pipeline_wrapper(pipeline)
# pipeline_profile = config.resolve(pipeline_wrapper)
# device = pipeline_profile.get_device()
# device_product_line = str(device.get_info(rs.camera_info.product_line))
#
# found_rgb = False
# for s in device.sensors:
#     if s.get_info(rs.camera_info.name) == 'RGB Camera':
#         found_rgb = True
#         break
# if not found_rgb:
#     print("The demo requires Depth camera with Color sensor")
#     exit(0)
#
# config.enable_stream(rs.stream.depth, 640, 480, rs.format.z16, 30)
# config.enable_stream(rs.stream.color, 640, 480, rs.format.bgr8, 30)
#
# # Start streaming
# profile = pipeline.start(config)

import winsound
import cv2
cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
flag = cap.isOpened()
index = 35
while (flag):
    ret, frame = cap.read()
    cv2.imshow("Capture_Paizhao", frame)
    k = cv2.waitKey(1) & 0xFF
    if k == ord('s'):  # 按下s键，进入下面的保存图片操作
        cv2.imwrite("E:/Python_ws/" + "HITGrasp" + str(index) + ".jpg", frame)
        print("save" + str(index) + ".jpg successfuly!")
        print("-------------------------")
        duration = 800  # millisecond
        freq = 440  # Hz
        winsound.Beep(freq, duration)
        index += 1
    elif k == ord('q'):  # 按下q键，程序退出
        break
cap.release() # 释放摄像头
cv2.destroyAllWindows()# 释放并销毁窗口