#ifndef QWINDUP_H
#define QWINDUP_H

#include "QCmpGPST.h"
/*������λ���Ľ����ʹ��RTKLIB�����װ
*/


class QWindUp
{
//��������
public:
	QWindUp(void);
	~QWindUp(void);
	double getWindUp(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,double *StaPos,double *RecPos,double &phw,double *psunpos = NULL);//SatPos��RecPos�������Ǻͽ��ջ�WGS84���� �����ܣ���λ�ܣ���Χ[-0.5 +0.5],phw���ϸ���Ԫ����λ��ת
private:
	void windupcorr(gtime_t time, const double *rs, const double *rr,	double *phw,double *psunpos = NULL);//����̫�����꣬���Լ����ظ�����
//��������
public:

private:
	QCmpGPST m_qCmpClass;
};

#endif

