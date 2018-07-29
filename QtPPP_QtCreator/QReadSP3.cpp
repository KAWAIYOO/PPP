#include "QReadSP3.h"

const int QReadSP3::lagrangeFact = 8;//������ʼ��


void QReadSP3::initVar()
{
	tempLine = "";
	isReadHead = false;
	isReadAllData = false;
	IsSigalFile = false;
	//ֻ����GPSϵͳ���32������
	m_EndHeadNum = 8;
	m_lastGPSTimeFlag = 0;//��Ϊ��ʼ���
	m_lastCmpGPSTime = 999999999;//��¼��ʼ�����GPSʱ��
	m_SP3FileName = "";
	m_WeekOrder = 0;
	InitStruct();

}

//��ʼ����Ҫ�õĽṹ���ڲ��ľ���(�н�Լ�ڴ�����)
void QReadSP3::InitStruct()
{
	if (isInSystem('G'))
	{
		epochData.MatrixDataGPS.resize(32,4);
		epochData.MatrixDataGPS.setZero();
	}
	if (isInSystem('R'))
	{
		epochData.MatrixDataGlonass.resize(32,4);
		epochData.MatrixDataGlonass.setZero();
	}
	if (isInSystem('C'))
	{
		epochData.MatrixDataBDS.resize(32,4);
		epochData.MatrixDataBDS.setZero();
	}
	if (isInSystem('E'))
	{
		epochData.MatrixDataGalieo.resize(32,4);
		epochData.MatrixDataGalieo.setZero();
	}
}

QReadSP3::QReadSP3()
{
	initVar();
}

QReadSP3::QReadSP3(QString SP3FileName)
{
	initVar();
	openFiles(SP3FileName);
	IsSigalFile = true;
}

QReadSP3::QReadSP3(QStringList SP3FileNames)
{
	initVar();
	//debug:2017.07.08
	if (SP3FileNames.length() == 1)
	{
		QString SP3FileName = SP3FileNames.at(0);
		openFiles(SP3FileName);
		IsSigalFile = true;
	}
	if (SP3FileNames.length() > 0)
	{
		m_SP3FileNames =SP3FileNames;
	}
	else
	{
		isReadAllData = true;
		return ; 
	}
}

void QReadSP3::openFiles(QString SP3FileName)
{
	
	if (!SP3FileName.isEmpty())
	{
		m_readSP3FileClass.setFileName(SP3FileName);
		m_readSP3FileClass.open(QFile::ReadOnly);
		m_SP3FileName = SP3FileName;
	}
	else
	{
		isReadAllData = true;
		return;	
	}
		
}

QReadSP3::~QReadSP3(void)
{
	releaseAllData();
}

//ת��GPSʱ��
int QReadSP3::YMD2GPSTime(int Year,int Month,int Day,int HoursInt,int Minutes,int Seconds,int *GPSWeek)//,int *GPSTimeArray
{
	double Hours = HoursInt + ((Minutes * 60) + Seconds)/3600.0;
	//Get JD
	double JD = 0.0;
	if(Month<=2)
		JD = (int)(365.25*(Year-1)) + (int)(30.6001*(Month+12+1)) + Day + Hours/24.0 + 1720981.5;
	else
		JD = (int)(365.25*(Year)) + (int)(30.6001*(Month+1)) + Day + Hours/24.0 + 1720981.5;
	
	int Week = (int)((JD - 2444244.5) / 7);
	int N =(int)(JD + 1.5)%7;
	if (GPSWeek) *GPSWeek = Week;
	return (N*24*3600 + HoursInt*3600 + Minutes*60 + Seconds);
}

//��ȡͷ�ļ�
void QReadSP3::readHeadData()
{
	if (isReadHead)
		return ;
	tempLine = m_readSP3FileClass.readLine();//��ȡ��һ��
	QString flagHeadEnd = "*";
	QString pre3Char = tempLine.mid(0,1);
	while (pre3Char != flagHeadEnd)
	{
		tempLine = m_readSP3FileClass.readLine();//��ȡ��һ��
		pre3Char = tempLine.mid(0,1);
	}
	//����ʱ��tempLine �����һ����Ԫ
	isReadHead = true;//debug:2017.07.08
}

//��ȡ�����ļ�����
void QReadSP3::readAllData2Vec()
{
	if (isReadAllData)
		return;
	if (!isReadHead)
		readHeadData();
	m_allEpochData.clear();//���ԭ������
	//�״ν���ʱ��tempLine �����һ����Ԫ * YYYY-MM.......
	int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0;
	double Seconds = 0;
	while (!m_readSP3FileClass.atEnd())
	{
		//�ж��Ƿ����
		if (tempLine.mid(0,3) == "EOF")
			break;
		//��ȡͷ����Ԫ����
		epochData.MatrixDataGPS.setZero();//��֮ǰ���������Ȼ�Ӵ���������ǰ�ȫ
		epochData.MatrixDataGlonass.setZero();
		epochData.MatrixDataBDS.setZero();
		epochData.MatrixDataGalieo.setZero();
		Year = tempLine.mid(3,4).toInt();
		Month = tempLine.mid(8,2).toInt();
		Day = tempLine.mid(11,2).toInt();
		Hours = tempLine.mid(14,2).toInt();
		Minutes = tempLine.mid(17,2).toInt();
		Seconds = tempLine.mid(20,11).toDouble();
		epochData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week) ;
		epochData.GPSWeek = Week;
		//��ȡ��������//��ȡ��������
		tempLine = m_readSP3FileClass.readLine();//��ȡһ����������
		while (tempLine.mid(0,3) != "EOF"&&tempLine.mid(0,1) == "P")
		{
			int PRN = 0;
			char tempSatType = '0';
			//GPSϵͳ
			tempSatType = *(tempLine.mid(1,1).toLatin1().data());
			PRN = tempLine.mid(2,2).toInt();
			if (tempSatType == 'G'&&isInSystem(tempSatType))
			{
				epochData.MatrixDataGPS(PRN - 1,0) = tempLine.mid(2,2).toDouble();//PRN
				epochData.MatrixDataGPS(PRN - 1,1) = 1000*tempLine.mid(5,13).toDouble();//X
				epochData.MatrixDataGPS(PRN - 1,2) = 1000*tempLine.mid(19,13).toDouble();//Y
				epochData.MatrixDataGPS(PRN - 1,3) = 1000*tempLine.mid(33,13).toDouble();//Z
			}
			//Glonassϵͳ
			else if (tempSatType == 'R'&&isInSystem(tempSatType))
			{
				epochData.MatrixDataGlonass(PRN - 1,0) = tempLine.mid(2,2).toDouble();//PRN
				epochData.MatrixDataGlonass(PRN - 1,1) = 1000*tempLine.mid(5,13).toDouble();//X
				epochData.MatrixDataGlonass(PRN - 1,2) = 1000*tempLine.mid(19,13).toDouble();//Y
				epochData.MatrixDataGlonass(PRN - 1,3) = 1000*tempLine.mid(33,13).toDouble();//Z
			}
			//BDSϵͳ
			else if (tempSatType == 'C'&&isInSystem(tempSatType))
			{
				epochData.MatrixDataBDS(PRN - 1,0) = tempLine.mid(2,2).toDouble();//PRN
				epochData.MatrixDataBDS(PRN - 1,1) = 1000*tempLine.mid(5,13).toDouble();//X
				epochData.MatrixDataBDS(PRN - 1,2) = 1000*tempLine.mid(19,13).toDouble();//Y
				epochData.MatrixDataBDS(PRN - 1,3) = 1000*tempLine.mid(33,13).toDouble();//Z
			}
			//Galieoϵͳ
			else if (tempSatType == 'E'&&isInSystem(tempSatType))
			{
				epochData.MatrixDataGalieo(PRN - 1,0) = tempLine.mid(2,2).toDouble();//PRN
				epochData.MatrixDataGalieo(PRN - 1,1) = 1000*tempLine.mid(5,13).toDouble();//X
				epochData.MatrixDataGalieo(PRN - 1,2) = 1000*tempLine.mid(19,13).toDouble();//Y
				epochData.MatrixDataGalieo(PRN - 1,3) = 1000*tempLine.mid(33,13).toDouble();//Z
			}
			tempLine = m_readSP3FileClass.readLine();//��ȡһ����������
		}
		m_allEpochData.append(epochData);//����һ���ļ�����
	}
	isReadAllData = true;
	m_readSP3FileClass.close();
}

//��ȡ����ļ�����(�˴�������ô����Ҳ����ֱ�ӵ������ڲ�����һ�����ļ���ȡ������Ĵ�����������)
void QReadSP3::readFileData2Vec(QStringList SP3FileNames)
{
	if (SP3FileNames.length() == 0) isReadAllData = true;
	if (isReadAllData)
		return;
	m_allEpochData.clear();
	//���ȶ�ȡͷ�ļ�����ʱ����С�����ȡ�ļ�
	int minGPSWeek = 999999999;//������С�� ����intԽ�磨��ת������ʱ��
	QVector< int > tempGPSWeek,tempGPSSeconds,fileFlagSeconds;//�����ļ��۲���ʵʱ��
	for (int i = 0;i < SP3FileNames.length();i++)
	{
		int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,GPSWeek = 0,GPSSeconds = 0;
		double Seconds = 0;
		QString Sp3FileName = SP3FileNames.at(i);
		QFile sp3file(Sp3FileName);
		if (!sp3file.open(QFile::ReadOnly))
		{
			QString erroInfo = "Open " + Sp3FileName + "faild!";
			ErroTrace(erroInfo);
		}
		//��ȡͷ�ļ�
		tempLine = sp3file.readLine();//��ȡ��һ��
		Year = tempLine.mid(3,4).toInt();
		Month = tempLine.mid(8,2).toInt();
		Day = tempLine.mid(11,2).toInt();
		Hours = tempLine.mid(14,2).toInt();
		Minutes = tempLine.mid(17,2).toInt();
		Seconds = tempLine.mid(20,11).toDouble();
		GPSSeconds = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&GPSWeek);
		if (GPSWeek <= minGPSWeek)
			minGPSWeek = GPSWeek;
		tempGPSWeek.append(GPSWeek);
		tempGPSSeconds.append(GPSSeconds);
		sp3file.close();
	}
	//ת��Ϊ��
	QVector< int > WeekOrder;//�������� ����Ƿ���ܣ��磺1 ���磺0//��ȡ����ļ���Ҫ�õ�
	for (int i = 0;i < tempGPSWeek.length();i++)
	{
		int Seconds = (tempGPSWeek.at(i) - minGPSWeek)*604800 + tempGPSSeconds.at(i);
		WeekOrder.append(tempGPSWeek.at(i) - minGPSWeek);
		fileFlagSeconds.append(Seconds);
	}
	//����ʱ������ļ���������
	for (int i = 0; i < fileFlagSeconds.length();i++)
	{
		for (int j = i+1;j < fileFlagSeconds.length();j++)
		{
			if (fileFlagSeconds.at(j) < fileFlagSeconds.at(i))//����
			{
				//�����ļ�����
				QString tempFileName = SP3FileNames.at(i);
				SP3FileNames[i] = SP3FileNames.at(j);
				SP3FileNames[j] = tempFileName;
				//�����ܱ�־
				int tempWeek = WeekOrder.at(i);
				WeekOrder[i] = WeekOrder.at(j);
				WeekOrder[j] = tempWeek;
			}
		}
	}
	m_WeekOrder = 0;
	for (int i = 0;i < WeekOrder.length();i++)
	{
		m_WeekOrder += WeekOrder[i];//������ܱ��
	}
	//����ʱ���С��ȡ�����ļ�
	for (int i = 0;i < SP3FileNames.length();i++)
	{
		QString Sp3FileName = SP3FileNames.at(i);
		//���ļ�
		QFile sp3file(Sp3FileName);
		if (!sp3file.open(QFile::ReadOnly))
		{
			QString erroInfo = "Open " + Sp3FileName + "faild!";
			ErroTrace(erroInfo);
		}
//��ȡͷ�ļ�
		tempLine = sp3file.readLine();//��ȡ��һ��
		QString flagHeadEnd = "*";
		QString pre3Char = tempLine.mid(0,1);
		while (pre3Char != flagHeadEnd)
		{
			tempLine = sp3file.readLine();//��ȡ��һ��
			pre3Char = tempLine.mid(0,1);
		}

//��ȡ���ݲ���
		//�״ν���ʱ��tempLine �����һ����Ԫ * YYYY-MM.......
		int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0;
		double Seconds = 0;
		while (!sp3file.atEnd())
		{
			//�ж��Ƿ����
			if (tempLine.mid(0,3) == "EOF")
				break;
			//��ȡͷ����Ԫ����
			epochData.MatrixDataGPS.setZero();//��֮ǰ���������Ȼ�Ӵ���������ǰ�ȫ
			epochData.MatrixDataGlonass.setZero();
			epochData.MatrixDataBDS.setZero();
			epochData.MatrixDataGalieo.setZero();
			Year = tempLine.mid(3,4).toInt();
			Month = tempLine.mid(8,2).toInt();
			Day = tempLine.mid(11,2).toInt();
			Hours = tempLine.mid(14,2).toInt();
			Minutes = tempLine.mid(17,2).toInt();
			Seconds = tempLine.mid(20,11).toDouble();
			epochData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week) + WeekOrder.at(i)*604800;//����ʱ�����604800s
			epochData.GPSWeek = Week;
			//��ȡ��������//��ȡ��������
			tempLine = sp3file.readLine();//��ȡһ����������
			while (tempLine.mid(0,3) != "EOF"&&tempLine.mid(0,1) == "P")
			{
				int PRN = 0;
				char tempSatType = '0';
				//GPSϵͳ
				tempSatType = *(tempLine.mid(1,1).toLatin1().data());
				PRN = tempLine.mid(2,2).toInt();
				if (tempSatType == 'G'&&isInSystem(tempSatType))
				{
					epochData.MatrixDataGPS(PRN - 1,0) = tempLine.mid(2,2).toDouble();//PRN
					epochData.MatrixDataGPS(PRN - 1,1) = 1000*tempLine.mid(5,13).toDouble();//X
					epochData.MatrixDataGPS(PRN - 1,2) = 1000*tempLine.mid(19,13).toDouble();//Y
					epochData.MatrixDataGPS(PRN - 1,3) = 1000*tempLine.mid(33,13).toDouble();//Z
				}
				//Glonassϵͳ
				else if (tempSatType == 'R'&&isInSystem(tempSatType))
				{
					epochData.MatrixDataGlonass(PRN - 1,0) = tempLine.mid(2,2).toDouble();//PRN
					epochData.MatrixDataGlonass(PRN - 1,1) = 1000*tempLine.mid(5,13).toDouble();//X
					epochData.MatrixDataGlonass(PRN - 1,2) = 1000*tempLine.mid(19,13).toDouble();//Y
					epochData.MatrixDataGlonass(PRN - 1,3) = 1000*tempLine.mid(33,13).toDouble();//Z
				}
				//BDSϵͳ
				else if (tempSatType == 'C'&&isInSystem(tempSatType))
				{
					epochData.MatrixDataBDS(PRN - 1,0) = tempLine.mid(2,2).toDouble();//PRN
					epochData.MatrixDataBDS(PRN - 1,1) = 1000*tempLine.mid(5,13).toDouble();//X
					epochData.MatrixDataBDS(PRN - 1,2) = 1000*tempLine.mid(19,13).toDouble();//Y
					epochData.MatrixDataBDS(PRN - 1,3) = 1000*tempLine.mid(33,13).toDouble();//Z
				}
				//Galieoϵͳ
				else if (tempSatType == 'E'&&isInSystem(tempSatType))
				{
					epochData.MatrixDataGalieo(PRN - 1,0) = tempLine.mid(2,2).toDouble();//PRN
					epochData.MatrixDataGalieo(PRN - 1,1) = 1000*tempLine.mid(5,13).toDouble();//X
					epochData.MatrixDataGalieo(PRN - 1,2) = 1000*tempLine.mid(19,13).toDouble();//Y
					epochData.MatrixDataGalieo(PRN - 1,3) = 1000*tempLine.mid(33,13).toDouble();//Z
				}
				tempLine = sp3file.readLine();//��ȡһ����������
			}
			m_allEpochData.append(epochData);//����һ���ļ�����
		}//��ȡ�ļ�����
		sp3file.close();
	}//for (int i = 0;i < SP3FileNames.length();i++)//��ȡ����ļ�����
	isReadAllData = true;
}

////pX,pY,pZ��lagrangeFact�������ꣻpGPST:lagrangeFact����GPS������;GPST���Ƿ���ʱ��������
void QReadSP3::get8Point(int PRN,char SatType,double *pX,double *pY,double *pZ,int *pGPST,double GPST)
{//��ȡ���ڽ���lagrangeFact����
	int lengthEpoch = m_allEpochData.length();
	//����GPST��������Ԫ�е�λ��
	int GPSTflag = m_lastGPSTimeFlag;
	int numPoint = lagrangeFact / 2;//ǰ��ȡnumPoint ����
	if (qAbs(m_lastCmpGPSTime - GPST) > 0.3)//����0.3s˵������һ����Ԫ����۲���Ԫ����϶���1s֮��(������ͬ��Ԫ��β�ѯλ��)
	{
		if (ACCELERATE)	m_lastCmpGPSTime = GPST;
		for (int i = m_lastGPSTimeFlag;i < lengthEpoch;i++)
		{
			SP3Data epochData = m_allEpochData.at(i);
			if (epochData.GPSTime >= GPST)
				break; 
			else
				GPSTflag++;
		}
	}
	if (ACCELERATE) m_lastGPSTimeFlag = GPSTflag;//�������µ�λ��

//ǰ��ȡ numPoint �� ���Ǳ߽�����
	if ((GPSTflag >= numPoint - 1) && (GPSTflag <= lengthEpoch - numPoint - 1))
	{//���м�λ��ǰ���ĸ��������� �����ĸ�����������
		for (int i = 0;i < lagrangeFact;i++)
		{
			SP3Data epochData = m_allEpochData.at(GPSTflag - numPoint + 1 + i);
			//�ж������Ǹ�ϵͳ����
			switch(SatType)
			{
			case 'G':
				pX[i] = epochData.MatrixDataGPS(PRN - 1,1);
				pY[i] = epochData.MatrixDataGPS(PRN - 1,2);
				pZ[i] = epochData.MatrixDataGPS(PRN - 1,3);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'R':
				pX[i] = epochData.MatrixDataGlonass(PRN - 1,1);
				pY[i] = epochData.MatrixDataGlonass(PRN - 1,2);
				pZ[i] = epochData.MatrixDataGlonass(PRN - 1,3);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'C':
				pX[i] = epochData.MatrixDataBDS(PRN - 1,1);
				pY[i] = epochData.MatrixDataBDS(PRN - 1,2);
				pZ[i] = epochData.MatrixDataBDS(PRN - 1,3);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'E':
				pX[i] = epochData.MatrixDataGalieo(PRN - 1,1);
				pY[i] = epochData.MatrixDataGalieo(PRN - 1,2);
				pZ[i] = epochData.MatrixDataGalieo(PRN - 1,3);
				pGPST[i] = epochData.GPSTime;
				break;
			default:
				pX[i] = 0;
				pY[i] = 0;
				pZ[i] = 0;
				pGPST[i] = 0;
			}
		}
	}
	else if(GPSTflag < numPoint - 1)
	{//�ڿ�ʼλ�ñ߽�
		for (int i = 0;i < lagrangeFact;i++)
		{
			SP3Data epochData = m_allEpochData.at(i);
			//�ж������Ǹ�ϵͳ����
			switch(SatType)
			{
			case 'G':
				pX[i] = epochData.MatrixDataGPS(PRN - 1,1);
				pY[i] = epochData.MatrixDataGPS(PRN - 1,2);
				pZ[i] = epochData.MatrixDataGPS(PRN - 1,3);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'R':
				pX[i] = epochData.MatrixDataGlonass(PRN - 1,1);
				pY[i] = epochData.MatrixDataGlonass(PRN - 1,2);
				pZ[i] = epochData.MatrixDataGlonass(PRN - 1,3);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'C':
				pX[i] = epochData.MatrixDataBDS(PRN - 1,1);
				pY[i] = epochData.MatrixDataBDS(PRN - 1,2);
				pZ[i] = epochData.MatrixDataBDS(PRN - 1,3);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'E':
				pX[i] = epochData.MatrixDataGalieo(PRN - 1,1);
				pY[i] = epochData.MatrixDataGalieo(PRN - 1,2);
				pZ[i] = epochData.MatrixDataGalieo(PRN - 1,3);
				pGPST[i] = epochData.GPSTime;
				break;
			default:
				pX[i] = 0;
				pY[i] = 0;
				pZ[i] = 0;
				pGPST[i] = 0;
			}
		}
	}
	else if(GPSTflag > lengthEpoch - numPoint - 1)
	{//�ڽ���λ�ñ߽�
		for (int i = 0;i < lagrangeFact;i++)
		{
			//debug:2017.07.08
			SP3Data epochData = m_allEpochData.at(lengthEpoch - (lagrangeFact-i));
			//�ж������Ǹ�ϵͳ����
			switch(SatType)
			{
			case 'G':
				pX[i] = epochData.MatrixDataGPS(PRN - 1,1);
				pY[i] = epochData.MatrixDataGPS(PRN - 1,2);
				pZ[i] = epochData.MatrixDataGPS(PRN - 1,3);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'R':
				pX[i] = epochData.MatrixDataGlonass(PRN - 1,1);
				pY[i] = epochData.MatrixDataGlonass(PRN - 1,2);
				pZ[i] = epochData.MatrixDataGlonass(PRN - 1,3);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'C':
				pX[i] = epochData.MatrixDataBDS(PRN - 1,1);
				pY[i] = epochData.MatrixDataBDS(PRN - 1,2);
				pZ[i] = epochData.MatrixDataBDS(PRN - 1,3);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'E':
				pX[i] = epochData.MatrixDataGalieo(PRN - 1,1);
				pY[i] = epochData.MatrixDataGalieo(PRN - 1,2);
				pZ[i] = epochData.MatrixDataGalieo(PRN - 1,3);
				pGPST[i] = epochData.GPSTime;
				break;
			default:
				pX[i] = 0;
				pY[i] = 0;
				pZ[i] = 0;
				pGPST[i] = 0;
			}
		}
	}
}

//�������շ���
void QReadSP3::lagrangeMethod(int PRN,char SatType,double GPST,double *pXYZ,double *pdXYZ)
{//�������ղ�ֵ �˴�ѡȡǰ��---��8��������ֵ GPST���Ƿ���ʱ��������
	for (int i = 0;i < 3;i++)
	{//��ʼ������ȫ
		pXYZ[i] = 0;
		pdXYZ[i] = 0;
	}
	//�ж��Ƿ����úϷ�ϵͳ
	if (!isInSystem(SatType)) 
		return ;
	//�ж��Ƿ��ȡ����
	if (!isReadAllData)
	{
		if (IsSigalFile)
			readAllData2Vec();
		else
			readFileData2Vec(m_SP3FileNames);
	}
	double pX[lagrangeFact]={0},pY[lagrangeFact] = {0},pZ[lagrangeFact] = {0};
	int pGPST[lagrangeFact] = {0};
	get8Point(PRN,SatType,pX,pY,pZ,pGPST,GPST);//���PRN���Ƿ���ʱ�����8������
	//���������Ƿ�ȱʧ�����������Ĺ����ֵ�����ݲ���ȱʧ��
	for (int i = 0;i <lagrangeFact;i++)
		if (!(pX[i]&&pY[i]&&pZ[i])) return ;

	double sumX = 0,sumY = 0,sumZ = 0;//��ֵ�������XYZ
	double sumDX[2] = {0},sumDY[2] = {0},sumDZ[2] = {0};//��ֵǰ��+-0.5s��������������ͬʱ��ȡ�ٶ�
	double lk = 1,ldk[2] = {1,1};
	for (int k = 0; k < lagrangeFact;k++)
	{
		for (int n = 0;n < lagrangeFact;n++)
		{
			if (k == n) continue;
			lk = lk*(GPST - pGPST[n])/(pGPST[k] - pGPST[n]);
			ldk[0] = ldk[0]*(GPST - 0.5 - pGPST[n])/(pGPST[k] - pGPST[n]);
			ldk[1] = ldk[1]*(GPST + 0.5 - pGPST[n])/(pGPST[k] - pGPST[n]);
		}
		sumX = sumX + pX[k]*lk;sumDX[0] = sumDX[0] + pX[k]*ldk[0];sumDX[1] = sumDX[1] + pX[k]*ldk[1];
		sumY = sumY + pY[k]*lk;sumDY[0] = sumDY[0] + pY[k]*ldk[0];sumDY[1] = sumDY[1] + pY[k]*ldk[1];
		sumZ = sumZ + pZ[k]*lk;sumDZ[0] = sumDZ[0] + pZ[k]*ldk[0];sumDZ[1] = sumDZ[1] + pZ[k]*ldk[1];

		lk = 1;ldk[0] = 1;ldk[1] = 1;
	}
	pXYZ[0] = sumX;pXYZ[1] = sumY;pXYZ[2] = sumZ;
	pdXYZ[0] = sumDX[1] - sumDX[0];pdXYZ[1] = sumDY[1] - sumDY[0];pdXYZ[2] = sumDZ[1] - sumDZ[0];
}

//��þ�������������ٶ�
void QReadSP3::getPrcisePoint(int PRN,char SatType,double GPST,double *pXYZ,double *pdXYZ)
{
	if (IsSigalFile)
		readAllData2Vec();
	else
		readFileData2Vec(m_SP3FileNames);
	//���λ��GPS�ܵ�һ��GPSTʱ����Ҫ����604800����ȡ����ԭ��
	if (!IsSigalFile&&m_WeekOrder > 0)
	{
		if (GPST < 24*3600)  GPST += 604800;
	}
	//if (GPST < 24*3600)  GPST += 604800;//���ǲ�ȫ��
	lagrangeMethod(PRN,SatType,GPST,pXYZ,pdXYZ);
}

//��ö�ȡ�����������
QVector< SP3Data > QReadSP3::getAllData()
{
	if (m_SP3FileNames.isEmpty()&&m_SP3FileName.isEmpty())
		return m_allEpochData;
	if (IsSigalFile)
		readAllData2Vec();
	else
		readFileData2Vec(m_SP3FileNames);
	return m_allEpochData;
}

//�ͷ���������
void QReadSP3::releaseAllData()
{
	m_allEpochData.clear();
}

bool QReadSP3::setSatlitSys(QString SystemStr)
{
	bool IsGood = QBaseObject::setSatlitSys(SystemStr);
	InitStruct();
	return IsGood;
}