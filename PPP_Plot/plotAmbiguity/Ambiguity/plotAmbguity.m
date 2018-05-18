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
        dNumi = linestr(39:40);
        Num = [Num str2double(dNumi)];
        dXi = linestr(1:10);
        %dXi = linestr(13:28);
        dYi = linestr(43:58);
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
    plot(dX,dY,'b--.')
    legend('ģ������ֵ')
    title([fileNames(i).name '  ģ���ȱ仯����,һ�칲�۲� '  num2str(length(dY)) '��'])
    grid on
    xlabel(['��Ԫ��Ŀ'])
    ylabel('ģ����(��)')
    
    xlim([0 3000])
   %axis([0  3000 ])
   %print(h,'-',[fileNames(i).name '.png'])
   saveas(h1,[fileNames(i).name '.fig'])
   %close(h1)
end
