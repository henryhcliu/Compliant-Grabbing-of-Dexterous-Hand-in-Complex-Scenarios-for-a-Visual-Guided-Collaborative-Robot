% this function is used to define the parameters of our camera
function camParam = IBVS_createCamera(cameramodel)
basler = 'basler';
rs1920 = 'rs1920';
rs960 = 'rs960';
davis = 'davis';
rs640 = 'rs640'
if(strcmp(cameramodel,basler))
    % 对应的是埃夫特末端相机，参数由gh获得
    cam = CentralCamera('focal', 3376, 'pixel', 4.8e-6, ...
        'resolution', [640 480], 'centre', [320 240], 'name', 'guocamera');
    disp('Your camera is basler in 640x480.');
elseif(strcmp(cameramodel,rs640))
    cam = CentralCamera('focal', 613, 'pixel', 2.753e-6, ...
        'resolution', [640 480], 'centre', [328 231], 'name', '435camerain960');
    disp('Your camera is realsense in 640x480.');
elseif(strcmp(cameramodel,rs1920))
    % 对应的是d435i相机   f=1.88mm=1366pixel，折算像素长度1.376e-6
    cam = CentralCamera('focal', 1366, 'pixel', 1.376e-6, ...
        'resolution', [1920 1080], 'centre', [976 545], 'name', '435camerain1920');
    disp('Your camera is realsense in 1920x1080.');
elseif(strcmp(cameramodel,rs960))
    cam = CentralCamera('focal', 683, 'pixel', 2.753e-6, ... % f辨识结果为675或671 ，下面 490 281
        'resolution', [960 540], 'centre', [488 272], 'name', '435camerain960');
    disp('Your camera is realsense in 960x540.');
elseif(strcmp(cameramodel,davis))
    cam = CentralCamera('focal', 323.96, 'pixel', 9e-6, ...
        'resolution', [346 260], 'centre', [183.7 115.9], 'name', 'davis');  % 中心标定结果 [183.7 115.9]  好用 [170 100]
    disp('Your camera is davis in 346x260.');
else
    cam = CentralCamera('focal', 9999, 'pixel', 9e-6, ...
        'resolution', [999 999], 'centre', [666 666], 'name', 'nocamera');
    disp('Camera model error!');
end
% 相机内部参数，f为焦距，rho为像素尺寸，u0、v0为图像中心，nu、nv为图像平面总像素值
f = cam.f;
rho = cam.rho;
u0 = cam.u0;
v0 = cam.v0;
nu =cam.nu;
nv = cam.nv;
camParam = [f rho u0 v0 nu nv];
end