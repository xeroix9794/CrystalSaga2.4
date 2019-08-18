//=============================================================================
// FileName: Item.cpp
// Creater: ZhangXuedong
// Date: 2004.09.21
// Comment: CItem class
//=============================================================================

#include "Item.h"
#include "GameCommon.h"
#include "GameAppNet.h"
#include "SubMap.h"
#include "gamedb.h"

_DBC_USING

CItem::CItem()
{T_B
	chValid = 0;
	m_pCItemRecord = 0;
	m_SGridContent.sID = 0;
	m_lFromEntityID = 0;
	m_chSpawType = enumITEM_APPE_NATURAL;
T_E}

void CItem::Initially()
{T_B
	Entity::Initially();

	chValid = 0;
	m_pCItemRecord = 0;
	m_SGridContent.sID = 0;
	m_chSpawType = enumITEM_APPE_NATURAL;
	m_ulStartTick = GetTickCount();
	m_ulOnTick = g_Config.m_lItemShowTime * 1000;
	m_ulProtOnTick = g_Config.m_lItemProtTime * 1000;
	m_ulProtID = 0;
	m_ulProtHandle = 0;
	m_chProtType = enumITEM_PROT_OWN;
T_E}

void CItem::Finally()
{T_B
	if (m_submap)
		m_submap->GoOut(this);
	Entity::Finally();
T_E}

void CItem::OnBeginSeen(CCharacter *pCMainCha)
{T_B
	WPACKET pk =GETWPACKET();
	WRITE_CMD(pk, CMD_MC_ITEMBEGINSEE);
	// ��������
	WRITE_LONG(pk, m_ID);							// world ID
	WRITE_LONG(pk, m_lHandle);
	WRITE_LONG(pk, m_pCItemRecord->lID);			// ID
	WRITE_LONG(pk, GetShape().centre.x);			// ��ǰxλ��
	WRITE_LONG(pk, GetShape().centre.y);			// ��ǰyλ��
	WRITE_SHORT(pk, m_sAngle);					// ����
	WRITE_SHORT(pk, m_SGridContent.sNum);			// ����
	//
	WRITE_CHAR(pk, m_chSpawType);
	WRITE_LONG(pk, m_lFromEntityID);
	// �¼���Ϣ
	WriteEventInfo(pk);

	pCMainCha->ReflectINFof(this,pk);//ͨ��
T_E}

void CItem::OnEndSeen(CCharacter *pCMainCha)
{T_B
	WPACKET pk =GETWPACKET();
	WRITE_CMD(pk, CMD_MC_ITEMENDSEE);
	WRITE_LONG(pk, m_ID);				//ID
	pCMainCha->ReflectINFof(this,pk);	//ͨ��
T_E}

void CItem::Run(dbc::uLong ulCurTick)
{
	if (m_ulProtID != 0)
		if (m_ulProtOnTick != 0 && ulCurTick - m_ulStartTick >= m_ulProtOnTick) // ����ʱ����ʧ
			m_ulProtID = 0;

	if (m_ulOnTick != 0 && ulCurTick - m_ulStartTick >= m_ulOnTick)
	{
		// �ж��Ƿ񴬳�֤������
		CItemRecord* pItem = m_pCItemRecord;
		if( pItem != NULL )
		{
			// �ж϶�������֤��
			if( pItem->sType == enumItemTypeBoat )
			{
				game_db.SaveBoatDelTag( this->GetGridContent()->GetDBParam( enumITEMDBP_INST_ID ), 1 );
			}
		}
		if (!m_submap)
			//LG("������ʧ����", "���� %s(ID %u��HANDLE %u��λ��[%d %d]) ����ʧʱ�������ͼΪ��\n", GetName(), GetID(), GetHandle(), GetPos().x, GetPos().y);
			LG("Item disappear error", "item %s(ID %u��HANDLE %u��position[%d %d]) when it disappear find the map is null\n", GetName(), GetID(), GetHandle(), GetPos().x, GetPos().y);
		else
		{
			Free();
		}
	}
}

//	yangyinyu	2008-8-18	add	function!
static	char	s_logText[512];

//
#include <string.h>

void	lgtool_PrintGetType(	const	char*	sGetType	)
{
	//strncpy(	s_logText,	sGetType,	512	);
	strncpy_s( s_logText, sizeof(s_logText), sGetType, _TRUNCATE );
};

//
void	lgtool_PrintItem(	
			const	char*	szPlyName,	//	��Ϸ�����֡�
			int				iChaId,		//	����ID�š�
			short			sPickupNum,
			CItemRecord*	pItem,		
			SItemGrid*		sig
			)
{
	static	char	sTextEx[512];
	
	//_snprintf(	sTextEx,	512,	"%s;%d;%s;%d;%d;%d;%d;%d;%d;",
	//	szPlyName,			//	������֡�
	//	iChaId,				//	����ID�š�
	//	pItem->szName,		//	�������֡�
	//	pItem->sType,		//	�������͡�
	//	sig->sID,			//	��������ID��
	//	sig->dwDBID,		//	ΨһID��
	//	sPickupNum,			//	������
	//	sig->GetDBParam(0),	//	������Ϣ��
	//	sig->GetDBParam(1)	//	����װ��ID��
	//	);
	_snprintf_s(	sTextEx,sizeof(sTextEx),_TRUNCATE,	"%s;%d;%s;%d;%d;%d;%d;%d;%d;",
		szPlyName,			//	������֡�
		iChaId,				//	����ID�š�
		pItem->szName,		//	�������֡�
		pItem->sType,		//	�������͡�
		sig->sID,			//	��������ID��
		sig->dwDBID,		//	ΨһID��
		sPickupNum,			//	������
		sig->GetDBParam(0),	//	������Ϣ��
		sig->GetDBParam(1)	//	����װ��ID��
		);

	//strcat(	s_logText,	sTextEx	);
	strncat_s( s_logText,sizeof(s_logText), sTextEx,_TRUNCATE);

	//	�����װ����
	if(	pItem->sType	<	31	)
	{
		struct	aaa
		{
		static	void	printfAttr(	char*	sText,	SItemGrid*	sig,	int	begin,	int	end	)
			{
				for(	int	i	=	begin;	i	<=	end;	i	++	)
				{
					short	attr	=	sig->GetAttr( i );

					if(	attr	)
					{
						//_snprintf(	sTextEx,	512,	"%d-%d,",	i,	attr	);
						_snprintf_s(	sTextEx,sizeof(sTextEx),_TRUNCATE,	"%d-%d,",	i,	attr	);
						//strncat(	sText,	sTextEx,	512	);
						strncat_s( sText,512, sTextEx,_TRUNCATE);
					};
				};
			};
		};

		//	ATTR_COUNT_BASE1    -	ATTR_BOAT_PRICE     
		aaa::printfAttr(	s_logText,	sig,	ATTR_COUNT_BASE1,	ATTR_BOAT_PRICE	);

		//	ATTR_COUNT_BASE2	-	ATTR_BOAT_BCSPD
		aaa::printfAttr(	s_logText,	sig,	ATTR_COUNT_BASE2,	ATTR_BOAT_BCSPD	);

		//	ATTR_COUNT_BASE3    -	ATTR_ITEMC_PDEF     
		aaa::printfAttr(	s_logText,	sig,	ATTR_COUNT_BASE3,	ATTR_ITEMC_PDEF	);

		//	ATTR_COUNT_BASE4	-	ATTR_ITEMV_PDEF
		aaa::printfAttr(	s_logText,	sig,	ATTR_COUNT_BASE4,	ATTR_ITEMV_PDEF	);

		//	ATTR_COUNT_BASE5	-	ATTR_STATEC_PDEF    
		aaa::printfAttr(	s_logText,	sig,	ATTR_COUNT_BASE5,	ATTR_STATEC_PDEF	);

		//	ATTR_COUNT_BASE6	-	ATTR_LHAND_ITEMV    
		aaa::printfAttr(	s_logText,	sig,	ATTR_COUNT_BASE6,	ATTR_LHAND_ITEMV	);

	}
	//	����Ǿ��飬����ؾ������ԡ�
	else	if(	pItem->sType	==	59	)
	{
		//	���ؾ�������������ݣ�����������רע��
		struct	bbb
		{	
			static	void	printPet(	char*	sText,	SItemGrid*	sig,	int	iAttrId	)
			{
				//_snprintf(	sTextEx,	512,	"%d-%d,",	iAttrId,	sig->GetAttr(	iAttrId	)	);
				_snprintf_s(	sTextEx,sizeof(sTextEx),_TRUNCATE,	"%d-%d,",	iAttrId,	sig->GetAttr(	iAttrId	)	);
				//strncat(	sText,	sTextEx,	512	);
				strncat_s( sText,512, sTextEx,_TRUNCATE);
			};
		};

		//	��ӡ��ص�ϵ����
		for(	int	i	=	ITEMATTR_COE_STR;	i	<=	ITEMATTR_COE_STA;	i	++	)
		{
			bbb::printPet(	s_logText,	sig,	i	);
		}

		//	��ӡ��صĳ�����
		for(	int	i	=	ITEMATTR_VAL_STR;	i	<=	ITEMATTR_VAL_STA;	i	++	)
		{
			bbb::printPet(	s_logText,	sig,	i	);
		}

		//	��ӡ���鼼�ܡ�
		//_snprintf(	sTextEx,	512,	"%d,",	sig->GetDBParam(	enumITEMDBP_FORGE	)	);
		_snprintf_s(	sTextEx,sizeof(sTextEx),_TRUNCATE,	"%d,",	sig->GetDBParam(	enumITEMDBP_FORGE	)	);
		//strncat(	s_logText,	sTextEx,	512	);
		strncat_s( s_logText,sizeof(s_logText), sTextEx,_TRUNCATE);

		//	��ӡ����ȼ���
		//_snprintf(	sTextEx,	512,	"%d",	sig->GetForgeLv()	);
		_snprintf_s(	sTextEx,sizeof(sTextEx),_TRUNCATE,	"%d",	sig->GetForgeLv()	);
		//strncat(	s_logText,	sTextEx,	512	);
		strncat_s( s_logText,sizeof(s_logText), sTextEx,_TRUNCATE);
	}
	//	����Ǳ�ʯ�����¼��ʯ�ȼ��ͱ�ʯЧ����
	else	if(	pItem->sType	==	49	)	//	�߳�������49�Ǳ�ʯ��
	{
		//	��ӡ��ʯ�ȼ���
		//_snprintf(	sTextEx,	512,	"%d,",	sig->GetForgeLv()	);
		_snprintf_s(	sTextEx,sizeof(sTextEx),_TRUNCATE,	"%d,",	sig->GetForgeLv()	);
		//strncat(	s_logText,	sTextEx,	512	);
		strncat_s( s_logText,sizeof(s_logText), sTextEx,_TRUNCATE);

		//	��ӡ��ʯЧ����
		//_snprintf(	sTextEx,	512,	"%d,",	sig->GetDBParam(	enumITEMDBP_FORGE	)	);
		_snprintf_s(	sTextEx,sizeof(sTextEx),_TRUNCATE,	"%d,",	sig->GetDBParam(	enumITEMDBP_FORGE	)	);
		//strncat(	s_logText,	sTextEx,	512	);
		strncat_s( s_logText,sizeof(s_logText), sTextEx,_TRUNCATE);
	}

	//	��ӡLOG��
	//strncat(	s_logText,	"\n",	512	);
	strncat_s( s_logText,sizeof(s_logText), "\n",_TRUNCATE);

	//	*
	LG("query_item",s_logText	);
};