// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

//#define _CRT_SECURE_NO_DEPRECATE
// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here

#include <list>
#include <map>
#include <vector>
#include <queue>
#include <set>
#include <strstream> 
using namespace std;

namespace GUI  //�������ֿռ�
{
};

using namespace GUI;

#include "Language.h"
#include "util.h"
#include "TryUtil.h"
#include "GlobalInc.h"
#include "LightEngine.h"

#include "LanguageRecord.h"

//extern CLanguageRecord g_oLangRec;	// ����������

//#include "ResourceBundleManage.h"		//Add by lark.li 20080130
//#define RES_STRING(a) g_ResourceBundleManage.LoadResString("" #a  "")

#include "i18n.h"

// Add by lark.li 20080730 begin
//#include "pi_Alloc.h"
//#include "pi_Memory.h"
// End
#include "MyMemory.h"

inline VOID D3DUtil_InitMaterialI( D3DMATERIAL8& mtrl, FLOAT r, FLOAT g, FLOAT b,
                           FLOAT a )
{
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
    mtrl.Diffuse.r = mtrl.Ambient.r = r;
    mtrl.Diffuse.g = mtrl.Ambient.g = g;
    mtrl.Diffuse.b = mtrl.Ambient.b = b;
    mtrl.Diffuse.a = mtrl.Ambient.a = a;
}

#define WM_MUSICEND WM_USER + 0x1000

//#define APP_DEBUG

#define _LOG_NAME_		// ���������ɫ����,��ݼ�

// #define FLOAT_INVALID   // ���������󣬽���⸡���쳣,��֪caLua���и����쳣

//#define USE_TIMERPERIOD
#define WM_USER_TIMER (WM_USER+99)

#define OPT_CULL_1
//#define OPT_CULL_2

#define CLIENT_BUILD

// #define KOP_TOM			// ����TOMƽ̨

//extern CResourceBundleManage g_ResourceBundleManage; //Add by lark.li 20080130

#define COLOR_INDEX_NUM 6

const DWORD Colors[COLOR_INDEX_NUM] = {
	D3DCOLOR_ARGB(0xFF,0xD6,0x13,0x1D),
	D3DCOLOR_ARGB(0xFF,66,244,92),
	D3DCOLOR_ARGB(0xFF,66, 182, 244),
	D3DCOLOR_ARGB(0xFF,255, 73, 182),
};