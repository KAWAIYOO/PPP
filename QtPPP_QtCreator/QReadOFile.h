#ifndef QREADOFILE_H
#define QREADOFILE_H

#include "QGlobalDef.h"


/*
1����ȡ���ǹ۲��ļ����̳���QReadGPSO ���ȡ2.0�汾
*/

typedef struct  _obsVarNames
{//�汾3ʹ��
	int obsNum3ver;//�汾3ʹ��
	QString SatType;//G R C S E��ϵͳ����
	QVector< QString > obsNames3ver;//�汾3ʹ��
	int CPflags[20];//0-3�洢Li,Lj,Ci,Cj(�汾3.X��,����˭��ǰ����˭)4-7�洢��Ӧ��Ƶ�ʺ���i,j,i,j
}obsVarNamesVer3;


class QReadOFile:public QBaseObject
{
//��������
public:
	QReadOFile(void);
	~QReadOFile(void);
	QReadOFile(QString OfileName);
//��ȡ���ݵļ�����Ҫ����
	void getEpochData(QVector< SatlitData > &epochData);//��ȡһ����Ԫ���ݣ��ɸ��ݰ汾������չ��
	void getMultEpochData(QVector< QVector< SatlitData > >&multEpochData,int epochNum);//��ȡepochNum����Ԫ����(��ǰ���ļ��ײ����ܲ���epochNum��)
	void closeFile();//�����ȡ���ݻ��ȡ��������ô˺����ر��ļ�
	bool isEnd();//�ж��Ƿ񵽴��ļ�β����������
//��ȡͷ�ļ���Ϣ�ĺ���
	QString getComment();//��ȡͷ�ļ�ע����Ϣ
	void getApproXYZ(double* AppXYZ);//��ȡ��������
	void getAntHEN(double* m_AntHEM);//��ȡ���ߵ�HEN����
	void getFistObsTime(int* m_YMDHM,double &Seconds);//�����ʼ�۲���Ԫʱ��
	QString getMakerName();//������߱�־������
	double getInterval();//��ȡ�۲�������λs��
	QString getAntType();//��ȡ���ջ���������
	QString getReciveType();//��ȡ���ջ�����
private:
	void initVar();//��ʼ������
	void getHeadInf();//��ȡȥͷ����Ϣ
	void getLPFlag2();//��ȡ�ز���α������λ�ã��汾С��3ʹ�ã�
	void getLPFlag3();//��ȡ�ز���α������λ�ã��汾����3ʹ�ã�
	void readEpochVer3(QVector< SatlitData > &epochData);//��ȡ3.x�汾�ļ�
	void readEpochVer2(QVector< SatlitData > &epochData);//��ȡ2.x�汾�ļ�
	double getMJD(int Year,int Month,int Day,int HoursInt,int Minutes,int Seconds);//����Լ�������գ�����δ�õ���
	bool getOneSatlitData(QString &dataString,SatlitData &oneSatlite);//���ַ�ת�����У��������������ݣ���ȡ���ǲ������ز�����
	//debug:2017.07.08
	void getFrequencyVer3(SatlitData &oneSatlite);//��������PRN�����ͻ�ȡL1��L2��Ƶ��
	void getFrequencyVer2(SatlitData &oneSatlite);//��������PRN�����ͻ�ȡL1��L2��Ƶ��
	void getFrequency(SatlitData &oneSatlite);//��������PRN�����ͻ�ȡL1��L2��Ƶ��
//���ݲ���
public:
	
private:
//�ڲ�����
	QFile m_readOFileClass;//��ȡO�ļ���
	QString m_OfileName;//����O�ļ�����

	bool isReadHead;
	QString tempLine;//һ���ַ�������
	int CPflags[20];//�洢L1,L2,C1,P2,P1(�汾2.X��)
	int CPflagVer3[5][20];//5�зֱ�洢��GPS��GLONASS��BDS��SBAS��Galieoϵͳ�������ز�λ��,�Լ�Ƶ��
	QVector< obsVarNamesVer3 > m_obsVarNamesVer3;//�汾3�洢SYS / # / OBS TYPES
	QRegExp matchHead;//�汾2.X��ƥ��ͷ�ļ�����ʼ��һ�μ��ɣ�
	int baseYear;//�汾2.X ���ڻ�ȡ������ �����磺1989 ��baseYear = 1900;2010 ��baseYear = 2000��
//ͷ�ļ���Ϣ
	//RINEX VERSION / TYPE
	double RinexVersion;
	QChar FileIdType;
	QChar SatelliteSys;
	//PGM / RUN BY / DATE
	QString PGM;
	QString RUNBY;
	QString CreatFileDate;
	//COMMENT
	QString CommentInfo;
	//MARKER NAME
	QString MarkerName;
	//MARKER NUMBER
	QString MarkerNumber;
	//OBSERVER / AGENCY
	QString ObserverNames;
	QString Agency;
	//REC # / TYPE / VERS
	QString ReciverREC;
	QString ReciverType;
	QString ReciverVers;
	//ANT # / TYPE
	QString AntNumber;
	QString AntType;
	//APPROX POSITION XYZ
	double ApproxXYZ[3];
	//ANTENNA: DELTA H/E/N
	double AntHEN[3];
	//WAVELENGTH FACT L1/2
	int FactL12[2];
	//# / TYPES OF OBSERV 
	int TypeObservNum;// �汾С��3ʹ��
	QVector<QString> ObservVarsNames;
	//INTERVAL
	double IntervalSeconds;
	//TIME OF FIRST OBS
	int YMDHM[5];
	double ObsSeconds;
	QString SateSystemOTime;//GPS UTC
};

#endif

