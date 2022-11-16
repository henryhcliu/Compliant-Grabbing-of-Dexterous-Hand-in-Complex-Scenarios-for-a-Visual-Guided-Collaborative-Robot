% 图像处理得到特征点像素坐标
function [CZ,overtimeflag] = iiwaVS_tcpgetcenter(serverIP,t0,CZbefore)
overtimeflag = 1;
dt = 0;
CZ(1:2) = -0.1*CZbefore(1:2); CZ(3) = 0;
t = tcpip(serverIP,5000,'Timeout', 60,'InputBufferSize',10240);
while(dt<0.2)
    fopen(t);
    fwrite(t,'get');
    while(1) 
        nBytes = get(t,'BytesAvailable');
        if nBytes>0
            break;
        end
    end
    receive = fread(t,nBytes);
    %fread(t);
    fclose(t);
    data = str2num(char(receive(2:end-1)')); 
    if ( ~isempty(data) && size(data,1)==1 && size(data,2)==3 )
        CZ = reshape(data,[3,1]);
        if(min(CZ(1:2))<0)
            overtimeflag = 1;
        else
            overtimeflag = 0; % 能到这里说明没有超时，且目标在视野内
        end
%         disp('get pixel successfully.');
%         disp(data);
        delete(t);
        break;
    end
    timenow = toc;
    dt = timenow - t0;
end
delete(t);
end