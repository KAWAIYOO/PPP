#pragma once
#include <QDebug>
#include<Eigen/Dense>
using namespace Eigen;

class QNewFunLib
{
//��������
public:
	QNewFunLib(void);
	~QNewFunLib(void);
	void computeCrossPoint(Vector3d Recv1Pos,Vector3d Recv2Pos,Vector3d SatPos,Vector3d *crossPoint,Vector3d *talpha = NULL);//����SatPos��1,2�Ž��ջ�ֱ���ϵ�ͶӰ��
private:

//���ݲ���
public:

private:
};

