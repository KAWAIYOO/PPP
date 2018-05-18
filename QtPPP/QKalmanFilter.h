#ifndef QKALMANFILTER_H
#define QKALMANFILTER_H

/*
1����������-����Eigen���㣬�����������
2���Ż����㲻��Ҫ����Eigen����
*/
#include "QGlobalDef.h"


class QKalmanFilter:public QBaseObject
{
//��������
public:
	QKalmanFilter(void);
	~QKalmanFilter(void);
	void initVar();//��ʼ��һЩ����
	
	//F:״̬ת�ƾ���Xk_1:ǰһ���˲���ֵ��Pk_1��ǰһ���˲�������Qk_1��ǰһ��״̬ת����������Bk���۲����
	//Rk:�۲���������Lk���۲�����
	void KalmanforStatic(MatrixXd Bk,VectorXd Lk,MatrixXd F,MatrixXd Qw,MatrixXd Rk,VectorXd &Xk_1,MatrixXd &Pk_1);
	void KalmanforStatic(QVector< SatlitData > &preEpoch,QVector< SatlitData > &currEpoch,double *m_ApproxRecPos,VectorXd &Xk_1,MatrixXd &Pk_1);
private:
	void printMatrix(MatrixXd mat);//��ӡ����Debug
	void initKalman(QVector< SatlitData > &currEpoch,MatrixXd &B,VectorXd &L);//��������ʼ��
	void changeKalmanPara(QVector< SatlitData > &epochSatlitData,QVector< int >oldPrnFlag );
//��������
public:

private:
	bool isInitPara;//�жϵ�һ����Ԫ�Ƿ��ʼ��
	MatrixXd m_Fk_1,m_Pk_1,m_Qwk_1,m_Rk_1;
	VectorXd m_Xk_1;//�ֱ�ΪdX,dY,dZ,dT(�춥������в�),dVt(���ջ��Ӳ�)��N1,N2...Nm(ģ����)
	bool m_VarChang;//��� �¸��˲�ʱ�� �����Ƿ����仯
};

#endif
