#include "QKalmanFilter.h"


QKalmanFilter::QKalmanFilter(void)
{
	initVar();
}


QKalmanFilter::~QKalmanFilter(void)
{
}

void QKalmanFilter::initVar()
{
	isInitPara = true;//�״���Ԫ��ʼ��ֻһ��
	m_VarChang = false;
}

//��ӡ����for Debug
void QKalmanFilter::printMatrix(MatrixXd mat)
{
	qDebug()<<"Print Matrix......";
	for (int i = 0; i < mat.rows();i++)
	{
		for (int j = 0;j< mat.cols();j++)
		{
			qDebug()<<mat(i,j);
		}
		qDebug()<<"___________________";
	}
}

//��ʼ��Kalman
void QKalmanFilter::initKalman(QVector< SatlitData > &currEpoch,MatrixXd &B,VectorXd &L)
{
	int epochLenLB = currEpoch.length();
	//Fk_1��ʼ��
	m_Fk_1.resize(5+epochLenLB,5+epochLenLB);
	m_Fk_1.setZero();
	m_Fk_1.setIdentity(5+epochLenLB,5+epochLenLB);
	//Fk_1(3,3) = 0;//��̬PPPֻ���Ӳ�Ϊ0

	//Xk_1��ʼ��,��С���˳�ʼ��
	MatrixXd B1,L1,X1;
	m_Xk_1.resize(epochLenLB+5);
	m_Xk_1.setZero();
	m_Xk_1 = (B.transpose()*B).inverse()*B.transpose()*L;
	//��ʼ��״̬Э�������Pk_1��ʼ��
	m_Pk_1.resize(5+epochLenLB,5+epochLenLB);
	m_Pk_1.setZero();
	m_Pk_1(0,0) = 1e5;m_Pk_1(1,1) = 1e5;m_Pk_1(2,2) = 1e5;
	m_Pk_1(3,3) = 0.5; m_Pk_1(4,4) = 1e5;
	for (int i = 0;i < currEpoch.length();i++)	m_Pk_1(5+i,5+i) = 1e6;
	//Qk_1ϵͳ������ʼ��
	m_Qwk_1.resize(5+epochLenLB,5+epochLenLB);
	m_Qwk_1.setZero();
	m_Qwk_1(3,3) = 3e-8;//�춥������в��	
	m_Qwk_1(4,4) = 1e+6;
	//Rk_1��ʼ�����ж�������Ŀû�б仯��
	isInitPara = false;//�˺��ڳ�ʼ��
}

//�ı�Kalman���� ��С
void QKalmanFilter::changeKalmanPara( QVector< SatlitData > &epochSatlitData,QVector< int >oldPrnFlag )
{
	int epochLenLB = epochSatlitData.length();
	m_Fk_1.resize(5+epochLenLB,5+epochLenLB);
	m_Fk_1.setZero();
	m_Fk_1.setIdentity(5+epochLenLB,5+epochLenLB);
	//Fk_1(4,4) = 0;//��̬PPPֻ���Ӳ�Ϊ0
	//Xk_1�仯
	VectorXd tempXk_1 = m_Xk_1;
	m_Xk_1.resize(epochLenLB+5);
	m_Xk_1.setZero();
	//Xk.resize(epochLenLB+5);
	for (int i = 0;i < 5;i++)
		m_Xk_1(i) = tempXk_1(i);
	for (int i = 0;i<epochLenLB;i++)
	{
		if (oldPrnFlag.at(i)!=-1)//�����ϵ�����ģ����
			m_Xk_1(5+i) = tempXk_1(oldPrnFlag.at(i)+5);
		else
		{//�µ�����ģ���ȼ���
			SatlitData oneStalit = epochSatlitData.at(i);
			m_Xk_1(5+i) = (oneStalit.PP3 - oneStalit.LL3)/M_GetLamta3(oneStalit.Frq[0],oneStalit.Frq[1]);
		}
	}
	//Qk_1ϵͳ����������£�ϵͳ�������ɲ���
	m_Qwk_1.resize(5+epochLenLB,5+epochLenLB);
	m_Qwk_1.setZero();
	m_Qwk_1(3,3) = 3e-8;//�춥������в��	
	m_Qwk_1(4,4) = 1e+6;
	//����Rk_1�۲���������(����������� �˴�����Ҫ�ظ�����)
	//����״̬Э�������Pk_1���ӻ����(�˴��Ƚϸ��ӣ���Ҫ˼����ȡ�����������ݣ����������ݳ�ʼ��)
	MatrixXd tempPk_1 = m_Pk_1;
	m_Pk_1.resize(5+epochLenLB,5+epochLenLB);
	m_Pk_1.setZero();
	//���������Ŀ�ı�
	for (int i = 0;i < 5;i++)
		for (int j = 0;j < 5;j++)
			m_Pk_1(i,j) = tempPk_1(i,j);

	for (int n = 0; n < epochLenLB;n++)
	{
		int flag = oldPrnFlag.at(n);
		if ( flag != -1)//˵������һ����Ԫ���б����������ݣ���Ҫ��tempPk_1ȡ��
		{
			flag+=5;//����������ԭʼ����tempPk_1�е�����
			for (int i = 0;i < tempPk_1.cols();i++)
			{//��tempPk_1ȡ��������oldPrnFlagΪ-1������
				if (i < 5)
				{
					m_Pk_1(n+5,i) = tempPk_1(flag,i);
					m_Pk_1(i,n+5) = tempPk_1(i,flag);
				}
				else
				{
					int findCols = i - 5,saveFlag = -1;
					//�����������������Ƿ���ڣ��Լ���Ҫ����λ��
					for (int m = 0;m < oldPrnFlag.length();m++)
					{
						if (findCols == oldPrnFlag.at(m))
						{
							saveFlag = m;
							break;
						}
					}
					if (saveFlag!=-1)
					{
						m_Pk_1(n+5,saveFlag+5) = tempPk_1(flag,i);
						//Pk_1(saveFlag+5,n+5) = tempPk_1(i,flag);
					}

				}//if (i < 5)
			}//for (int i = 0;i < tempPk_1.cols();i++)

		}
		else
		{
			m_Pk_1(n+5,n+5) = 1e20;
			for (int i = 0;i < 5;i++)
			{
				m_Pk_1(n+5,i) = 1;
				m_Pk_1(i,n+5) = 1;
			}
		}
	}//Pk_1�����������

	m_VarChang = true;
}


//��һ���汾
void QKalmanFilter::KalmanforStatic(MatrixXd Bk,VectorXd Lk,MatrixXd F,MatrixXd Qwk,MatrixXd Rk,VectorXd &tXk_1,MatrixXd &tPk_1)
{
	//ʱ�����
	VectorXd Xkk_1 = F*tXk_1,Vk;
	MatrixXd Pkk_1 = F*tPk_1*F.transpose() + Qwk,I,tempKB,Kk;
	//�����������
	Kk = (Pkk_1*Bk.transpose())*((Bk*Pkk_1*Bk.transpose() + Rk).inverse());
	//�˲�����
	Vk = Lk - Bk*Xkk_1;
	//����X
	tXk_1 = Xkk_1 + Kk*Vk;

	tempKB = Kk*Bk;
	I.resize(tempKB.rows(),tempKB.cols());
	I.setIdentity();
	//����P
	tPk_1 = (I - tempKB)*Pkk_1;
	//printMatrix(tPk_1);
	//tPk_1 = 0.5*(tPk_1 + tPk_1.transpose());	//(������Ӧ�ü��ϵ��Ǽ��Ϻ� ������������ı���ԭʼ������ʽ)
	//printMatrix(tPk_1);
}

//�ڶ����汾
void QKalmanFilter::KalmanforStatic(QVector< SatlitData > &preEpoch,QVector< SatlitData > &currEpoch,double *m_ApproxRecPos,VectorXd &X,MatrixXd &P)
{
	//���α���ز����� L = BX
	int preEpochLen = preEpoch.length();
	int epochLenLB = currEpoch.length();
	MatrixXd B(2*epochLenLB,epochLenLB+5);
	VectorXd L(2*epochLenLB);
	for (int i = 0; i < epochLenLB;i++)
	{
		SatlitData oneSatlit = currEpoch.at(i);
		double li = 0,mi = 0,ni = 0,p0 = 0,dltaX = 0,dltaY = 0,dltaZ = 0;
		dltaX = oneSatlit.X - m_ApproxRecPos[0];
		dltaY = oneSatlit.Y - m_ApproxRecPos[1];
		dltaZ = oneSatlit.Z - m_ApproxRecPos[2];
		p0 = qSqrt(dltaX*dltaX+dltaY*dltaY+dltaZ*dltaZ);
		li = dltaX/p0;mi = dltaY/p0;ni = dltaZ/p0;
		//����B����
		//L3�ز�����
		B(i,0) = li;B(i,1) = mi;B(i,2) = ni;B(i,3) = -oneSatlit.StaTropMap;B(i,4) = -1;
		for (int n = 0;n < epochLenLB;n++)//����Խ��߲���ȫ����ʼ��Lamta3�Ĳ���������Ϊ0
			if (i == n)	
				B(i,5+n) = M_GetLamta3(oneSatlit.Frq[0],oneSatlit.Frq[1]);//LL3����
			else 
				B(i,5+n) = 0;
		
		//P3α�������
		B(i+epochLenLB,0) = li;B(i+epochLenLB,1) = mi;B(i+epochLenLB,2) = ni;B(i+epochLenLB,3) = -oneSatlit.StaTropMap;B(i+epochLenLB,4) = -1;
		for (int n = 0;n < epochLenLB;n++)//���沿��ȫ��Ϊ0
			B(i+epochLenLB,5+n) = 0;

		//����L����
		double dlta = 0;//�����Ǹ�����
		dlta =  - oneSatlit.StaClock + oneSatlit.SatTrop - oneSatlit.Relativty - 
			oneSatlit.Sagnac - oneSatlit.TideEffect - oneSatlit.AntHeight - oneSatlit.SatOffset;
			//�ز�L
		L(i) = p0 - oneSatlit.LL3 + dlta;
		//α����L
		L(i+epochLenLB) = p0 - oneSatlit.PP3 + dlta;
	}//B,L�������


	if (isInitPara) initKalman(currEpoch,B,L);//��һ����Ԫ��ʼ��

	//�ж����Ǹ����Ƿ����仯��ǰ��������Ԫ�Աȣ�
	QVector< int > oldPrnFlag;//�����һ����Ԫ��ͬ��������λ�ã�δ�ҵ���-1��ʾ
	int oldSatLen = 0;
	for (int i = 0;i < epochLenLB;i++)
	{//ѭ��ǰ����Ԫ���Ǽ���Ƿ���ȫ���
		SatlitData epochSatlit = currEpoch.at(i);
		bool Isfind = false;//����Ƿ��ҵ��ϸ���Ԫ����
		for (int j = 0;j < preEpochLen;j++)
		{
			SatlitData preEpochSatlit = preEpoch.at(j);
			if (epochSatlit.PRN == preEpochSatlit.PRN&&epochSatlit.SatType == preEpochSatlit.SatType)
			{
				oldPrnFlag.append(j);
				Isfind = true;
				oldSatLen++;
				break;
			}
		}
		if (!Isfind)	oldPrnFlag.append(-1);
	}

	////���ӻ��߼���n������
	if (preEpochLen!=0&&(oldSatLen!=preEpochLen||epochLenLB!=preEpochLen))
		changeKalmanPara(currEpoch,oldPrnFlag);//��������kalman�������ݴ�С

		
	//����Rk_1����ʱ������Ŀû�б仯��
	m_Rk_1.resize(2*epochLenLB,2*epochLenLB);
	m_Rk_1.setZero();
	for (int i = 0;i < epochLenLB;i++)
	{
		SatlitData oneSatlit = currEpoch.at(i);
		m_Rk_1(i,i) = 3.6e-5/oneSatlit.SatWight;//α�෽�̵�Ȩ ����(����С)
		m_Rk_1(i+epochLenLB,i+epochLenLB) = 9e-2/oneSatlit.SatWight;//�ز�Ȩ ����(������)
	}

	
	if (m_VarChang)
	{
		KalmanforStatic(B,L,m_Fk_1,m_Qwk_1,m_Rk_1,m_Xk_1,m_Pk_1);
		m_VarChang = false;
	}

	//���õ�һ�汾Kalman�˲�
	KalmanforStatic(B,L,m_Fk_1,m_Qwk_1,m_Rk_1,m_Xk_1,m_Pk_1);
	X = m_Xk_1;//���汾��Ԫ����������������ʼ�����ݣ�
	/*for (int i = 0;i < X.size();i++)
	{
	qDebug()<<X[i];
	}*/
	P = m_Pk_1;
	
}