#include "../stdafx.h"
#include "TinyMFEncode.h"
#include <algorithm>
#include <limits>

namespace TinyUI
{
	namespace Media
	{
		TinyMFEncode::TinyMFEncode()
			:m_bIsAsync(FALSE),
			m_dwInputID(0),
			m_dwOutputID(0)
		{

		}
		TinyMFEncode::~TinyMFEncode()
		{
			Close();
		}
		BOOL TinyMFEncode::GetInputType(IMFMediaType** mediaType)
		{
			ASSERT(m_encoder);
			return SUCCEEDED(m_encoder->GetInputCurrentType(m_dwInputID, mediaType));
		}
		BOOL TinyMFEncode::GetOutputType(IMFMediaType** mediaType)
		{
			ASSERT(m_encoder);
			return SUCCEEDED(m_encoder->GetOutputCurrentType(m_dwOutputID, mediaType));
		}
		BOOL TinyMFEncode::Create(const GUID& clsID, IMFMediaType* inputType, IMFMediaType* outputType)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IUnknown> unknow;
			hRes = unknow.CoCreateInstance(clsID, NULL, CLSCTX_INPROC_SERVER);
			if (hRes != S_OK)
				return FALSE;
			hRes = unknow->QueryInterface(IID_PPV_ARGS(&m_encoder));
			if (hRes != S_OK)
				return FALSE;
			DWORD dwInputStreams = 0;
			DWORD dwOutputSample = 0;
			hRes = m_encoder->GetStreamCount(&dwInputStreams, &dwOutputSample);
			if (hRes != S_OK)
				return FALSE;
			if (dwInputStreams == 0 || dwOutputSample == 0)
				return FALSE;
			TinyScopedArray<DWORD> dwInputIDs;
			TinyScopedArray<DWORD> dwOutputIDs;
			hRes = m_encoder->GetStreamIDs(dwInputStreams, dwInputIDs, dwOutputSample, dwOutputIDs);
			if (hRes == S_OK)
			{
				m_dwInputID = dwInputIDs[0];
				m_dwOutputID = dwOutputIDs[0];
			}
			if (IsAsyncMFT(m_encoder, m_bIsAsync) && m_bIsAsync)
			{
				hRes = m_encoder->QueryInterface(&m_eventGenerator);
				if (hRes != S_OK)
					return FALSE;
			}
			if (inputType != NULL)
			{
				hRes = m_encoder->SetInputType(m_dwInputID, inputType, 0);
				if (hRes != S_OK)
					return FALSE;
				if (outputType == NULL)
				{
					TinyComPtr<IMFMediaType> mediaType;
					DWORD dwTypeIndex = 0;
					while (SUCCEEDED(m_encoder->GetOutputAvailableType(m_dwInputID, dwTypeIndex++, &mediaType)))
					{
						BOOL bResult;
						hRes = mediaType->Compare(mediaType, MF_ATTRIBUTES_MATCH_OUR_ITEMS, &bResult);
						if (SUCCEEDED(hRes) && bResult)
						{
							hRes = m_encoder->SetOutputType(m_dwOutputID, mediaType, 0);
							if (hRes != S_OK)
								return FALSE;
							break;
						}
					}
				}
				else
				{
					hRes = m_encoder->SetOutputType(m_dwOutputID, outputType, 0);
					if (hRes != S_OK)
						return FALSE;
				}
			}
			else
			{
				if (outputType == NULL)
					return FALSE;
				hRes = m_encoder->SetOutputType(m_dwOutputID, outputType, 0);
				if (hRes != S_OK)
					return FALSE;
				TinyComPtr<IMFMediaType> mediaType;
				DWORD dwTypeIndex = 0;
				while (SUCCEEDED(m_encoder->GetInputAvailableType(m_dwInputID, dwTypeIndex++, &mediaType)))
				{
					BOOL bResult;
					hRes = mediaType->Compare(mediaType, MF_ATTRIBUTES_MATCH_OUR_ITEMS, &bResult);
					if (SUCCEEDED(hRes) && bResult)
					{
						hRes = m_encoder->SetInputType(m_dwInputID, mediaType, 0);
						if (hRes != S_OK)
							return FALSE;
						break;
					}
				}
			}
			hRes = m_encoder->GetInputStreamInfo(m_dwInputID, &m_inputInfo);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_encoder->GetOutputStreamInfo(m_dwOutputID, &m_outputInfo);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFEncode::Open(const GUID& clsID, IMFMediaType* inputType, IMFMediaType* outputType, Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			m_callback = std::move(callback);
			if (!Create(clsID, inputType, outputType))
				return FALSE;
			HRESULT hRes = S_OK;
			hRes = m_encoder->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_encoder->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_encoder->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFEncode::CreateInputSample(const BYTE* bits, DWORD dwSize)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaBuffer> buffer;
			if (!m_inputSample)
			{
				hRes = MFCreateSample(&m_inputSample);
				if (hRes != S_OK)
					return FALSE;
				if (m_inputInfo.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(m_inputInfo.cbSize, dwSize), m_inputInfo.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(m_inputInfo.cbSize, dwSize), &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				hRes = m_inputSample->AddBuffer(buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			hRes = m_inputSample->GetBufferByIndex(0, &buffer);
			if (hRes != S_OK)
				return FALSE;
			DWORD dwMax = 0;
			hRes = buffer->GetMaxLength(&dwMax);
			if (hRes != S_OK)
				return FALSE;
			if (dwMax < dwSize)
			{
				hRes = m_inputSample->RemoveAllBuffers();
				if (hRes != S_OK)
					return FALSE;
				if (m_inputInfo.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(m_inputInfo.cbSize, dwSize), m_inputInfo.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(m_inputInfo.cbSize, dwSize), &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				hRes = m_inputSample->AddBuffer(buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			BYTE* ps = NULL;
			hRes = buffer->Lock(&ps, NULL, NULL);
			if (hRes != S_OK)
				return FALSE;
			memcpy(ps, bits, dwSize);
			hRes = buffer->Unlock();
			if (hRes != S_OK)
				return FALSE;
			hRes = buffer->SetCurrentLength(dwSize);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFEncode::CreateOutputSample(DWORD dwSize)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaBuffer> buffer;
			if (!m_outputSample)
			{
				hRes = MFCreateSample(&m_outputSample);
				if (hRes != S_OK)
					return FALSE;
				if (m_outputInfo.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(m_outputInfo.cbSize, dwSize), m_outputInfo.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(m_outputInfo.cbSize, dwSize), &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				hRes = m_outputSample->AddBuffer(buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			hRes = m_outputSample->GetBufferByIndex(0, &buffer);
			if (hRes != S_OK)
				return FALSE;
			DWORD dwMax = 0;
			hRes = buffer->GetMaxLength(&dwMax);
			if (hRes != S_OK)
				return FALSE;
			if (dwMax < dwSize)
			{
				hRes = m_outputSample->RemoveAllBuffers();
				if (hRes != S_OK)
					return FALSE;
				if (m_outputInfo.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(m_outputInfo.cbSize, dwSize), m_outputInfo.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(m_outputInfo.cbSize, dwSize), &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				hRes = m_outputSample->AddBuffer(buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			return TRUE;
		}
		BOOL TinyMFEncode::GetOutputSample(DWORD dwSize)
		{
			ASSERT(m_encoder);
			for (;;)
			{
				DWORD dwFlags = 0;
				HRESULT hRes = m_encoder->GetOutputStatus(&dwFlags);
				if (hRes != S_OK)
					return FALSE;
				if (dwFlags != MFT_OUTPUT_STATUS_SAMPLE_READY)
					break;

				MFT_OUTPUT_DATA_BUFFER samples = { 0 };
				if (!(m_outputInfo.dwFlags & (MFT_OUTPUT_STREAM_PROVIDES_SAMPLES | MFT_OUTPUT_STREAM_CAN_PROVIDE_SAMPLES)))
				{
					if (!CreateOutputSample(dwSize))
						return FALSE;
					samples.pSample = m_outputSample;
				}
				DWORD dwStatus;
				hRes = m_encoder->ProcessOutput(0, 1, &samples, &dwStatus);
				if (hRes != S_OK)
				{
					if (hRes != MF_E_TRANSFORM_NEED_MORE_INPUT)
						return FALSE;
					break;
				}
				TinyComPtr<IMFMediaBuffer> buffer;
				hRes = samples.pSample->ConvertToContiguousBuffer(&buffer);
				if (hRes != S_OK)
					return FALSE;
				DWORD dwCurrentSize = 0;
				hRes = buffer->GetCurrentLength(&dwCurrentSize);
				if (hRes != S_OK)
					return FALSE;
				BYTE* pBuffer = NULL;
				hRes = buffer->Lock(&pBuffer, NULL, NULL);
				if (hRes != S_OK)
					return FALSE;
				OnDataAvailable(pBuffer, dwCurrentSize, this);
				hRes = buffer->Unlock();
				if (hRes != S_OK)
					return FALSE;
			}
			return TRUE;
		}
		BOOL TinyMFEncode::Encode(const BYTE* bits, DWORD size, LONGLONG hnsSampleTime, LONGLONG hnsSampleDuration)
		{
			if (!CreateInputSample(bits, size))
				return FALSE;
			TinyComPtr<IMFMediaType> mediaType;
			HRESULT hRes = m_encoder->GetInputCurrentType(m_dwInputID, &mediaType);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_inputSample->SetSampleTime(hnsSampleTime);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_inputSample->SetSampleDuration(hnsSampleDuration);
			if (hRes != S_OK)
				return FALSE;
			DWORD dwStatus = 0;
			hRes = m_encoder->GetInputStatus(m_dwInputID, &dwStatus);
			if (MFT_INPUT_STATUS_ACCEPT_DATA != dwStatus)
				return TRUE;
			hRes = m_encoder->ProcessInput(m_dwInputID, m_inputSample, 0);
			if (hRes != S_OK)
				return FALSE;
			return GetOutputSample(size * 2);
		}
		BOOL TinyMFEncode::Close()
		{
			if (!m_encoder)
				return FALSE;
			HRESULT hRes = m_encoder->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_encoder->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_encoder->ProcessMessage(MFT_MESSAGE_NOTIFY_END_STREAMING, NULL);
			if (hRes != S_OK)
				return FALSE;
			m_encoder.Release();
			return TRUE;
		}

		void TinyMFEncode::OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter)
		{
			if (!m_callback.IsNull())
			{
				m_callback(bits, size, lpParameter);
			}
		}
	};
}
