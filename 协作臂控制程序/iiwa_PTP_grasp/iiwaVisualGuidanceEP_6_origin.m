clc;clear;
close all;
% 将ivgep5改成给本科毕设用的程序
把这里面的函数换成自己的
手眼换位这个可以放到5里面
5里面最好再写一个真正意义上的手眼偏移（通过算角度来实现，rMat到theta通过eul2rotm或者rotm2eul可以准确的）
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

%% 与相机服务器TCPIP通讯
serverIP = '192.168.100.169'; % 使用C1-305无线网

%% 参数初始化
% 预设机器人DH参数
[robot, L, jointsNum] = IBVS_createRobot('iiwa');
% 读取相机参数
camParam = IBVS_createCamera('rs640');

% 机器人末端到相机前端
Lcamera = 0.12;
% 希望预留的相机与物体距离
Loc = 0.3;
EPoffset = [0 0 Lcamera+Loc]';

% 机器人末端与相机（假设在手心）之间的转换矩阵
% TEC = [0  1 0 0.04;
%     -1  0 0 -0.035;
%     0  0 1 0.12;
%     0  0 0 1];
TEC = [0  1 0 -0.04;
    -1  0 0 0.03;
    0  0 1 0.12;
    0  0 0 1];

% 期望深度（由像素坐标反算相机坐标时的理论深度）
Zdepth = 0.55;

%% 预设一些矩阵及标志位
PointsNum = 4;
CaptureNum = 10;
pixel = zeros(2,PointsNum,CaptureNum); % 图像坐标
overtimeflag = 1; % 为1说明该周期超时（超出视野）
safetyflag = 0; % 为0说明超出安全范围不能动
trycatchflag = 0;

%% 循环之前的其他工作
% 初始化到固定位置，这个初始位置在白桌那一侧
qinit = [4.55, 36.69, 4.53, -41.59, -2.75, 100.81, 6.30]'*pi/180;
jPosinit = mat2cell(qinit,[1 1 1 1 1 1 1])';
relVel = 0.2; % relative velocity
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
if(overtimeflag)
%     iiwa.realTime_stopDirectServoCartesian();
    iiwa.net_turnOffServer();
    disp('No image.');
    warning('on');
end
meanpixel = mean(pixel,3);
disp(meanpixel);
% % plot_poly(meanpixel); set(gca, 'Ydir' , 'reverse');
% meanpixel = [347   363   333   316;   236   266   283   252];
%% 计算目标在基系中位置 计算末端应达位置
centerpoint = mean(meanpixel,2);
% EEFgoto_inBase = pixel2position(centerpoint,TEC,camParam,Zdepth,T_robotinit,EPoffset);
[EEFgoto_inBase, EEF_Rz_plus] = position2(centerpoint,TEC,camParam,Zdepth,T_robotinit,EPoffset,meanpixel);
% disp('result: '); disp(EEFgoto_inBase); disp(EEF_Rz_plus);

% 判断是否在可达空间（参数已调整）
safetyflag = iiwaVS_isPosOK(EEFgoto_inBase);

%% 计算相机转到目标方向所需角度
% 尽管分析计算的时候都在绕z轴方向旋转，但示教器手动操作结论是Rz与Tx同轴，即x和z轴的旋转角度控制互换了

EEF_Rot = iiwaVS_Rze2b(EEF_Rz_plus, rMat, p_eef(4:6));
%% 差分
stepNum = 1000;
% EEFgoto6_inBase = [EEFgoto_inBase;EEF_Rot];
EEFgoto6_inBase = [EEFgoto_inBase*1000; EEF_Rot]; % EEFgoto6_inBase就已经把单位转化和p_eef一致了
EEF_Ntarget = iiwaVS_Ntarget(EEFgoto6_inBase, p_eef, stepNum);
timenow =  zeros(stepNum+1,1);
%% 机器人末端运动
if (safetyflag)
    try
        % 发送末端位置，mm单位
        
%         % 发很多次，记得开实时
%         for i = 1:1:stepNum
%             timenow(i) = toc;
%             pause(0.02);
%             sep = mat2cell(EEF_Ntarget(:,i),[1 1 1 1 1 1]);
%             iiwa.sendEEfPositionf(sep);
%         end
%         timenow(stepNum+1) = toc;

        % 只发一次，记得关实时
        p_eef(1:3) = EEFgoto_inBase*1000; % 米转毫米
        p_eef(4:6) = EEF_Rot;
        mple = mat2cell(p_eef,[1 1 1 1 1 1]);
        vel = 100;
        [ret3] = iiwa.movePTPLineEEF(mple, vel);

        disp('VG OK');
        
    catch exception
        trycatchflag = 1;
%         iiwa.realTime_stopDirectServoCartesian();
        iiwa.net_turnOffServer();
        disp('Error during execution the direct servo motion');
        warning('on');
    end
else
    disp('Your target position is out of range.');
end


%% 断开机器人连接

if(trycatchflag)
    disp('Closed');
else
%     iiwa.realTime_stopDirectServoCartesian();
    iiwa.net_turnOffServer();
    disp('Completed successfully');
    warning('on');
end

toc;

% figure();plot(EEF_Ntarget(1,:));hold on;plot(EEF_Ntarget(2,:));hold on;plot(EEF_Ntarget(3,:));hold on;plot(EEF_Ntarget(4,:));legend('x','y','z','qz');
% figure();plot(timenow);
% EEF_V = zeros(6,stepNum);
% for i = 1:1:stepNum
%     EEF_V(:,i) = EEF_Ntarget(:,i)/( timenow(i+1)-timenow(i) );
% end
% figure();plot(EEF_V(1,:));hold on;plot(EEF_V(2,:));hold on;plot(EEF_V(3,:));hold on;plot(EEF_V(4,:));legend('x','y','z','qz');

