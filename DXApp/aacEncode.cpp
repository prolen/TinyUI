#include "stdafx.h"
#include "aacEncode.h"


aacEncode::aacEncode()
	:m_dwTime(0),
	m_inputSamples(0),
	m_maxOutputBytes(0),
	m_dwINC(0),
	m_dwPTS(0),
	m_aacFile(NULL)
{
	fopen_s(&m_aacFile, "D:\\123.aac", "wb");;
}

aacEncode::~aacEncode()
{
	fclose(m_aacFile);
}

BOOL aacEncode::GetSpecificInfo(vector<BYTE>& info)
{
	BYTE* buffer = NULL;
	ULONG size = 0;
	if (faacEncGetDecoderSpecificInfo(m_aac, &buffer, &size) != 0)
	{
		SAFE_FREE(buffer);
		return FALSE;
	}
	info.resize(size);
	memcpy(&info[0], buffer, size);
	SAFE_FREE(buffer);
	return TRUE;
}
DWORD aacEncode::GetLatestPTS() const
{
	return m_dwPTS;
}
BOOL aacEncode::Open(const WAVEFORMATEX& wfx, INT audioRate)
{
	Close();
	m_wfx = wfx;
	m_aac = faacEncOpen(wfx.nSamplesPerSec, wfx.nChannels, &m_inputSamples, &m_maxOutputBytes);
	if (!m_aac)
		return FALSE;
	//AAC�̶�1024
	m_dwTime = AAC_TIMEBASE * AAC_TIMEDEN / wfx.nSamplesPerSec;//����һ֡ʱ��
	m_config = faacEncGetCurrentConfiguration(m_aac);
	switch (wfx.wBitsPerSample)
	{
	case 16:
		m_config->inputFormat = FAAC_INPUT_16BIT;
		break;
	case 24:
		m_config->inputFormat = FAAC_INPUT_24BIT;
		break;
	case 32:
		m_config->inputFormat = FAAC_INPUT_32BIT;
		break;
	default:
		m_config->inputFormat = FAAC_INPUT_FLOAT;
		break;
	}
	m_config->mpegVersion = MPEG4;
	m_config->aacObjectType = LOW;
	m_config->allowMidside = 1;
	m_config->useLfe = 0;
	m_config->useTns = 0;
	m_config->quantqual = 100;
	m_config->bandWidth = 0;
	m_config->outputFormat = 1; //ADTS
	m_config->bitRate = audioRate * 1000 / wfx.nChannels;//����λ
	INT iRes = faacEncSetConfiguration(m_aac, m_config);
	if (!iRes)
		return FALSE;
	m_bits.Reset(new BYTE[m_maxOutputBytes]);
	return TRUE;
}
BOOL aacEncode::Encode(BYTE* bits, INT size)
{
	if (!bits)
		return FALSE;
	INT s = faacEncEncode(m_aac, (int32_t*)bits, m_inputSamples, m_bits, m_maxOutputBytes);
	if (s > 0)
	{
		m_dwPTS = m_dwINC++ * m_dwTime;
		TinyScopedReferencePtr<Sample> sample(new Sample(s));
		sample->Fill(bits, s);
		sample->INC = m_dwINC;
		sample->PTS = m_dwPTS;
		sample->DTS = 0;
		sample->Track = 1;//��Ƶ
		OnDone(sample);
		fwrite(m_bits, 1, s, m_aacFile);
		return TRUE;
	}
	return FALSE;
}
void aacEncode::Close()
{
	if (m_aac)
	{
		faacEncClose(m_aac);
		m_aac = NULL;
	}
}
void aacEncode::OnDone(TinyScopedReferencePtr<Sample>& sample)
{
	EVENT_DONE(sample);
}