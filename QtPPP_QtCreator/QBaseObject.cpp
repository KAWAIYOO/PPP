#include "QBaseObject.h"


//��ʼ������
void QBaseObject::inintVar()
{
	IsaddGPS = false;
	IsaddGLOSS = false;
	IsaddBDS = false;
	IsaddGalieo = false;
	m_SystemNum = 0;
}

//
QBaseObject::QBaseObject(void)
{
	inintVar();
}

//
QBaseObject::~QBaseObject(void)
{
}

//�����ļ�ϵͳ SystemStr:"G"(����GPSϵͳ);"GR":(����GPS+GLONASSϵͳ);"GRCE"(ȫ������)��
//����GPS,GLONASS,BDS,Galieo�ֱ�ʹ�ã���ĸG,R,C,E
bool QBaseObject::setSatlitSys(QString SystemStr)
{
	if (!(SystemStr.contains("G")||SystemStr.contains("R")||SystemStr.contains("C")||SystemStr.contains("E")))
		return	false;
	//
	if (SystemStr.contains("G"))
	{
		IsaddGPS = true;
		m_SystemNum++;
	}
	else
		IsaddGPS = false;
	//
	if (SystemStr.contains("R"))
	{
		IsaddGLOSS = true;
		m_SystemNum++;
	}
	else
		IsaddGLOSS = false;
	//
	if (SystemStr.contains("C"))
	{
		IsaddBDS = true;
		m_SystemNum++;
	}
	else
		IsaddBDS = false;
	//
	if (SystemStr.contains("E"))
	{
		IsaddGalieo = true;
		m_SystemNum++;
	}
	else
		IsaddGalieo = false;
	return true;
}


//Sys = G R C E(�ֱ����GPS��GLONASS��BDS��Galieoϵͳ)�ж��Ƿ���Ҫ��ϵͳ����
bool QBaseObject::isInSystem(char Sys)
{
	if (IsaddGPS&&Sys == 'G')
		return true;
	else if (IsaddGLOSS&&Sys == 'R')
		return true;
	else if (IsaddBDS&&Sys == 'C')
		return true;
	else if (IsaddGalieo&&Sys == 'E')
		return true;
	return false;
}

//��ȡ��ǰ���趨�˼���ϵͳ
int QBaseObject::getSystemnum()
{
	return m_SystemNum;
}
