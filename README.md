# Compliant-Grabbing-of-Dexterous-Hand-in-Complex-Scenarios-for-a-Visual-Guided-Collaborative-Robot
This repository contains: 
1. the necessary codes driving the InspireHand Dexterous Hand (right one) to open and close based on the feedback of force sensors mounted on every DoF.
2. the SGG-Net codes for visual object detection and grasp generation simultaneously based on YOLO v5 and GG-CNN.
3. the KUKA LBR iiwa 14 R820 collaborative robot pick-and-place control code, which contains torque-based collision detection and impedance control schemes.

# Remain to be updated before my return in July, 2022.

Folder contents and instructions for use:

1. The "Dexterous Hand Control Program" folder contains the adaptive force-threshold-based force threshold grabber for the robot's dexterous hand, including two parts, the open and grip programs, whose executable files are in their respective x64 folders with the suffix .exe.

2. The "Object Detection and Scraping Generator" folder contains two sub-modules, the object detection network and the robot scraping generation network, as well as my self-made HIT Grasp dataset and its original captured images. The running program of the object detection network is: detect_HITGrasp_v1.py in the yolov5 folder. The running programs of the crawling generation network are: rsVgGetObjPos_ggcnn_v2.py (single-objective crawling network), VgGetObjPos_ggcnn_for_yolo (multi-objective selective crawling network)

3. The "Collaborative Arm Control Program" folder contains the motion control program of KUKA LBR iiwa 14 R820, which runs the following programs: iiwaVG_1.m (Visually Guided Gripper), collision_iiwaVG_1.m (Visually Guided Gripper with Collision Detection and Response Control), u0616_KSTclass_Tutorial_ realTimeImpedencePlotTorqueFeedBack.m (Experimental procedure for the compliance control of sinusoidal trajectory)

4. The "Master's Thesis Final Draft and Attachments" folder contains the final thesis draft, final defense PPT, multiple single-target capture videos, compliance control experiment videos, multiple multi-objective capture videos, etc. *Due to copyright reasons, the final thesis and final defense PPT were deleted.*

*If in doubt: 15169553502,haichao-liu@outlook.com*

# 文件夹内容与使用说明：

1. “灵巧手控制程序”文件夹内含因时机器人灵巧手的基于力阈值的自适应抓取程序，包括张开与抓握程序两部分，其可执行文件在各自的x64文件夹中，后缀为.exe。

2. “目标检测与抓取生成程序”文件夹内含目标检测网络和机器人抓取生成网络两个子模块，以及本人自制的HIT Grasp数据集及其原始拍摄图像。其中目标检测网络的运行程序为：yolov5文件夹中的detect_HITGrasp_v1.py。抓取生成网络的运行程序为：rsVgGetObjPos_ggcnn_v2.py（单目标抓取网络）、VgGetObjPos_ggcnn_for_yolo（多目标选择性抓取网络）

3. “协作臂控制程序”文件夹内包含KUKA LBR iiwa 14 R820的运动控制程序，运行程序为：iiwaVG_1.m（视觉引导抓取程序）、collision_iiwaVG_1.m（支持碰撞检测与响应控制的视觉引导抓取）、u0616_KSTclass_Tutorial_realTimeImpedencePlotTorqueFeedBack.m（正弦曲线轨迹的柔顺控制实验程序）

4. “硕士学位论文终稿与附件”文件夹内包含论文终稿、终期答辩PPT、多个单目标抓取视频、柔顺控制实验视频、多个多目标抓取视频等。由于版权原因，删去论文终稿、终期答辩PPT。

*如有疑问：15169553502，haichao-liu@outlook.com*
