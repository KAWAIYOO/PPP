 clc;clear all;
x=[1 2 3 4 5];%������
y1=[10 20 30 40 50];%������1�¶�
y2=[500 400 300 200 100];%������2ѹ��
hold on;
[ax,h1,h2]=plotyy(x,y1,x,y2);
set(get(ax(1),'Ylabel'),'string','Temperature, \circC');
set(get(ax(2),'Ylabel'),'string','Pressure, GPa');
xlabel('feed ,\mum');
hold off;