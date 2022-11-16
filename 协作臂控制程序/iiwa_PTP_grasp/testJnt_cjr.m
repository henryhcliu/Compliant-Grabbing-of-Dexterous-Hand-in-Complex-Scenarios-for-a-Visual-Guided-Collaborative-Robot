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

%% PTP运动，检验机器人的初始位置和终止位置的合法性
% qinit = [4.55, 36.69, 4.53, -41.59, -2.75, 100.81, 6.30]'*pi/180; %【输入机器人各关节初始角度，弧度制】
qinit = Jnt(859,:)';
qinit(4) = -qinit(4);
jPosinit = mat2cell(qinit,[1 1 1 1 1 1 1])';
relVel = 0.05; % relative velocity
[ret] = iiwa.movePTPJointSpace(jPosinit, relVel);
pause(1);
disp('Go to initial position.');

%% 断开机器人连接
iiwa.net_turnOffServer();
disp('Completed successfully');