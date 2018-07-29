#include "QReadClk.h"


QReadClk::QReadClk()
{

}

QReadClk::QReadClk(QString ClkFileName)
{
	initVar();
	openFiles(ClkFileName);
	IsSigalFile =true;
}
QReadClk::QReadClk(QStringList ClkFileNames)
{
	initVar();
	//debug:2017.07.08
	if (ClkFileNames.length() == 1)
	{
		QString ClkFileName = ClkFileNames.at(0);
		openFiles(ClkFileName);
		IsSigalFile = true;
	}
	if (ClkFileNames.length() > 0)
	{
		m_ClkFileNames =ClkFileNames;
		IsSigalFile = false;
	}
	else
	{
		isReadAllData = true;
		return ; 
	}
}

//��������
QReadClk::~QReadClk(void)
{
	releaseAllData();
}

void QReadClk::initVar()
{
	isReadHead = false;
	IsSigalFile = false;
	isReadAllData = false;
	m_ClkFileName = "";
	lagrangeFact = 10;
	//ֻ����GPSϵͳ���32������
	m_lastGPSTimeFlag = 0;//��Ϊ��ʼ���
	m_lastCmpGPSTime = 999999999;//��¼��ʼ�����GPSʱ��
	m_EndHeadNum = 8;
	m_WeekOrder = 0;
	InitStruct();
}

//��ʼ����Ҫ�õĽṹ���ڲ��ľ���(�н�Լ�ڴ�����)
void QReadClk::InitStruct()
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


void QReadClk::openFiles(QString ClkFileName)
{
	initVar();
	if (!ClkFileName.isEmpty())
	{
		m_readCLKFileClass.setFileName(ClkFileName);
		m_readCLKFileClass.open(QFile::ReadOnly);
		m_ClkFileName = ClkFileName;
	}
	else
	{
		isReadAllData = true;
		return;
	}
		
}

//ת��GPSʱ��
double QReadClk::YMD2GPSTime(int Year,int Month,int Day,int HoursInt,int Minutes,double Seconds,int *GPSWeek)//,int *GPSTimeArray
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
	if (GPSWeek)	*GPSWeek = Week;
	return (N*24*3600 + HoursInt*3600 + Minutes*60 + Seconds);
}

//��ȡͷ�ļ�
void QReadClk::readAllHead()
{
	if (isReadHead)
		return ;
	tempLine = m_readCLKFileClass.readLine();//��ȡ��һ��
	QString flagHeadEnd = "END";
	QString endHeadStr = tempLine.mid(60,20).trimmed();

	while (!endHeadStr.contains(flagHeadEnd,Qt::CaseInsensitive))
	{
		//����ͷ�ļ����ݶ�ȡ......
		//�˴�����ͷ�ļ�

		//��ȡ��һ��
		tempLine = m_readCLKFileClass.readLine();//��ȡ��һ��
		endHeadStr = tempLine.mid(60,20).trimmed();
	}
	tempLine = m_readCLKFileClass.readLine();//��ȡ��һ�н�����������
	isReadHead = true;
}

//��ȡ�����ļ���������
void QReadClk::readAllData()
{
	if (isReadAllData) return ;
	if (!isReadHead)	readAllHead();
	//�״ν���ʱ��tempLine AR......
	int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0;
	double Seconds = 0;
	while (!m_readCLKFileClass.atEnd())
	{
		//������������ͷ������
		while(tempLine.mid(0,3) != "AS ")
			tempLine = m_readCLKFileClass.readLine();
		//��ȡһ����Ԫ�����
		epochData.MatrixDataGPS.setZero();//��֮ǰ���������Ȼ�Ӵ���������ǰ�ȫ
		epochData.MatrixDataGlonass.setZero();
		epochData.MatrixDataBDS.setZero();
		epochData.MatrixDataGalieo.setZero();
		while (tempLine.mid(0,3) == "AS ")
		{//�����һ����Ԫ
			int PRN = 0;
			char tempSatType = '0';
			tempSatType = *(tempLine.mid(3,1).toLatin1().data());
			//GPSϵͳ
			if (isInSystem(tempSatType))
			{
				//��ȡ������ʱ����
				Year = tempLine.mid(8,4).toInt();
				Month = tempLine.mid(13,2).toInt();
				Day = tempLine.mid(16,2).toInt();
				Hours = tempLine.mid(19,2).toInt();
				Minutes = tempLine.mid(22,2).toInt();
				Seconds = tempLine.mid(25,9).toDouble();
				epochData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week);//
				epochData.GPSWeek = Week;
				//��ȡ����PRN ����� �����
				PRN = tempLine.mid(4,2).toInt();//PRN
				epochData.MatrixDataGPS(PRN - 1,0) = PRN;
				epochData.MatrixDataGPS(PRN - 1,1) = tempLine.mid(40,20).toDouble();//�����
				epochData.MatrixDataGPS(PRN - 1,2) = tempLine.mid(60,20).toDouble();//�����
			}
			//Glonassϵͳ
			else if (isInSystem(tempSatType))
			{
				//��ȡ������ʱ����
				Year = tempLine.mid(8,4).toInt();
				Month = tempLine.mid(13,2).toInt();
				Day = tempLine.mid(16,2).toInt();
				Hours = tempLine.mid(19,2).toInt();
				Minutes = tempLine.mid(22,2).toInt();
				Seconds = tempLine.mid(25,9).toDouble();
				epochData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week);//����ʱ�����604800s
				epochData.GPSWeek = Week;
				//��ȡ����PRN ����� �����
				PRN = tempLine.mid(4,2).toInt();//PRN
				epochData.MatrixDataGlonass(PRN - 1,0) = PRN;
				epochData.MatrixDataGlonass(PRN - 1,1) = tempLine.mid(40,20).toDouble();//�����
				epochData.MatrixDataGlonass(PRN - 1,2) = tempLine.mid(60,20).toDouble();//�����
			}
			//BDSϵͳ
			else if (isInSystem(tempSatType))
			{
				//��ȡ������ʱ����
				Year = tempLine.mid(8,4).toInt();
				Month = tempLine.mid(13,2).toInt();
				Day = tempLine.mid(16,2).toInt();
				Hours = tempLine.mid(19,2).toInt();
				Minutes = tempLine.mid(22,2).toInt();
				Seconds = tempLine.mid(25,9).toDouble();
				epochData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week);//����ʱ�����604800s
				epochData.GPSWeek = Week;
				//��ȡ����PRN ����� �����
				PRN = tempLine.mid(4,2).toInt();//PRN
				epochData.MatrixDataBDS(PRN - 1,0) = PRN;
				epochData.MatrixDataBDS(PRN - 1,1) = tempLine.mid(40,20).toDouble();//�����
				epochData.MatrixDataBDS(PRN - 1,2) = tempLine.mid(60,20).toDouble();//�����
			}
			//Galieoϵͳ
			else if (isInSystem(tempSatType))
			{
				//��ȡ������ʱ����
				Year = tempLine.mid(8,4).toInt();
				Month = tempLine.mid(13,2).toInt();
				Day = tempLine.mid(16,2).toInt();
				Hours = tempLine.mid(19,2).toInt();
				Minutes = tempLine.mid(22,2).toInt();
				Seconds = tempLine.mid(25,9).toDouble();
				epochData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week);//����ʱ�����604800s
				epochData.GPSWeek = Week;
				//��ȡ����PRN ����� �����
				PRN = tempLine.mid(4,2).toInt();//PRN
				epochData.MatrixDataGalieo(PRN - 1,0) = PRN;
				epochData.MatrixDataGalieo(PRN - 1,1) = tempLine.mid(40,20).toDouble();//�����
				epochData.MatrixDataGalieo(PRN - 1,2) = tempLine.mid(60,20).toDouble();//�����
			}
			tempLine = m_readCLKFileClass.readLine();//��ȡһ����������
		}
		m_allEpochData.append(epochData);
	}
	isReadAllData = true;
	m_readCLKFileClass.close();
	//�ж��������Ԫ���ȷ����ֵ�����
	CLKData epoch1 = m_allEpochData.at(0);
	CLKData epoch2 = m_allEpochData.at(1);
	if (qAbs(epoch1.GPSTime - epoch2.GPSTime) < 60)
		lagrangeFact = 2;
}

//��ȡ����ļ���������
void QReadClk::readFileData2Vec(QStringList ClkFileNames)
{
	if (ClkFileNames.length() == 0)	isReadAllData = true;
	if (isReadAllData)
		return;
	m_allEpochData.clear();
	//���ȶ�ȡͷ�ļ�����ʱ����С�����ȡ�ļ�
	int minGPSWeek = 999999999;//������С�� ����intԽ�磨��ת������ʱ��
	QVector< int > tempGPSWeek,fileFlagSeconds;//�����ļ��۲���ʵʱ��
	QVector< double > tempGPSSeconds;
	for (int i = 0;i < ClkFileNames.length();i++)
	{
		int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,GPSWeek = 0;
		double Seconds = 0,GPSSeconds = 0;
		QString CLKFileName = ClkFileNames.at(i);
		QFile clkfile(CLKFileName);
		if (!clkfile.open(QFile::ReadOnly))
		{
			QString erroInfo = "Open " + CLKFileName + "faild!(QReadClk::readFileData2Vec)";
			ErroTrace(erroInfo);
		}
		//����ͷ�ļ�
		do 
		{
			tempLine = clkfile.readLine();//��ȡ��һ��
			if (clkfile.atEnd())
			{
				ErroTrace("Can not read clk file!(QReadClk::readFileData2Vec)");
				break;
			}
		} while (!tempLine.contains("END OF HEADER",Qt::CaseInsensitive));
		//����AS�ж�Ӧ��ʱ��
		do 
		{
			tempLine = clkfile.readLine();//��ȡ��һ��
			if (clkfile.atEnd())
			{
				ErroTrace("Can not read clk file!(QReadClk::readFileData2Vec)");
				break;
			}
		} while (!tempLine.mid(0,2).contains("AS",Qt::CaseInsensitive));
		//��ȡʱ��
		Year = tempLine.mid(8,4).toInt();
		Month = tempLine.mid(13,2).toInt();
		Day = tempLine.mid(16,2).toInt();
		Hours = tempLine.mid(19,2).toInt();
		Minutes = tempLine.mid(22,2).toInt();
		Seconds = tempLine.mid(25,11).toDouble();
		GPSSeconds = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&GPSWeek);
		if (GPSWeek <= minGPSWeek)
			minGPSWeek = GPSWeek;
		tempGPSWeek.append(GPSWeek);
		tempGPSSeconds.append(GPSSeconds);
		clkfile.close();
	}
	//ת��Ϊ��
	QVector< int > WeekOrder;//�������� ����Ƿ���ܣ��磺1 ���磺0//��ȡ����ļ���Ҫ�õ�
	for (int i = 0;i < tempGPSWeek.length();i++)
	{
		double Seconds = (tempGPSWeek.at(i) - minGPSWeek)*604800 + tempGPSSeconds.at(i);
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
				QString tempFileName = ClkFileNames.at(i);
				ClkFileNames[i] = ClkFileNames.at(j);
				ClkFileNames[j] = tempFileName;
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
	for (int i = 0;i < ClkFileNames.length();i++)
	{
		QString CLKFileName = ClkFileNames.at(i);
//���ļ�
		QFile clkfile(CLKFileName);
		if (!clkfile.open(QFile::ReadOnly))
		{
			QString erroInfo = "Open " + CLKFileName + "faild!(QReadClk::readFileData2Vec)";
			ErroTrace(erroInfo);
		}
//��ȡͷ�ļ�����
		tempLine = clkfile.readLine();//��ȡ��һ��
		QString flagHeadEnd = "END";
		QString endHeadStr = tempLine.mid(60,20).trimmed();
		while (!endHeadStr.contains(flagHeadEnd,Qt::CaseInsensitive))
		{
			//����ͷ�ļ����ݶ�ȡ......
			//�˴�����ͷ�ļ�

			//��ȡ��һ��
			tempLine = clkfile.readLine();//��ȡ��һ��
			endHeadStr = tempLine.mid(60,20).trimmed();
		}
		tempLine = clkfile.readLine();//��ȡ��һ�н�����������
//�׸��ļ���ȡβ��8������
		if (i == 0&&ClkFileNames.length() >= 3)
		{
			int flag = 0,i = 1,CharNum = 80;
			QString preStr = "AS";//����֮ǰ��ÿ��ǰ�����ַ�
			//�ж��ļ���һ��60�ַ�����80�ַ�
			clkfile.seek(clkfile.size() - 80);//�е��Ӳ��ļ���80�е���60
			tempLine = clkfile.readLine();//��ȡһ��
			//�ڶ����ַ�����S����60�ַ�
			if (tempLine.mid(1,1) != "S")
				CharNum = 60;
			do 
			{
				clkfile.seek(clkfile.size() - i*CharNum);//����ÿ��һ����80�ַ�
				i++;
				tempLine = clkfile.readLine();
				if (tempLine.mid(0,2) == "AR" && preStr == "AS")
				{
					flag++;
				}
				preStr = tempLine.mid(0,2);
			} while (flag < m_EndHeadNum);
		}

//���һ���ļ���ȡͷ��8������
		bool isEndFile = false;//�б��Ƿ������һ���ļ����
		int endFileBlockNum = 0;//��ȡ���һ���ļ��������
		if (i == (ClkFileNames.length()-1)&&ClkFileNames.length() >= 3)
		{
			isEndFile = true;
		}
//��ȡ��������
		//�״ν���ʱ��tempLine AR......
		int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0;
		double Seconds = 0;
		while (!clkfile.atEnd())
		{
			//������������ͷ������
			while(tempLine.mid(0,3) != "AS ")
				tempLine = clkfile.readLine();
			//��ȡһ����Ԫ�����
			epochData.MatrixDataGPS.setZero();//��֮ǰ���������Ȼ�Ӵ���������ǰ�ȫ
			epochData.MatrixDataGlonass.setZero();
			epochData.MatrixDataBDS.setZero();
			epochData.MatrixDataGalieo.setZero();
			while (tempLine.mid(0,3) == "AS ")
			{//�����һ����Ԫ
				int PRN = 0;
				char tempSatType = '0';
				tempSatType = *(tempLine.mid(3,1).toLatin1().data());
				//GPSϵͳ
				if (tempSatType == 'G'&&isInSystem(tempSatType))
				{
					//��ȡ������ʱ����
					Year = tempLine.mid(8,4).toInt();
					Month = tempLine.mid(13,2).toInt();
					Day = tempLine.mid(16,2).toInt();
					Hours = tempLine.mid(19,2).toInt();
					Minutes = tempLine.mid(22,2).toInt();
					Seconds = tempLine.mid(25,9).toDouble();
					epochData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week) + WeekOrder.at(i)*604800;//����ʱ�����604800s
					epochData.GPSWeek = Week;
					//��ȡ����PRN ����� �����
					PRN = tempLine.mid(4,2).toInt();//PRN
					epochData.MatrixDataGPS(PRN - 1,0) = PRN;
					epochData.MatrixDataGPS(PRN - 1,1) = tempLine.mid(40,20).toDouble();//�����
					epochData.MatrixDataGPS(PRN - 1,2) = tempLine.mid(60,20).toDouble();//�����
				}
				//Glonassϵͳ
				else if (tempSatType == 'R'&&isInSystem(tempSatType))
				{
					//��ȡ������ʱ����
					Year = tempLine.mid(8,4).toInt();
					Month = tempLine.mid(13,2).toInt();
					Day = tempLine.mid(16,2).toInt();
					Hours = tempLine.mid(19,2).toInt();
					Minutes = tempLine.mid(22,2).toInt();
					Seconds = tempLine.mid(25,9).toDouble();
					epochData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week) + WeekOrder.at(i)*604800;//����ʱ�����604800s
					epochData.GPSWeek = Week;
					//��ȡ����PRN ����� �����
					PRN = tempLine.mid(4,2).toInt();//PRN
					epochData.MatrixDataGlonass(PRN - 1,0) = PRN;
					epochData.MatrixDataGlonass(PRN - 1,1) = tempLine.mid(40,20).toDouble();//�����
					epochData.MatrixDataGlonass(PRN - 1,2) = tempLine.mid(60,20).toDouble();//�����
				}
				//BDSϵͳ
				else if (tempSatType == 'C'&&isInSystem(tempSatType))
				{
					//��ȡ������ʱ����
					Year = tempLine.mid(8,4).toInt();
					Month = tempLine.mid(13,2).toInt();
					Day = tempLine.mid(16,2).toInt();
					Hours = tempLine.mid(19,2).toInt();
					Minutes = tempLine.mid(22,2).toInt();
					Seconds = tempLine.mid(25,9).toDouble();
					epochData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week) + WeekOrder.at(i)*604800;//����ʱ�����604800s
					epochData.GPSWeek = Week;
					//��ȡ����PRN ����� �����
					PRN = tempLine.mid(4,2).toInt();//PRN
					epochData.MatrixDataBDS(PRN - 1,0) = PRN;
					epochData.MatrixDataBDS(PRN - 1,1) = tempLine.mid(40,20).toDouble();//�����
					epochData.MatrixDataBDS(PRN - 1,2) = tempLine.mid(60,20).toDouble();//�����
				}
				//Galieoϵͳ
				else if (tempSatType == 'E'&&isInSystem(tempSatType))
				{
					//��ȡ������ʱ����
					Year = tempLine.mid(8,4).toInt();
					Month = tempLine.mid(13,2).toInt();
					Day = tempLine.mid(16,2).toInt();
					Hours = tempLine.mid(19,2).toInt();
					Minutes = tempLine.mid(22,2).toInt();
					Seconds = tempLine.mid(25,9).toDouble();
					epochData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week) + WeekOrder.at(i)*604800;//����ʱ�����604800s
					epochData.GPSWeek = Week;
					//��ȡ����PRN ����� �����
					PRN = tempLine.mid(4,2).toInt();//PRN
					epochData.MatrixDataGalieo(PRN - 1,0) = PRN;
					epochData.MatrixDataGalieo(PRN - 1,1) = tempLine.mid(40,20).toDouble();//�����
					epochData.MatrixDataGalieo(PRN - 1,2) = tempLine.mid(60,20).toDouble();//�����
				}
				tempLine = clkfile.readLine();//��ȡһ����������
			}
			m_allEpochData.append(epochData);
			if (isEndFile)
			{
				endFileBlockNum++;
				//��ֹ��ȡ���һ���ļ�
				if (endFileBlockNum >=m_EndHeadNum)	break;
			}
		}//��ȡ�ļ����ݽ���while (!clkfile.atEnd())
		clkfile.close();
	}//for (int i = 0;i < ClkFileNames.length();i++)
	isReadAllData = true;
	//�ж��������Ԫ���ȷ����ֵ�����
	CLKData epoch1 = m_allEpochData.at(20);
	CLKData epoch2 = m_allEpochData.at(21);
	if (qAbs(epoch1.GPSTime - epoch2.GPSTime) < 60)
		lagrangeFact = 2;
}

//��������
QVector< CLKData > QReadClk::getAllData()
{
	if (IsSigalFile)	
		readAllData();
	else
		readFileData2Vec(m_ClkFileNames);
	return m_allEpochData;
}

//�ͷ���������
void QReadClk::releaseAllData()
{
	m_allEpochData.clear();
}

////pX,pY,pZ��lagrangeFact�������ꣻpGPST:lagrangeFact����GPS������;GPST���Ƿ���ʱ��������
void QReadClk::get8Point(int PRN,char SatType,double *pCLKT,double *pGPST,double GPST)
{//��ȡ���ڽ���lagrangeFact����
	//����Ƿ����
	int lengthEpoch = m_allEpochData.length();
	//����GPST��������Ԫ�е�λ��
	int GPSTflag = m_lastGPSTimeFlag;
	int numPoint = lagrangeFact / 2;//ǰ��ȡnumPoint ����
	if (qAbs(m_lastCmpGPSTime - GPST) > 0.3)//����0.3s˵������һ����Ԫ����۲���Ԫ����϶���1s֮��(������ͬ��Ԫ��β�ѯλ��)
	{
		if (ACCELERATE)	m_lastCmpGPSTime = GPST;
		for (int i = m_lastGPSTimeFlag;i < lengthEpoch;i++)
		{
			CLKData epochData = m_allEpochData.at(i);
			if (epochData.GPSTime >= GPST)
				break; 
			else
				GPSTflag++;
		}
		//�����ֵʱ���������Ǵ���0.00n�� ����GPSTflag��1 ��Ҫ��һ
		if (numPoint == 1) GPSTflag--;
	}



//ǰ��ȡ lagrangeFact �� ���Ǳ߽�����

	if (GPSTflag < 0) GPSTflag = 0;
	if (ACCELERATE)	m_lastGPSTimeFlag = GPSTflag;//�������µ�λ��

	if ((GPSTflag >= numPoint - 1) && (GPSTflag <= lengthEpoch - numPoint - 1))
	{//���м�λ��ǰ���ĸ��������� �����ĸ�����������
		for (int i = 0;i < lagrangeFact;i++)
		{
			CLKData epochData = m_allEpochData.at(GPSTflag - numPoint + 1 + i);
			//�ж������Ǹ�ϵͳ����
			switch (SatType)
			{
			case 'G':
				pCLKT[i] = epochData.MatrixDataGPS(PRN - 1,1);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'R':
				pCLKT[i] = epochData.MatrixDataGlonass(PRN - 1,1);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'C':
				pCLKT[i] = epochData.MatrixDataBDS(PRN - 1,1);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'E':
				pCLKT[i] = epochData.MatrixDataGalieo(PRN - 1,1);
				pGPST[i] = epochData.GPSTime;
				break;
			default:
				pCLKT[i] = 0;
				pGPST[i] = 0;
			}
		}
	}
	else if(GPSTflag < numPoint - 1)
	{//�ڿ�ʼλ�ñ߽�
		for (int i = 0;i < lagrangeFact;i++)
		{
			CLKData epochData = m_allEpochData.at(i);
			//�ж������Ǹ�ϵͳ����
			switch (SatType)
			{
			case 'G':
				pCLKT[i] = epochData.MatrixDataGPS(PRN - 1,1);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'R':
				pCLKT[i] = epochData.MatrixDataGlonass(PRN - 1,1);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'C':
				pCLKT[i] = epochData.MatrixDataBDS(PRN - 1,1);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'E':
				pCLKT[i] = epochData.MatrixDataGalieo(PRN - 1,1);
				pGPST[i] = epochData.GPSTime;
				break;
			default:
				pCLKT[i] = 0;
				pGPST[i] = 0;
			}
		}
	}
	else if(GPSTflag > lengthEpoch - numPoint - 1)
	{//�ڽ���λ�ñ߽�
		for (int i = 0;i < lagrangeFact;i++)
		{
			//debug:2017.07.08
			CLKData epochData = m_allEpochData.at(lengthEpoch - (lagrangeFact-i));
			//�ж������Ǹ�ϵͳ����
			switch (SatType)
			{
			case 'G':
				pCLKT[i] = epochData.MatrixDataGPS(PRN - 1,1);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'R':
				pCLKT[i] = epochData.MatrixDataGlonass(PRN - 1,1);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'C':
				pCLKT[i] = epochData.MatrixDataBDS(PRN - 1,1);
				pGPST[i] = epochData.GPSTime;
				break;
			case 'E':
				pCLKT[i] = epochData.MatrixDataGalieo(PRN - 1,1);
				pGPST[i] = epochData.GPSTime;
				break;
			default:
				pCLKT[i] = 0;
				pGPST[i] = 0;
			}
		}
	}
}


//�������շ���
void QReadClk::lagrangeMethod(int PRN,char SatType,double GPST,double *pCLKT)
{//�������ղ�ֵ �˴�ѡȡǰ��---��8��������ֵ GPST���Ƿ���ʱ��������
	*pCLKT = 0;
	//�ж��Ƿ����úϷ�ϵͳ
	if (!isInSystem(SatType)) 
		return ;
	//�ж��Ƿ��ȡ����
	if (!isReadAllData) 
	{
		if (IsSigalFile)
			readAllData();
		else
			readFileData2Vec(m_ClkFileNames);
	}
	double m_CLKT[12] = {0};//�����ȡ�Ĳ�ֵ����
	double m_pGPST[12] = {0};
	get8Point(PRN,SatType,m_CLKT,m_pGPST,GPST);//���PRN���Ƿ���ʱ�����8����2�����Ӳ�
	//�������������ԣ������Ӳ��ֵ�����ݲ���ȱʧ��
	for (int i = 0;i <lagrangeFact;i++)
		if (!m_CLKT[i]) return ;
	//�������ղ�ֵ
	double sumCLK = 0;//��ֵ�������XYZ
	for (int k = 0; k < lagrangeFact;k++)
	{
		double lk = 1;
		for (int n = 0;n < lagrangeFact;n++)
		{
			if (k == n) continue;
			lk *= (GPST - m_pGPST[n])/(m_pGPST[k] - m_pGPST[n]);
		}
		sumCLK += m_CLKT[k]*lk;
	}
	*pCLKT = sumCLK*M_C;
}



//��÷���ʱ�����������(��ϵͳ)
void QReadClk::getStaliteClk(int PRN,char SatType,double GPST,double *pCLKT)
{
	if (IsSigalFile)	
		readAllData();
	else
		readFileData2Vec(m_ClkFileNames);
	//���λ��GPS�ܵ�һ��GPSTʱ����Ҫ����604800����ȡ����ԭ��
	if (!IsSigalFile&&m_WeekOrder > 0)
	{
		if (GPST < 24*3600)  GPST += 604800;
	}
	//if (GPST < 24*3600)  GPST += 604800;//���ǲ�ȫ��
	lagrangeMethod(PRN,SatType,GPST,pCLKT);
}

bool QReadClk::setSatlitSys(QString SystemStr)
{
	bool IsGood = QBaseObject::setSatlitSys(SystemStr);
	InitStruct();
	return IsGood;
}