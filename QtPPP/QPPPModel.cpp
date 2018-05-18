#include "QPPPModel.h"

//��ʼ������
void QPPPModel::initVar()
{
	for (int i = 0;i < 3;i++)
		m_ApproxRecPos[0] = 0;
	m_OFileName = "";
	multReadOFile = 1000;
	m_leapSeconds = 0;
}

//��������
QPPPModel::~QPPPModel()
{

}

//Ϊ�յĹ��캯��
QPPPModel::QPPPModel(void)
{
	initVar();
}

//���캯��
QPPPModel::QPPPModel(QString OFileName,QStringList Sp3FileNames,QStringList ClkFileNames,QString ErpFileName,QString BlqFileName,QString AtxFileName,QString GrdFileName)
	:m_ReadOFileClass(OFileName),m_ReadSP3Class(Sp3FileNames),m_ReadClkClass(ClkFileNames),m_ReadTropClass(GrdFileName,"GMF"),
	m_ReadAntClass(AtxFileName),m_TideEffectClass(BlqFileName,ErpFileName)
{
//��ʼ������
	initVar();

//�����ļ���
	m_OFileName = OFileName;
	m_Sp3FileNames = Sp3FileNames;
	m_ClkFileNames = ClkFileNames;
//�����������
	int obsTime[5] = {0};
	double Seconds = 0,ObsJD = 0;
	m_ReadOFileClass.getApproXYZ(m_ApproxRecPos);//���O�ļ���������
	m_ReadOFileClass.getFistObsTime(obsTime,Seconds);//�����ʼ�۲�ʱ��
	ObsJD = qCmpGpsT.computeJD(obsTime[0],obsTime[1],obsTime[2],obsTime[3],obsTime[4],Seconds);
	m_ReadAntClass.setObsJD(m_ReadOFileClass.getAntType(),ObsJD);//����������Чʱ��
	m_TideEffectClass.setStationName(m_ReadOFileClass.getMakerName());//���ú�����Ҫ��վ����
//��ȡ����
	m_leapSeconds = qCmpGpsT.getLeapSecond(obsTime[0],obsTime[1],obsTime[2],obsTime[3],obsTime[4],Seconds);
//���ö�ϵͳ����
	//�����ļ�ϵͳ SystemStr:"G"(����GPSϵͳ);"GR":(����GPS+GLONASSϵͳ);"GRCE"(ȫ������)��
	//����GPS,GLONASS,BDS,Galieo�ֱ�ʹ�ã���ĸG,R,C,E
	//QString SysStr = "GRCE";
	QString SysStr = "G";
	setSatlitSys(SysStr);

//��ȡ��Ҫ�õ��ļ����ļ�ģ�飨��ʱ��
	m_ReadAntClass.getAllData();//��ȡ���Ǻͽ��ջ���������
	m_TideEffectClass.getAllData();//��ȡ��ϫ����
	m_ReadTropClass.getAllData();//��ȡgrd�ļ����ں�����������
	m_ReadSP3Class.getAllData();//��ȡ����SP3�ļ�
	m_ReadClkClass.getAllData();//��ȡ������ļ����ں������
	
}

//�����ļ�ϵͳ SystemStr:"G"(����GPSϵͳ);"GR":(����GPS+GLONASSϵͳ);"GRCE"(ȫ������)��
//����GPS,GLONASS,BDS,Galieo�ֱ�ʹ�ã���ĸG,R,C,E
bool QPPPModel::setSatlitSys(QString SystemStr)
{
	bool IsGood = QBaseObject::setSatlitSys(SystemStr);
	//���ö�ȡO�ļ�
	m_ReadOFileClass.setSatlitSys(SystemStr);
	//���ö�ȡSp3
	m_ReadSP3Class.setSatlitSys(SystemStr);
	//���ö�ȡClk�ļ�
	m_ReadClkClass.setSatlitSys(SystemStr);
	//���ý��ջ���������ϵͳ
	m_ReadAntClass.setSatlitSys(SystemStr);
	//����kalman�˲���ϵͳ
	m_KalmanClass.setSatlitSys(SystemStr);
	return IsGood;
}

//��SP3�������ݻ�ȡ����Ԫ���ǵ�����
void QPPPModel::getSP3Pos(double GPST,int PRN,char SatType,double *p_XYZ,double *pdXYZ)
{
	m_ReadSP3Class.getPrcisePoint(PRN,SatType,GPST,p_XYZ,pdXYZ);
}

//��CLK�����л�ȡ�����
void QPPPModel::getCLKData(int PRN,char SatType,double GPST,double *pCLKT)
{
	m_ReadClkClass.getStaliteClk(PRN,SatType,GPST,pCLKT);
}

//�����Դ�����
double QPPPModel::getSagnac(double X,double Y,double Z,double *approxRecvXYZ)
{//��������Դ�����
	double dltaP = M_We*((X - approxRecvXYZ[0])*Y - (Y - approxRecvXYZ[1])*X)/M_C;
	return -dltaP;//�����෴��ʹ��p = p' + dltaP;����ֱ�����
}

//���������ЧӦ
double QPPPModel::getRelativty(double *pSatXYZ,double *pRecXYZ,double *pSatdXYZ)
{
	/*double c = 299792458.0;
	double dltaP = -2*(pSatXYZ[0]*pSatdXYZ[0] + pSatdXYZ[1]*pdXYZ[1] + pSatXYZ[2]*pSatdXYZ[2]) / c;*/
	double b[3] = {0},a = 0,R = 0,Rs = 0,Rr = 0,v_light = 299792458.0,GM=3.9860047e14,dltaP = 0;
	b[0] = pRecXYZ[0] - pSatXYZ[0];
	b[1] = pRecXYZ[1] - pSatXYZ[1];
	b[2] = pRecXYZ[2] - pSatXYZ[2];
	a = pSatXYZ[0]*pSatdXYZ[0] + pSatXYZ[1]*pSatdXYZ[1] + pSatXYZ[2]*pSatdXYZ[2];
	R=qCmpGpsT.norm(b,3);
	Rs = qCmpGpsT.norm(pSatXYZ,3);
	Rr = qCmpGpsT.norm(pRecXYZ,3);
	dltaP=-2*a/M_C + (2*M_GM/qPow(M_C,2))*qLn((Rs+Rr+R)/(Rs+Rr-R));
	return dltaP;//m
}

//����EA��E�����Ǹ߶Ƚ�,A����λ��
void QPPPModel::getSatEA(double X,double Y,double Z,double *approxRecvXYZ,double *EA)
{//����EA//����BUG ���ڼ���XYZתBLHʱ�� L����ؾ��ȣ���Ȼ�෴��y < 0 ,x > 0.L = -atan(y/x)���� Ӧ���� L = -atan(-y/x)
	double pSAZ[3] = {0};
	qCmpGpsT.XYZ2SAE(X,Y,Z,pSAZ,approxRecvXYZ);//����Bug
	EA[0] = (PI/2 - pSAZ[2])*360/(2*PI);
	EA[1] = pSAZ[1]*360/(2*PI);
}

//ʹ��Sassģ�� ������������ʹ�õ�ģ���Լ�ͶӰ���� �Լ�GPT2ģ��
double QPPPModel::getTropDelay(double MJD,int TDay,double E,double *pBLH,double *mf)
{
	double GPT2_Trop = m_ReadTropClass.getGPT2SasstaMDelay(MJD,TDay,E,pBLH,mf);//GPT2ģ�� ֻ���ظ��ӳٹ��ƺ�ʪ�ӳٺ���
	//double UNB3m_Trop = m_ReadTropClass.getUNB3mDelay(pBLH,TDay,E,mf);//UNB3Mģ�� ֻ���ظ��ӳٹ��ƺ�ʪ�ӳٺ���
	return GPT2_Trop;
}

//������ջ�L1��L2��λ���ĸ���PCO+PCV,L1Offset��L2Offset�������߷���ľ������
bool QPPPModel::getRecvOffset(double *EA,char SatType,double &L1Offset,double &L2Offset)
{
	if (m_ReadAntClass.getRecvL12(EA[0],EA[1],SatType,L1Offset,L2Offset))
		return true;
	else
	{
		L1Offset = 0; L2Offset = 0;
		return false;
	}
}

//��������PCO+PCV��������Ϊ���� G1��G2Ƶ��һ�������������θ�����һ���ģ�StaPos��RecPos�����Ǻͽ��ջ�WGS84���꣨��λm��
double QPPPModel::getSatlitOffset(int Year,int Month,int Day,int Hours,int Minutes,double Seconds,int PRN,char SatType,double *StaPos,double *RecPos)
{
	return m_ReadAntClass.getSatOffSet(Year,Month,Day,Hours,Minutes,Seconds,PRN,SatType,StaPos,RecPos);//pXYZ��������������
}

//���㳱ϫ�����߷���ĸ�������λm��
double QPPPModel::getTideEffect(int Year,int Month,int Day,int Hours,int Minutes,double Seconds,double *pXYZ,double *EA,double *psunpos/* =NULL */, double *pmoonpos /* = NULL */,double gmst /* = 0 */,QString StationName /* = "" */)
{
	return m_TideEffectClass.getAllTideEffect(Year,Month,Day,Hours,Minutes,Seconds,pXYZ,EA,psunpos,pmoonpos,gmst,StationName);
}

//SatPos��RecPos�������Ǻͽ��ջ�WGS84���� �����ܣ���λ�ܣ���Χ[-0.5 +0.5]
double QPPPModel::getWindup(int Year,int Month,int Day,int Hours,int Minutes,double Seconds,double *StaPos,double *RecPos,double &phw,double *psunpos)
{
	return m_WinUpClass.getWindUp(Year,Month,Day,Hours,Minutes,Seconds,StaPos,RecPos,phw,psunpos);
}

//�������������pLP����ά���飬��һ����W-M��ϣ�N2-N1 < 5�� �ڶ����������в<0.3�� �������ǣ�lamt2*N2-lamt1*N1 < 5��
bool QPPPModel::CycleSlip(const SatlitData &oneSatlitData,double *pLP)
{//
	if (oneSatlitData.L1*oneSatlitData.L2*oneSatlitData.C1*oneSatlitData.P2 == 0)//�ж��Ƿ���˫Ƶ����
		return false; 
	double F1 = oneSatlitData.Frq[0],F2 = oneSatlitData.Frq[1];//��ȡ�������ǵ�Ƶ��
	double Lamta1 = M_C/F1,Lamta2 = M_C/F2;

	double NL12 = ((F1-F2)/(F1+F2))*(oneSatlitData.C1/Lamta1 + oneSatlitData.P2/Lamta2) - (oneSatlitData.L1 - oneSatlitData.L2);
	double IocL = Lamta1*oneSatlitData.L1 - Lamta2*oneSatlitData.L2;
	double IocLP = IocL+(oneSatlitData.C1 - oneSatlitData.P2); 
	pLP[0] = NL12;
	pLP[1] =IocL;
	pLP[2] = IocLP;
	return true;
}

//����ǰһ����Ԫ����λ����
double QPPPModel::getPreEpochWindUp(QVector< SatlitData > &prevEpochSatlitData,int PRN,char SatType)
{
	int preEopchLen = prevEpochSatlitData.length();
	if (0 == preEopchLen) return 0;

	for (int i = 0;i < preEopchLen;i++)
	{
		SatlitData oneSatalite = prevEpochSatlitData.at(i);
		if (PRN == oneSatalite.PRN&&oneSatalite.SatType == SatType)
			return oneSatalite.AntWindup;
	}
	return 0;
}

//ɸѡ��ȱʧ���ݣ���������������ߣ��߶Ƚǣ������ȣ�������
void QPPPModel::getGoodSatlite(QVector< SatlitData > &prevEpochSatlitData,QVector< SatlitData > &epochSatlitData,double eleAngle)
{
	//�������
	QVector< int > CycleFlag;//��¼����λ��
	int preEpochLen = prevEpochSatlitData.length();
	int epochLen = epochSatlitData.length();
	CycleFlag.resize(epochLen);
	for (int i = 0;i < epochLen;i++) CycleFlag[i] = 0;
	for (int i = 0;i < epochLen;i++)
	{
		SatlitData epochData = epochSatlitData.at(i);
		//���ݲ�Ϊ 0 
		if (!(epochData.L1&&epochData.L2&&epochData.C1&&epochData.P2))
			CycleFlag[i] = -1;
		//���������Ϊ��
		if (!(epochData.X&&epochData.Y&&epochData.Z&&epochData.StaClock&&epochData.SatTrop))
			CycleFlag[i] = -1;
		//�������ƣ��߶Ƚ� α���ֵ��
		if (epochData.EA[0] < eleAngle || (epochData.C1 - epochData.P2) > 50)
			CycleFlag[i] = -1;
		//�������
		for (int j = 0;j < preEpochLen;j++)
		{
			SatlitData preEpochData = prevEpochSatlitData.at(j);
			if (epochData.PRN == preEpochData.PRN&&epochData.SatType == preEpochData.SatType)
			{//��Ҫ�ж�ϵͳ
				double epochLP[3]={0},preEpochLP[3]={0},diffLP[3]={0};
				CycleSlip(epochData,epochLP);
				CycleSlip(preEpochData,preEpochLP);
				for (int n = 0;n < 3;n++)
					diffLP[n] = qAbs(epochLP[n] - preEpochLP[n]);
				//���ݾ���ȷ��������ֵ��
				if (diffLP[0] > 3.5||diffLP[1] > 0.3||diffLP[2] > 3.5||qAbs(epochData.AntWindup - preEpochData.AntWindup) > 0.3)
				{//��������
					CycleFlag[i] = -1;//�����������Ǳ�־
				}
				break;
			}
			else
			{
				continue;
			}
		}
	}
	//ɾ������������������
	QVector< SatlitData > tempEpochSatlitData;
	for (int i = 0;i < epochLen;i++)
	{
		if (CycleFlag.at(i) != -1)
		{
			tempEpochSatlitData.append(epochSatlitData.at(i));
		}
	}
	epochSatlitData = tempEpochSatlitData;
		
}

//��ȡ�������֮���.ppp�ļ����Զ�������ļ��ĸ�ʽ,��
void QPPPModel::readPPPFile(QString pppFileName,QVector< QVector< SatlitData > > &allEpochData)
{
	QFile pppFile(pppFileName);
	if (!pppFile.open(QFile::ReadOnly))
	{
		QString erroInfo = "Open " + pppFileName + "faild!";
		ErroTrace(erroInfo);
		return ;
	}
	QString tempLine = "";
	//��ȡͷ�ļ�
	bool endFileHead = false;//ͷ�ļ�������־
	do 
	{//�˴�Ϊ������ֻ��ȡ������Ҫ���ݣ��Ժ���Ҫ����
		tempLine = pppFile.readLine();//��ȡһ��
		if (tempLine.indexOf("APPROX_POSITION") >= 0)
		{//��ȡ��������
			m_ApproxRecPos[0] = tempLine.mid(16,15).toDouble();
			m_ApproxRecPos[1] = tempLine.mid(32,15).toDouble();
			m_ApproxRecPos[2] = tempLine.mid(48,15).toDouble();
		}
		else if (tempLine.indexOf("PRN") >= 0)
		{
			endFileHead = true;//ͷ�ļ�����
		}
	} while (!endFileHead);
	//��ȡ���ݲ���
	bool endDataFlag = false;//�����ļ���ȡ������־
	do 
	{//��ȡÿһ����Ԫ����
		QVector < SatlitData > epochStaliData;//����ÿ����Ԫ����
		tempLine = pppFile.readLine();//���ݶ���ʼ��־
		if (tempLine.indexOf("Number") >= 0)
		{
			double SatlitNum = tempLine.mid(17,3).toDouble();
			for (int i = 0;i < SatlitNum;i++)
			{//��ȡ���Ǹ������
				SatlitData tempSatlitData;//�洢������������
				//��ȡһ������
				tempLine = pppFile.readLine();
				//��ȡPRN
				tempSatlitData.PRN = tempLine.mid(1,2).toInt();
				tempSatlitData.SatType = *(tempLine.mid(0,1).toLatin1().data());
				//����Ƶ��
				if (tempSatlitData.SatType == 'G')
				{
					tempSatlitData.Frq[0] = M_F1;
					tempSatlitData.Frq[1] = M_F2;
				}
				else if(tempSatlitData.SatType == 'R')
				{
					tempSatlitData.Frq[0] = M_GLONASSF1(g_GlonassK[tempSatlitData.PRN - 1]);
					tempSatlitData.Frq[1] = M_GLONASSF2(g_GlonassK[tempSatlitData.PRN - 1]);
				}
				else
				{
					tempSatlitData.Frq[0] = 0;
					tempSatlitData.Frq[1] = 0;
				}
				double F1 = tempSatlitData.Frq[0],F2 = tempSatlitData.Frq[1];
				double Lamta1 = M_C/F1,Lamta2 = M_C/F2;
				double alpha1 = (F1*F1)/(F1*F1 - F2*F2),alpha2 = (F2*F2)/(F1*F1 - F2*F2);

				//��ȡ����
				tempSatlitData.X = tempLine.mid(6,15).toDouble();
				tempSatlitData.Y = tempLine.mid(22,15).toDouble();
				tempSatlitData.Z = tempLine.mid(38,15).toDouble();
				//��ȡ���������
				tempSatlitData.StaClock = tempLine.mid(54,15).toDouble();
				//��ȡ���Ǹ߶Ƚ�
				tempSatlitData.EA[0] = tempLine.mid(70,15).toDouble();
				tempSatlitData.EA[1] = tempLine.mid(86,15).toDouble();
				tempSatlitData.SatWight = (qSin(tempSatlitData.EA[0]*PI/180)*qSin(tempSatlitData.EA[0]*PI/180));//�趨���ǵ�Ȩ��
				//��ȡ�����ز���α����
				tempSatlitData.C1 = tempLine.mid(102,15).toDouble();
				tempSatlitData.P2 = tempLine.mid(118,15).toDouble();
				tempSatlitData.L1 = tempLine.mid(134,15).toDouble();
				tempSatlitData.L2 = tempLine.mid(150,15).toDouble();
				//��ȡ�������ӳٸ���
				tempSatlitData.SatTrop = tempLine.mid(166,15).toDouble();
				tempSatlitData.StaTropMap = tempLine.mid(182,15).toDouble();
				//��ȡ����۸���
				tempSatlitData.Relativty = tempLine.mid(198,15).toDouble();;
				//��ȡ�����Դ�����
				tempSatlitData.Sagnac = tempLine.mid(214,15).toDouble();
				//��ȡ��ϫӰ�����
				tempSatlitData.TideEffect = tempLine.mid(230,15).toDouble();
				//��ȡ���ջ����߸߸���
				tempSatlitData.AntHeight = tempLine.mid(246,15).toDouble();
				//��ȡ�������߸߸���
				tempSatlitData.SatOffset = tempLine.mid(262,15).toDouble();
				//��ȡ���ջ���λ���ĸ���
				tempSatlitData.L1Offset = tempLine.mid(278,15).toDouble();
				tempSatlitData.L2Offset = tempLine.mid(294,15).toDouble();
				//��ȡwindup����
				tempSatlitData.AntWindup = tempLine.mid(310,15).toDouble();
				//����LL3��PP3���޸�Ϊ��ϵͳ��
				tempSatlitData.LL3 = alpha1*(tempSatlitData.L1 + tempSatlitData.L1Offset - tempSatlitData.AntWindup)*Lamta1 - alpha2*(tempSatlitData.L2 + tempSatlitData.L2Offset - tempSatlitData.AntWindup)*Lamta2;//��ȥ������ز�LL3
				tempSatlitData.PP3 = alpha1*(tempSatlitData.C1 + Lamta1*tempSatlitData.L1Offset) - alpha2*(tempSatlitData.P2 + Lamta2 *tempSatlitData.L2Offset);//��ȥ������ز�PP3
				//����ÿһ����������
				epochStaliData.append(tempSatlitData);
			}
			allEpochData.append(epochStaliData);//����ÿ����Ԫ����

		}//if (tempLine.indexOf("satellite") >= 0)

		if (pppFile.atEnd())
		{
			endDataFlag = true;
		}

	} while (!endDataFlag);
}

//��ȡ.ppp�ļ�����kalman�˲�
void QPPPModel::Run(QString pppFileName)
{
	QVector < QVector < SatlitData > > allEpochData;
	readPPPFile(pppFileName,allEpochData);//��ȡ.ppp��������
	QVector < SatlitData > prevEpochSatlitData,epochSatlitData;//�洢��һ����Ԫ���������ݣ��������ʹ��
	RecivePos epochRecivePos;
	for (int i = 0; i < allEpochData.length();i++)
	{
		if (!epochSatlitData.isEmpty())
		{
			prevEpochSatlitData = epochSatlitData;//�����ϸ���Ԫ����
			epochSatlitData.clear();
		}
		epochSatlitData = allEpochData.at(i);//��ȡ����Ԫ����
		m_writeFileClass.allPPPSatlitData.append(epochSatlitData);//��������д��.sp3�ļ�
		//�����������������
		getGoodSatlite(prevEpochSatlitData,epochSatlitData);
		//Kalman�˲�
		MatrixXd P;
		VectorXd X;//�ֱ�ΪdX,dY,dZ,dT(�춥������в�),dVt(���ջ��Ӳ�)��N1,N2...Nm(ģ����)
		m_KalmanClass.KalmanforStatic(prevEpochSatlitData,epochSatlitData,m_ApproxRecPos,X,P);

		int epochLen = epochSatlitData.length();
		double pENU[3] = {0};
		qCmpGpsT.XYZ2ENU(X(0)+m_ApproxRecPos[0],X(1)+m_ApproxRecPos[1],X(2)+m_ApproxRecPos[2],pENU,m_ApproxRecPos);
		qDebug()<<"____Print ENU____";
		qDebug()<<epochLen;
		for (int i = 0;i < 3;i++)
		{
			qDebug()<<pENU[i];
		}
		qDebug()<<"______END X______";
		//������ջ�����д�뵽�ļ�
		epochRecivePos.Year = 0;
		epochRecivePos.Month = 0;
		epochRecivePos.Day = 0;
		epochRecivePos.Hours = 0;
		epochRecivePos.Minutes = 0;
		epochRecivePos.Seconds = 0;
		epochRecivePos.totolEpochStalitNum = epochLen;
		epochRecivePos.dX = pENU[0];
		epochRecivePos.dY = pENU[1];
		epochRecivePos.dZ = pENU[2];
		m_writeFileClass.allReciverPos.append(epochRecivePos);
	}//������Ԫ�������

	//д�������ļ�
	m_writeFileClass.writeRecivePos2Txt("Kalman.txt");
	m_writeFileClass.writePPP2Txt("Kalman.ppp");
}

int FlagN = 0;
//��ȡO�ļ���sp3�ļ���clk�ļ�,�Լ����������㣬Kalman�˲� ......................
void QPPPModel::Run()
{	
	//�ⲿ��ʼ���̶����� �ӿ�����ٶ�
	double p_HEN[3] = {0};//��ȡ���߸�
	m_ReadOFileClass.getAntHEN(p_HEN);

	//�����Ԫ�������� ��ȡO�ļ�����
	QVector < SatlitData > prevEpochSatlitData;//�洢��һ����Ԫ���������ݣ��������ʹ�ã���������������ȡmultReadOFile����Ԫ���ڶ�ȡ�ͻ��������ڵ��ڣ�
	while (!m_ReadOFileClass.isEnd())
	{
		QVector< QVector < SatlitData > > multepochSatlitData;//�洢�����Ԫ����
		m_ReadOFileClass.getMultEpochData(multepochSatlitData,multReadOFile);//��ȡmultReadOFile����Ԫ
//�����Ԫѭ������
		for (int n = 0; n < multepochSatlitData.length();n++)
		{
			qDebug()<<FlagN++;
			QVector< SatlitData > epochSatlitData;//��ʱ�洢ÿ����Ԫ����δ��������
			QVector< SatlitData > epochResultSatlitData;// �洢ÿ����Ԫ���Ǽ���������������
			
			epochSatlitData = multepochSatlitData.at(n);
			GPSPosTime epochTime = epochSatlitData.at(0).UTCTime;//��ȡ�۲�ʱ�̣���Ԫÿ�����Ƕ��洢�˹۲�ʱ�䣩
			//Debug
			RecivePos epochRecivePos;
			qDebug()<<epochTime.Hours<<":"<<epochTime.Minutes<<":"
				<<epochTime.Seconds<<" SatalitNum= "<<epochSatlitData.length();
			epochRecivePos.Year = epochTime.Year;epochRecivePos.Month = epochTime.Month;
			epochRecivePos.Day = epochTime.Day;epochRecivePos.Hours = epochTime.Hours;
			epochRecivePos.Minutes = epochTime.Minutes;epochRecivePos.Seconds = epochTime.Seconds;
//һ����Ԫѭ����ʼ
			for (int i = 0;i < epochSatlitData.length();i++)
			{
				
				SatlitData tempSatlitData = epochSatlitData.at(i);//�洢����ĸ��������������

				//��ȡGPSʱ
				double m_PrnGpst = qCmpGpsT.YMD2GPSTime(epochTime.Year,epochTime.Month,epochTime.Day,
					epochTime.Hours,epochTime.Minutes,epochTime.Seconds);
				//��CLK�ļ���ȡ���������
				double stalitClock = 0;//��λm
				getCLKData(tempSatlitData.PRN,tempSatlitData.SatType,m_PrnGpst - tempSatlitData.P2/M_C,&stalitClock);
				tempSatlitData.StaClock = stalitClock;
				//��SP3�������ݻ�ȡ����Ԫ���ǵ�����
				double pXYZ[3] = {0},pdXYZ[3] = {0};//��λm
				getSP3Pos(m_PrnGpst - tempSatlitData.P2/M_C - tempSatlitData.StaClock/M_C,tempSatlitData.PRN,tempSatlitData.SatType,pXYZ,pdXYZ);//��ȡ���Ƿ���ʱ�̵ľ�����������(������Ҫ��ȥ���ǵ������ tempSatlitData.StaClock/M_C �������������20cm)
				tempSatlitData.X = pXYZ[0];tempSatlitData.Y = pXYZ[1];tempSatlitData.Z = pXYZ[2];
//����һ�¾����������Ӳ�״̬�Լ��ز���α���Ƿ��쳣
				if (!(tempSatlitData.X&&tempSatlitData.Y&&tempSatlitData.Z&&tempSatlitData.StaClock&&tempSatlitData.L1&&tempSatlitData.L2&&tempSatlitData.C1&&tempSatlitData.P2))
				{
					epochResultSatlitData.append(tempSatlitData);
					continue;
				}
//���㲨������Ҫ��Զ�ϵͳ��
				double F1 = tempSatlitData.Frq[0],F2 = tempSatlitData.Frq[1];
				double Lamta1 = M_C/F1,Lamta2 = M_C/F2;
				double alpha1 = (F1*F1)/(F1*F1 - F2*F2),alpha2 = (F2*F2)/(F1*F1 - F2*F2);
				//��������۸���
				double relative = 0;
				relative = getRelativty(pXYZ,m_ApproxRecPos,pdXYZ);
				tempSatlitData.Relativty = relative;
				//�������Ǹ����Ƚ�(���Ž��ջ����Ʊ�仯)
				double EA[2]={0};
				getSatEA(tempSatlitData.X,tempSatlitData.Y,tempSatlitData.Z,m_ApproxRecPos,EA);
				tempSatlitData.EA[0] = EA[0];tempSatlitData.EA[1] = EA[1];
				EA[0] = EA[0]*PI/180;EA[1] = EA[1]*PI/180;//ת�����ȷ����������
				tempSatlitData.SatWight = (qSin(EA[0])*qSin(EA[0]));//�趨���ǵ�Ȩ��
				//��������Դ�����
				double earthW = 0;
				earthW = getSagnac(tempSatlitData.X,tempSatlitData.Y,tempSatlitData.Z,m_ApproxRecPos);
				tempSatlitData.Sagnac = earthW;
				//�����������ӳ�
				double TropDelay = 0,TropMap = 0;
				double MJD = qCmpGpsT.computeJD(epochTime.Year,epochTime.Month,epochTime.Day,
					epochTime.Hours,epochTime.Minutes,epochTime.Seconds) - 2400000.5;//�򻯵�������
				//���㱣�������
				double TDay = qCmpGpsT.YearAccDay(epochTime.Year,epochTime.Month,epochTime.Day);
				double p_BLH[3] = {0},mf = 0;
				qCmpGpsT.XYZ2BLH(m_ApproxRecPos[0],m_ApproxRecPos[1],m_ApproxRecPos[2],p_BLH);
				TropDelay = getTropDelay(MJD,TDay,EA[0],p_BLH,&mf);
				tempSatlitData.SatTrop = TropDelay;
				tempSatlitData.StaTropMap = mf;
				//�������߸�ƫ�Ƹ��� Antenna Height
				tempSatlitData.AntHeight = p_HEN[0]*qSin(EA[0]);
				//���ջ�L1 L2ƫ�Ƹ���
				double L1Offset = 0,L2Offset = 0;
				getRecvOffset(EA,tempSatlitData.SatType,L1Offset,L2Offset);
				tempSatlitData.L1Offset = L1Offset/Lamta1;
				tempSatlitData.L2Offset = L2Offset/Lamta2;
				//����������λ���ĸ���
				double SatAnt = 0.0;
				SatAnt = getSatlitOffset(epochTime.Year,epochTime.Month,epochTime.Day,
					epochTime.Hours,epochTime.Minutes,epochTime.Seconds - tempSatlitData.P2/M_C,
					tempSatlitData.PRN,tempSatlitData.SatType,pXYZ,m_ApproxRecPos);//pXYZ��������������
				tempSatlitData.SatOffset = SatAnt;
				//���㳱ϫ����
				double pENU[3] = {0},effctDistance = 0;
				effctDistance = getTideEffect(epochTime.Year,epochTime.Month,epochTime.Day,
					epochTime.Hours,epochTime.Minutes,epochTime.Seconds,m_ApproxRecPos,EA,
					m_ReadAntClass.m_sunpos,m_ReadAntClass.m_moonpos,m_ReadAntClass.m_gmst);
				tempSatlitData.TideEffect = effctDistance;
				//����������λ����
				double AntWindup = 0,preAntWindup = 0;
				//����ǰһ����Ԫ �Ƿ���ڱ������� ����ȡ������preAntWindup ����preAntWindup=0
				preAntWindup = getPreEpochWindUp(prevEpochSatlitData,tempSatlitData.PRN,tempSatlitData.SatType);//��ȡǰһ����Ԫ��WindUp
				AntWindup = getWindup(epochTime.Year,epochTime.Month,epochTime.Day,
					epochTime.Hours,epochTime.Minutes,epochTime.Seconds - tempSatlitData.P2/M_C,
					pXYZ,m_ApproxRecPos,preAntWindup,m_ReadAntClass.m_sunpos);
				tempSatlitData.AntWindup = AntWindup;

				//�������������α����ز����(�˴������˽��ջ��ز�ƫת��WindUp)
				tempSatlitData.LL3 = alpha1*(tempSatlitData.L1 + tempSatlitData.L1Offset - tempSatlitData.AntWindup)*Lamta1 - alpha2*(tempSatlitData.L2 + tempSatlitData.L2Offset - tempSatlitData.AntWindup)*Lamta2;//��ȥ������ز�LL3
				tempSatlitData.PP3 = alpha1*(tempSatlitData.C1 + Lamta1*tempSatlitData.L1Offset) - alpha2*(tempSatlitData.P2 + Lamta2 *tempSatlitData.L2Offset);//��ȥ������ز�PP3

				//����һ����Ԫ�������� 
				epochResultSatlitData.append(tempSatlitData);
			}
//һ����Ԫ����for (int i = 0;i < epochSatlitData.length();i++)

			m_writeFileClass.allPPPSatlitData.append(epochResultSatlitData);
			//�����������������
			getGoodSatlite(prevEpochSatlitData,epochResultSatlitData);
			
			//Kalman�˲�
			MatrixXd P;
			VectorXd X;//�ֱ�ΪdX,dY,dZ,dT(�춥������в�),dVt(���ջ��Ӳ�)��N1,N2...Nm(ģ����)
			m_KalmanClass.KalmanforStatic(prevEpochSatlitData,epochResultSatlitData,m_ApproxRecPos,X,P);
			//�����ϸ���Ԫ��������
			prevEpochSatlitData = epochResultSatlitData;

			//���������
			int epochLen = epochResultSatlitData.length();
			double pENU[3] = {0};
			qCmpGpsT.XYZ2ENU(X(0)+m_ApproxRecPos[0],X(1)+m_ApproxRecPos[1],X(2)+m_ApproxRecPos[2],pENU,m_ApproxRecPos);
			qDebug()<<"____Print ENU____";
			qDebug()<<epochLen;
			for (int i = 0;i < 3;i++)
			{
				qDebug()<<pENU[i];
			}

			qDebug()<<"______END________";
//������ջ�����д�뵽�ļ�
			//�洢��������
			epochRecivePos.totolEpochStalitNum = epochLen;
			epochRecivePos.dX = X[0];
			epochRecivePos.dY = X[1];
			epochRecivePos.dZ = X[2];
			m_writeFileClass.allReciverPos.append(epochRecivePos);
			//������ջ��Ӳ�
			ClockData epochRecClock;
			epochRecClock.UTCTime.Year = epochRecivePos.Year;epochRecClock.UTCTime.Month = epochRecivePos.Month;epochRecClock.UTCTime.Day = epochRecivePos.Day;
			epochRecClock.UTCTime.Hours = epochRecivePos.Hours;epochRecClock.UTCTime.Minutes = epochRecivePos.Minutes;epochRecClock.UTCTime.Seconds = epochRecivePos.Seconds;
			epochRecClock.ZTD_W = X(3);//�洢ʪ�ӳ�
			epochRecClock.clockData = X(4);//�洢���ջ��Ӳ�
			m_writeFileClass.allClock.append(epochRecClock);
			//��������ģ����
			Ambiguity oneSatAmb;
			for (int i = 0;i < epochResultSatlitData.length();i++)
			{
				SatlitData oneSat = epochSatlitData.at(i);
				oneSatAmb.PRN = oneSat.PRN;
				oneSatAmb.SatType = oneSat.SatType;
				oneSatAmb.UTCTime = epochRecClock.UTCTime;
				oneSatAmb.isIntAmb = false;
				oneSatAmb.Amb = X(i+5);
				oneSatAmb.epochNum = FlagN;
				m_writeFileClass.allAmbiguity.append(oneSatAmb);
			}
		}
//�����Ԫ����for (int n = 0; n < multepochSatlitData.length();n++)
		
	}
//������Ԫ����while (!m_ReadOFileClass.isEnd())

	//д�������ļ�
	m_writeFileClass.WriteEpochPRN(".//Product//Epoch_PRN.txt");
	m_writeFileClass.writeRecivePos2Txt(".//Product//Kalman.txt");
	m_writeFileClass.writePPP2Txt(".//Product//Kalman.ppp");
	m_writeFileClass.writeClockZTDW2Txt(".//Product//ZTDW_Clock.txt");
	m_writeFileClass.writeAmbiguity2Txt();//·��Ϊ.//Ambiguity//
}

