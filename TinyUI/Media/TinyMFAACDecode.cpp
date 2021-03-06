#include "../stdafx.h"
#include "TinyMFAACDecode.h"
#include <algorithm>
#include <limits>

namespace TinyUI
{
	namespace Media
	{
		INT GetSRIndex(UINT sampleRate)
		{
			if (92017 <= sampleRate) return 0;
			if (75132 <= sampleRate) return 1;
			if (55426 <= sampleRate) return 2;
			if (46009 <= sampleRate) return 3;
			if (37566 <= sampleRate) return 4;
			if (27713 <= sampleRate) return 5;
			if (23004 <= sampleRate) return 6;
			if (18783 <= sampleRate) return 7;
			if (13856 <= sampleRate) return 8;
			if (11502 <= sampleRate) return 9;
			if (9391 <= sampleRate) return 10;
			return 11;
		}

		TinyMFAACDecode::TinyMFAACDecode()
		{
		}
		TinyMFAACDecode::~TinyMFAACDecode()
		{

		}

		BOOL TinyMFAACDecode::Open(const WAVEFORMATEX* pFMT, DWORD dwBitRate, BOOL bADTS)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaType> inputType;
			hRes = MFCreateMediaType(&inputType);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, pFMT->wBitsPerSample);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, pFMT->nSamplesPerSec);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, pFMT->nChannels);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, dwBitRate / 8);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, 1);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AAC_PAYLOAD_TYPE, bADTS ? 1 : 0);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION, 0x29);
			if (hRes != S_OK)
				return FALSE;
			BYTE heaac[MF_MT_USER_DATA_SIZE] = { 0 };
			heaac[0] = bADTS ? 1 : 0;
			heaac[2] = 0x29;
			UINT AUDIO_OBJECT_AAC_LC = 2;
			UINT SAMPLE_FREQUENCY_IDX = GetSRIndex(pFMT->nSamplesPerSec);
			UINT audioSpecificConfig = (AUDIO_OBJECT_AAC_LC << 11) | (SAMPLE_FREQUENCY_IDX << 7) | (pFMT->nChannels << 3);
			heaac[MF_MT_USER_DATA_SIZE - 2] = (audioSpecificConfig & 0xFF00) >> 8;;
			heaac[MF_MT_USER_DATA_SIZE - 1] = audioSpecificConfig & 0x00FF;
			hRes = inputType->SetBlob(MF_MT_USER_DATA, heaac, MF_MT_USER_DATA_SIZE);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaType> outputType;
			hRes = MFCreateMediaType(&outputType);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFInitMediaTypeFromWaveFormatEx(outputType, pFMT, sizeof(WAVEFORMATEX) + pFMT->cbSize);
			if (hRes != S_OK)
				return FALSE;
			return TinyMFDecode::Open(CLSID_CMSAACDecMFT, inputType, outputType);
		}
	};
}
