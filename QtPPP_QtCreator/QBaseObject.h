#ifndef QBASEOBJECT_H
#define QBASEOBJECT_H

#include <QString>
#include <QDebug>
/*
1��˵����
QString tempLine = "G";
char tempSatType = '0';
tempSatType = *(tempLine.mid(0,1).toLatin1().data());//����QString ת���� char

*/


class QBaseObject
{
//��������
public:
	QBaseObject(void);
	~QBaseObject(void);
	//�����ļ�ϵͳ SystemStr:"G"(����GPSϵͳ);"GR":(����GPS+GLONASSϵͳ);"GRCE"(ȫ������)��
	bool setSatlitSys(QString SystemStr);//����GPS,GLONASS,BDS,Galieo�ֱ�ʹ�ã���ĸG,R,C,E
	bool isInSystem(char Sys);//Sys:G R C(����GPS��GLONASS��BDSϵͳ)�ж��Ƿ���Ҫ��ϵͳ����
	int getSystemnum();//��ȡ��ǰ���趨�˼���ϵͳ
private:
	void inintVar();//��ʼ������(Ĭ�Ͽ���GPS)
	int m_SystemNum;
//���ݲ���
protected:
	//����ϵͳ�ж�
	bool IsaddGPS;//�Ƿ����GPS(Ĭ�Ͽ���)
	bool IsaddGLOSS;//�Ƿ����GLONASS
	bool IsaddBDS;//�Ƿ���뱱��
	bool IsaddGalieo;//�Ƿ����Galieo
private:
};

#endif
