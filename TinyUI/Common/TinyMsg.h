#pragma once
#include "TinyObject.h"
#include "TinyCollection.h"

/// <summary>
/// 消息对象 包括消息体,消息筛选器,消息映射宏
/// </summary>
namespace TinyUI
{
	/// <summary>
	/// Msg对象
	/// </summary>
	class NO_VTABLE TinyMsg : public MSG
	{
	public:
		INT cbSize;
		BOOL bHandled;
		TinyMsg();
		TinyMsg(HWND hWND, UINT uMsg, WPARAM wParamIn, LPARAM lParamIn, DWORD dwTime, POINT ptIn, BOOL bHandledIn);
		TinyMsg(HWND hWND, UINT uMsg, WPARAM wParamIn, LPARAM lParamIn, BOOL bHandledIn = TRUE);
		TinyMsg(MSG& msg, BOOL bHandledIn = TRUE);
	};
	/// <summary>
	/// 消息映射
	/// </summary>
	class NO_VTABLE TinyMessageMap
	{
	public:
		virtual BOOL ProcessWindowMessage(HWND hWND, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
		{
			UNUSED(hWND);
			UNUSED(uMsg);
			UNUSED(wParam);
			UNUSED(lParam);
			UNUSED(lResult);
			return FALSE;
		};
	};
	/// <summary>
	/// 消息筛选器
	/// </summary>
	class NO_VTABLE TinyMessageFilter
	{
	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg) = 0;
	};
	/// <summary>
	/// 消息空闲处理
	/// </summary>
	class NO_VTABLE TinyIdleHandler
	{
	public:
		virtual BOOL OnIdle() = 0;
	};
	/// <summary>
	/// 消息筛选器集合
	/// </summary>
	class TinyMessageFilters
	{
	public:
		TinyMessageFilters();
		~TinyMessageFilters();
		BOOL Add(TinyMessageFilter* element);
		BOOL Remove(TinyMessageFilter* element);
		BOOL RemoveAt(INT index);
		void RemoveAll();
		INT  GetSize() const;
		const TinyMessageFilter* operator[] (INT index) const;
		TinyMessageFilter* operator[] (INT index);
		INT Lookup(TinyMessageFilter* element) const;
	private:
		TinyArray<TinyMessageFilter*> m_array;
	};
	/// <summary>
	/// 消息空闲处理
	/// </summary>
	class TinyIdleHandlers
	{
	public:
		TinyIdleHandlers();
		~TinyIdleHandlers();
		BOOL Add(TinyIdleHandler* element);
		BOOL Remove(TinyIdleHandler* element);
		BOOL RemoveAt(INT index);
		void RemoveAll();
		INT  GetSize() const;
		const TinyIdleHandler* operator[] (INT index) const;
		TinyIdleHandler* operator[] (INT index);
		INT Lookup(TinyIdleHandler* element) const;
	private:
		TinyArray<TinyIdleHandler*> m_array;
	};
	/// <summary>
	/// 消息循环
	/// </summary>
	class TinyMessageLoop
	{
	public:
		MSG m_msg;
		TinyMessageFilters	m_messageFilters;
		TinyIdleHandlers	m_idleHandles;
	public:
		BOOL AddMessageFilter(TinyMessageFilter* pMessageFilter);
		BOOL RemoveMessageFilter(TinyMessageFilter* pMessageFilter);
		BOOL AddIdleHandler(TinyIdleHandler* pIdleHandler);
		BOOL RemoveIdleHandler(TinyIdleHandler* pIdleHandler);
		INT MessageLoop();
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		virtual BOOL OnIdle(INT idleCount);
	};
	/// <summary>
	/// 消息处理宏
	/// </summary>
#define BEGIN_MSG_MAP(theClass,theBaseClass) \
	typedef theBaseClass TheBaseClass; \
public: \
	virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) \
							{ \
	BOOL bHandled = TRUE; \
	(hWnd); \
	(uMsg); \
	(wParam); \
	(lParam); \
	(lResult); \
	(bHandled);
#define MESSAGE_HANDLER(msgs, func) \
	if (uMsg == msgs) \
							{ \
	bHandled = TRUE; \
	lResult = func(uMsg, wParam, lParam, bHandled); \
	if (bHandled) \
	return TRUE; \
							}
#define MESSAGE_RANGE_HANDLER(msgFirst, msgLast, func) \
	if (uMsg >= msgFirst && uMsg <= msgLast) \
							{ \
	bHandled = TRUE; \
	lResult = func(uMsg, wParam, lParam, bHandled); \
	if (bHandled) \
	return TRUE; \
							}
#define COMMAND_HANDLER(id, code, func) \
	if (uMsg == WM_COMMAND && id == LOWORD(wParam) && code == HIWORD(wParam)) \
							{ \
	bHandled = TRUE; \
	lResult = func(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled); \
	if (bHandled) \
	return TRUE; \
							}
#define CHAIN_MSG_MAP(theBaseClass) \
							{ \
	if (theBaseClass::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) \
	return TRUE; \
							}
#define END_MSG_MAP() \
							{ \
	if (TheBaseClass::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) \
	return TRUE; \
							}\
	return FALSE; \
							}
}

