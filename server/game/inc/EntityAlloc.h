//=============================================================================
// FileName: EntityAlloc.h
// Creater: ZhangXuedong
// Date: 2005.01.18
// Comment: Entity Alloc

// modifed by knight.gong 2005.5.16. (To alloc all entities by the template of allocer)
//=============================================================================

#ifndef ENTITYALLOC_H
#define ENTITYALLOC_H

#include "Npc.h"
#include "Item.h"
#include "EventEntity.h"
#include "Player.h"

#define defENTI_ALLOC_TYPE_PLY		0x01000000
#define defENTI_ALLOC_TYPE_CHA		0x02000000
#define defENTI_ALLOC_TYPE_ITEM		0x03000000
#define defENTI_ALLOC_TYPE_TNPC		0x04000000

#define defENTI_ENTEVENT 0x06000000
#define defENTI_ENTBASE(n) (defENTI_ENTEVENT + (n<<24))
#define defENTI_ALLOC_TYPE_ENTBASE			defENTI_ENTBASE(1)	// ���������¼�ʵ��
#define defENTI_ALLOC_TYPE_ENTRESOURCE		defENTI_ENTBASE(2)	// ��Դ�����¼�ʵ��
#define defENTI_ALLOC_TYPE_ENTTRANSIT		defENTI_ENTBASE(3)	// ���������¼�ʵ��
#define defENTI_ALLOC_TYPE_ENTBERTH			defENTI_ENTBASE(4)	// ͣ�������¼�ʵ��


/**
 * @class CAlloc 
 * @author XX
 * @brief �ڴ����ģ��
 * @bug 20081222 ��֧�ֶ��߳�
 * @todo ������������Ԥ�����Ԫ�أ������Զ�����
 */
template <class T>
class CAlloc
{
public:
	CAlloc();
	~CAlloc();

	void  clear();

	// ��ȡ�������
	LONG getHoldSize()  { return m_lHoldSize; }
	LONG getMaxHoldSize()  { return m_lMaxHoldSize; }
	LONG getAllocSize() { return m_lAllocSize; }
	
	// ����ָ����������
	BOOL  create( LONG lSize, LONG lFlag = 0 );
	
	//BOOL  resize(LONG lSize);

	T*	  alloc();

	void  destroy( LONG lID );

	// ѭ����������������Ϣ
	void  begin();
	T*	  end();
	T*    next();

	void  revbegin();
	T*    revnext();

	T*	  getinfo( LONG lID );

private:
	LONG m_lCur;			// ����ʱʹ�õ�����
	LONG m_lHoldSize;		// ռ�õĴ�С
	LONG m_lAllocSize;		// ����Ԫ�ظ���
	LONG m_lMaxHoldSize;	// ���ռ�ø���
	LONG m_lFlag;			// ���͵ı�ʾ

	T**	  m_pHold;			// 
	T*	  m_pAlloc;

	CSLock      m_lock;
};

template<class T>
CAlloc< T >::CAlloc()
{
	m_lCur = 0;
	m_lHoldSize = 0;
	m_lMaxHoldSize = 0;
	m_lAllocSize = 0;
	m_pHold = NULL;
	m_pAlloc = NULL;
}

template<class T>
CAlloc< T >::~CAlloc()
{
	clear();
}

template<class T>
void CAlloc< T >::clear()
{
	m_lCur = 0;
	m_lHoldSize = 0;
	m_lMaxHoldSize = 0;
	m_lAllocSize = 0;
	SAFE_DELETE_ARRAY(m_pHold);
	SAFE_DELETE_ARRAY(m_pAlloc);
}

template<class T>
BOOL CAlloc< T >::create( LONG lSize, LONG lFlag )
{
	if( lSize <= 0 ) {
		return FALSE;
	}

	clear();

	m_lFlag = lFlag;
	m_lAllocSize = lSize;
	m_pHold = new T*[m_lAllocSize];
	m_pAlloc = new T[m_lAllocSize];
	if( !m_pHold || !m_pAlloc )
	{
		clear();
		return FALSE;
	}

	memset( m_pHold, 0, sizeof(T*)*m_lAllocSize );

	for( LONG i = 0; i < m_lAllocSize; i++ )
	{
		m_pAlloc[i].SetHandle( m_lFlag | i );
		m_pAlloc[i].SetHoldID( -1 );
		m_pHold[i] = m_pAlloc + i;
	}

	return TRUE;
}

//template<class T>
//BOOL CAlloc< T >::resize( LONG lSize )
//{
//	if(lSize <= m_lAllocSize)
//		return FALSE;
//
//
//	return TRUE;
//}

template<class T>
T* CAlloc< T >::alloc()
{
	m_lock.lock();

	if( m_lHoldSize < m_lAllocSize )
	{
		try
		{
			m_pHold[m_lHoldSize]->SetHoldID( m_lHoldSize );
			m_pHold[m_lHoldSize]->Initially();
			m_lHoldSize++;
			if (m_lHoldSize > m_lMaxHoldSize)
				m_lMaxHoldSize = m_lHoldSize;

		}
		catch(...)
		{
			LG("MemError", "alloc\n");
		}

		m_lock.unlock();
		return m_pHold[m_lHoldSize - 1];
	}
	//else
	//{
		//resize(m_lAllocSize + 8);
	//}


	m_lock.unlock();
	return NULL;
}

template<class T>
void CAlloc< T >::destroy( LONG lID )
{
	if( lID < 0 || lID >= m_lAllocSize )
	{
		return;
	}

	LONG lHoldID = m_pAlloc[lID].GetHoldID();
	if( lHoldID < 0 || lHoldID >= m_lHoldSize )
	{
		return;
	}

	m_lock.lock();

	try
	{
		T *pTemp;
		m_lHoldSize--;
		pTemp = m_pHold[m_lHoldSize];
		m_pHold[m_lHoldSize] = m_pHold[lHoldID];
		m_pHold[lHoldID] = pTemp;
		m_pHold[lHoldID]->SetHoldID( lHoldID );

		if (m_lCur == m_lHoldSize) 
			m_lCur--;

		long	lTempHoldS = m_lHoldSize;
		try
		{
			m_pHold[m_lHoldSize]->Finally();
		}
		catch (...)
		{
		}
		m_pHold[lTempHoldS]->SetHoldID(-1);
	}
	catch(...)
	{
		LG("MemError", "destroy\n");
	}
	m_lock.unlock();
}

template<class T>
T* CAlloc< T >::getinfo( LONG lID )
{
	if( lID >= m_lAllocSize ) {
		return NULL;
	}

	LONG lHoldID = m_pAlloc[lID].GetHoldID();
	if( lHoldID < 0 || lHoldID >= m_lHoldSize ) {
		return NULL;
	}

	return m_pAlloc + lID;
}

template<class T>
void CAlloc< T >::begin()
{
	m_lCur = 0;
}

template< class T>
T* CAlloc< T >::end()
{
	return NULL;
}

template< class T>
T* CAlloc< T >::next()
{
	if( m_lCur >= m_lHoldSize ) 
	{
		return NULL;	
	}	
	return m_pHold[m_lCur++]; 
}

template<class T>
void CAlloc< T >::revbegin()
{
	m_lCur = m_lHoldSize - 1;
}

template< class T>
T* CAlloc< T >::revnext()
{
	if( m_lCur < 0 ) 
	{
		return NULL;	
	}	
	return m_pHold[m_lCur--]; 
}

/**
 * @class CEntityAlloc 
 * @author XX
 * @brief ʵ���ڴ�Ԥ������
 *			���ڴ������¸���ļӺ�
 *			
 *			��ɫ�ڴ�    = (����� * 3 + ������) * sizeof(CCharacter)  32224Bytes
 *			�����ڴ�    = ������ * sizeof(CItem)  744Bytes
 *			�Ի�NPC�ڴ� = �Ի�NPC�� * sizeof(CTalkNpc)  32444Bytes
 *
 *			�ۿ��ڴ�	= �ۿ��ڴ��� * sizeof(CBerthEntity) 32236Bytes  ȱʡ1000��
 *			��Դ�ڴ�	= ��Դ�� * sizeof(CResourceEntity)	 32236Bytes ȱʡ1000��
 */
class	CEntityAlloc
{
public:
	CEntityAlloc(long lChaNum = 5000, long lItemNum = 3000, long lTNpcNum = 200);
	~CEntityAlloc();

	CCharacter*	GetNewCha();
	CItem*		GetNewItem();
	mission::CTalkNpc*	GetNewTNpc();
	mission::CEventEntity* GetEventEntity( BYTE byType );
	BOOL		ClearEventEntity();

	Entity		*GetEntity(long lID);
	void		ReturnEntity(long lID);

	void		BeginGetTNpc(void) { m_TalkNpcAlloc.revbegin(); }
	mission::CTalkNpc*	GetNextTNpc(void) {return m_TalkNpcAlloc.revnext();}

	void		BeginGetItem(void) {m_ItemAlloc.revbegin();}
	CItem*		GetNextItem(void) {return m_ItemAlloc.revnext();}

	void		BeginGetCha(void) {m_ChaAlloc.revbegin();}
	CCharacter*	GetNextCha(void) {return m_ChaAlloc.revnext();}

	long		GetHoldChaNum(void) { return m_ChaAlloc.getHoldSize(); }
	long		GetHoldItemNum(void) { return m_ItemAlloc.getHoldSize(); }
	long		GetHoldTNpcNum(void) { return m_TalkNpcAlloc.getHoldSize(); }
	long		GetMaxHoldChaNum(void) { return m_ChaAlloc.getMaxHoldSize(); }
	long		GetMaxHoldItemNum(void) { return m_ItemAlloc.getMaxHoldSize(); }
	long		GetMaxHoldTNpcNum(void) { return m_TalkNpcAlloc.getMaxHoldSize(); }

	long		GetAllocChaNum(void) { return m_ChaAlloc.getAllocSize(); }
	long		GetAllocItemNum(void) { return m_ItemAlloc.getAllocSize(); }
	long		GetAllocTNpcNum(void) { return m_TalkNpcAlloc.getAllocSize(); }

private:
	typedef CAlloc<CCharacter> CHA_ALLOC;
		CHA_ALLOC m_ChaAlloc;
	typedef CAlloc<CItem> ITEM_ALLOC;
		ITEM_ALLOC m_ItemAlloc;
	typedef CAlloc<mission::CTalkNpc> TALKNPC_ALLOC;
		TALKNPC_ALLOC m_TalkNpcAlloc;
	typedef CAlloc<mission::CBerthEntity> BERTH_ALLOC;
		BERTH_ALLOC	m_BerthAlloc;
	typedef CAlloc<mission::CResourceEntity> RESOURCE_ALLOC;
		RESOURCE_ALLOC m_ResourceAlloc;

};

/**
 * @class CPlayerAlloc 
 * @author XX
 * @brief ����ڴ������
 *				����ڴ� = ����� * sizeof(CPlayer)
 */
class	CPlayerAlloc
{
public:
	CPlayerAlloc(long lPlyNum = 3000) {m_PlyAlloc.create( lPlyNum);}
	~CPlayerAlloc() {	m_PlyAlloc.clear();}

	CPlayer*	GetNewPly();
	CPlayer*	GetPly(long lID) {return m_PlyAlloc.getinfo( lID );}
	void		ReturnPly(long lID) {m_PlyAlloc.destroy( lID );}

	long		GetHoldPlyNum(void) { return m_PlyAlloc.getHoldSize(); }
	long		GetMaxHoldPlyNum(void) { return m_PlyAlloc.getMaxHoldSize(); }
	long		GetAllocPlyNum(void) { return m_PlyAlloc.getAllocSize(); }

protected:

private:
	typedef CAlloc<CPlayer> PLAYER_ALLOC;
		PLAYER_ALLOC m_PlyAlloc;

};

#endif // ENTITYALLOC_H