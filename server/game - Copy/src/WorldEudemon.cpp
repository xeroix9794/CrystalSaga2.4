// WorldEudemon.cpp Created by knight-gongjian 2005.3.9.
//---------------------------------------------------------

#include "WorldEudemon.h"
#include "SubMap.h"
#include "GameApp.h"
#include "GameAppNet.h"
#include "Script.h"
#include "lua_gamectrl.h"
//---------------------------------------------------------

namespace mission
{
	CWorldEudemon g_WorldEudemon;

	CWorldEudemon::CWorldEudemon()
	: CNpc()
	{		 
	}

	CWorldEudemon::~CWorldEudemon()
	{
	}

	BOOL CWorldEudemon::InitScript( const char szFunc[], const char szName[] )
	{T_B
		if( szFunc[0] == '0' ) return TRUE;

		// ��ʼ��NPC�ű�ȫ�ֱ�����Ϣ
		lua_getglobal( g_pLuaState, "ResetNpcInfo" );
		if( !lua_isfunction( g_pLuaState, -1 ) )
		{
			lua_pop( g_pLuaState, 1 );
			LG( "lua_invalidfunc", "ResetNpcInfo" );
			return FALSE;
		}

		lua_pushlightuserdata( g_pLuaState, this );
		lua_pushstring( g_pLuaState, szName );		

		int nStatus = lua_pcall( g_pLuaState, 2, 0, 0 );
		if( nStatus )
		{
			//LG( "NpcInit", "npc[%s]�Ľű���ʼ��������[ResetNpcInfo]����ʧ��!", szName );
			LG( "NpcInit", "npc[%s]'s script init dispose function[ResetNpcInfo]transfer error!", szName );
			//printf( "npc[%s]�Ľű���ʼ��������[ResetNpcInfo]����ʧ��!\n", szName );
			printf( RES_STRING(GM_WORLDEUDEMON_CPP_00001), szName );
			lua_callalert( g_pLuaState, nStatus );
			lua_settop(g_pLuaState, 0);
			return FALSE;
		}
		lua_settop(g_pLuaState, 0);

		// ����NPC��ʼ��ȫ�ֱ�����Ϣ��ں���
		lua_getglobal( g_pLuaState, szFunc );
		if( !lua_isfunction( g_pLuaState, -1 ) )
		{
			lua_pop( g_pLuaState, 1 );
			LG( "lua_invalidfunc", szFunc );
			return FALSE;
		}

		nStatus = lua_pcall( g_pLuaState, 0, 0, 0 );
		if( nStatus )
		{
			//LG( "NpcInit", "npc[%s]�Ľű����ݴ�����[%s]����ʧ��!", szName, szFunc );
			LG( "NpcInit", "npc[%s]'s script data dispose function[%s]transfer failed!", szName, szFunc );
			//printf( "npc[%s]�Ľű����ݴ�����[%s]����ʧ��!\n", szName, szFunc );
			//printf( RES_STRING(GM_WORLDEUDEMON_CPP_00002), szName, szFunc );
			char szData[128];
			CFormatParameter param(2);
			param.setString( 0, szName );
			param.setString( 1, szFunc );
			RES_FORMAT_STRING( GM_WORLDEUDEMON_CPP_00002, param, szData );
			printf( szData );
			lua_callalert( g_pLuaState, nStatus );
			lua_settop(g_pLuaState, 0);
			return FALSE;
		}
		lua_settop(g_pLuaState, 0);

		// ��ȡNPC�ĶԻ���Ϣ�ͽ�����Ϣ
		lua_getglobal( g_pLuaState, "GetNpcInfo" );
		if( !lua_isfunction( g_pLuaState, -1 ) )
		{
			lua_pop( g_pLuaState, 1 );
			LG( "lua_invalidfunc", "GetNpcInfo" );
			return FALSE;
		}

		lua_pushlightuserdata( g_pLuaState, this );
		lua_pushstring( g_pLuaState, szName );

		nStatus = lua_pcall( g_pLuaState, 2, 0, 0 );
		if( nStatus )
		{
			//LG( "NpcInit", "npc[%s]�Ľű���ʼ��������[GetNpcInfo]����ʧ��!", szName );
			LG( "NpcInit", "npc[%s]'s script init dispose fuction[GetNpcInfo]transfer failed!", szName );
			//printf( "npc[%s]�Ľű���ʼ��������[GetNpcInfo]����ʧ��!\n", szName );
			printf( RES_STRING(GM_WORLDEUDEMON_CPP_00003), szName );
			lua_callalert( g_pLuaState, nStatus );
			lua_settop(g_pLuaState, 0);
			return FALSE;
		}
		lua_settop(g_pLuaState, 0);

		return TRUE;
	T_E}

	BOOL CWorldEudemon::Load( const char szMsgProc[], const char szName[], dbc::uLong ulID )
	{
		Clear();

		// ��ʼ��npc�ű���Ϣ
		InitScript( szMsgProc, szName );
		
		// npc���ñ���
		m_sNpcID = 0;

		// 
		//strncpy( m_name, szName, 32 - 1 );
		strncpy_s( m_name, sizeof(m_name), szName, _TRUNCATE );
		//strncpy( m_szMsgProc, szMsgProc, ROLE_MAXSIZE_MSGPROC - 1 );
		strncpy_s( m_szMsgProc, sizeof(m_szMsgProc), szMsgProc, _TRUNCATE );

		m_ID = ulID;
		Char szLogName[defLOG_NAME_LEN] = "";
		//sprintf(szLogName, "Cha-%s+%u", GetName(), GetID());
		_snprintf_s(szLogName,sizeof(szLogName),_TRUNCATE, "Cha-%s+%u", GetName(), GetID());

		m_CLog.SetLogName(szLogName);

		// Ĭ��ʹ�õ�һ����ɫ��Ϣ
		m_pCChaRecord = (CChaRecord*)GetChaRecordInfo( 1 );
		m_cat = (SHORT)m_pCChaRecord->lID;

		m_CChaAttr.Init( 1 );		
		setAttr(ATTR_CHATYPE, enumCHACTRL_NPC);

		return TRUE;
	}
    
	HRESULT CWorldEudemon::MsgProc( CCharacter& character, RPACKET& packet )
	{
		// ����NPC�ű��Ի�������
		lua_getglobal( g_pLuaState, "NpcProc" );
		if( !lua_isfunction( g_pLuaState, -1 ) )
		{
			lua_pop( g_pLuaState, 1 );
			LG( "lua_invalidfunc", "NpcProc" );
			return FALSE;
		}

		lua_pushlightuserdata( g_pLuaState, (void*)&character );
		lua_pushlightuserdata( g_pLuaState, (void*)this );
		lua_pushlightuserdata( g_pLuaState, (void*)&packet );
		lua_pushnumber( g_pLuaState, m_sScriptID );

		int nStatus = lua_pcall( g_pLuaState, 4, 0, 0 );
		if( nStatus )
		{
			//character.SystemNotice( "npc[%s]�Ľű���Ϣ������[NpcProc]����ʧ��!", m_name );
			character.SystemNotice( RES_STRING(GM_WORLDEUDEMON_CPP_00004), m_name );
			lua_callalert( g_pLuaState, nStatus );
			lua_settop(g_pLuaState, 0);
			return EN_FAILER;
		}
		lua_settop(g_pLuaState, 0);

		return EN_OK;
	}


}

