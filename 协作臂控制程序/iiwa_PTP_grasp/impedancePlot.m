x_1 = 1:1:646;
x_2 = 18/646;
x_1 = x_1*x_2;

y_1 = torque_array;
y_2 = jnt_save;
y_3 = eef_pose_save;
bs=['-',"-.",':','-','--','-.','-'];
figure;
for i=1:7
   plot(x_1,y_1,bs(i),'linewidth',1.2);
   hold on;
end
xlabel('time(s)'),ylabel('Moment on joint (N)'),legend('J1','J2','J3','J4','J5','J6','J7');
figure;
for i=1:7
   plot(x_1,y_2(:,i),bs(i),'linewidth',1.2);
   hold on;
end
xlabel('time(s)'),ylabel('Joint angle (radian)'),legend('J1','J2','J3','J4','J5','J6','J7');

figure;
for i=1:3
   plot(x_1,y_3(:,i),bs(i),'linewidth',1.2);
   hold on;
end
xlabel('time(s)'),ylabel('End Effector Position'),legend('x','y','z','r_x','r_y','r_z');

figure;
plot3(y_3(:,1),y_3(:,2),y_3(:,3));

