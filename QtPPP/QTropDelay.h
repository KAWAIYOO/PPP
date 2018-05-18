#ifndef QTROPDELAY_H
#define QTROPDELAY_H

#include "QGlobalDef.h"

/*
1��˵����
���������GPT2 UNB3Mģ�� �Լ���ȡGPT2��GPT�����ķ�����
������ģ�ͣ�Sassģ�͡�Hofieldģ�͡�
������ͶӰ����ģ�ͣ�Neill��VMF1,GMF
#ifdef EIGEN_CORE_H (Eigen��ı�־��û��Eigen�Զ�����UNB3Mģ��)
.........
#endif

2��ʾ����
��1����ĳ�ʼ����
QTropDelay tropDelay;//��ʹ��GPT2��ص�ģ�ͣ�ֻ����UNB3Mģ�ͣ�
QTropDelay tropDelay("D:\\gpt2_5.grd","Neil","GPT2"),ʹ��GPT2��ص�ģ�ͱ�����gpt2_5.grd�ļ�·���������������Բ�ѡĬ��Neil��UNB3M

ʹ��GPT2�Լ���֮��ص�Sass��Hofildģ��֮ǰ��Ҫ��ȡ��������
tropDelay.getAllData();

��2��GPT2+Sassģ�ͣ�Ĭ��ͶӰ����Neil��ʼ��ʱ���������VMF1ͶӰ����
double MJD = 852221.0,E = 0.72,pBLH[3] ={0.3,1.5,88.3};//Լ��������,�߶Ƚǣ�rad��,BLH��B���ȣ�rad����BLH��Lγ�ȣ�rad����BLH��H��m��
int TDay = 132;//�����
double TropDelay = 0,mf = 0;//��������ӳ٣�m����ͶӰ������m��
TropDelay = tropDelay.getGPT2SasstaMDelay(MJD,TDay,E,pBLH,&mf);//GPT2+Sass��������TropDelay��mf����

��3��UNB3Mģ�ͣ�ͶӰ����Neil��
int TDay = 132;//�����
double pBLH[3] ={0.3,1.5,88.3},E = 0.72;//BLH��rad,rad,m���� ���Ǹ߶Ƚ�E��rad��
double TropDelay = 0,mf = 0;//��������ӳ٣�m����ͶӰ������m��
TropDelay = tropDelay.getUNB3mDelay(pBLH, TDay, E,&mf);

��4��GPT������ȡ��
double MJD = 852221.0,Lat = 0.3,Lon = 1.5,Hell = 88.3;//MJDԼ��������,Lat��BLH��B���ȣ�rad����Lon��BLH��Lγ�ȣ�rad����Hell��BLH��H��m��
GPT2Result tempGPT2;//������
tempGPT2 = tropDelay.getGPT2Model(MJD,Lat,Lon,Hell);//����GPT2 ��GPT2Result�ṹ�屣��

double GPTresult[3] = {0};//�洢�� ��ѹ��hPa�� �¶ȣ�C�� ����߲�ֵ��Geoid undulation����m��(ǰ��������)
tropDelay.getGPTModel(MJD,Lat,Lon,Hell��GPTresult);//����GPT�� GPTresult��������
��5��������ͶӰ����
double MJD = 852221.0,pBLH[3] ={0.3,1.5,88.3},E = 0.72;
double md = 0,mw = 0;//����(md)��ʪ��(mw)ͶӰ����
tropDelay.getGMFParm(MJD,pBLH,E,&md,&mw);//����GMFͶӰ������md��mw

GPT2Result tempGPT2;//������
int TDay = 132;//�����
tempGPT2 = tropDelay.getGPT2Model(MJD,pBLH[0],pBLH[1],pBLH[2]);
getVMF1Parm(tempGPT2.ah,tempGPT2.aw,E,pBLH[0],pBLH[2],TDay,&md,&mw);//����VMF1ͶӰ������md��mw

tropDelay.getNeilParm(E,pBLH[2],pBLH[0],TDay,&md,&mw);//����NeilͶӰ������md��mw

3��ע�⣺���෵�صĶ������ӳ�ֻ�Ǹ��ӳ٣�������òο���Ӧ�����ġ�β�������и�̾��ע�͵ط����м򵥸��ģ�
4���ο���վ��http://ggosatm.hg.tuwien.ac.at/DELAY/SOURCE/ ������matlab�Լ����ݣ�
*/

#define d2r  (Pi/180.0)
#define r2d  (180.0/Pi)

#define MIN(x,y)    ((x)<(y)?(x):(y))
#define MAX(x,y)    ((x)>(y)?(x):(y))
#define SQRT(x)     ((x)<=0.0?0.0:sqrt(x))

typedef struct _GPT2Result
{//����GPT2�Ľ��
	double p;//pressure in hPa
	double T;//temperature in degrees
	double dT;//temperature lapse rate in degrees per km
	double e;// water vapour pressure in hPa
	double ah;//hydrostatic mapping function coefficient at zero height (VMF1)
	double aw;//wet mapping function coefficient (VMF1)
	double undu;//geoid undulation in m
}GPT2Result;

typedef struct _GrdFileVar
{//�洢GRD�ļ��������ڼ���
	double lat;//latitude
	double lon;//lontitude
	double pgrid[5];//pressure in Pascal
	double Tgrid[5];// temperature in Kelvin
	double Qgrid[5];//specific humidity in kg/kg
	double dTgrid[5];//temperature lapse rate in Kelvin/m
	double u;//geoid undulation in m
	double Hs;//orthometric grid height in m
	double ahgrid[5];//hydrostatic mapping function coefficient, dimensionless
	double awgrid[5];// wet mapping function coefficient, dimensionless
}GrdFileVar;

class QTropDelay
{
//��������
public:
	QTropDelay(QString GrdFileName = "",QString ProjectionFun = "Neil",QString Model = "UNB3M");//��ȡGPT2ģ�͵������ļ� GrdFileName�������㺯��ProjectionFun = "VMF1" �� "Neil";Model����ѡ��GPT ���� GPT2ģ�ͣ���Ҫ��ȡ.grd�ļ����ݣ���ȡʧ���Զ�ѡ��GPT����
	~QTropDelay(void);
	//��������������GPT2���ƵĶ�����ģ��
	double getGPT2HopfieldDelay(double MJD,int TDay,double E,double *pBLH,double *mf = NULL);//��ȡGPT2+Hopfield+VMF1���춥ZHD+ZWD���ӳ� MJD:�������� T����� Lat,lon,Hell����γ�ȣ���վ�� T:����� E�߶Ƚǣ�rad��,*mf:ͶӰ����
	double getGPT2SasstaMDelay(double MJD,int TDay,double E,double *pBLH,double *mf = NULL);//��ȡGPT2+Hopfield+VMF1���춥ZHD+ZWD���ӳ� MJD:�������� T����� Lat,lon,Hell����γ�ȣ���վ�� T:����� E�߶Ƚǣ�rad��,*mf:ͶӰ����
	//���溯����UNB3���ƵĶ�����ģ��
#ifdef EIGEN_CORE_H
	double getUNB3mDelay(double *pBLH, double TDay, double E,double *mf = NULL);
#endif
	//�����ǻ��GPT2��GPT�������� 
	GPT2Result getGPT2Model(double dmjd,double dlat,double dlon,double hell,double it = 0);//ʹ��GPT2����� ��ϸ����������� it��1��ʾ��ʱ��仯 0����ʱ��仯
	void getGPTModel(double dmjd,double dlat,double dlon,double hell,double *GPTdata);//ʹ��GPT����� GPTdata[3]�洢�� ��ѹ��hPa�� �¶ȣ�C�� ����߲�ֵ��Geoid undulation����m��
	//�����ǻ�ȡͶӰ��������
	void getVMF1Parm(double ah,double aw,double E,double Lat,double H,int TDay,double &md,double &mw);//����VMF1����(md)��ʪ��(mw)ͶӰ���� EΪ�߶Ƚǣ�rad��HΪ���ߣ�m��TΪ����� LatΪ���γ��(rad ��γ������ ��γС����) aw bw����GPT2 model����
	void getNeilParm(double E,double H,double Lat,int TDay,double &md,double &mw);//����Neil����(md)��ʪ��(mw)ͶӰ���� EΪ�߶Ƚǣ�rad��HΪ���ߣ�m��phΪ���γ��(rad) T����� 
	void getGMFParm(double MJD,double *pBLH,double E,double &md,double &mw);//MJD��Լ�������գ�pBLH�������ϵ(rad,rad,m), EΪ�߶Ƚǣ�rad�� ����(md)��ʪ��(mw)ͶӰ���� 
	//�����Sassstaģ��(�춥����)
	double getSassDelay(double &ZHD,double &ZWD,double B, double H,double E);
	//�����Hopfieldģ��(���߷���)
	double getHopfieldDelay(double &SD,double &SW, double H,double E);//���þ���ģ�ͼ������߷���H����վ��BLH����λm��E�����Ǹ߶Ƚǣ���λ�����ȣ���
	void getAllData();//��ȡ�������� ֻ�ж�ȡ�����ݲſ��Լ��㣨�����ڳ����ʼ����ʱ���ȡ��
private:
	void initVar();
	bool openGrdFile(QString GrdFileName);
	void readGrdFile(QString grdFileName);//��ȡGRD�ļ��������ڼ���
	GPT2Result HopfieldDelay(double &ZHD,double &ZWD,double dmjd,double dlat,double dlon,double hell,double it = 0);//����GPT2����+Hopfield���춥����  it��1��ʾ��ʱ��仯 0����ʱ��仯
	GPT2Result SassstaMDelay(double &ZHD,double &ZWD,double dmjd,double dlat,double dlon,double hell,double it = 0);//����GPT2����+�򻯵�Saastamoinenģ�ͣ��춥����  it��1��ʾ��ʱ��仯 0����ʱ��仯
	void trop_map_gmf(double dmjd,double dlat,double dlon,double dhgt,double zd,double *gmfh,double *gmfw);
	void trop_gpt(double dmjd,double dlat,double dlon,double dhgt,double *pres,double *temp,double *undu);
	int ipow(int base,int exp);//��trop_gpt����
#ifdef EIGEN_CORE_H
	VectorXd UNB3M(Vector3d &BLH, double DAYOYEAR, double ELEVRAD);//UNB3 ģ�ͣ�����Eigen�⣩
#endif
//�������� 
public:

private:
	//���涨��Neilģ�Ͳ���
	//ad,bd,cd 15-75�ȵĸɷ���ƽ��ֵ�Ͳ������ȱ�
	double lat[5];
	double Avgad[5],Avgbd[5],Avgcd[5];
	double Ampad[5],Ampbd[5],Ampcd[5];
	//���涨��ʪ����ϵ����
	double Avgaw[5],Avgbw[5],Avgcw[5];
	double m_PI;//Բ����
	QString m_GrdFileName;
	QFile m_ReadFileClass;
	QVector< GrdFileVar > m_allGrdFile;
	QString tempLine;
	bool isReadAllData;
	bool isGPT2;//�ж��Ƿ���GPT2ģ�ͣ�1 ����ѡ��GPT��0
	QString m_ProjectionFun;//ѡ��ʹ�õ�ͶӰ����
	int m_ProjectFunFlag;//1:Neil 2:VMF1 3:GMF
};

#endif

