// RoleData.h Created by knight-gongjian 2004.11.22.
//---------------------------------------------------------
#pragma once

#ifndef _ROLEDATA_H_
#define _ROLEDATA_H_


#include "GameAppNet.h"
#include "GameCommon.h"
#include "TryUtil.h"
//---------------------------------------------------------

namespace mission
{

	// ������������ṹ
	typedef struct _ROLE_ITEM_COMMAND
	{
		USHORT usCmd;								// ������Ŀ����
		char   szDesc[ROLE_MAXNUM_FUNCITEMSIZE];	// ������Ϣ	
		BYTE   byParam;								// ������Ϣ
		WORD   wParam;
		DWORD  dwParam;

		union
		{
			struct 
			{
				DWORD  dwParam1;
				DWORD  dwParam2;
			};
			char szParam[16];
		};

	} ROLE_ITEM_COMMAND, *PROLE_ITEM_COMMAND;

	// ��ʾ����Ի�������Ϣ
	typedef struct _ROLE_DESCRIPTION
	{
		BYTE byCmd;							// �����Ի�����ť����ID
		char szDesc[ROLE_MAXNUM_DESPSIZE];	// ������Ϣ

	} ROLE_DESCRIPTION, *PROLE_DESCRIPTION;

	//����ʾ������������Ϣ
	typedef struct _ROLE_FUNCTION
	{		
		BYTE byCount;		// ���������
		ROLE_ITEM_COMMAND FuncItem[ROLE_MAXNUM_FUNCITEM]; // ��Ӧ�Ĺ�����Ŀ

	} ROLE_FUNCTION, *PROLE_FUNCTION;

	// npc��Ϣ�洢�ṹ
	// ��ʷ���񵥸���ǽṹ
	typedef struct _ROLE_FLAGINFO
	{
		WORD  wRoleID;
		WORD  wParam;
		char  szFlag[ROLE_MAXNUM_FLAGSIZE];

		_ROLE_FLAGINFO()
		{
			Clear();	
		}

		void Clear()
		{
			wRoleID = 0;
			wParam  = 0;
			memset( szFlag, 0, ROLE_MAXNUM_FLAGSIZE*sizeof(char) );
		}

		BOOL Load( LPBYTE pBuf, DWORD dwSize )
		{
			if( dwSize >= ROLE_MAXNUM_RECORDSIZE ) return FALSE;
			memcpy( szFlag, pBuf, dwSize );
			return TRUE;
		}

		BOOL IsValid( WORD wFlag )
		{
			return wFlag < ROLE_MAXNUM_FLAGSIZE<<3;
		}

		BOOL IsSet( WORD wFlag )
		{
			if( wFlag >= ROLE_MAXNUM_FLAGSIZE<<3 ) return FALSE;
			DWORD dwIndex = wFlag>>3;// wFlag/ROLE_MAXNUM_INDEXSIZE;
			char szTag = 1<<(wFlag%ROLE_MAXNUM_INDEXSIZE);
			return szFlag[dwIndex] & szTag;
		}

		// ���û�������趨���
		BOOL SetFlag( WORD wFlag, BOOL bValid )
		{
			if( wFlag >= ROLE_MAXNUM_FLAGSIZE<<3 ) return FALSE;
			DWORD dwIndex = wFlag>>3;// wFlag/ROLE_MAXNUM_INDEXSIZE;
			char szTag = 1<<(wFlag%ROLE_MAXNUM_INDEXSIZE);
			if( bValid )
			{
				szFlag[dwIndex] |= szTag;
			}
			else 
			{
				szFlag[dwIndex] &= ~szTag;
			}

			return TRUE;
		}

	} ROLE_FLAGINFO, *PROLE_FLAGINFO;

	typedef struct _ROLE_EXFLAGINFO
	{
		DWORD dwRoleID;
		WORD  wParam;
		char  szFlag[ROLE_MAXNUM_FLAGSIZE];
		char  szParam[ROLE_MAXNUM_FLAGSIZE*4];

		_ROLE_EXFLAGINFO()
		{
			Clear();
		}

		void Clear()
		{
			dwRoleID = 0;
			memset( szFlag, 0, ROLE_MAXNUM_FLAGSIZE*sizeof(char) );
		}

		BOOL Load( LPBYTE pBuf, DWORD dwSize )
		{
			if( dwSize >= ROLE_MAXNUM_RECORDSIZE ) return FALSE;
			return TRUE;
		}

		BOOL IsValid( DWORD dwFlag )
		{
			return dwFlag < ROLE_MAXNUM_FLAGSIZE<<3;
		}

		BOOL IsSet( DWORD dwFlag )
		{
			DWORD dwIndex = dwFlag>>3; // dwFlag/ROLE_MAXNUM_INDEXSIZE;
			char szTag = 1<<(dwFlag%ROLE_MAXNUM_INDEXSIZE);
			return szFlag[dwIndex] & szTag;
		}

		// ���û�������趨���
		void SetFlag( DWORD dwFlag, BOOL bValid )
		{
			DWORD dwIndex = dwFlag>>3; // dwFlag/ROLE_MAXNUM_INDEXSIZE;
			char szTag = 1<<(dwFlag%ROLE_MAXNUM_INDEXSIZE);
			if( bValid )
			{
				szFlag[dwIndex] |= szTag;
			}
			else 
			{
				szFlag[dwIndex] &= ~szTag;
			}

			dwIndex = dwFlag>>1;
			DWORD dwTag = dwFlag&1;
			if( dwTag == 0 )
			{
				// ����λ
				szParam[dwIndex] &= 0xF;
			}
			else
			{
				// ����λ
				szParam[dwIndex] &= 0xF0;
			}
		}

		// ���ñ����Ӧ������Ϣ
		BOOL SetParam( DWORD dwFlag, BYTE byParam )
		{
			if( byParam >= ROLE_MAXVALUE_PARAM )
				return FALSE;

			DWORD dwIndex = dwFlag>>1;
			DWORD dwTag = dwFlag&1;
			if( dwTag == 0 )
			{
				// ����λ
				szParam[dwIndex] &= 0xF;
				szParam[dwIndex] |= byParam<<4;
			}
			else
			{
				// ����λ
				szParam[dwIndex] &= 0xF0;
				szParam[dwIndex] |= byParam;
			}
			return TRUE;
		}

		BYTE GetParam( DWORD dwFlag )
		{
			DWORD dwIndex = dwFlag>>1;
			DWORD dwTag = dwFlag&1;
			if( dwTag == 0 )
			{
				// ����λ
				return (szParam[dwIndex]&0xF0)>>4;
			}
			else
			{
				// ����λ
				return szParam[dwIndex]&0xF;
			}

			return TRUE;
		}
		
	} ROLE_EXFLAGINFO, *PROLE_EXFLAGINFO;

	// ���н�ɫ����ִ����ʷ�����ǽṹ
	typedef struct _ROLE_RECORDINFO
	{
		char  szID[ROLE_MAXNUM_RECORDSIZE];

		_ROLE_RECORDINFO()
		{
			Clear();
		}

		BOOL Load( LPBYTE pBuf, DWORD dwSize )
		{T_B
			if( dwSize >= ROLE_MAXNUM_RECORDSIZE<<3 ) return FALSE;
			memcpy( szID, pBuf, dwSize );
			return TRUE;
		T_E}

		void Clear()
		{T_B
			memset( szID, 0, ROLE_MAXNUM_RECORDSIZE*sizeof(char) );
		T_E}

		BOOL IsValid( DWORD dwID )
		{T_B
			return dwID < ROLE_MAXNUM_RECORDSIZE<<3;
		T_E}

		BOOL IsSet( DWORD dwID )
		{T_B
			if( dwID >= ROLE_MAXNUM_RECORDSIZE<<3 ) return FALSE;
			DWORD dwIndex = dwID>>3; // dwID/ROLE_MAXNUM_INDEXSIZE;
			char szTag = 1<<(dwID%ROLE_MAXNUM_INDEXSIZE);
			return szID[dwIndex] & szTag;
		T_E}

		// ���û�������趨���
		BOOL SetID( DWORD dwID, BOOL bValid )
		{T_B
			if( dwID >= ROLE_MAXNUM_RECORDSIZE<<3 ) return FALSE;
			DWORD dwIndex = dwID>>3; // dwID/ROLE_MAXNUM_INDEXSIZE;
			char szTag = 1<<(dwID%ROLE_MAXNUM_INDEXSIZE);
			if( bValid )
			{
				szID[dwIndex] |= szTag;
			}
			else 
			{
				szID[dwIndex] &= ~szTag;
			}

			return TRUE;
		T_E}

	} ROLE_RECORDINFO, *PROLE_RECORDINFO;


	// ��������Ϣ��¼
	typedef struct _TRIGGER_DATA
	{
		WORD wTriggerID;	// ������ID
		WORD wMissionID;	// ����ID(����ȡ��ʱ��Ҫ��⣬�����Ӧ����Ĵ�����)
		BYTE byType;		// �¼�����
		WORD wParam1;		// ����������3
		WORD wParam2;		// ����������4,��������
		WORD wParam3;		// (��������������������ʼ��¼��ǩ����ʼλ�ã����ݵ��������ű���ʹ��)
		WORD wParam4;		// 
		WORD wParam5;
		WORD wParam6;

		BYTE byIsDel : 1;	// ��־�������Ƿ�Ӧ��ɾ��
		BYTE byParam : 7;

	} TRIGGER_DATA, *PTRIGGER_DATA;

	enum { TRIGGER_VALID, TRIGGER_DELED };

	// ��������Ϣ��¼
	typedef struct _NPC_TRIGGER_DATA
	{
		WORD wTID;			// ������ID
		WORD wParam;
		BYTE byType;		// �¼�����
		WORD wParam1;		// ����������
		WORD wParam2;
		WORD wParam3;
		WORD wParam4;

	} NPC_TRIGGER_DATA, *PNPC_TRIGGER_DATA;

	// �������������Ϣ
	typedef struct _MISSION_DATA
	{
		WORD	wParam1;				// ��������ִ��������Ϣ����
		WORD	wParam2;
		WORD	wParam3;
		WORD	wParam4;
		WORD	wParam5;
		WORD	wParam6;

		void*	pData;					// ����ʱ��ʱ����
	} MISSION_DATA, *PMISSION_DATA;

	typedef struct _MISSION_INFO
	{
		WORD	wRoleID;				// ����ID
		BYTE	byState : 3;			// �������״̬
		BYTE	byMisType : 5;			// ��������
		BYTE	byType;					// ���������Ϣ����
		BYTE	byLevel;				// �ȼ���Ϣ
		BYTE	byNumData;				// ִ�����ݼ���
		DWORD	dwExp;					// ִ�и�����ɵõľ���
		DWORD	dwMoney;				// ִ�и�����Ľ�Ǯ����
		WORD	wItem;					// �������ĸ߼���Ʒ������ֵΪ0��ʾ�ޣ�������Ϊ1��
		WORD	wParam1;				// ����
		WORD	wParam2;
		ROLE_FLAGINFO RoleInfo;			// ��־������ID��Ϣ
		MISSION_DATA RandData[ROLE_MAXNUM_RAND_DATA];	// �������������Ϣ
		
	} MISSION_INFO, *PMISSION_INFO;

	// ��������������ṹ
	typedef struct _RAND_MISSION_COUNT
	{
		WORD	wRoleID;				// ����ID
		WORD	wCount;					// ������ɼ���
		WORD	wNum;					// ��������Ȧ��

	} RAND_MISSION_COUNT, *PRAND_MISSION_COUNT;

	// NPC������Ϣ״̬��¼
	typedef struct _STATE_DATA
	{
		BYTE byID : 5;		// ÿ��npc�Դ���������Ϣ�б������
		BYTE byState : 3;	// ��������״̬
	
	} STATE_DATA, *PSTATE_DATA;

	typedef struct _MISSION_STATE
	{
		DWORD dwNpcID;
		BYTE  byNpcState;
		BYTE  byMisNum;			// ������������
		STATE_DATA StateInfo[ROLE_MAXNUM_MISSIONSTATE];	// ����������Ϣ
	
	} MISSION_STATE, *PMISSION_STATE;
}

//---------------------------------------------------------

#endif // _ROLEDATA_H_