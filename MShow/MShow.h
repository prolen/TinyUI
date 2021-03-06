#pragma once
#include "resource.h"
#include "MSearchWindow.h"
#include "MSearchController.h"
#include "MClientWindow.h"
#include "MClientController.h"
#include "Windowless/TinyVisualManage.h"

using namespace TinyUI;

namespace MShow
{
	class MShowApp
	{
	public:
		MShowApp();
		~MShowApp();
	public:
		BOOL				Initialize(HINSTANCE hInstance, LPTSTR  lpCmdLine, INT nCmdShow, LPCTSTR lpTableName);
		BOOL				Uninitialize();
		MSearchWindow&		GetSearchView();
		MSearchController&	GetSearchController();
		MClientWindow&		GetClientView();
		MClientController&	GetClientController();
		INT					Run();
		QWORD				GetQPCTimeNS();
		QWORD				GetQPCTimeMS();
		QWORD				GetQPCTime100NS();
		BOOL				SleepNS(QWORD qwNSTime);
		void				SetCurrentAudioTS(QWORD ts);
		QWORD				GetCurrentAudioTS();
		string				GetDefaultPath();
		MAppConfig&			AppConfig();
	public:
		static MShowApp&	GetInstance() throw();
	private:
		string				m_szPath;
		QWORD				m_audioTS;
		MSearchWindow		m_searchView;
		MSearchController	m_searchCTRL;
		MClientWindow		m_clientView;
		MClientController	m_clientCRTL;
		TinyLock			m_lock;
		TinyMessageLoop		m_msgLoop;
		MAppConfig			m_appConfig;
	};
}

