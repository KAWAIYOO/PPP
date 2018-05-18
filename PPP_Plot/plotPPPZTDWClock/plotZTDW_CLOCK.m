clc
clear
close all
fileNames = dir('*.txt');
lenName = numel(fileNames);
tic
for i = 1:lenName
    [num2str(i) '/' num2str(lenName) ' -> ' fileNames(i).name]
    fid = fopen(fileNames(i).name,'r');
    dX=[];dY=[];dZ=[];Num=[];
    while ~feof(fid)
        linestr = fgetl(fid);
        dNumi = linestr(37:48);
        Num = [Num str2double(dNumi)];
        dXi = linestr(49:59);
        dYi = linestr(61:76);
        %dZi = linestr(78:88);
        dX = [dX str2double(dXi)];
        dY = [dY str2double(dYi)];
        %dZ = [dZ str2double(dZi)];
    end
    fclose(fid);
%     flags = find(Num < 4);
%     dX(flags)=[];
%     dY(flags)=[];
%     dZ(flags)=[];
%     Num(flags)=[];
    h1 = figure;
    plot(dX,'b-')
    xlabel(['��Ԫ��'])
    ylabel('��λ(m)')
    legend('�춥ʪ�ӳ�')
    title(['�춥ʪ�ӳ�����ͼ'])
    grid on
     %������Լ��
    %resultVector = [dX(end - 10)];
    %ylim([resultVector-0.03  resultVector+0.03])
    
    
    h2 = figure
    plot(dY,'b-')
    xlabel(['��Ԫ��'])
    ylabel('��λ(m)')
    legend('���ջ��Ӳ�')
    title(['���ջ��Ӳ�����ͼ'])
    grid on
   
   % axis([0  3000 -5 0.5])
   %print(h,'-dpng',[fileNames(i).name '.png'])
    saveas(h1,['һ���춥ʪ�ӳٱ仯' '.fig'])
    saveas(h2,['һ����ջ��Ӳ�' '.fig'])
   %close(h1)
   %close(h2)
end
