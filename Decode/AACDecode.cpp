#include "stdafx.h"
#include "AACDecode.h"

namespace Decode
{
	AACDecode::AACDecode()
		:m_handle(NULL),
		m_dwINC(0)
	{
	}
	AACDecode::~AACDecode()
	{
		Close();
	}
	BOOL AACDecode::Open(BYTE* adts, LONG size, WORD wBitsPerSample)
	{
		ZeroMemory(&m_sMFT, sizeof(m_sMFT));
		m_handle = NeAACDecOpen();
		if (!m_handle)
			return FALSE;
		ULONG sampleRate = 0;
		BYTE channel = 0;
		if (NeAACDecInit2(m_handle, adts, size - 1, &sampleRate, &channel))
			return FALSE;
		m_sMFT.nSamplesPerSec = sampleRate;
		m_sMFT.nChannels = channel;
		m_sMFT.wBitsPerSample = wBitsPerSample;
		m_sMFT.nBlockAlign = wBitsPerSample *channel / 8;
		m_sMFT.nAvgBytesPerSec = m_sMFT.nSamplesPerSec * m_sMFT.nBlockAlign;
		m_sMFT.wFormatTag = WAVE_FORMAT_PCM;
		return TRUE;
	}
	BOOL AACDecode::Open(WORD wBitsPerSample, WORD wSampleRate)
	{
		m_handle = NeAACDecOpen();
		if (!m_handle)
			return FALSE;
		NeAACDecConfiguration* config = NeAACDecGetCurrentConfiguration(m_handle);
		if (!config)
			return FALSE;
		config->outputFormat = FAAD_FMT_FLOAT;
		switch (wBitsPerSample)
		{
		case 16:
			config->outputFormat = FAAD_FMT_16BIT;
			break;
		case 24:
			config->outputFormat = FAAD_FMT_24BIT;
			break;
		case 32:
			config->outputFormat = FAAD_FMT_32BIT;
			break;
		}
		config->defSampleRate = wSampleRate;
		config->defObjectType = LOW;
		config->downMatrix = 1;
		config->useOldADTSFormat = 0;
		if (NeAACDecSetConfiguration(m_handle, config))
			return FALSE;
		return TRUE;
	}
	WAVEFORMATEX AACDecode::GetFormat() const
	{
		return m_sMFT;
	}
	BOOL AACDecode::Decode(BYTE* bits, LONG size)
	{
		ASSERT(m_handle);
		BYTE* data = reinterpret_cast<BYTE*>(NeAACDecDecode(m_handle, &m_frame, bits, size));
		if (m_frame.error > 0 || !data || m_frame.samples <= 0)
			return FALSE;
		LONG o = m_frame.samples * m_frame.channels;
		OnDone(data, o, NULL);
		return TRUE;
	}
	BOOL AACDecode::Close()
	{
		if (m_handle != NULL)
		{
			NeAACDecClose(m_handle);
			m_handle = NULL;
		}
		return TRUE;
	}
	void AACDecode::OnDone(BYTE* bits, LONG size, LPVOID ps)
	{
		EVENT_DONE(bits, size, ps);
	}
}
