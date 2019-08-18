#ifndef LESharedPtr_H
#define LESharedPtr_H

#include "LEEffPrerequisites.h"

template <class T> class LESharedPtr
{
protected:
	T* m_pRep;
	unsigned int* m_pUseCount;

public:
	LE_AUTO_SHARED_MUTEX; // ���з���Ȩ�������ⲿ����

	/** Ĭ�Ϲ��캯������û�г�ʼ��SharedPtr
	@remarks
	<b>Σ��!</b> ����ʹ��SharedPtr֮ǰ�������bind() .
	*/
	LESharedPtr() : m_pRep(0), m_pUseCount(0) {}

	explicit LESharedPtr(T* rep) : m_pRep(rep), m_pUseCount(new unsigned int(1))
	{
		LE_NEW_AUTO_SHARED_MUTEX
	}

	LESharedPtr(const LESharedPtr& r)
	{
		// ��������������mutex��ָ��
		LE_LOCK_MUTEX(*(r.LE_AUTO_MUTEX_NAME));
		LE_COPY_AUTO_SHARED_MUTEX(r.LE_AUTO_MUTEX_NAME);

		m_pRep = r.m_pRep;
		m_pUseCount = r.m_pUseCount;

		if (m_pUseCount)
		{
			++(*m_pUseCount);
		}
	}

	LESharedPtr& operator=(const LESharedPtr& r) {
		if (m_pRep == r.m_pRep)
			return *this;
		release();

		// ��������������mutex��ָ��
		LE_LOCK_MUTEX(*(r.LE_AUTO_MUTEX_NAME));
		LE_COPY_AUTO_SHARED_MUTEX(r.LE_AUTO_MUTEX_NAME);

		m_pRep = r.m_pRep;
		m_pUseCount = r.m_pUseCount;
		if (m_pUseCount)
		{
			++(*m_pUseCount);
		}
		return *this;
	}

	virtual ~LESharedPtr() {
		release();
	}

	inline T& operator*() const { assert(m_pRep); return *m_pRep; }
	inline T* operator->() const { assert(m_pRep); return m_pRep; }
	inline T* get() const { return m_pRep; }

	/** ��ָ��󶨵�SharedPtr��.
	@remarks
		�������SharedPtr��û�г�ʼ��!
	*/
	void bind(T* rep) {
		assert(!pRep && !pUseCount);
		LE_NEW_AUTO_SHARED_MUTEX;
		LE_LOCK_AUTO_SHARED_MUTEX;

		m_pUseCount = new unsigned int(1);
		m_pRep = rep;
	}

	inline bool unique() const { assert(m_pUseCount); LE_LOCK_AUTO_SHARED_MUTEX; return (*m_pUseCount) == 1; }
	inline unsigned int useCount() const { assert(m_pUseCount); LE_LOCK_AUTO_SHARED_MUTEX return *m_pUseCount; }
	inline unsigned int* useCountPointer() const { return m_pUseCount; }

	inline T* getPointer() const { return m_pRep; }

	inline bool isNull(void) const { return m_pRep == 0; }

	inline void setNull(void) {
		if (m_pRep)
		{
			// ���ڲ����������ﲻ������˫�жϣ�
			release();
			m_pRep = 0;
			m_pUseCount = 0;
			LE_COPY_AUTO_SHARED_MUTEX(0)
		}
	}

protected:

	inline void release(void)
	{
		bool destroyThis = false;
		{
			// �������޵ķ�Χ�������Լ���mutex(������֮ǰ�������)
			LE_LOCK_AUTO_SHARED_MUTEX;

			if (m_pUseCount)
			{
				if (--(*m_pUseCount) == 0)
				{
					destroyThis = true;
				}
			}
			// �������Զ��������鿴boost������
		}
		if (destroyThis)
			destroy();
	}

	virtual void destroy(void)
	{
		// ���������������������϶��ڽ�������֮ǰû���ͷ�ָ��
		// �ڽ�������֮ǰʹ��setNull(),��ȷ�����ָ���ڳ������ǰ�������ķ�Χ���Զ�������
		delete m_pRep;
		delete m_pUseCount;
		LE_DELETE_AUTO_SHARED_MUTEX;
	}

};

template<class T, class U> inline bool operator==(LESharedPtr<T> const& a, LESharedPtr<U> const& b)
{
	return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(LESharedPtr<T> const& a, LESharedPtr<U> const& b)
{
	return a.get() != b.get();
}

#endif