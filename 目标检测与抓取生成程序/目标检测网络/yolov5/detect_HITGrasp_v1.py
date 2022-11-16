import numpy as np
import torch
import argparse
from align_depth_color import *
from pathlib import Path
import os
import sys
from utils.torch_utils import select_device
from models.common import DetectMultiBackend
from utils.general import non_max_suppression

FILE = Path(__file__).resolve()
ROOT = FILE.parents[0]  # YOLOv5 root directory
if str(ROOT) not in sys.path:
    sys.path.append(str(ROOT))  # add ROOT to PATH
ROOT = Path(os.path.relpath(ROOT, Path.cwd()))  # relative


def parse_opt(known=False):
    parser = argparse.ArgumentParser()
    parser.add_argument('--weights', type=str, default=ROOT / 'bestHITGraspV2_100epo24bs.pt',
                        help='initial weights path')
    parser.add_argument('--save_path', type=str, default=ROOT / 'E:\Python_ws\ggcnn\iiwa_input_imgs',
                        help='ggcnn input depth images')

    opt = parser.parse_known_args()[0] if known else parser.parse_args()
    return opt


class HIT_img_det_tool:
    def __init__(self, weights=ROOT / 'yolov5s.pt',  # model.pt path(s)
                 source=ROOT / 'data/images',  # file/dir/URL/glob, 0 for webcam # CHANGED
                 save_path=ROOT / '../ggcnn_input/',  # ggcnn input depth images
                 imgsz=(640, 640),  # inference size (height, width)
                 conf_thres=0.25,  # confidence threshold
                 iou_thres=0.45,  # NMS IOU threshold
                 max_det=30,  # maximum detections per image
                 device='',  # cuda device, i.e. 0 or 0,1,2,3 or cpu
                 view_img=True,  # show results #CHANGED
                 save_txt=False,  # save results to *.txt
                 save_conf=False,  # save confidences in --save-txt labels
                 save_crop=False,  # save cropped prediction boxes
                 nosave=False,  # do not save images/videos
                 classes=None,  # filter by class: --class 0, or --class 0 2 3
                 agnostic_nms=False,  # class-agnostic NMS
                 augment=False,  # augmented inference
                 visualize=False,  # visualize features
                 update=False,  # update all models
                 project=ROOT / 'runs/detect',  # save results to project/name
                 name='exp',  # save results to project/name
                 exist_ok=False,  # existing project/name ok, do not increment
                 line_thickness=3,  # bounding box thickness (pixels)
                 hide_labels=False,  # hide labels
                 hide_conf=False,  # hide confidences
                 half=False,  # use FP16 half-precision inference
                 dnn=False,  # use OpenCV DNN for ONNX inference
                 ):
        # 网络初始化
        self.pred_np = None
        self.pred = None
        self.device = select_device(device)
        self.model = DetectMultiBackend(weights, device=self.device, dnn=dnn, fp16=half)
        self.stride, self.names, self.pt = self.model.stride, self.model.names, self.model.pt

        self.max_det = max_det
        self.augment = augment
        self.vusualize = visualize
        self.conf_thres = conf_thres
        self.iou_thres = iou_thres
        self.classes = classes
        self.agnostic_nms = agnostic_nms
        self.save_path = save_path

        self.color_image = np.zeros([480, 640, 3], np.uint8)
        self.depth_image = np.zeros([480, 640])
        self.ggcnn_input = np.zeros([300, 300])
        self.output_data = []

    def detect_img(self, img=''):
        # Load_data:
        if img == '':
            img = self.color_image
        img_bgr = img[..., ::-1].copy()  # transform image from RGB to GBR
        seen = 0
        im = np.transpose(img_bgr, (2, 0, 1))
        im = torch.from_numpy(im).to(self.device)
        im = im.half() if self.model.fp16 else im.float()  # uint8 to fp16/32
        im /= 255  # 0 - 255 to 0.0 - 1.0

        if len(im.shape) == 3:
            im = im[None]  # expand for batch dim
        # Inference:
        self.pred = self.model(im, augment=self.augment, visualize=self.vusualize)
        # NMS:
        self.pred = non_max_suppression(self.pred, self.conf_thres, self.iou_thres, self.classes, self.agnostic_nms,
                                        max_det=self.max_det)
        # Process and Put out Detection Results:
        self.pred_np = self.pred[0].cpu().numpy()
        print("Platform-existing Object(s): ")
        for i, c in enumerate(self.pred_np[:, -1]):
            print(str(i) + ": " + self.names[int(c)])

    def crop_save_chosen_obj(self, chosen_num):
        if chosen_num == 'a':
            pass
        else:
            if self.pred_np.shape[0] < (int(chosen_num)+1):
                print("This object doesn't exist!")
                return 1
            chosen_num = int(chosen_num)
            obj_pred = self.pred_np[chosen_num]
            obj_lx = int(obj_pred[0])
            obj_ly = int(obj_pred[1])
            obj_rx = int(obj_pred[2])
            obj_ry = int(obj_pred[3])
            out_depth = np.zeros([480, 640])
            out_color = np.zeros([480, 640, 3])
            out_color.fill(133)

            out_depth[obj_ly:obj_ry, obj_lx:obj_rx] = self.depth_image[obj_ly:obj_ry, obj_lx:obj_rx]
            out_color[obj_ly:obj_ry, obj_lx:obj_rx, :] = self.color_image[obj_ly:obj_ry, obj_lx:obj_rx, :]
            cv2.imshow("origin_color", self.color_image / 255)
            cv2.imshow("origin_depth", self.depth_image / 550)
            cv2.imshow("cropped_color", out_color / 255)
            cv2.imshow("cropped_depth", out_depth / 550)
            cv2.waitKey(1)
            savename_depth = 'depth_2'
            savename_color = 'color_2'
            np.save(os.path.join(self.save_path, savename_depth), out_depth)
            np.save(os.path.join(self.save_path, savename_color), out_color)
            # cv2.imwrite(os.path.join(self.save_path, savename_depth), out_depth)
            # cv2.imwrite(os.path.join(self.save_path, savename_color), out_color)
            #
            # cv2.waitKey(0)
            # txt_path = str(save_dir / 'labels' / p.stem) + ('' if dataset.mode == 'image' else f'_{frame}')  # im.txt
            # s += '%gx%g ' % im.shape[2:]  # print string
            # gn = torch.tensor(im0.shape)[[1, 0, 1, 0]]  # normalization gain whwh
            # imc = im0.copy() if save_crop else im0  # for save_crop
            # annotator = Annotator(im0, line_width=line_thickness, example=str(names))
            # if len(det):
            #     # Rescale boxes from img_size to im0 size
            #     det[:, :4] = scale_coords(im.shape[2:], det[:, :4], im0.shape).round()
            #
            #     # Print results
            #     for c in det[:, -1].unique():
            #         n = (det[:, -1] == c).sum()  # detections per class
            #         s += f"{n} {names[int(c)]}{'s' * (n > 1)}, "  # add to string
            #
            #     # Write results
            #     for *xyxy, conf, cls in reversed(det):
            #         if save_txt:  # Write to file
            #             xywh = (xyxy2xywh(torch.tensor(xyxy).view(1, 4)) / gn).view(-1).tolist()  # normalized xywh
            #             line = (cls, *xywh, conf) if save_conf else (cls, *xywh)  # label format
            #             with open(f'{txt_path}.txt', 'a') as f:
            #                 f.write(('%g ' * len(line)).rstrip() % line + '\n')
            #
            #         if save_img or save_crop or view_img:  # Add bbox to image
            #             c = int(cls)  # integer class
            #             label = None if hide_labels else (names[c] if hide_conf else f'{names[c]} {conf:.2f}')
            #             annotator.box_label(xyxy, label, color=colors(c, True))
            #         if save_crop:
            #             save_one_box(xyxy, imc, file=save_dir / 'crops' / names[c] / f'{p.stem}.jpg', BGR=True)
            #
            # # Stream results
            # im0 = annotator.result()
            # if view_img:
            #     cv2.imshow(str(p), im0)
            #     cv2.waitKey(1)  # 1 millisecond


def main(opt):
    init_RS_images()
    detector = HIT_img_det_tool(**vars(opt))
    detector.color_image, detector.depth_image = send_RS_images()
    detector.detect_img()
    obj_name = input(
        "Please choose one object to be grasped by KUKA iiwa Robot! \ninput it's number or input 'a' to grasp all of them: ")
    detector.crop_save_chosen_obj(obj_name)


if __name__ == "__main__":
    opt = parse_opt()
    main(opt)
