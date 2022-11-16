文件夹内容与使用说明：

“灵巧手控制程序”文件夹内含因时机器人灵巧手的基于力阈值的自适应抓取程序，包括张开与抓握程序两部分，其可执
行文件在各自的x64文件夹中，后缀为.exe。

“目标检测与抓取生成程序”文件夹内含目标检测网络和机器人抓取生成网络两个子模块，以及本人自制的HIT Grasp
数据集及其原始拍摄图像。其中目标检测网络的运行程序为：yolov5文件夹中的detect_HITGrasp_v1.py。抓取生成网络
的运行程序为：rsVgGetObjPos_ggcnn_v2.py（单目标抓取网络）、VgGetObjPos_ggcnn_for_yolo（多目标选择性抓
取网络）

“协作臂控制程序”文件夹内包含KUKA LBR iiwa 14 R820的运动控制程序，运行程序为：iiwaVG_1.m（视觉引导抓取
程序）、collision_iiwaVG_1.m（支持碰撞检测与响应控制的视觉引导抓取）、u0616_KSTclass_Tutorial_realTimeImp
edencePlotTorqueFeedBack.m（正弦曲线轨迹的柔顺控制实验程序）

“硕士学位论文终稿与附件”文件夹内包含论文终稿、终期答辩PPT、多个单目标抓取视频、柔顺控制实验视频、多个多
目标抓取视频等。

如有疑问：15169553502