#pragma once
#include "TinyWave.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include <vector>
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dsound.lib")

namespace TinyUI
{
	namespace Media
	{
		typedef struct tagCAPTUREDEVICE
		{
			GUID	Guid;
			string	Description;
			string	Module;
		}CAPTUREDEVICE;
		/// <summary>
		/// DirectSound��Ƶ����
		/// </summary>
		class TinySoundCapture
		{
			DISALLOW_COPY_AND_ASSIGN(TinySoundCapture)
		public:
			TinySoundCapture();
			virtual ~TinySoundCapture();
		public:
			static BOOL Enumerate(std::vector<CAPTUREDEVICE>& devices);
			/// <summary>
			/// WAVE_FORMAT_1M08	11.025 kHz, mono, 8-bit
			/// WAVE_FORMAT_1M16	11.025 kHz, mono, 16 - bit
			/// WAVE_FORMAT_1S08	11.025 kHz, stereo, 8 - bit
			/// WAVE_FORMAT_1S16	11.025 kHz, stereo, 16 - bit
			/// WAVE_FORMAT_2M08	22.05 kHz, mono, 8 - bit
			/// WAVE_FORMAT_2M16	22.05 kHz, mono, 16 - bit
			/// WAVE_FORMAT_2S08	22.05 kHz, stereo, 8 - bit
			/// WAVE_FORMAT_2S16	22.05 kHz, stereo, 16 - bit
			/// WAVE_FORMAT_4M08	44.1 kHz, mono, 8 - bit
			/// WAVE_FORMAT_4M16	44.1 kHz, mono, 16 - bit
			/// WAVE_FORMAT_4S08	44.1 kHz, stereo, 8 - bit
			/// WAVE_FORMAT_4S16	44.1 kHz, stereo, 16 - bit
			/// WAVE_FORMAT_96M08	96 kHz, mono, 8 - bit
			/// WAVE_FORMAT_96M16	96 kHz, mono, 16 - bit
			/// WAVE_FORMAT_96S08	96 kHz, stereo, 8 - bit
			/// WAVE_FORMAT_96S16	96 kHz, stereo, 16 - bit
			/// </summary>
			static BOOL GetCaps(const GUID& guid, DWORD& dwFormats);
			BOOL	SetNotifys(DWORD dwCount, LPCDSBPOSITIONNOTIFY pNotify);
			BOOL	SetFormat(const  WAVEFORMATEX* pFMT, DWORD dwDataSize);
			BOOL	Open(const GUID& guid);
			BOOL	Start(DWORD dwFlags = DSCBSTART_LOOPING);
			BOOL	Stop();
			BOOL	Close();
			BOOL	GetBuffer(BYTE*& bits, LONG& size, DWORD dwFlag = DSCBLOCK_ENTIREBUFFER);
			DWORD	GetSize() const;
		private:
			static BOOL CALLBACK DSEnumCallback(LPGUID, LPCSTR, LPCSTR, LPVOID);
		private:
			DWORD									m_dwCount;
			DWORD									m_dwSize;
			DWORD									m_dwOffset;
			TinyScopedArray<BYTE>					m_bits;
			TinyScopedArray<BYTE>					m_waveFMT;
			TinyComPtr<IDirectSoundCapture8>		m_dsc8;
			TinyComPtr<IDirectSoundCaptureBuffer8>	m_dscb8;
		};
		/// <summary>
		/// DirectSound����AEC��Ƶ����
		/// </summary>
		class TinySoundCaptureAEC
		{
			DISALLOW_COPY_AND_ASSIGN(TinySoundCaptureAEC)
		public:
			TinySoundCaptureAEC();
			virtual ~TinySoundCaptureAEC();
			BOOL	Open(LPGUID captureGUID, LPGUID renderGUID, const  WAVEFORMATEX* pFMT, DWORD dwSize);
		private:
			TinyComPtr<IDirectSoundFullDuplex>		m_dsduplex;
			TinyComPtr<IDirectSoundBuffer8>			m_dsb8;
			TinyComPtr<IDirectSoundCaptureBuffer8>	m_dscb8;
		};
	}
}

