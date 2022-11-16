% this function is used to define the parameters of our robot
function [robot,L,jointnum]= IBVS_createRobot(robotmodel)
efort = 'eft';
universal = 'ur5';
kukaiiwa = 'iiwa';

if(strcmp(robotmodel,efort))
    % 埃夫特机器人
    % 通过示教器上给出的参数进行DH坐标系建立，其中对于5轴的0.0785的省略代表定义5轴和6轴的中心位置重合还是有偏置
    L(1) = Link([0,      0.32,     0.049079, -pi/2 ], 'standard');
    L(2) = Link([-pi/2, -0.000023, 0.270537,  0    ], 'standard');
    L(3) = Link([0,      0,        0.069887, -pi/2 ], 'standard');
    L(4) = Link([0,      0.299097, 0,         pi/2 ], 'standard');
    L(5) = Link([pi/2,   0,        0,        -pi/2 ], 'standard'); % 这里有没有初始的pi/2待议
    L(6) = Link([0,      0,        0,         0    ], 'standard');
    % 关节参数范围限定
    L(1).qlim = [-167 167]*pi/180;
    L(2).qlim = [-130 90]*pi/180;
    L(3).qlim = [-71 101]*pi/180;
    L(4).qlim = [-180 180]*pi/180;
    L(5).qlim = [-113 113]*pi/180;
    L(6).qlim = [-360 360]*pi/180;
    robot = SerialLink(L,'name', 'EFORT');
    jointnum = 6;
    disp('Your robot is Efort.');

elseif(strcmp(robotmodel,universal))
    % UR5机器人
    L(1) = Link([0,      0.089159, 0,       pi/2 ], 'standard');
    L(2) = Link([0,      0,       -0.425,   0    ], 'standard');
    L(3) = Link([0,      0,       -0.39225, 0    ], 'standard');
    L(4) = Link([0,      0.10915,  0,       pi/2 ], 'standard');
    L(5) = Link([0,      0.09465,  0,      -pi/2 ], 'standard');
    L(6) = Link([0,      0.08230,  0,       0    ], 'standard');
    robot = SerialLink(L,'name', 'UR5');
    jointnum = 6;
    disp('Your robot is UR5.');
    
elseif(strcmp(robotmodel,kukaiiwa))
    % KUKAIIWA机器人    
    L(1) = Link([0, 0.36,  0, -pi/2 ], 'standard');
    L(2) = Link([0, 0,     0,  pi/2 ], 'standard');
    L(3) = Link([0, 0.42,  0,  pi/2 ], 'standard');
    L(4) = Link([0, 0,     0, -pi/2 ], 'standard');
    L(5) = Link([0, 0.4,   0, -pi/2 ], 'standard');
    L(6) = Link([0, 0,     0,  pi/2 ], 'standard');
    L(7) = Link([0, 0.126+0.035, 0,  0    ], 'standard');
    robot = SerialLink(L,'name', 'KUKA');
    jointnum = 7;
    disp('Your robot is KUKA iiwa.');
    
else
    % 不是机器人
    L = Link([0, 0.32, 0.049079, pi/2 ], 'standard');
    robot = SerialLink( L ,'name', 'NOrobot');
    jointnum = 1;
    disp('Robot model error!');
end
