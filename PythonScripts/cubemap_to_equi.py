import numpy as np
import os
import glob
from PIL import Image, ImageOps
from py360convert import cube_list2h, cube_h2dice, c2e

airsim_export_folder = "C:/Users/yujiew/Documents/AirSim/2020-12-20-17-57-39"
camera_list = [0,2,5,1,3,4] # [F, R, B, L, U, D]
img_type_list = [0,3,5]


equi_dir = os.path.join(airsim_export_folder, "pano")
if not os.path.exists(equi_dir):
    os.mkdir(equi_dir)

for img_type in img_type_list:
    out_dir = os.path.join(equi_dir, str(img_type))
    if not os.path.exists(out_dir):
        os.mkdir(out_dir)

    images_by_cam = []
    for cam in camera_list:
        image_list = glob.glob(os.path.join(airsim_export_folder, "images", "img_{0}_{1}_*.png".format(cam, img_type)))
        image_list.sort()
        images_by_cam.append(image_list)



    for i in range(len(images_by_cam[0])):
        cubemap = []
        for j in range(6):
            if(j == 4):
                cubemap.append(np.array(ImageOps.flip(Image.open(images_by_cam[j][i]))))
            elif(j == 1 or j == 2):
                cubemap.append(np.array(ImageOps.mirror(Image.open(images_by_cam[j][i]))))
            else:
                cubemap.append(np.array(Image.open(images_by_cam[j][i])))

        equi = c2e(cubemap, 1024, 2048, cube_format='list')
        equi_img = Image.fromarray(equi.astype(np.uint8))
        equi_img.save(os.path.join(out_dir, "pano_{0}_{1}.png".format(img_type, i)), 'PNG')


