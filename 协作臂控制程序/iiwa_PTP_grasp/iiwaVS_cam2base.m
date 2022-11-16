function P_inBase = iiwaVS_cam2base(Tbc, P_inCamera)
% 相机系坐标转基系坐标
Pc = [P_inCamera;1];
Pb = Tbc * Pc;
P_inBase = Pb(1:3);
end