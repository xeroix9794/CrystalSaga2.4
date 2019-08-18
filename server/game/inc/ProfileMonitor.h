/**
 * Copyright (C), 2008-2010.
 * @file  ProfileMonitor.h
 * @brief Ч�ʼ�����
 * @history
 * 
 * @author Lark.Li
 * @version 1.0
 * @date 2008-11-18
 * @warning ʹ����STL��Windows API
 * 
 * @todo ��Ϊʱ��ԭ�򲻿��ǵ��ò�Σ�����
*/

#ifndef PROFILEMONITOR_H
#define PROFILEMONITOR_H


#include <map>
#include <list>
#include <string>
#include <utility>
#include <iostream>
#include <strstream>

#include <Windows.h>

#include <util.h>
#include "config.h"

//#define SET_MONITOR

#ifdef SET_MONITOR
#define MONITOR_STEP(step) ProfileMonitor::Monitor m(__FUNCSIG__, "" #step  "");
#define MONITOR_VALUE(step)		std::ostrstream buf;\
	buf << step;\
	buf << std::ends;\
	ProfileMonitor::Monitor m(__FUNCSIG__, buf.str());\
	buf.freeze(false);

#define MONITOR MONITOR_STEP(@)
#define MONITOR_DUMP if(ProfileMonitor::MonitorManager::Instance()->GetEnable())\
						ProfileMonitor::MonitorManager::Instance()->Dump();
#define MONITOR_ENABLE(flag) ProfileMonitor::MonitorManager::Instance()->SetEnable(flag);
#else
#define MONITOR_STEP(step) 
#define MONITOR_VALUE(step) 
#define MONITOR 
#define MONITOR_DUMP
#define MONITOR_ENABLE(flag)
#endif

namespace ProfileMonitor
{
	/**
	* @class MonitorManager 
	* @author Lark.Li
	* @brief ����Ч��(����ǩ�������ĵ�ʱ�䣬���õĴ��������ò�Σ�λ�ã�����)
	*/
	class MonitorManager
	{
	public:
		/**
		* @brief ʵ����
		* @return MonitorManager* ȫ��ʵ����
		*/
		static MonitorManager* Instance()
		{
			if(_instance == NULL)
			{
				std::string fileName;
				fileName.append("./");
				fileName.append(szConfigFileN);

				int enable = ::GetPrivateProfileInt("Monitor", "enable", 0, fileName.c_str());
				_instance = new MonitorManager(enable > 0);
			}

			return _instance;
		}

		/**
		* @brief �Ƿ�����
		* @return bool ���ñ�ʾ
		*/
		inline bool GetEnable()
		{
			return Enable_;
		}

		/**
		* @brief �������ñ�ʾ
		* @param[in]  bool  ���ñ�ʾ
		*/
		inline void SetEnable(bool Enable)
		{
			if(GetEnable() == Enable)
			{
				std::cout << "���ܲ�������δ�仯" << std::endl;
				return;
			}

			// ֮ǰ�ǿ���ʱ������Ϣ
			if(GetEnable())
				Dump();
			else
				Init();

			Enable_ = Enable;

			std::cout << "���ܲ������óɹ�" << std::endl;
		}

		/**
		* @brief ע�ắ��
		* @param[in]  string  ����ǩ��
		* @param[in]  LONGLONG  ������������
		*/
		inline void Register(std::string& funcsig_, LONGLONG span)
		{
			LONGLONG start =  GetTickCount();

			Lock_.Lock();

			try
			{
				// Step 1
				FunNumMapIt it = FunNumMap_.find(funcsig_);

				if(it != FunNumMap_.end())
				{
					it->second.first ++;
					it->second.second = ( it->second.second + span );
				}
				else
				{
					//2016ע�ʹ˴��Ѿ�֤ʵ����
					//FunNumMap_.insert(FunNumMapT::value_type(funcsig_, std::make_pair<int, LONGLONG>(1, span)));
				}

				// Step 2
				FunSpanIt it1 = FunSpanList_.end();
				FunSpanIt it2 = FunSpanList_.end();

				for(FunSpanIt it = FunSpanList_.begin(); it != FunSpanList_.end(); it++)
				{
					if(span > it->second)
					{
						it1 = it;
					}

					if(it->first.compare(funcsig_) == 0)
					{
						it2 = it;
					}
				}

				if(it1 != FunSpanList_.end())
				{
					FunSpanList_.insert(++it1, FunSpanListT::value_type(funcsig_, span));

					if(it2 != FunSpanList_.end())
					{
						FunSpanList_.erase(it2);
					}
					else
					{
						FunSpanList_.pop_front();
					}
				}

			}
			catch(...)
			{

			}

			Lock_.Unlock();

			TotalTick_ += (GetTickCount() - start);
		}

		/**
		* @brief ȡ�õ�ǰʱ��
		* @return LONGLONG ��ǰʱ��
		*/
		static LONGLONG GetTickCount()
		{
			LARGE_INTEGER lv;
			QueryPerformanceCounter( &lv );
			return lv.QuadPart;
		}

		void Dump();

	protected:
		/**
		* @brief ���캯��
		* @param[in]  bool ��ʼ���ñ�ʾ   
		*/
		MonitorManager(bool Enable = true)
		{ 
			Enable_ = Enable;

			if(GetEnable())
				Init();
		}
		virtual ~MonitorManager() 
		{ 
			FunNumMap_.clear();
			FunSpanList_.clear();
		}

	private:
		void Init();

	private:
		static MonitorManager* _instance;

		typedef std::pair<int, LONGLONG> NumSpanT;
		typedef std::map<std::string, NumSpanT> FunNumMapT;
		typedef std::map<std::string, NumSpanT>::iterator FunNumMapIt;

		typedef std::pair<std::string, LONGLONG> FunSpanT;
		typedef std::list<FunSpanT> FunSpanListT;
		typedef std::list<FunSpanT>::iterator FunSpanIt;

		FunNumMapT FunNumMap_;
		FunSpanListT FunSpanList_;

		CThrdLock	Lock_;

		LONGLONG TicksPerSecond_;
		LONGLONG TotalTick_;

		bool	Enable_;
	};

	class Monitor
	{
	public:
		Monitor(char* funcsig, const char* step)
		{
			if(!MonitorManager::Instance()->GetEnable())
			{
				start_ = 0;
			}
			else
			{
				start_ = MonitorManager::GetTickCount();

				if(step != "@")
				{
					char * p =NULL;

					std::ostrstream buf;
					buf << funcsig;
					buf << "#";
					buf << step;
					buf << std::ends;

					funcsig_ = buf.str();

					buf.freeze(false);
				}
				else
				{
					funcsig_ = funcsig;
				}

				//printf("Monitor %s\r\n", funcsig_.c_str()); 
			}
		};

		virtual ~Monitor()
		{
			if(start_ != 0)
			{
				LONGLONG span = MonitorManager::GetTickCount() - start_;
				MonitorManager::Instance()->Register(funcsig_, span);

				//printf("~Monitor %s %d\r\n", funcsig_.c_str(), span); 
			}
		};

	private:
		std::string funcsig_;
		LONGLONG start_;
	};

	//MonitorManager* MonitorManager::_instance = NULL;
}

#endif