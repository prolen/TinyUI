#include "../stdafx.h"
#include "TinyCore.h"

namespace TinyUI
{
	/////////////////////////////////////////////////////////////////////////
	TinyCriticalSection::TinyCriticalSection() throw()
	{
		memset(&section, 0, sizeof(CRITICAL_SECTION));
	}
	TinyCriticalSection::~TinyCriticalSection()
	{
	}
	HRESULT TinyCriticalSection::Lock() throw()
	{
		EnterCriticalSection(&section);
		return S_OK;
	}
	BOOL TinyCriticalSection::Try() throw()
	{
		return ::TryEnterCriticalSection(&section);
	}
	HRESULT TinyCriticalSection::Unlock() throw()
	{
		LeaveCriticalSection(&section);
		return S_OK;
	}
	HRESULT TinyCriticalSection::Initialize() throw()
	{
		InitializeCriticalSection(&section);
		return S_OK;
	}
	HRESULT TinyCriticalSection::Uninitialize() throw()
	{
		DeleteCriticalSection(&section);
		return S_OK;
	}
	/////////////////////////////////////////////////////////////////////////
	TinyEvent::TinyEvent()
		:m_hEvent(NULL)
	{

	}
	TinyEvent::~TinyEvent()
	{
		Close();
	}
	TinyEvent::operator HANDLE() const
	{
		return (HANDLE)(this == NULL ? NULL : m_hEvent);
	}
	HANDLE TinyEvent::Handle() const
	{
		return m_hEvent;
	}
	BOOL TinyEvent::CreateEvent(BOOL bInitiallyOwn, BOOL bManualReset, LPCTSTR lpszNAme, LPSECURITY_ATTRIBUTES lpsaAttribute)
	{
		m_hEvent = ::CreateEvent(lpsaAttribute, bManualReset,
			bInitiallyOwn, lpszNAme);
		return m_hEvent == NULL ? FALSE : TRUE;
	}
	BOOL TinyEvent::OpenEvent(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName)
	{
		m_hEvent = ::OpenEvent(dwDesiredAccess, bInheritHandle, lpName);
		return m_hEvent == NULL ? FALSE : TRUE;
	}
	BOOL TinyEvent::SetEvent()
	{
		ASSERT(m_hEvent != NULL);
		return ::SetEvent(m_hEvent);
	}
	BOOL TinyEvent::PulseEvent()
	{
		ASSERT(m_hEvent != NULL);
		return ::PulseEvent(m_hEvent);
	}
	BOOL TinyEvent::ResetEvent()
	{
		ASSERT(m_hEvent != NULL);
		return ::ResetEvent(m_hEvent);
	}
	BOOL TinyEvent::Lock(DWORD dwTimeout)
	{
		ASSERT(m_hEvent != NULL);
		DWORD dwRet = ::WaitForSingleObject(m_hEvent, dwTimeout);
		if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED)
			return TRUE;
		else
			return FALSE;
	}
	BOOL TinyEvent::Unlock()
	{
		return TRUE;
	}
	void TinyEvent::Close()
	{
		if (m_hEvent != NULL)
		{
			::CloseHandle(m_hEvent);
			m_hEvent = NULL;
		}
	}
	/////////////////////////////////////////////////////////////////////////
	TinyMutex::TinyMutex()
		:m_hMutex(NULL)
	{

	}
	TinyMutex::~TinyMutex()
	{
		Close();
	}
	BOOL TinyMutex::Create(BOOL bInitiallyOwn, LPCTSTR lpszName, LPSECURITY_ATTRIBUTES lpsaAttribute)
	{
		m_hMutex = ::CreateMutex(lpsaAttribute, bInitiallyOwn, lpszName);
		return m_hMutex == NULL ? FALSE : TRUE;
	}
	BOOL TinyMutex::Open(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName)
	{
		m_hMutex = ::OpenMutex(dwDesiredAccess, bInheritHandle, lpName);
		return m_hMutex == NULL ? FALSE : TRUE;
	}
	TinyMutex::operator HANDLE() const
	{
		return (HANDLE)(this == NULL ? NULL : m_hMutex);
	}
	HANDLE TinyMutex::Handle() const
	{
		return m_hMutex;
	}
	BOOL TinyMutex::Lock(DWORD dwTimeout)
	{
		ASSERT(m_hMutex != NULL);
		DWORD dwRet = ::WaitForSingleObject(m_hMutex, dwTimeout);
		if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED)
			return TRUE;
		else
			return FALSE;
	}
	BOOL TinyMutex::Unlock()
	{
		ASSERT(m_hMutex != NULL);
		return ::ReleaseMutex(m_hMutex);
	}
	void TinyMutex::Close()
	{
		if (m_hMutex != NULL)
		{
			::CloseHandle(m_hMutex);
			m_hMutex = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TinyLock::TinyLock()
	{
		m_section.Initialize();
	}
	TinyLock::~TinyLock()
	{
		m_section.Uninitialize();
	}
	void TinyLock::Acquire()
	{
		m_section.Lock();
	}
	void TinyLock::Release()
	{
		m_section.Unlock();
	}
	BOOL TinyLock::Try()
	{
		return m_section.Try();
	}
	//////////////////////////////////////////////////////////////////////////
	TinyAutoLock::TinyAutoLock(TinyLock& lock) : m_lock(lock)
	{
		m_lock.Acquire();
	}
	TinyAutoLock::~TinyAutoLock()
	{
		m_lock.Release();
	}
	//////////////////////////////////////////////////////////////////////////
	TinyScopedLibrary::TinyScopedLibrary()
		:m_hInstance(NULL)
	{

	}
	TinyScopedLibrary::TinyScopedLibrary(LPCSTR pzName)
	{
		m_hInstance = ::LoadLibrary(pzName);
	}
	TinyScopedLibrary::~TinyScopedLibrary()
	{
		if (m_hInstance)
		{
			::FreeLibrary(m_hInstance);
			m_hInstance = NULL;
		}
	}
	TinyScopedLibrary::operator HINSTANCE() const
	{
		return m_hInstance;
	}

	HINSTANCE TinyScopedLibrary::Handle() const
	{
		return m_hInstance;
	}
	FARPROC TinyScopedLibrary::GetFunctionPointer(LPCSTR lpProcName) const
	{
		return ::GetProcAddress(m_hInstance, lpProcName);
	}
	BOOL TinyScopedLibrary::IsValid() const
	{
		return m_hInstance != NULL;
	}
	void TinyScopedLibrary::Reset(HINSTANCE hInstance)
	{
		if (m_hInstance)
		{
			::FreeLibrary(m_hInstance);
			m_hInstance = NULL;
		}
		m_hInstance = hInstance;
	}
	void TinyScopedLibrary::Reset(LPCSTR pzName)
	{
		if (m_hInstance)
		{
			::FreeLibrary(m_hInstance);
			m_hInstance = NULL;
		}
		m_hInstance = LoadLibrary(pzName);
	}
};
