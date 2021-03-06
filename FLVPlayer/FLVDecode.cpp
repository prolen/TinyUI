#include "stdafx.h"
#include "FLVDecode.h"
using namespace TinyUI::Network;

namespace FLVPlayer
{
	FLVDecode::FLVDecode(HWND hWND)
		:m_hWND(hWND),
		m_audioQueue(m_lock1),
		m_videoQueue(m_lock2),
		m_videoTask(*this),
		m_audioTask(*this),
		m_audioRender(m_audioTask),
		m_videoRender(m_videoTask),
		m_baseTime(-1),
		m_basePTS(-1),
		m_bFirstI(FALSE)
	{
		m_aac.Reset(new AACDecoder());
		m_x264.Reset(new x264Decoder());
		m_close.CreateEvent(FALSE, FALSE, NULL, NULL);
	}

	FLVDecode::~FLVDecode()
	{

	}
	BOOL FLVDecode::Submit()
	{
		//if (m_reader.OpenFile("D:\\Media\\1.flv"))
		if (m_reader.OpenURL("rtmp://10.10.13.98/live/lb_junlvjuchang_720p", BindCallback(&FLVDecode::OnError, this)))
		{
			m_size.cx = static_cast<LONG>(m_reader.GetScript().width);
			m_size.cy = static_cast<LONG>(m_reader.GetScript().height);
			if (TinyTask::Submit(BindCallback(&FLVDecode::OnMessagePump, this)))
			{
				m_audioRender.Submit();
				m_audioTask.Submit();
				m_videoRender.Submit();
				m_videoTask.Submit();
				return TRUE;
			}
		}
		return FALSE;
	}
	void FLVDecode::OnError(INT iError)
	{

	}
	BOOL FLVDecode::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		m_audioRender.Close(dwMS);
		m_videoRender.Close(dwMS);
		m_videoTask.Close(dwMS);
		m_audioTask.Close(dwMS);
		return TinyTask::Close(dwMS);
	}

	void FLVDecode::OnData(BYTE* bits, LONG size, LPVOID ps)
	{

	}

	void FLVDecode::OnMessagePump()
	{
		CoInitialize(NULL);
		SampleTag tag = { 0 };
		FLV_BLOCK block = { 0 };
		for (;;)
		{
			if (m_close.Lock(0))
			{
				break;
			}
			INT size = m_audioQueue.GetSize() + m_videoQueue.GetSize();
			if (size > MAX_QUEUE_SIZE)
			{
				Sleep(5);
				continue;
			}
			if (!m_reader.ReadBlock(block))
			{
				break;
			}
			if (block.type == FLV_AUDIO)
			{
				if (block.audio.codeID == FLV_CODECID_AAC)
				{
					if (block.audio.packetType == FLV_AudioSpecificConfig)
					{
						if (!m_aac->Open(block.audio.data, block.audio.size, block.audio.bitsPerSample == 0 ? 8 : 16))
						{
							goto _ERROR;
						}
						SAFE_DELETE_ARRAY(block.audio.data);
						SAFE_DELETE_ARRAY(block.video.data);
					}
					if (block.audio.packetType == FLV_AACRaw)
					{
						if (!m_bFirstI)
						{
							SAFE_DELETE_ARRAY(block.audio.data);
							SAFE_DELETE_ARRAY(block.video.data);
							continue;
						}
						ZeroMemory(&tag, sizeof(tag));
						tag.size = block.video.size;
						tag.bits = new BYTE[tag.size];
						memcpy_s(tag.bits, tag.size, block.audio.data, block.audio.size);
						tag.sampleDTS = block.dts;
						tag.samplePTS = block.pts;
						m_audioQueue.Push(tag);
					}
				}
			}
			if (block.type == FLV_VIDEO)
			{
				if (block.video.codeID == FLV_CODECID_H264)
				{
					if (block.video.packetType == FLV_AVCDecoderConfigurationRecord)
					{
						if (!m_decoder)
						{
							m_decoder.Reset(new TinyMFIntelQSVDecode());
						}
						BOOL bRes = m_decoder->Open(m_size, 25);
						BYTE* bo = NULL;
						DWORD so = 0;
						SampleTag sampleTag;
						sampleTag.bits = block.video.data;
						sampleTag.size = block.video.size;
						bRes = m_decoder->Decode(sampleTag, bo, so);
						if (!m_x264->Initialize(m_size, m_size))
						{
							goto _ERROR;
						}
						if (!m_x264->Open(block.video.data, block.video.size))
						{
							goto _ERROR;
						}
						SAFE_DELETE_ARRAY(block.audio.data);
						SAFE_DELETE_ARRAY(block.video.data);
					}
					if (block.video.packetType == FLV_NALU)
					{
						if (!m_bFirstI)
						{
							if (block.video.codeType != 1)
							{
								SAFE_DELETE_ARRAY(block.audio.data);
								SAFE_DELETE_ARRAY(block.video.data);
								continue;
							}
							m_bFirstI = TRUE;
						}
						ZeroMemory(&tag, sizeof(tag));
						tag.size = block.video.size;
						tag.bits = new BYTE[tag.size];
						memcpy_s(tag.bits, tag.size, block.video.data, block.video.size);
						tag.sampleDTS = block.dts;
						tag.samplePTS = block.pts;
						BYTE* bo = NULL;
						DWORD so = 0;
						BOOL bRes = m_decoder->Decode(tag, bo, so);
						m_videoQueue.Push(tag);
					}
				}
			}
		}
	_ERROR:
		SAFE_DELETE_ARRAY(block.audio.data);
		SAFE_DELETE_ARRAY(block.video.data);
		CoUninitialize();
	}

	//////////////////////////////////////////////////////////////////////////
	FLVAudioRender::FLVAudioRender(FLVVAudioTask& decode)
		:m_decode(decode),
		m_bInitialize(FALSE),
		m_bFlag(FALSE),
		m_dwMS(0)
	{
		m_close.CreateEvent();
		m_events[0].CreateEvent(TRUE, FALSE);
		m_events[1].CreateEvent(TRUE, FALSE);
		m_events[2].CreateEvent(TRUE, FALSE);
	}
	FLVAudioRender::~FLVAudioRender()
	{

	}
	BOOL FLVAudioRender::Submit()
	{
		m_player.Close();
		if (!m_player.Open(m_decode.m_decode.m_hWND))
			return FALSE;
		return TinyTask::Submit(BindCallback(&FLVAudioRender::OnMessagePump, this));
	}
	BOOL FLVAudioRender::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTask::Close(dwMS);
	}
	void FLVAudioRender::OnMessagePump()
	{
		TinyPerformanceTimer timer;
		DWORD	dwOffset = 0;
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			SampleTag tag = m_decode.m_queue.Pop();
			if (tag.size <= 0)
				continue;
			if (tag.samplePTS == m_decode.m_decode.m_basePTS)
			{
				TinyAutoLock lock(m_decode.m_decode.m_lockTime);
				m_decode.m_decode.m_baseTime = timeGetTime();
			}
			while (m_decode.m_decode.m_baseTime == -1);
			if (!m_bInitialize)
			{
				m_time.BeginTime();
				m_bInitialize = TRUE;
				if (!m_player.SetFormat(m_decode.m_decode.m_aac->GetFormat(), tag.size * 3))
					break;
				DSBPOSITIONNOTIFY vals[3];
				vals[0].dwOffset = tag.size - 1;
				vals[0].hEventNotify = m_events[0];
				vals[1].dwOffset = tag.size * 2 - 1;
				vals[1].hEventNotify = m_events[1];
				vals[2].dwOffset = tag.size * 3 - 1;
				vals[2].hEventNotify = m_events[2];
				m_player.SetNotifys(3, vals);
				m_time.EndTime();
				m_decode.m_decode.m_lockTime.Lock();
				m_decode.m_decode.m_baseTime += m_time.GetMillisconds();
				m_decode.m_decode.m_lockTime.Unlock();
				DWORD dwMS = timeGetTime() - m_decode.m_decode.m_baseTime;
				INT offset = tag.samplePTS - dwMS;
				if (timer.Waiting(offset, 1000))
				{
					m_player.Fill(tag.bits, tag.size, dwOffset);
				}
				m_player.Play();
			}
			else
			{
				m_player.Fill(tag.bits, tag.size, dwOffset);
			}
			SAFE_DELETE_ARRAY(tag.bits);
			DWORD dwMS = timeGetTime() - m_decode.m_decode.m_baseTime;
			INT offset = tag.samplePTS - dwMS;
			TRACE("Audio Delay:%d\n", offset);
			HANDLE handles[3] = { m_events[0],m_events[1],m_events[2] };
			HRESULT hRes = WaitForMultipleObjects(3, handles, FALSE, INFINITE);
			switch (hRes)
			{
			case WAIT_OBJECT_0:
				dwOffset = 0;
				break;
			case WAIT_OBJECT_0 + 1:
				dwOffset = m_player.GetSize() / 3 * 1;
				break;
			case WAIT_OBJECT_0 + 2:
				dwOffset = m_player.GetSize() / 3 * 2;
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	FLVVideoRender::FLVVideoRender(FLVVideoTask& decode)
		:m_decode(decode),
		m_wPTS(0),
		m_bFlag(FALSE),
		m_dwMS(0)
	{
		m_close.CreateEvent(FALSE, FALSE, NULL, NULL);
	}
	FLVVideoRender::~FLVVideoRender()
	{

	}
	BOOL FLVVideoRender::Submit()
	{
		TinyRectangle rectangle;
		GetClientRect(m_decode.m_decode.m_hWND, &rectangle);
		BOOL bRes = m_graphics.Initialize(m_decode.m_decode.m_hWND, rectangle.Size());
		bRes = m_image.Create(m_graphics.GetDX9(), m_decode.m_decode.m_size.cx, m_decode.m_decode.m_size.cy, NULL);
		m_font2D.Load(m_graphics.GetDX9(), (HFONT)GetStockObject(DEFAULT_GUI_FONT));
		return TinyTask::Submit(BindCallback(&FLVVideoRender::OnMessagePump, this));
	}
	BOOL FLVVideoRender::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTask::Close(dwMS);
	}
	void FLVVideoRender::OnMessagePump()
	{
		TinyPerformanceTimer timer;
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			SampleTag tag = m_decode.m_queue.Pop();
			if (tag.size <= 0)
				continue;
			if (tag.samplePTS == m_decode.m_decode.m_basePTS)
			{
				TinyAutoLock lock(m_decode.m_decode.m_lockTime);
				m_decode.m_decode.m_baseTime = timeGetTime();
			}
			while (m_decode.m_decode.m_baseTime == -1);
			DWORD dwMS = timeGetTime() - m_decode.m_decode.m_baseTime;
			INT offset = tag.samplePTS - dwMS;
			if (timer.Waiting(offset, 1000))
			{
				OnRender(tag.bits, tag.size);
			}
			SAFE_DELETE_ARRAY(tag.bits);
		}
	}
	void FLVVideoRender::OnRender(BYTE* bits, LONG size)
	{
		if (!m_graphics.IsActive())
		{
			if (m_graphics.GetDX9().CheckReason(D3DERR_DEVICENOTRESET))
			{
				if (m_graphics.Reset())
				{
					m_image.Destory();
					m_image.Create(m_graphics.GetDX9(), m_decode.m_decode.m_size.cx, m_decode.m_decode.m_size.cy, NULL);
				}
			}
		}
		else
		{
			m_image.Copy(bits, size);
			m_graphics.SetRenderView(NULL);
			m_graphics.GetRenderView()->BeginDraw();
			string val("ABC");
			m_graphics.DrawString(&m_font2D, val.c_str(), val.size(), NULL, DT_CENTER, D3DCOLOR_XRGB(255, 255, 0));
			m_graphics.DrawImage(&m_image);
			m_graphics.GetRenderView()->EndDraw();
			m_graphics.Present();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	FLVVideoTask::FLVVideoTask(FLVDecode& decode)
		:m_decode(decode),
		m_queue(m_lock)
	{
		m_close.CreateEvent(FALSE, FALSE, NULL, NULL);
	}
	FLVVideoTask::~FLVVideoTask()
	{

	}
	BOOL FLVVideoTask::Submit()
	{
		return TinyTask::Submit(BindCallback(&FLVVideoTask::OnMessagePump, this));
	}
	BOOL FLVVideoTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTask::Close(dwMS);
	}

	void FLVVideoTask::OnQSV(Media::SampleTag& tag)
	{
		if (m_decode.m_basePTS == -1)
		{
			m_decode.m_basePTS = tag.samplePTS;
		}
		m_queue.Push(tag);
	}

	void FLVVideoTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
			{
				break;
			}
			INT size = m_queue.GetSize();
			if (size > MAX_VIDEO_QUEUE_SIZE)
			{
				Sleep(3);
				continue;
			}
			SampleTag tag = m_decode.m_videoQueue.Pop();
			if (tag.size > 0)
			{
				//mfxFrameSurface1* surface1 = NULL;
				//if (m_decode.m_qsv.Decode(tag, surface1))
				//{
				//	QSV::QSVAllocator* pAllocator = m_decode.m_qsv.GetAllocator();
				//	pAllocator->Lock(pAllocator->pthis, surface1->Data.MemId, &(surface1->Data));
				//	tag.size = surface1->Info.CropH * surface1->Data.Pitch;
				//	tag.bits = new BYTE[tag.size];
				//	memcpy(tag.bits, surface1->Data.B, tag.size);
				//	pAllocator->Unlock(pAllocator->pthis, surface1->Data.MemId, &(surface1->Data));
				//	m_decode.m_qsv.UnlockSurface(surface1);
				//	if (m_decode.m_basePTS == -1)
				//	{
				//		m_decode.m_basePTS = tag.samplePTS;
				//	}
				//	m_queue.Push(tag);
				//}
				BYTE* bo = NULL;
				LONG  so = 0;
				if (m_decode.m_x264->Decode(tag, bo, so))
				{
					SAFE_DELETE_ARRAY(tag.bits);
					tag.bits = new BYTE[so];
					memcpy(tag.bits, bo, so);
					tag.size = so;
					tag.samplePTS = m_decode.m_x264->GetYUV420()->pts;
					tag.sampleDTS = tag.samplePTS;
					if (m_decode.m_basePTS == -1)
					{
						m_decode.m_basePTS = tag.samplePTS;
					}
					m_queue.Push(tag);
				}
				else
				{
					SAFE_DELETE_ARRAY(tag.bits);
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	FLVVAudioTask::FLVVAudioTask(FLVDecode& decode)
		:m_decode(decode),
		m_queue(m_lock)
	{
		m_close.CreateEvent(FALSE, FALSE, NULL, NULL);
	}
	FLVVAudioTask::~FLVVAudioTask()
	{

	}
	BOOL FLVVAudioTask::Submit()
	{
		return TinyTask::Submit(BindCallback(&FLVVAudioTask::OnMessagePump, this));
	}
	BOOL FLVVAudioTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTask::Close(dwMS);
	}
	void FLVVAudioTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
			{
				break;
			}
			INT size = m_queue.GetSize();
			if (size > MAX_AUDIO_QUEUE_SIZE || m_decode.m_audioQueue.IsEmpty())
			{
				Sleep(3);
				continue;
			}
			SampleTag tag = m_decode.m_audioQueue.Pop();
			if (tag.size > 0)
			{
				BYTE* bo = NULL;
				LONG  so = 0;
				if (m_decode.m_aac->Decode(tag, bo, so))
				{
					tag.bits = new BYTE[so];
					memcpy(tag.bits, bo, so);
					tag.size = so;
					if (m_decode.m_basePTS == -1)
					{
						m_decode.m_basePTS = tag.samplePTS;
					}
					m_queue.Push(tag);
				}
			}
		}
	}
}

