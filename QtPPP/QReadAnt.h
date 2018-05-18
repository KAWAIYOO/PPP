#ifndef QREADANT_H
#define QREADANT_H



#include "QCmpGPST.h"


/*����˵��
��ʼ����ʱ������޷���"���ջ�"�������ͺ͹۲�"��ʼ"ʱ��������obvJD���룬�������setObsJD��������
�������ͺ������� �����޷�ƥ��������ʼ��Чʱ�����ߵ�����
1����ĳ�ʼ��
QString AntFileName = "D:\\antmod.atx",AntType = "ASH700936B_M    SNOW";//AntFileName:�����ļ�·��������Ϊ�ջ�ȥ��ǰĿ¼��"antmod.atx"�� AntType:O�ļ����۲��ļ�����ȡ����������
double ObsJD = 2455296.50;//�۲���ʼʱ�̣���Լʱ�̣�������
QReadAnt readAnt(AntFileName,AntType,ObsJD);
����
QReadAnt readAnt(AntFileName);
readAnt.setObsJD(AntType,ObsJD);
2�������ļ��Ķ�ȡ
�ڳ�ʼ���ط�ֻ��ȡһ�����ݣ���ʱ��
readAnt.getAllData();//��ȡ���Ǻͽ��ջ���������
3���������÷���
double E = 0.7,A = 0.3;//�߶ȽǺͷ�λ��
double L1Offset = 0, L2Offset = 0;//������ջ�L1��L2���߷����������λm��
readAnt.getRecvL12(E,A,&L1Offset,&L2Offset);//������ջ�L1��L2���߷����������λm��

int Year = 2010,Month =  4,Day = 10,Hours = 0,Minuts = 0,PRN = 32;
double Seconds = 0.0;
double StaPos[3] = {9999,999,9999},RecPos[3] = {9999,9999,9999};
double L12OffSet = 0;
gL12OffSet = readAnt.getSatOffSet(Year,Month,Day,Hours,Minuts,Seconds,PRN,StaPos,RecPos);//ʱ�������Ƿ���ʱ��UTC������ʱ���룬StaPos:����XYZ���꣬RecPos:���ջ�XYZ���꣬SatAntH:�������߸��� (���ڸ�����������Ƶ�ʵ�PCO��PCV��ͬ����ֻ�践��һ���������뼴��)����m

*/

//����洢�������ݽṹ �˴�ֻ���泣�õ�����,��ȡ�������ݻ�����Ĵ���
typedef struct _FrqunceData
{//����һ��Ƶ�ε�PCO��PCV���ݸ�ʽ
	QString FrqFlag;//START OF FREQUENCY
	double PCO[3];
	//PCV ��������
	short int Hang;//PCVAZI �и���
	short int Lie;//PCVAZI��PCVNoAZI �и���
	QVector< double > PCVNoAZI;
	QVector< double > PCVAZI;//����ת��Ϊ1ά����
}FrqData;

typedef struct _AntDataType
{
	QString StrAntType;//��������
	QString SatliCNN;//CNN ���Ǳ�� G,R,E...
	double DAZI;//�洢��λ�Ǽ�� 360/DAZI
	double ZEN_12N[3];//�洢�߶Ƚ����估���  (ZEN_12N[1] - ZEN_12N[0])/ZEN_12N[2]
	short NumFrqu;//���ǵ�Ƶ�θ���
	double ValidJD;//��Ч��ʼʱ��
	double EndJD;//��Ч����ʱ��
	QVector <FrqData> PCOPCV;//�洢���Ƶ�ε�PCO��PCV����
	bool IsSat;//true �������� false ������ջ�����
	bool isNan;//���������������ļ�δ�ҵ������false;//��ʼ����������ҵ�����Ϊtrue
}AntDataType;


class QReadAnt:public QBaseObject
{
//��������
public:
	QReadAnt(QString AntFileName = "",QString AnTypeName = "",double ObvJD = 0);
	~QReadAnt(void);
	bool getRecvL12(double E,double A,char SatType,double &L1Offset,double &L2Offset);//E:���Ǹ߶Ƚ�;A:���Ƿ�λ�ǣ���λ���ȣ���L1Offset��L2Offset����������������λm��
	double getSatOffSet(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,int PRN,char SatType,double *StaPos,double *RecPos);//ʱ�������Ƿ���ʱ��UTC������ʱ���룬StaPos:����XYZ���꣬RecPos:���ջ�XYZ���꣬SatAntH:�������߸��� (���ڸ�����������Ƶ�ʵ�PCO��PCV��ͬ����ֻ�践��һ���������뼴��)����m
	void setObsJD(QString AnTypeName,double ObsJD);//�������ù۲��ļ�������
	bool getAllData();//��ȡ��Ч�������������ݺͽ��ջ���������
private:
	void initVar();//��ʼ������
	bool openFiles(QString AntTypeName);//���ļ�
	bool readFileAntHead();//��ȡͷ�ļ�
	bool readAntFile(QString AntTypeName);//��ȡ�����ļ�
	bool readSatliData();//��ȡ������Ҫ�õ�����
	bool readRecvData();//��ȡ������Ҫ���ļ�
	double computeJD(int Year,int Month,int Day,int HoursInt=0,int Minutes=0,int Seconds=0);//����������
	bool getPCV(const AntDataType &tempAntData,char SatType,double *PCV12,double Ztop,double AZI = 0);//����PCV;Z:�����춨�� = pi/2 - �߶Ƚ�;A:���Ƿ�λ�ǣ���λ���ȣ�
//��������
public:
	double m_sunpos[3],m_moonpos[3],m_gmst;//ÿ����Ԫ����һ��Ϊ���������ṩ���꣬�����ظ�����
private:
	QString m_AntFileName;//�������������ļ�����
	QString m_AntType;//������ջ�������������
	QFile m_ReadFileClass;//��ȡ�ļ���
	bool isReadAllData;//�Ƿ��ȡ�������ݱ�־
	bool isReadHead;//�Ƿ��ȡͷ�ļ�
	bool isReadSatData;//�Ƿ��ȡ������������
	bool isReadRecvData;//�Ƿ��ȡ���ջ���������
	QString m_tempLine;//�����ȡ��һ������
	double m_ObeservTime;//�洢�۲�O�ļ��������� ƥ������������Чʱ��
	AntDataType m_RecvData;//������ջ�����PCO��PCV����
	QVector< AntDataType > m_SatData;//��������������PCO��PCV����
	double m_pi;//Բ���� ����
	QCmpGPST m_CmpClass;//���㺯����
	double m_sunSecFlag;//����ο���Ԫ�仯����Ҫ���¼���̫������洢�ο���Ԫ����
};

#endif