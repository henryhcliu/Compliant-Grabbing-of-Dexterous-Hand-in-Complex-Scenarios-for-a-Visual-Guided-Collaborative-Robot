function VG_output = python_test()
cmd_1 = "E: && cd E:\Python_ws\ggcnn && python .\VgGetObjPos_ggcnn_for_yolo.py"; % 多目标选择抓取
cmd_2 = "E: && cd E:\Python_ws\ggcnn && python .\rsVgGetObjPos_ggcnn_v2.py"; % 单目标抓取
% dos(cmd_1);
% dos(cmd_2);
[status, cmdout] = dos(cmd_2);
if status == 0
    VG_output = cmdout;
else
    VG_output = 1;
end
end