#include "ProfileMonitor.h"

namespace ProfileMonitor
{
	MonitorManager* MonitorManager::_instance = NULL;

	void MonitorManager::Init()
	{
		TotalTick_ = 0;

		char c = 'a';

		FunNumMap_.clear();
		FunSpanList_.clear();

		for(int i = 0; i < 100; i++)
		{
			std::ostrstream buf;
			buf << (char)(c + i);
			buf << std::ends;

			FunSpanList_.push_back(FunSpanListT::value_type(buf.str(), 0));
		}

		LARGE_INTEGER lv;
		QueryPerformanceFrequency(&lv);
		TicksPerSecond_ = lv.QuadPart;
	}

	void MonitorManager::Dump()
	{
		if(!GetEnable())
		{
			std::cout << "���ܲ���δ����" << std::endl;
			return;
		}

		Lock_.Lock();

		try
		{
			FILE* file;
			char fileName[256];

			SYSTEMTIME systime;
			GetLocalTime(&systime);
			_snprintf_s(fileName, sizeof(fileName), _TRUNCATE, "log\\PM_%u%02u%02u%02u%02u%02u.csv", systime.wYear,systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);

			errno_t err = fopen_s(&file, fileName, "w+");

			if(err == 0)
			{
				fprintf_s(file, "�����ʱ,%I64i,ÿ��ʱ����,%I64i\n\n���,λ��,���ô���,������(ʱ��)\n", TotalTick_, TicksPerSecond_);

				// �밴�ջ���ʱ�� ���� ���� ���
				int i = 0;
				for(FunNumMapIt it = FunNumMap_.begin(); it != FunNumMap_.end(); it++, i++)
				{
					fprintf_s(file, "%d,\"%s\",%d,%I64i\n", (i + 1), it->first.c_str(), it->second.first, it->second.second);
				}

				fprintf_s(file, "\n\n���,λ��,�������(ʱ��)\n");

				i = 0;
				for(FunSpanIt it = FunSpanList_.begin(); it != FunSpanList_.end(); it++)
				{
					if( it->second > 0.0)
					{
						fprintf_s(file, "%d,\"%s\",%I64i\n", (i + 1), it->first.c_str(), it->second);
						i ++;
					}
				}

				fclose(file);

				std::cout << "���ܲ��Դ��̽���" << std::endl;
			}
			else
			{
				std::cout << "���ܲ���ʧ��" << std::endl;
			}
		}
		catch(...)
		{

		}

		Lock_.Unlock();
	}

}
