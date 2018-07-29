#include "QWrite2File.h"


QWrite2File::QWrite2File(void)
{
}


QWrite2File::~QWrite2File(void)
{
}

////����ENU����Ľ��д��txt
bool QWrite2File::writeRecivePos2Txt(QString fileName)
{
	if (allReciverPos.length() == 0)//���û�ж�ȡ���κ�����
		return false;

	QFile saveFile(fileName);//������ļ�
	if (!saveFile.open(QFile::WriteOnly|QFile::Text))
	{
		QString erroInfo = "Open " + fileName + " File Error!";
		ErroTrace(erroInfo);
		return false;
	}

	QTextStream saveFileOut(&saveFile);

	int lenRecivePos = allReciverPos.length();

	for (int i = 0;i <lenRecivePos;i++ )
	{
		RecivePos oneRecivePos = allReciverPos.at(i);
		saveFileOut.setFieldWidth(10);
		saveFileOut<<i;
		saveFileOut.setFieldWidth(2);
		saveFileOut<<": ";
		//���������ʱ����
		saveFileOut.setFieldWidth(4);
		saveFileOut<<QString::number(oneRecivePos.Year);
		saveFileOut.setFieldWidth(1);
		saveFileOut<<"-";
		saveFileOut.setFieldWidth(2);
		saveFileOut<<QString::number(oneRecivePos.Month);
		saveFileOut.setFieldWidth(1);
		saveFileOut<<"-";
		saveFileOut.setFieldWidth(2);
		saveFileOut<<QString::number(oneRecivePos.Day);
		saveFileOut.setFieldWidth(1);
		saveFileOut<<" ";
		saveFileOut.setFieldWidth(2);
		saveFileOut<<QString::number(oneRecivePos.Hours);
		saveFileOut.setFieldWidth(1);
		saveFileOut<<":";
		saveFileOut.setFieldWidth(2);
		saveFileOut<<QString::number(oneRecivePos.Minutes);
		saveFileOut.setFieldWidth(1);
		saveFileOut<<":";
		saveFileOut.setFieldWidth(7);
		saveFileOut<<QString::number(oneRecivePos.Seconds,'f',4);

		//����dE dN dU
		saveFileOut.setFieldWidth(10);
		saveFileOut<<QString::number(oneRecivePos.totolEpochStalitNum);
		saveFileOut.setFieldWidth(2);
		saveFileOut<<"  ";
		saveFileOut.setFieldWidth(10);
		saveFileOut<<QString::number(oneRecivePos.dX,'f',4);
		saveFileOut.setFieldWidth(2);
		saveFileOut<<"  ";
		saveFileOut.setFieldWidth(10);
		saveFileOut<<QString::number(oneRecivePos.dY,'f',4);
		saveFileOut.setFieldWidth(2);
		saveFileOut<<"  ";
		saveFileOut.setFieldWidth(10);
		saveFileOut<<QString::number(oneRecivePos.dZ,'f',4);
		saveFileOut.setFieldWidth(2);
		saveFileOut<<endl;
	}
	saveFileOut.flush();
	saveFile.close();

	return true;
}

//����������д��.ppp�ļ�
bool QWrite2File::writePPP2Txt(QString fileName)
{
	QFile saveFile(fileName);//������ļ�
	if (!saveFile.open(QFile::WriteOnly|QFile::Text))
	{
		QString erroInfo = "Open " + fileName + " File Error!";
		ErroTrace(erroInfo);
		return false;
	}
	QTextStream saveFileOut(&saveFile);

	int epochLen = allPPPSatlitData.length();
	if (0 == epochLen) return false;
	for (int i = 0;i < epochLen;i++)
	{
		QVector < SatlitData > epochSatlite = allPPPSatlitData.at(i);
		RecivePos recvPos = allReciverPos.at(i);
		int StallitNumbers = epochSatlite.length();
		//���ͷ��Ϣ
		saveFileOut<<"Satellite Number:";
		saveFileOut.setFieldWidth(3);
		saveFileOut<<StallitNumbers;
		saveFileOut<<",(yyyy-mm-dd-hh-mm-ss):";
		saveFileOut.setFieldWidth(4);
		saveFileOut<<recvPos.Year;
		saveFileOut.setFieldWidth(1);
		saveFileOut<<"-";
		saveFileOut.setFieldWidth(2);
		saveFileOut<<recvPos.Month;
		saveFileOut.setFieldWidth(1);
		saveFileOut<<"-";
		saveFileOut.setFieldWidth(2);
		saveFileOut<<recvPos.Day;
		saveFileOut.setFieldWidth(1);
		saveFileOut<<"-";
		saveFileOut.setFieldWidth(2);
		saveFileOut<<recvPos.Hours;
		saveFileOut.setFieldWidth(1);
		saveFileOut<<"-";
		saveFileOut.setFieldWidth(2);
		saveFileOut<<recvPos.Minutes;
		saveFileOut.setFieldWidth(10);
		saveFileOut.setFieldAlignment(QTextStream::AlignRight);
		saveFileOut.setRealNumberNotation(QTextStream::FixedNotation);
		saveFileOut<<recvPos.Seconds;
		saveFileOut<<",ztd:";
		saveFileOut<<endl;
		//���������Ϣ
		for (int j = 0;j < StallitNumbers;j++)
		{
			SatlitData oneStallit= epochSatlite.at(j);
			saveFileOut.setFieldWidth(1);
			saveFileOut<<oneStallit.SatType;
			saveFileOut.setFieldWidth(2);
			if (oneStallit.PRN < 10)
				saveFileOut<<"0"+QString::number(oneStallit.PRN);
			else
				saveFileOut<<oneStallit.PRN;

			saveFileOut.setFieldWidth(2);
			saveFileOut<<": ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.X,'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.Y,'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.Z,'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.StaClock,'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.EA[0],'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.EA[1],'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.SatTrop,'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.StaTropMap,'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.Relativty,'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.Sagnac,'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.TideEffect,'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.AntHeight,'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.SatOffset,'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.L1Offset,'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.L2Offset,'f',4);
			saveFileOut.setFieldWidth(2);
			saveFileOut<<", ";
			saveFileOut.setFieldWidth(14);
			saveFileOut<<QString::number(oneStallit.AntWindup,'f',4);
			//QString::number(oneStallit.pos[2],'f',4);

			saveFileOut<<endl;//�����Ԫ��һ�����ǵ�����
		}
	}
	return true;
}

//���춥ʪ�ӳٺ��Ӳ�д��txt ��һ��ʪ�ӳٵڶ����Ӳ�
bool QWrite2File::writeClockZTDW2Txt(QString fileName)
{
	if (allClock.length() == 0)//���û�ж�ȡ���κ�����
		return false;

	QFile saveFile(fileName);//������ļ�
	if (!saveFile.open(QFile::WriteOnly|QFile::Text))
	{
		QString erroInfo = "Open " + fileName + " File Error!";
		ErroTrace(erroInfo);
		return false;
	}

	QTextStream saveFileOut(&saveFile);

	int lenClock = allClock.length();

	for (int i = 0;i <lenClock;i++ )
	{
		ClockData epochZTDWClock = allClock.at(i);
		saveFileOut.setFieldWidth(10);
		saveFileOut<<i;
		saveFileOut.setFieldWidth(2);
		saveFileOut<<": ";
		//���������ʱ����
		saveFileOut.setFieldWidth(4);
		saveFileOut<<QString::number(epochZTDWClock.UTCTime.Year);
		saveFileOut.setFieldWidth(1);
		saveFileOut<<"-";
		saveFileOut.setFieldWidth(2);
		saveFileOut<<QString::number(epochZTDWClock.UTCTime.Month);
		saveFileOut.setFieldWidth(1);
		saveFileOut<<"-";
		saveFileOut.setFieldWidth(2);
		saveFileOut<<QString::number(epochZTDWClock.UTCTime.Day);
		saveFileOut.setFieldWidth(1);
		saveFileOut<<" ";
		saveFileOut.setFieldWidth(2);
		saveFileOut<<QString::number(epochZTDWClock.UTCTime.Hours);
		saveFileOut.setFieldWidth(1);
		saveFileOut<<":";
		saveFileOut.setFieldWidth(2);
		saveFileOut<<QString::number(epochZTDWClock.UTCTime.Minutes);
		saveFileOut.setFieldWidth(1);
		saveFileOut<<":";
		saveFileOut.setFieldWidth(7);
		saveFileOut<<QString::number(epochZTDWClock.UTCTime.Seconds,'f',4);

		//����dE dN dU
		saveFileOut.setFieldWidth(10);
		saveFileOut<<QString::number(2);
		saveFileOut.setFieldWidth(2);
		saveFileOut<<"  ";
		saveFileOut.setFieldWidth(10);
		saveFileOut<<QString::number(epochZTDWClock.ZTD_W,'f',4);
		saveFileOut.setFieldWidth(2);
		saveFileOut<<"  ";
		saveFileOut.setFieldWidth(16);
		saveFileOut<<QString::number(epochZTDWClock.clockData,'f',4);
		saveFileOut.setFieldWidth(2);
		saveFileOut<<"  ";
		saveFileOut.setFieldWidth(10);
		saveFileOut<<QString::number(0,'f',4);
		saveFileOut.setFieldWidth(2);
		saveFileOut<<endl;
	}
	saveFileOut.flush();
	saveFile.close();

	return true;
}

//��������д��txt�ļ���������Ϊ"G32.txt","C02.txt","R08.txt"����ʽ����һ��ģ����
bool QWrite2File::writeAmbiguity2Txt()
{
	//�ҵ��۲�����ϵͳ�����ǵ�����
	int lenSatAbm = allAmbiguity.length();
	QVector< char > store_SatType;
	QVector< int > store_SatPRN;
	for (int i = 0;i < lenSatAbm;i++)
	{
		Ambiguity oneSatAbm = allAmbiguity.at(i);
		char tSatType = '0';
		int tSatPRN = -1;
		bool isFind = false;
		for (int j = 0;j < store_SatPRN.length();j++)
		{
			tSatPRN = store_SatPRN.at(j);
			tSatType = store_SatType.at(j);
			if (tSatPRN == oneSatAbm.PRN && tSatType == oneSatAbm.SatType)
			{
				isFind = true;
				break;
			}
		}
		if (!isFind)
		{
			store_SatType.append(oneSatAbm.SatType);
			store_SatPRN.append(oneSatAbm.PRN);
		}
	}
	//��������������д���ļ�
	for (int i = 0;i < store_SatPRN.length();i++)
	{
		char tSatType = '0';
		int tSatPRN = -1;
		tSatType = store_SatType.at(i);
		tSatPRN = store_SatPRN.at(i);
		WriteAmbPRN(tSatPRN,tSatType);
	}
	return true;
}

//д��PRN�����ǵ�ģ����
bool QWrite2File::WriteAmbPRN(int PRN,char SatType)
{
	if (allAmbiguity.length() == 0)//���û�ж�ȡ���κ�����
		return false;
	QString fileName,strPRN;
	strPRN = QString::number(PRN);
	if(PRN < 10)
		strPRN = QString::number(0) + strPRN;
	fileName = QString(SatType) + strPRN + QString(".txt");
	fileName = ".//Ambiguity//" + fileName;
	QFile saveFile(fileName);//������ļ�
	if (!saveFile.open(QFile::WriteOnly|QFile::Text))
	{
		QString erroInfo = "Open " + fileName + " File Error!";
		ErroTrace(erroInfo);
		return false;
	}

	QTextStream saveFileOut(&saveFile);

	int lenAbm = allAmbiguity.length();
	for (int i = 0;i <lenAbm;i++ )
	{
		Ambiguity oneSatAmb = allAmbiguity.at(i);
		if (oneSatAmb.PRN != PRN || oneSatAmb.SatType != SatType)
		{
			continue;
		}
		saveFileOut.setFieldWidth(10);
		saveFileOut<<(oneSatAmb.epochNum - 1);
		saveFileOut.setFieldWidth(2);
		saveFileOut<<": ";
		//���GPS������
		saveFileOut.setFieldWidth(16);
		double GPSSecond = m_qcmpClass.YMD2GPSTime(oneSatAmb.UTCTime.Year,oneSatAmb.UTCTime.Month,oneSatAmb.UTCTime.Day,oneSatAmb.UTCTime.Hours,oneSatAmb.UTCTime.Minutes,oneSatAmb.UTCTime.Seconds);
		saveFileOut<<QString::number(GPSSecond,'f',6);
		saveFileOut.setFieldWidth(2);
		saveFileOut<<"  ";

		//д��ģ����
		saveFileOut.setFieldWidth(10);
		saveFileOut<<QString::number(1);
		saveFileOut.setFieldWidth(2);
		saveFileOut<<"  ";
		saveFileOut.setFieldWidth(16);
		if (oneSatAmb.isIntAmb)
			saveFileOut<<QString::number(qRound(oneSatAmb.Amb));
		else
			saveFileOut<<QString::number(oneSatAmb.Amb,'f',4);

		saveFileOut.setFieldWidth(2);
		saveFileOut<<"  ";
		
		saveFileOut<<endl;
	}
	saveFileOut.flush();
	saveFile.close();

	return true;
}

//������allAmbiguity�洢������д�뵽�ļ�
bool QWrite2File::WriteEpochPRN(QString fileName)
{
	if (allAmbiguity.length() == 0)//���û�ж�ȡ���κ�����
		return false;
	
	QFile saveFile(fileName);//������ļ�
	if (!saveFile.open(QFile::WriteOnly|QFile::Text))
	{
		QString erroInfo = "Open " + fileName + " File Error!";
		ErroTrace(erroInfo);
		return false;
	}
	QTextStream saveFileOut(&saveFile);
	//ֻд������
	int lengAllSat = allAmbiguity.length();
	QString tQstrPRN = "";
	for (int i = 0; i < lengAllSat;i++)
	{
		Ambiguity oneSatAmb = allAmbiguity.at(i);
		//д����Ԫ��
		saveFileOut.setFieldWidth(10);
		saveFileOut<<(oneSatAmb.epochNum - 1);
		saveFileOut.setFieldWidth(2);
		saveFileOut<<": ";
		//д���������ͺͱ�ţ�����G24,C01
		saveFileOut.setFieldWidth(1);
		saveFileOut<<QString(oneSatAmb.SatType);
		saveFileOut.setFieldWidth(2);
		if (oneSatAmb.PRN < 10)
			tQstrPRN = QString::number(0) + QString::number(oneSatAmb.PRN);
		else
			tQstrPRN = QString::number(oneSatAmb.PRN);
		saveFileOut<<tQstrPRN;
		saveFileOut<<endl;
	}
	saveFileOut.flush();
	saveFile.close();

	return true;
}
