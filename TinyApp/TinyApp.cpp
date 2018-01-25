// TinyApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "TinyApp.h"
#include "MainFrame.h"
#include "ChatFrame.h"
#include "ChatDlg.h"
#include "MFFrame.h"
#include "Windowless/TinyVisualWindowless.h"
#include "Windowless/TinyVisualTextBox.h"
#include "Windowless/TinyVisualContextMenu.h"
#include "Render/TinyDDraw.h"
#include "Media/TinyWASAPIAudioCapture.h"
#include "Media/TinyWASAPIAudioRender.h"
#include "Media/TinyAudioDSPCapture.h"
#include "Network/TinyIOServer.h"
#include "Network/TinySocket.h"
#include "Media/TinyWave.h"
#include "Common/TinyHook.h"
#include "Media/TinyMFMP3Decode.h"
#include "MPG123Decode.h"
#include "FLVParser.h"
#include "Media/TinyMP3File.h"
#include "Network/TinyURL.h"
#include "Network/TinyDNS.h"
#include "Common/TinySignal.h"
#include "SkinWindow.h"
#include "Media/TinyWave.h"
#include "FLVParser.h"
#include "TSReader.h"
#include "Windowless/TinyVisualLayeredWindow.h"
#include "Media/TinySoundCapture.h"
#include "QSVEncoder.h"
#include "MediaTest.h"
#include "Media/TinyMFIntelQSVDecode.h"
#include <fstream>
#include "IO/TinyBitWriter.h"
#include "Network/TinyPing.h"

using namespace TinyUI;
using namespace TinyUI::Network;
using namespace TinyUI::Media;
using namespace Decode;

BOOL LoadSeDebugPrivilege()
{
	DWORD   err;
	HANDLE  hToken;
	LUID    Val;
	TOKEN_PRIVILEGES tp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		err = GetLastError();
		return FALSE;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Val))
	{
		err = GetLastError();
		CloseHandle(hToken);
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = Val;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
	{
		err = GetLastError();
		CloseHandle(hToken);
		return FALSE;
	}
	CloseHandle(hToken);
	return TRUE;
}

class TSDecoder
{
public:
	TSDecoder();
	virtual ~TSDecoder();
	BOOL Open(LPCSTR pzFile);
	void Invoke();
	void Close();
	void OnConfigChange(const BYTE* bits, LONG size, LPVOID);
private:
	INT			m_count;
	x264Decoder m_decoder;
	TSReader	m_reader;
};
TSDecoder::TSDecoder()
	:m_count(0)
{

}
TSDecoder::~TSDecoder()
{

}
BOOL TSDecoder::Open(LPCSTR pzFile)
{
	m_reader.Close();
	m_reader.SetConfigCallback(BindCallback(&TSDecoder::OnConfigChange, this));
	m_reader.OpenFile(pzFile);
	return TRUE;
}

void TSDecoder::Close()
{
	m_reader.Close();
}

void TSDecoder::OnConfigChange(const BYTE* bits, LONG size, LPVOID)
{
	m_decoder.Close();
	m_decoder.Initialize({ 1280,720 }, { 1280,720 });
	m_decoder.Open(const_cast<BYTE*>(bits), size);
}

void TSDecoder::Invoke()
{
	for (;;)
	{
		TS_BLOCK block = { 0 };
		m_reader.ReadBlock(block);
		if (block.streamType == TS_STREAM_TYPE_VIDEO_H264)
		{
			SampleTag tag = { 0 };
			tag.size = block.video.size;
			tag.bits = new BYTE[tag.size];
			tag.sampleDTS = 40;
			tag.samplePTS = 40;
			memcpy(tag.bits, block.video.data, tag.size);
			BYTE* bo = NULL;
			LONG so = 0;
			m_decoder.Decode(tag, bo, so);

			SAFE_DELETE_ARRAY(tag.bits);
		}
		SAFE_DELETE_ARRAY(block.audio.data);
		SAFE_DELETE_ARRAY(block.video.data);
	}
}

INT APIENTRY _tWinMain(HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	INT       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WSADATA   wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	MFStartup(MF_VERSION);

	HRESULT hRes = OleInitialize(NULL);
	LoadSeDebugPrivilege();
	CoInitialize(NULL);
	avcodec_register_all();
	/*FILE* hFile1 = NULL;
	fopen_s(&hFile1, "D:\\test.264", "wb+");*/

	//FILE* hFile2 = NULL;
	//fopen_s(&hFile2, "D:\\test.aac", "wb+");

	//FLVParser parser;
	//parser.Open("D:\\10s.flv");
	//parser.Parse();
	//parser.Close();

	TSDecoder decoder;
	decoder.Open("D:\\1.ts");
	decoder.Invoke();
	decoder.Close();
	/*reader.OpenFile("D:\\1.ts");
	for (;;)
	{
		TS_BLOCK block;
		if (!reader.ReadBlock(block))
		{
			break;
		}
		if (block.streamType == TS_STREAM_TYPE_VIDEO_H264)
		{
			++count;
			TRACE("Size:%d\n", block.video.size);
		}
		SAFE_DELETE_ARRAY(block.audio.data);
		SAFE_DELETE_ARRAY(block.video.data);
	}*/
	//fclose(hFile1);
	//fclose(hFile2);
	//MediaTest test;
	//test.H264ToI420("D:\\Media\\test.264", "D:\\Media\\test.yuv");
	//test.MP3ToWave("D:\\Media\\李玉刚-刚好遇见你.mp3", "D:\\Media\\李玉刚-刚好遇见你.wav");
	/*FILE* hFile = NULL;
	fopen_s(&hFile, "D:\\test.264", "rb");
	TinyMFIntelQSVDecode decoder;
	decoder.Open({ 1280,720 }, 25);
	INT pos = 0;
	TinyScopedArray<BYTE> bits(new BYTE[1024 * 64]);
	for (;;)
	{
		size_t count = fread(bits, 1, 1024 * 64, hFile);
		if (count == 0)
			break;
		BYTE* bo = NULL;
		DWORD so = 0;
		SampleTag tag = { 0 };
		tag.bits = bits;
		tag.size = count;
		decoder.Decode(tag, bo, so);
	}
	fclose(hFile);*/

	CoUninitialize();

	/*TinyVisualResource::GetInstance().Load("skin\\resource.xml");
	::DefWindowProc(NULL, 0, 0, 0L);
	TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_TINYAPP));
	TinyMessageLoop theLoop;
	TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
	TinyVisualLayeredWindow uiImpl;
	string szFile = StringPrintf("%s\%s", TinyVisualResource::GetInstance().GetDefaultPath().c_str(), "skin\\contextmenu.xml");
	uiImpl.Create(NULL, szFile.c_str());
	TinyVisualContextMenu* contextmenu = (TinyVisualContextMenu*)uiImpl.GetDocument().GetVisualByName("menu1");
	contextmenu->Add("Test1");
	contextmenu->Add("Test2");
	contextmenu->Add("Test3");
	uiImpl.Update();*/
	/*INT loopRes = theLoop.MessageLoop();
	TinyApplication::GetInstance()->RemoveMessageLoop();
	TinyApplication::GetInstance()->Uninitialize();*/

	OleUninitialize();
	MFShutdown();
	WSACleanup();

	return 0;
};