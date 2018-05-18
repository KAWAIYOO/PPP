#ifndef QTIDEEFFECT_H
#define QTIDEEFFECT_H

#include "QCmpGPST.h"
/*
Ŀǰӵ�еĳ�ϫ����Ϊ��
1����������Ҫ����erp�ļ���ʹ��RTKLIB C���Ա�׼���ȡ����erp�ļ�
2�����峱������2��3�׳���Ƶ���������Լ�Ĭ�ϲ��������ñ���
3������ϫ��Ҫ���á���վ��������4���ַ�������ʹ��getAllTideEffectENU��getAllTideEffect���������վ����

4��ʾ����
��1�����ʼ����
QString OCEANFileName = "",QString erpFileName = "";//û�п���Ϊ�գ�������ڵ�ǰִ��Ŀ¼�����Զ�����
QString StationName = "BJFS";//�����վ���� �����������ݣ�4���ֽڣ�

QTideEffect tideEffect(OCEANFileName,erpFileName);
tideEffect.setStationName(StationName);//���ú���ϫ �������ò�վ����
tideEffect.getAllData();//ѡ���ȡ����

��2���������÷�����
//�������г�ϫ����
//pXYZ����վ���� EA:�߶ȽǺͷ�λ�ǣ�psunpos̫������ pmoonpos���������꣨������ټ��㣬û�п��Բ�����gmst��ƽ��������ʱ��
double pXYZ[3]={99999,999999,99999},EA[2] = {0.9,0.9},psunpos[3] = {99999,9999,9999},pmoonpos[3] = {99999,9999,9999},gmst = 0;
QString StationName = "BJFS";//�����վ���� �����������ݣ�4���ֽڣ�
doubel result = 0;//��ϫ�����߷����Ӱ����루��λm��
result = tideEffect.getAllTideEffect(Year,Month,Day,Hours,Minutes,Seconds,pXYZ,EA,psunpos,pmoonpos,gmst,StationName);

double pENU[3] = {0};//���泱ϫ��ENU�����Ӱ��
tideEffect.getAllTideEffectENU(Year,Month,Day,Hours,Minutes,Seconds,pXYZ,pENU,psunpos,pmoonpos,gmst,StationName);



��ע��ʹ��getAllTideEffect��������̫����������gmst��ֵ���Բ����ظ�����̫���������꣨Ҳ����ʵʱʹ��setSunMoonPos�����������Լ��ټ�������
	  ��Щ��ֵ���Դ�QReadAnt���ȡ����ͨ�������������߸����Ϳ��Եĵ��������ݡ�
*/

#include <QFile>

typedef struct _OCEANData
{
	QString StationName;//��дվ��
	double amp[3][11];
	double phasedats[3][11];
	bool isRead;//�ж϶�ȡ�����Ƿ���ȷ�����ܶ�ȡʧ�ܣ�true��ȷ��flaseʧ��
}OCEANData;

//��ϫ������
class QTideEffect
{
//��������
public:
	QTideEffect(QString OCEANFileName = "",QString erpFileName = "");//���뺣������,erp�ļ�·��������ʹӵ�ǰĿ¼���� ����Ͳ����ú���ϫ�ͼ�������
	~QTideEffect(void);
	//��ȡ���߷��� ת�����������
	void getAllTideEffectENU(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,double *pXYZ,double *pENU,double *psunpos=NULL,
		double *pmoonpos = NULL,double gmst = 0,QString StationName = "");//������еĺ���Ӱ���ENU�����Ӱ��
	//��ȡENU����ĸ���
	double getAllTideEffect(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,double *pXYZ,double *EA,double *psunpos=NULL,
		double *pmoonpos = NULL,double gmst = 0,QString StationName = "");//������еĺ��������߷����Ӱ��pXYZ:��վWGS84����ϵ��EA�������ߵĸ߶ȽǺͷ�λ��(����)����ά����EA[2]
	//������ȡ���������峱������ENU����
	void getPoleTide(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,double *pBLH,double *pTideENU);//����۲�ʱ��������ʱ���룬pXYZ��IΪ��վWGS84����,�����վXYZ����*pTideXYZ��O����
	void getSoildTide(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,double *pXYZ,double *pTideENU,bool isElimate = false);//����۲�ʱ��������ʱ���룬pXYZ��IΪ��վWGS84����,�����վXYZ����*pTideXYZ��O���� isElimate:�Ƿ������ñ��Σ����ñ���ģ��ò�Ʋ���ȷ���޴��Ӱ�죬��ѡ
	void getOCEANTide(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,double *pXYZ,double *pTideENU,QString StationName = "");//���㺣��Ӱ�� StationName:��վ���֣����磺bjfs��BJFS��, StationName Ϊ�վ�ȥ��ǰĿ¼����������ʹ�ú�������
	//��ȡ����֮ǰ��Ҫ���ò�վ����
	void setStationName(QString StationName = "");//��ʼ��֮ǰ�����롱���ò�վ����
	void getAllData();
	//��ȡ�����ļ�����Ҫ��վ���֣����û���ļ����ֵ�ǰĿ¼��������.blq�ļ�
	bool readOCEANFile(QString  StationName,OCEANData &oceaData,QString  OCEANFileName = "");// StationName:��վ���֣����磺bjfs��BJFS����ʹ��QFile��ȡ��ֻ�ܶ�ȡIGSվ������ ��������һ�㲻���Զ�ȡ��Ҳ���԰ѽ��ջ������滻Ϊ�ٽ���վ��OCEANFileNameΪ�վ�ʹ�����ʼ���ļ��������������ǰĿ¼.dat�ļ�������ʹ�ú���ϫ
	void setSunMoonPos(double *psun,double *pmoon,double gmst=0);//����-�������꣨��getAllTideEffectENU������������һ�����ټ��㣩
private:
	void initVar();
	bool readRepFile();//����erp�ļ��Ͷ�ȡ
	bool getErpV(gtime_t obsGPST,double *erpV);//��ȡerp�ļ����ݵ�˽�б���
	void tide_pole(const double *pos, const double *erpv, double *denu);
	void subSolidTide(double *sunmoonPos,double *pXYZ,double *pTideXYZ,int flag);//flag:0�������������1����̫��
//��������
public:

private:
	QString m_erpFileName;
	erp_t m_erpData;
	QCmpGPST m_cmpClass;//��Ҫ��QCmpGPST�������㺯��
	bool isOCEANTide;
	bool isPoleEffect;//�Ƿ�ʹ�ü��� Ĭ��true
	bool isSolidTide;//�Ƿ�ʹ�ù��峱 Ĭ��true
	//���峱��Ҫ����
	double m_GMi[3];//�ֱ�洢�˵���������̫����������
	double loveShida2[2];//���ι��峱love��Shadi������ʼ������
	bool isgetLoveShida2;//�Ƿ�õ����ι��峱love��Shadi����
	double loveShida3[2];//���ι��峱love��Shadi����
	double m_SationBLH[3];//��վBLH
	double m_SecondFlag;//������Ԫ�룬��ֹ��μ��㣬���ټ�����
	double m_AllTideENU[3];//���汾��ԪENU����ֹ��μ��㣬���ټ�����
	int m_epochFlag;//�洢��Ԫ�������,����50����Ԫ���վ������Ծ�ȷ�������ظ�����Ͳ�վ�����йصļ�����
	int m_MaxCompution;//epochFlag ������ Ĭ��50���ڹ��캯��
	double m_pSolidENU[3];//�洢����Ĺ��峱
	double m_pPoleENU[3];//�洢����ļ���
	double m_pOCEANENU[3];//�洢����ĺ���
	double m_erpV[5];//�洢��һ����Ԫ�ļ��Ʋ���
	bool isReadErp;//�ж��Ƿ��ȡERP�ļ�(ֻ��һ��,��ʹʧ��Ҳ��true�����ڶ�ȡ����Ӧ�ĳ�ϫ��رգ�û��Ӱ��)
	bool isReadOCEAN;//�ж��Ƿ��ȡ��OCEAN����(ֻ��һ��,��ʹʧ��Ҳ��true�����ڶ�ȡ����Ӧ�ĳ�ϫ��رգ�û��Ӱ��)
	//��ȡ����������
	QString m_OCEANFileName;//���������ļ�����
	QFile m_readOCEANClass;
	OCEANData m_OCEANData;//�����վ����
	QString m_StationName;
	static const double args[][5];
	double LeapSeconds;//��ǰ��ݵ����룬����������ת��UTC
	//����̫����������gmst��ֵ���Բ����ظ�����̫����������
	double m_sunpos[3];
	double m_moonpos[3];
	double m_gmst;
	bool isGetPos;//�ж��Ƿ��ȡ��������
};

#endif