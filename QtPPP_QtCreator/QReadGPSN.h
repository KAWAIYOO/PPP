#pragma once
#include "QGlobalDef.h"




class QReadGPSN
{
//��������
public:
	QReadGPSN(void);
	QReadGPSN(QString NFileName);
	~QReadGPSN(void);
	QVector< BrdData > getAllData();//��ȡ���й㲥�������ݵ�allBrdData
	void getSatPos(int PRN,char SatType,double CA,int Year,int Month,int Day,int Hours,int Minutes,double Seconds,double *pXYZ,double *pdXYZ);//PRN:���Ǻţ�SatType:��������(G,C,R,E),������ʱ����Ϊ�۲�ʱ��UTCʱ��(BDS��GLONASS�����ڲ��Զ�ת��)
private:
	void initVar();//��ʼ������
	void getHeadInf();//��ȡȥͷ����Ϣ
	void readNFileVer2(QVector< BrdData > &allBrdData);//��ȡRinex 2.X�㲥��������
	int SearchNFile(int PRN,char SatType,double GPSOTime);//��������ĵ�������
	double YMD2GPSTime(int Year,int Month,int Day,int Hours,int Minutes,int Seconds,int *WeekN = NULL);//YMD Change to GPST //GPSTimeArray[4]=GPSWeek  GPS_N�����ڵڼ��죩 GPST_second JD
	double getLeapSecond(int Year,int Month,int Day,int Hours=0,int Minutes=0,int Seconds=0);//��ȡ����
	double computeJD(int Year,int Month,int Day,int HoursInt,int Minutes = 0,double Seconds = 0.0);
	Vector3d GlonassFun(Vector3d Xt,Vector3d dXt,Vector3d ddX0);
	Vector3d RungeKuttaforGlonass(const BrdData &epochBrdData,double tk,double t0,Vector3d &dX);
//���ݲ���
public:

private:
	QFile m_readGPSNFile;//��ȡ�㲥�����ļ�
	QString m_NfileName;//����㲥�����ļ�����

	int m_BaseYear;//������ ����Ϊ2000
	double m_leapSec;//��������
	QVector< BrdData > m_allBrdData;
	bool isReadHead;//�ж��Ƿ��ȡͷ�ļ�
	bool isReadAllData;//�ж��Ƿ��ȡ��������
	QString tempLine;//����һ���ַ���
	int m_epochDataNum;//�洢һ�����ݶ�(GPS��BDS��28��7��GLONASS��12��3��)
//������ͷ�ļ����ݲ���
	//RINEX VERSION / TYPE
	double RinexVersion;
	char FileIdType;//G,C,R �ֱ����GPS,BDS,GLONASSϵͳ
	//PGM / RUN BY / DATE
	QString PGM;
	QString RUNBY;
	QString CreatFileDate;
	//COMMENT
	QString CommentInfo;
	//ION ALPHA
	double IonAlpha[4];
	double IonBeta[4];
	//DELTA-UTC: A0,A1,T,W
	double DeltaA01[2];
	int DeltaTW[2];
	//	IsReadHeadInfo
	bool IsReadHeadInfo;
};

