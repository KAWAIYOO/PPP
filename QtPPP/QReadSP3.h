#ifndef QREADSP3_H
#define QREADSP3_H


#include "QGlobalDef.h"

/*
1���˴���Ҫ�Ż����㣺8���ֵ ����ҪƵ����ÿ�����ǲ�ֵ
2���������ڽ׶β���ȷ����
3��ͬʱ���ز�ֵʱ�����ǵ��ٶ�
4��ʾ��:

(1)��ĳ�ʼ��
QReadSP3 readSP3("D:\\igs15786.sp3");//����ָ��·��

(2)�ڵ�����֮ǰ��Ҫ��ȡ����һ�Σ�����Ҫ��ȡ���ݵ���getAllData();��ʱ�ϳ�
readSP3.getAllData();

(3)Ȼ�����getPrcisePoint(int PRN,double GPST,double *pXYZ,double *pdXYZ = NULL);
double pXYZ[3] ={0}��pdXYZ[3] = {0};//����������ٶ�
getPrcisePoint(32,514800,pXYZ,pdXYZ);//�õ�������ٶ�
����getPrcisePoint(32,514800,pXYZ);//�õ�����

5��ע�⣺
��1��������Զ�ȡ RINEX VERSION / TYPE��3.00 / C (��ǰֻ��ȡGPS���� �����޸Ŀ����Լ����޸�readFileData2Vec��readAllData����) 
��2������getPrcisePoint���鰴��GPSʱ�䣨GPST����С���� ���ò��ܡ����������ٶȡ����������ԡ�������밴��ʱ���С������ ��ACCELERATE ��Ϊ0
*/

#define  ACCELERATE  1 // 1�����ٳ���getPrcisePointʱ����С���� 0�������ٳ���ʱ������
//����ϵͳ����SP3�Դ�����ϵͳ
typedef struct _SP3Data
{//SP3�ļ������ݸ�ʽ
	int GPSWeek;//GPS��
	int GPSTime;//GPS������
	MatrixXd MatrixDataGPS;//GPSϵͳ��i�зֱ���PRN X,Y,Z
	MatrixXd MatrixDataGlonass;//Glonass
	MatrixXd MatrixDataBDS;//BDS
	MatrixXd MatrixDataGalieo;//Galieo
}SP3Data;

class QReadSP3: public QBaseObject
{
//��������
public:
	QReadSP3();
	QReadSP3(QString SP3FileName);//��ȡһ���ļ�
	QReadSP3(QStringList SP3FileNames);//��ȡ����ļ�����һ��
	~QReadSP3(void);
	QVector< SP3Data > getAllData();//��ȡ�������ݣ���ʱ��
	bool setSatlitSys(QString SystemStr);//����GPS,GLONASS,BDS,Galieo�ֱ�ʹ�ã���ĸG,R,C,E�����Ǹ��ࣩ
	void getPrcisePoint(int PRN,char SatType,double GPST,double *pXYZ,double *pdXYZ = NULL);//��þ�����������pXYZ �Լ��ٶ� pDXYZ(��ά)
	void releaseAllData();
private:
	void initVar();//��ʼ������
	void InitStruct();//��ʼ����Ҫ�õĽṹ���ڲ��ľ���(�н�Լ�ڴ�����)
	void openFiles(QString SP3FileName);//ֻ����ʽ���ļ�
	void readAllData2Vec();//��ȡ��ʼ�������ļ��������� ��m_allEpochData
	void readFileData2Vec(QStringList SP3FileNames);// ��ȡ����ļ����� ��m_allEpochData
	void get8Point(int PRN,char SatType,double *pX,double *pY,double *pZ,int *pGPST,double GPST);//pX,pY,pZ��8�������ꣻpGPST:8����GPS������;GPST���Ƿ���ʱ��������
	void lagrangeMethod(int PRN,char SatType,double GPST,double *pXYZ,double *pdXYZ = NULL);//�߽��������ղ�ֵ �˴�ѡȡǰ��---��8��������ֵ GPST���Ƿ���ʱ�������� ����pXYZ��sp3��ʽ��WGS84������; pdXYZsp3��ʽ��WGS84���ٶ�
	void readHeadData();//��ȡͷ�ļ���Ϣ
	int YMD2GPSTime(int Year,int Month,int Day,int Hours,int Minutes,int Seconds,int *GPSWeek = NULL);//����GPSʱ�䡣���� GPS�����룬GPSWeek:GPS��
//���ݲ���
public:
	
private:
	QFile m_readSP3FileClass;//��ȡ�ļ���
	QString m_SP3FileName;//���浥���ļ�����
	QStringList m_SP3FileNames;
	QVector< SP3Data > m_allEpochData;//ÿ����Ԫ�������ݡ�������ʱ���� ��i�зֱ���PRN X,Y,Z
	SP3Data epochData;//�洢ÿ����Ԫ����
	QString tempLine;//��ʱ��Ŷ�ȡ��һ������
	bool isReadHead;//�Ƿ��ȡͷ�ļ�
	bool isReadAllData;//�Ƿ��ȡ�����ļ�����
	bool IsSigalFile;//�Ƿ����ļ��򵥸��ļ�
	int m_WeekOrder;//��������ļ���Ŀ
	static const int lagrangeFact;
//�����Ż������Ӳ�ı��� ��ֹһֱ��0��ʼ����
	int m_lastGPSTimeFlag;//�����ϴ�get8Point��ȡ�ĵ�һ��GPSʱ������
	double m_lastCmpGPSTime;//�����ϴμ����GPST��ֹһ����Ԫ
	int m_EndHeadNum;//��һ�������һ���Ӳ��ļ���ȡ����
};

#endif
