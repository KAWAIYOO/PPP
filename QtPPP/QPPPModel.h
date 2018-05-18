#ifndef QPPPMODEL_H
#define QPPPMODEL_H

#include "QCmpGPST.h"
#include "QReadOFile.h"
#include "QReadSP3.h"
#include "QReadClk.h"
#include "QTropDelay.h"
#include "QReadAnt.h"
#include "QWindUp.h"
#include "QTideEffect.h"
#include "QKalmanFilter.h"
#include "QWrite2File.h"

//ʹ��Engin����ͼ���
using namespace Eigen;

//ʹ���޵������PPPģ��



class QPPPModel:public QBaseObject
{
//��������
public:
	QPPPModel(void);
	QPPPModel(QString OFileName,QStringList Sp3FileNames,QStringList ClkFileNames,QString ErpFileName = "",QString BlqFileName = "OCEAN-GOT48.blq",QString AtxFileName = "antmod.atx",QString GrdFileName = "gpt2_5.grd");
	~QPPPModel();
	void Run();//��ȡ���������Լ��������������ں������ ����SatlitData
	void Run(QString pppFileName);;//��ȡ.ppp�ļ�����kalman�˲�
	//�����ļ�ϵͳ SystemStr:"G"(����GPSϵͳ);"GR":(����GPS+GLONASSϵͳ);"GRCE"(ȫ������)��
	bool setSatlitSys(QString SystemStr);//����GPS,GLONASS,BDS,Galieo�ֱ�ʹ�ã���ĸG,R,C,E
private:
	void initVar();
	void getSP3Pos(double GPST,int PRN,char SatType,double *p_XYZ,double *pdXYZ = NULL);//GPST���ǵ������� p_XYZ ����WGS84���� ���ٶ�
	void getCLKData(int PRN,char SatType,double GPST,double *pCLKT);//��ȡ��������� GPST ���Ƿ���ʱ��������
	void getSatEA(double X,double Y,double Z,double *approxRecvXYZ,double *EA);//����߶Ƚ�EA
	double getSagnac(double X,double Y,double Z,double *approxRecvXYZ);//�����Դ�
	double getRelativty(double *pSatXYZ,double *pRecXYZ,double *pSatdXYZ);//���������ЧӦ
	double getTropDelay(double MJD,int TDay,double E,double *pBLH,double *mf = NULL);//����������ӳ�.MJD:�������գ�TDay:����գ�E���߶Ƚǣ�rad�� pBLH���������ϵ��*mf��ͶӰ����
	bool getRecvOffset(double *EA,char SatType,double &L1Offset,double &L2Offset);//������ջ�L1��L2��λ���ĸ���PCO+PCV,EA���߶Ƚǣ���λ�ǣ���λrad����L1Offset��L2Offset�������߷���ľ������(��λm)
	double getSatlitOffset(int Year,int Month,int Day,int Hours,int Minutes,double Seconds,int PRN,char SatType,double *StaPos,double *RecPos);//��������PCO+PCV��������Ϊ���� G1��G2Ƶ��һ�������������θ�����һ���ģ�StaPos��RecPos�����Ǻͽ��ջ�WGS84���꣨��λm��
	double getTideEffect(int Year,int Month,int Day,int Hours,int Minutes,double Seconds,double *pXYZ,double *EA,double *psunpos=NULL,
					   double *pmoonpos = NULL,double gmst = 0,QString StationName = "");//���㳱ϫ�����߷���ĸ�������λm������̫����������gmst���ݿ��Լ�����Щ�����ظ����㣬StationName�ؿ��Դ���
	double getWindup(int Year,int Month,int Day,int Hours,int Minutes,double Seconds,double *StaPos,double *RecPos,double &phw,double *psunpos = NULL);//SatPos��RecPos�������Ǻͽ��ջ�WGS84���� �����ܣ���λ�ܣ���Χ[-0.5 +0.5]
	bool CycleSlip(const SatlitData &oneSatlitData,double *pLP);//����̽��//����pLP����ά���飬��һ����W-M��ϣ�N2-N1 < 5�� �ڶ����������в<0.3�� �������ǣ�lamt2*N2-lamt1*N1 < 5��
	double getPreEpochWindUp(QVector < SatlitData > &prevEpochSatlitData,int PRN,char SatType);//��ȡǰһ����Ԫ��WindUp û�з���0
	void getGoodSatlite(QVector < SatlitData > &prevEpochSatlitData,QVector < SatlitData > &epochSatlitData,double eleAngle = 5);//�õ������ߵ����ǰ��������� �߶Ƚ� �����Ƿ�ȱʧ C1-P2<50 ������ԪWindUp < 0.3 ;preEpochSatlitData:ǰһ����Ԫ�������� epochSatlitData����ǰ��Ԫ�������� (�Զ�ɾ������������);eleAngle:�߶Ƚ�
	void readPPPFile(QString pppFileName,QVector < QVector < SatlitData > > &allEpochData);//��ȡ�������֮���.ppp�ļ����Զ�������ļ��ĸ�ʽ��
//���ݲ���
public:
	//QVector< QVector < SatlitData > > g_AllSatlitData; //�洢������Ԫ�����Լ��������
private:
	QString m_OFileName;//O�ļ�·��+�ļ���
	QStringList m_Sp3FileNames;//SP3�ļ�·��+�ļ���
	QStringList m_ClkFileNames;//CLK�ļ�·��+�ļ���
	double m_ApproxRecPos[3];//��վ��������
	int multReadOFile;//ÿ�λ���O�ļ�����Ԫ���ݣ���ĿԽ��ռ���ڴ�Խ�ߣ��ٶ���Կ�Щ������Ĭ��1000��
	int m_leapSeconds;
	//����������ڼ�����������kalman�˲����ļ�����
	QCmpGPST qCmpGpsT;//���ڼ���GPSʱ�� �Լ�����ת���ȵĺ�����
	QReadSP3 m_ReadSP3Class;//���ڶ�ȡ�ͼ������ǹ��
	QReadClk m_ReadClkClass;//��ȡ�����Ӳ��ļ�
	QReadOFile m_ReadOFileClass;//��ȡO�ļ���
	QTropDelay m_ReadTropClass;//��ȡ��������Ҫ�ļ�
	QReadAnt m_ReadAntClass;//��ȡ����������
	QWindUp m_WinUpClass;//��λ���
	QTideEffect m_TideEffectClass;//��ϫӰ��
	QKalmanFilter m_KalmanClass;//kalman�˲�
	QWrite2File m_writeFileClass;//д���ļ���
};

#endif // QPPPMODEL_H
