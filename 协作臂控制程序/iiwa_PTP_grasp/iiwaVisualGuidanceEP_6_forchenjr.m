clc;clear;
close all;
load jnt_cjr.mat;
Jnt(:,4) = -Jnt(:,4);
% 将ivgep5改成给cjr毕设用的程序
% 把这里面的函数换成自己的
% 手眼换位这个可以放到5里面
% 5里面最好再写一个真正意义上的手眼偏移（通过算角度来实现，rMat到theta通过eul2rotm或者rotm2eul可以准确的）
%% 连接机器人
warning('off');
ip='172.31.1.147';
arg1=KST.LBR14R820; % choose the robot iiwa7R800 or iiwa14R820
arg2=KST.Medien_Flansch_elektrisch; % choose the type of flange
Tef_flange=eye(4); % transofrm matrix of EEF with respect to flange
iiwa = daobanKST(ip,arg1,arg2,Tef_flange); % create the object
flag = iiwa.net_establishConnection();
if flag==0
    disp('KUKA iiwa Connection Failed!')
    return;
end
pause(1);
disp('Go!');

%% 循环之前的其他工作
% 初始化到固定位置，这个初始位置在白桌那一侧
%qinit = [4.55, 36.69, 4.53, -41.59, -2.75, 100.81, 6.30]'*pi/180; %【输入机器人各关节初始角度，弧度制】
qinit = Jnt(1,:)';
jPosinit = mat2cell(qinit,[1 1 1 1 1 1 1])';
relVel = 0.1; % relative velocity
iiwa.movePTPJointSpace(jPosinit, relVel);
pause(1);
disp('Go to initial position.');

% 开启机器人实时控制

iiwa.realTime_startDirectServoJoints();
pause(1);
disp('Devices are ready.');
eef_pos_buff_save=zeros(0,3);
joint_pos_buff_save = zeros(0,7);
%% 机器人关节运动
for i=1:1:859
    jPos = mat2cell(Jnt(i,:)',[1 1 1 1 1 1 1])'; % 【自己计算好的每一步的各轴关节角的序列数据】
    [eef_pos_buff] = iiwa.sendJointsPositionsGetActualEEFpos(jPos);
    [joint_pos_buff] = iiwa.sendJointsPositionsGetActualJpos(jPos);
    meef_pos_buff = cell2mat(eef_pos_buff);
    eef_pos_buff_save = [eef_pos_buff_save; meef_pos_buff];
    mjoint_pos_buff = cell2mat(joint_pos_buff);
    joint_pos_buff_save = [joint_pos_buff_save; mjoint_pos_buff];
    pause(0.005);
end

%% 断开机器人连接
iiwa.realTime_stopDirectServoJoints();
iiwa.net_turnOffServer();
disp('Completed successfully');
warning('on');