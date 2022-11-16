clc;clear;
close all;
% 接收realsense相机二维码识别结果的kuka的视觉引导程序
% 使用了KST类
% 在ivgep3的基础上加上最后一段的手眼偏移

%% 连接机器人
warning('off');
ip='172.31.1.147';
arg1=KST.LBR14R820; % choose the robot iiwa7R800 or iiwa14R820
arg2=KST.Medien_Flansch_elektrisch; % choose the type of flange
Tef_flange=eye(4); % transofrm matrix of EEF with respect to flange
iiwa = daobanKST(ip,arg1,arg2,Tef_flange); % create the object
flag = iiwa.net_establishConnection();
if flag==0
    return;
end
pause(1);
disp('Go!');
% % % % ip='172.31.1.147';
% % % % global t_Kuka;
% % % % t_Kuka = net_establishConnection( ip );
% % % % if ~exist('t_Kuka','var') || isempty(t_Kuka) || strcmp(t_Kuka.Status,'closed')
% % % %     disp('Connection could not be establised, script aborted');
% % % %     return;
% % % % end

%% 与相机服务器TCPIP通讯
serverIP = '192.168.100.115'; % 使用C1-305无线网

%% 参数初始化
% 预设机器人DH参数（已经写在kuka_jacobi里面）
[robot, L, jointsNum] = IBVS_createRobot('iiwa');
% 读取相机参数（预设相机参数）    % 需要一个直接从相机读参数的程序
camParam = IBVS_createCamera('rs640');

% 机器人末端到相机前端（事件相机取本体）距离
Lcamera = 0.12;
% 希望预留的相机与物体距离，可以根据抓取任务调整
Loc = 0.3;
EPoffset = [0 0 Lcamera+Loc]';
% 手眼偏移（在末端坐标系中） 反映还要走多远才能让手替换眼的位置
% CHoffset_inE = [-0.06 0.035 0]';
CHoffset_inE = [0 0.035 0]';

% 机器人末端与相机之间的转换矩阵
TEC = [0 1 0 0.04;
    -1  0 0 -0.035;
    0 0 1 Lcamera;
    0 0 0 1];

% 期望深度（由像素坐标反算相机坐标时的理论深度）
Zdepth = 0.55;

%% 预设一些矩阵及标志位
PointsNum = 4;
CaptureNum = 20;
pixel = zeros(2,PointsNum,CaptureNum); % 图像坐标
overtimeflag = 1; % 为1说明该周期超时（其实是说明超出视野）
safetyflag = 0; % 为0说明超出安全范围不能动
trycatchflag = 0; % 为1说明进入了catch异常处理当中

%% 循环之前的其他工作
% 初始化到固定位置，这个初始位置在白桌那一侧
% qinit = [-0.0193, 0.7930, 0.3200, -1.4184, -0.4818, 0.7822, -0.4017]';
% qinit = [0.0841, 0.7337, 0.0792, -1.1393, -0.0578, 1.2520, 0.1603]';
qinit = [4.55, 36.69, 4.53, -41.59, -2.75, 100.81, 6.30]'*pi/180;
jPosinit = mat2cell(qinit,[1 1 1 1 1 1 1])';
relVel = 0.1; % relative velocity
[ret] = iiwa.movePTPJointSpace(jPosinit, relVel);
pause(1);
disp('Go to initial position.');

% 读取机器人齐次变换矩阵
rMat = iiwa.getEEFOrientationR();
Pos = iiwa.getEEFCartesianPosition();
T_robotinit(1:3,1:3) = rMat;
T_robotinit(1:3,4) = cell2mat(Pos)/1000; % 注意将mm转到m
T_robotinit(4,4) = 1;
T_caminit = T_robotinit * TEC;

% 读取机器人末端位置姿态
[gep] = iiwa.getEEFPos();
p_eef = cell2mat(gep)';

% iiwa.realTime_startDirectServoCartesian();
pause(1);
disp('Devices are ready.');
tic;

%% 读取若干帧图像并求平均
for i = 1:1:CaptureNum
    t0 = toc;
    if(i>1)
        [pixel(:, :, i),overtimeflag] = iiwaVS_tcpgetpixel(serverIP,t0,pixel(:, :, i-1));
    else
        [pixel(:, :, i),overtimeflag] = iiwaVS_tcpgetpixel(serverIP,t0,pixel(:, :, 1));
    end
end
meanpixel = mean(pixel,3);
disp(meanpixel);
% % plot_poly(meanpixel); set(gca, 'Ydir' , 'reverse');

%% 计算目标在基系中位置 计算末端应达位置
P_inCamera = iiwaVS_img2cam(camParam, meanpixel, Zdepth);
P_inBase = iiwaVS_cam2base(T_caminit, P_inCamera);
EEFgoto_inBase = P_inBase + EPoffset;
% 判断是否在可达空间（参数已调整）
safetyflag = iiwaVS_isPosOK(EEFgoto_inBase);

%% 计算相机转到目标方向所需角度
% 尽管分析计算的时候都在绕z轴方向旋转，但示教器手动操作结论是Rz与Tx同轴，即x和z轴的旋转角度控制互换了
EEF_Rz_plus = iiwaVS_img2gamma(meanpixel);
% disp('result: '); disp(EEFgoto_inBase); disp(EEF_Rz_plus); disp(p_eef(4:6));
p_eef(4:6) = iiwaVS_Rze2b(EEF_Rz_plus, rMat, p_eef(4:6));
% disp(p_eef(4:6));
%% 机器人末端运动
if (safetyflag)
    try
        p_eef(1:3) = EEFgoto_inBase*1000; % 米转毫米
        % 发送末端位置
        % 在sendEEfPosition的时候发送的是mm单位下的位置
        sep = mat2cell(p_eef,[1 1 1 1 1 1]);
%         [ret2] = iiwa.sendEEfPositions(sep);
        vel = 30;         
        [ret3] = iiwa.movePTPLineEEF(sep, vel);
        pause(0.1);
        rMatGrasp = iiwa.getEEFOrientationR();
        p_eef(1:3) = p_eef(1:3) + rMatGrasp*CHoffset_inE*1000;
        [ret4] = iiwa.movePTPLineEEF(sep, vel);
        
        disp('VG OK');
        
    catch exception
        trycatchflag = 1;
        iiwa.net_turnOffServer();
        disp('Error during execution the direct servo motion');
        warning('on');
    end
else
    disp('Your target position is out of range.');
end


%% 断开机器人连接
% iiwa.realTime_stopDirectServoCartesian();
if(trycatchflag)
    disp('Closed');
else
    iiwa.net_turnOffServer();
    disp('Completed successfully');
    warning('on');
end

toc;
% 该版本没有多少输出的数据，故不画图



