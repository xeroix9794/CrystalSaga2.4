// ������������ָϵͳ�����ĵ���״̬, �����,  ��,  ��
// ���ڸõ���״̬�µĿͻ��˱���Ϊ���ŷ�Χ��Ч


// Ŀǰ����ϵͳ��2������
// һ. Ϊ����ʿ�ı��ǳ�ֵ
// ��. Ӱ����ҵ��ƶ��ٶ�, ����ֵ��, �ḻ��Ϸ����

#pragma once

class SubMap;

class CWeather
{
public:

	CWeather(BYTE btType) // ����ʱ���봫����������
	:_dwLastLocationTick(0), _dwFrequence(2)
	{
		_btType = btType;
		_dwStateLastTime = 20; // Ĭ��Ϊ20��
	}
	
	void	SetRange(int sx, int sy, int w, int h);	// ָ��һ��������귶Χ
	void	RandLocation(SubMap *pMap);				// ��ָ���ķ�Χ��, ���ѡ��ص����
	void	SetFrequence(DWORD dwFre);				// �趨����Ƶ��, ��λ����
	void	SetStateLastTime(DWORD dwTime);			// ����״̬����ʱ��, ��λ����

protected:

	BYTE	_btType;  // �������� 0��  1��  2��
	int		_sx;	  // ��¼���귶Χ
	int		_sy;
	int		_w;
	int		_h;
	DWORD	_dwLastLocationTick;
	DWORD	_dwFrequence;
	DWORD	_dwStateLastTime;
};

inline void CWeather::SetRange(int sx, int sy, int w, int h)
{
	_sx = sx;
	_sy = sy;
	_w  = w;
	_h  = h;
}

inline void CWeather::SetFrequence(DWORD dwFre)
{
	_dwFrequence = dwFre;
}

inline void CWeather::SetStateLastTime(DWORD dwTime)
{
	_dwStateLastTime = dwTime;
}


// ����������, �������, ɾ��, ����������������Ϊÿ����ͼ����ʵ����һ��
class CWeatherMgr 
{
public:
	
	~CWeatherMgr();
	void	AddWeatherRange(CWeather *pWeather);
	void	RemoveWeather(CWeather *pWeather);
	void	Run(SubMap *pMap);	// ����ʱ������, �ڲ������ÿһ��Weather��RandLocation
	void	ClearAll();

protected:

	list<CWeather*>  _WeatherList;
};

inline void CWeatherMgr::AddWeatherRange(CWeather *pWeather)
{
	_WeatherList.push_back(pWeather);
}

inline void CWeatherMgr::RemoveWeather(CWeather *pWeather)
{
	_WeatherList.remove(pWeather);
	delete pWeather;
}

inline void CWeatherMgr::ClearAll()
{
	for(list<CWeather*>::iterator it = _WeatherList.begin(); it!=_WeatherList.end(); it++)
	{
		CWeather *pCur = (*it);
		delete pCur;
	}

	_WeatherList.clear();
}


