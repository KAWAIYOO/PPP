#ifndef QWRITE2FILE_H
#define QWRITE2FILE_H



#include "QCmpGPST.h"


class QWrite2File
{
//��������
public:
	QWrite2File(void);
	~QWrite2File(void);
	bool writeRecivePos2Txt(QString fileName);//����ENU����Ľ��д��txt
	bool writePPP2Txt(QString fileName);//����������д��.ppp�ļ�
	bool writeClockZTDW2Txt(QString fileName);//���춥ʪ�ӳٺ��Ӳ�д��txt ��һ��ʪ�ӳٵڶ����Ӳ�
	bool writeAmbiguity2Txt();//������allAmbiguity�洢������д��txt�ļ���������Ϊ"G32.txt","C02.txt","R08.txt"����ʽ����һ��ģ����
	bool WriteEpochPRN(QString fileName);//������allAmbiguity�洢������д�뵽�ļ�����һ����Ԫ�����ڶ������Ǳ��
private:
	bool WriteAmbPRN(int PRN,char SatType);//д��PRN�����ǵ�ģ����
//��������
public:
	QVector< RecivePos > allReciverPos;//����ENU����Ľ��д��txt
	QVector< QVector < SatlitData > > allPPPSatlitData;//����ppp���������д��.ppp�ļ�
	QVector< Ambiguity> allAmbiguity;//�洢���Ƕ�Ӧ��ģ����
	QVector< ClockData > allClock;//�洢һ��
private:
	QCmpGPST m_qcmpClass;//�����������

};

#endif

