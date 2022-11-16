function [safetyflag, P_inBase] = iiwaVS_isPosOK(P_inBase)
% 手动控制测试的末端范围（直接按示教器上的读数来）
% x轴 500~750； y轴 -220~220； z轴 186~700
% 单位均为mm，转成m来比较
if(P_inBase(1)>0.5 && P_inBase(1)<0.75 && ...
        P_inBase(2)>-0.22 && P_inBase(2)<0.22 && ...
        P_inBase(3)<0.7)
    safetyflag = 1;
else
    safetyflag = 0;
end
if(P_inBase(3)<0.193)
    %P_inBase(3) = 0.20
    P_inBase(3) = 0.193
    disp("Your input depth is limited!")
end
end