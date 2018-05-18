#ifndef QREADCLK_H
#define QREADCLK_H


#include "QGlobalDef.h"

/*

1��˵���������������ղ�ֵ��ȡ��������30s�Ӳ��Զ�����2�����ֵ������ʱ������8���ֵ
����޷���ȡ�������ݣ������Ϊ0

2��ʾ������:
(1)��ĳ�ʼ��
QReadClk readClk("D:\\igs15786.clk");//����ָ��·��

(2)�ڵ�����֮ǰ��Ҫ��ȡ����һ�Σ�����Ҫ��ȡ���ݵ���getAllData();��ʱ�ϳ�
readClk.getAllData();

(3)Ȼ�����getStaliteClk(int PRN,double GPST,double *pCLKT);
double GPST = 518400,SatClock = 0;//����GPS������GPST,�Լ�Ҫ������Ӳ�
readClk.getStaliteClk(32,GPST,&SatClock);//��������Ӳ��ӳٱ��浽SatClock

3��ע�⣺
(1)������Զ�ȡ RINEX VERSION / TYPE��3.00 / C (��ǰֻ��ȡGPS���� �����޸Ŀ����Լ����޸�readFileData2Vec��readAllData����)                                      
(2)����getStaliteClk���鰴��GPSʱ�䣨GPST����С���� ���ò��ܡ����������ٶȡ����������ԡ�������밴��ʱ���С������ ��ACCELERATE ��Ϊ0


*/

#define  ACCELERATE  1 // 1�����ٳ���ʱ����С���� 0�������ٳ���ʱ������

typedef struct _CLKData
{//Clk�ļ������ݸ�ʽ
	int GPSWeek;
	double GPSTime;//GPS������
	MatrixXd MatrixDataGPS;//GPSϵͳ��i�зֱ���PRN X,Y,Z
	MatrixXd MatrixDataGlonass;//Glonass
	MatrixXd MatrixDataBDS;//BDS
	MatrixXd MatrixDataGalieo;//Galieo
}CLKData;

class QReadClk:public QBaseObject
{
//��������
public:
	QReadClk();
	QReadClk(QString ClkFileName);
	QReadClk(QStringList ClkFileNames);
	~QReadClk(void);
	QVector< CLKData > getAllData();//�����������
	bool setSatlitSys(QString SystemStr);//����GPS,GLONASS,BDS,Galieo�ֱ�ʹ�ã���ĸG,R,C,E�����Ǹ��ࣩ
	void releaseAllData();//���꼰ʱ�ͷ��ڴ�
	void getStaliteClk(int PRN,char SatType,double GPST,double *pCLKT);//��ȡ��������� GPST ���Ƿ���ʱ��������
private:
	void initVar();//��ʼ������
	void InitStruct();//��ʼ����Ҫ�õĽṹ���ڲ��ľ���(�н�Լ�ڴ�����)
	void openFiles(QString ClkFileName);//ֻ����ʽ���ļ�
	void readAllHead();//��ȡͷ�ļ���Ϣ
	void readAllData();//��ȡ�����ļ����������
	void readFileData2Vec(QStringList ClkFileNames);// ��ȡ����ļ����� ��m_allEpochData
	void get8Point(int PRN,char SatType,double *pCLKT,double *pGPST,double GPST);//pCLKT��8�������ꣻpGPST:8����GPS������;GPST���Ƿ���ʱ��������
	void lagrangeMethod(int PRN,char SatType,double GPST,double *pCLKT);//�߽��������ղ�ֵ �˴�ѡȡǰ��---��8��������ֵ GPST���Ƿ���ʱ��������
	double YMD2GPSTime(int Year,int Month,int Day,int Hours,int Minutes,double Seconds,int *GPSWeek = NULL);//YMD Change to GPST //GPSTimeArray[4]=GPSWeek  GPS_N�����ڵڼ��죩 GPST_second JD
//��������
public:

private:
	QFile m_readCLKFileClass;//��ȡ.clk�ļ���
	QVector< CLKData > m_allEpochData;
	CLKData epochData;
	QString m_ClkFileName;
	QStringList m_ClkFileNames;
	QString tempLine;
	int m_WeekOrder;//��������ļ���Ŀ
	bool IsSigalFile;//�Ƿ����ļ��򵥸��ļ�
	bool isReadHead;//�Ƿ��ȡͷ�ļ�
	bool isReadAllData;//�Ƿ��ȡ�����ļ�����
	int lagrangeFact;//�ж�clk�ļ���30s����5min 30s�Ϳ��Լ򵥲�ֵlagrangeFact = 2;5min lagrangeFact = 8
//�����Ż������Ӳ�ı��� ��ֹһֱ��0��ʼ����
	int m_lastGPSTimeFlag;//�����ϴ�get8Point��ȡ�ĵ�һ��GPSʱ������
	double m_lastCmpGPSTime;//�����ϴμ����GPST��ֹһ����Ԫ
	int m_EndHeadNum;//��һ�������һ���Ӳ��ļ���ȡ����
	MatrixXd *pSysChMat;//ѡ��ϵͳָ��
	
};

#endif

