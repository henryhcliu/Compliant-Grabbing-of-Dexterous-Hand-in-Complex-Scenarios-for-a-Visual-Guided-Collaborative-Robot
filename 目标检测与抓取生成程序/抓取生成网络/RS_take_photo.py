from align_depth_color import *
import os

init_RS_images()
img_color, img_depth = send_RS_images()
data_path = 'E:\Python_ws\ggcnn\iiwa_input_imgs'
savename_depth = 'depth_0'
savename_color = 'color_0'
np.save(os.path.join(data_path, savename_depth), img_color)
np.save(os.path.join(data_path, savename_color), img_depth)