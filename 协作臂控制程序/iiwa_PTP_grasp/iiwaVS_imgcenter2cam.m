function P_inCamera = iiwaVS_imgcenter2cam(camParam, centerpixel, Zdepth)
% 像素坐标转相机系坐标
f =  camParam(1,1);%非真实的
nw = camParam(1,2);%像素尺寸
nh = camParam(1,3);
u0 = camParam(1,4);%中心点
v0 = camParam(1,5);
nu = camParam(1,6);%总像素值
nv = camParam(1,7);

% from point in image to point in cam
P_inCamera = zeros(3,1);
P_inCamera(1) = (centerpixel(1)-u0)/f*Zdepth;
P_inCamera(2) = (centerpixel(2)-v0)/f*Zdepth;
P_inCamera(3) = Zdepth;

end