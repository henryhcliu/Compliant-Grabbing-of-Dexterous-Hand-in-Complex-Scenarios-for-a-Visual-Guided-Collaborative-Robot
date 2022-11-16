function roteef_inb = iiwaVS_Rze2b(EEF_Rz_plus, rMat, roteef_inb)
% 将末端坐标系绕z轴旋转角增量转化为基系下末端角度
% 实验证明不能直接相加
% 示教器手动操作发现x和z轴的旋转角度控制互换了

roteef_ine = inv(rMat) * roteef_inb;
roteef_ine(1) = roteef_ine(1) + EEF_Rz_plus;
roteef_ine(2) = roteef_ine(2) + 0.64;
roteef_inb = rMat * roteef_ine;

end