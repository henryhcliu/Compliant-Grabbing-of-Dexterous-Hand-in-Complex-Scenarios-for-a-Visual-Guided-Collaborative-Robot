clc;clear;
close all;
%% 连接机器人
warning('off');
ip='172.31.1.147';
arg1=daobanKST.LBR14R820; % choose the robot iiwa7R800 or iiwa14R820
arg2=daobanKST.Medien_Flansch_elektrisch; % choose the type of flange
Tef_flange=eye(4); % transofrm matrix of EEF with respect to flange
iiwa = daobanKST(ip,arg1,arg2,Tef_flange); % create the object
flag = iiwa.net_establishConnection();
if flag==0
    disp('Connection could not be establised, script aborted');
    return;
end
pause(1);
disp('Go!');
%% 初始化到固定位置，这个初始位置在白桌那一侧
% qinit = [0.0841, 0.7337, 0.0792, -1.1393, -0.0578, 1.2520, 0.1603]';
% qinit_deg = [4.19, 35.35, 4.53, -52.15, -2.61, 91.6, 6.46]';
qinit_deg = [4.55, 36.69, 4.53, -41.59, -2.75, 100.81, 6.30]';
qinit = qinit_deg*pi/180;
jPosinit = mat2cell(qinit,[1 1 1 1 1 1 1])';
relVel = 0.2; % relative velocity
[ret] = iiwa.movePTPJointSpace(jPosinit, relVel);
InspireHandOpen = "E:\VS2019_ws\InspireHandOpen\InspireHandOpen_lhc\x64\Release\InspireHandOpen_lhc.exe"
dos(InspireHandOpen);
pause(1);
disp('Go to the initial position successfully!');
%% 读取机器人末端位置姿态
[gep] = iiwa.getEEFPos();
p_eef = cell2mat(gep)';
p_eef(3) = p_eef(3)-100;
p_eef(5) = p_eef(5)+30/180*pi;
sep = mat2cell(p_eef,[1 1 1 1 1 1]);
vel = 40;
[ret3] = iiwa.movePTPLineEEF(sep, vel);
InspireHandClose = "E:\VS2019_ws\InspireHandClose\InspireHandClose_lhc\x64\Release\InspireHandClose_lhc.exe"
dos(InspireHandClose);
[ret] = iiwa.movePTPJointSpace(jPosinit, relVel);
disp('Visual Guidance Complete!');

%% 断开机器人连接
% iiwa.realTime_stopDirectServoCartesian();
iiwa.net_turnOffServer();
disp('Mission Complete!');
warning('on');