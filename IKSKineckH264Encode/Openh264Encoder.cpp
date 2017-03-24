#include "Openh264Encoder.h"

Openh264Encoder::Openh264Encoder()
	:m_pSVCEncoder(nullptr)
{
}

Openh264Encoder::~Openh264Encoder()
{
	if (m_pSVCEncoder)
	{
		m_pSVCEncoder->Uninitialize();
		WelsDestroySVCEncoder(m_pSVCEncoder);
		m_pSVCEncoder = NULL;
	}
}

E_ISWH264ENC_RET Openh264Encoder::Open()
{
	int ret;
	if (m_pSVCEncoder)
		return ERR_ENCODER_HAD_OPENED;
	ret = WelsCreateSVCEncoder(&m_pSVCEncoder);
	if (ret < 0)
		return ERR_ENCODER_FAILED_OPEN;

	m_pSVCEncoder->GetDefaultParams(&m_SVCParam);
	m_SVCParam.iUsageType = CAMERA_VIDEO_REAL_TIME;
	m_SVCParam.fMaxFrameRate = (float)m_param.iFrameRate;
	m_SVCParam.iPicHeight = m_param.iHeight;
	m_SVCParam.iPicWidth = m_param.iWidth;
	m_SVCParam.iTargetBitrate = m_param.iBitrate;
	m_SVCParam.iNumRefFrame = 1;
	m_SVCParam.iMultipleThreadIdc = 1;
	m_SVCParam.iMaxQp = 51;
	m_SVCParam.iMinQp = 10;
	m_SVCParam.iSpatialLayerNum = 1;

	m_SVCParam.sSpatialLayers[0].iDLayerQp = 23;		//QP constant
	m_SVCParam.sSpatialLayers[0].fFrameRate = m_SVCParam.fMaxFrameRate;
	m_SVCParam.sSpatialLayers[0].iVideoHeight = m_SVCParam.iPicHeight;
	m_SVCParam.sSpatialLayers[0].iVideoWidth = m_SVCParam.iPicWidth;
	m_SVCParam.sSpatialLayers[0].uiProfileIdc = PRO_BASELINE;
	m_SVCParam.sSpatialLayers[0].uiLevelIdc = LEVEL_1_0;// LEVEL_3_1;
	m_SVCParam.sSpatialLayers[0].iMaxSpatialBitrate = m_SVCParam.iTargetBitrate;
	m_SVCParam.sSpatialLayers[0].iSpatialBitrate = m_SVCParam.iTargetBitrate;
	//m_SVCParam.sSpatialLayers[0].uiVideoFormat = EVideoFormatSPS;

	if (m_param.eProfile != PROFILE_BASELINE)
		m_SVCParam.iEntropyCodingModeFlag = 1; // open CBABC,so that is 'main' profile
											   // do not know how to change profile by SetOption function at present.

	switch (m_param.eRcMode)
	{
	case RC_CRF:
	case RC_AVR:
		m_SVCParam.iRCMode = RC_BITRATE_MODE;
		break;
	case RC_CQP:
		m_SVCParam.iRCMode = RC_QUALITY_MODE;
		break;
	default:
		break;
	}

	ret = m_pSVCEncoder->InitializeExt(&m_SVCParam);
	if (ret < 0)
	{
		m_pSVCEncoder->Uninitialize();
		WelsDestroySVCEncoder(m_pSVCEncoder);
		m_pSVCEncoder = NULL;
		return ERR_ENCODER_FAILED_OPEN;
	}

	int inter = 1024;
	m_pSVCEncoder->SetOption(ENCODER_OPTION_IDR_INTERVAL, &inter);

	// initial picture
	memset(&m_pic_in, 0, sizeof(m_pic_in));
	m_pic_in.iPicHeight = m_SVCParam.iPicHeight;
	m_pic_in.iPicWidth = m_SVCParam.iPicWidth;
	m_pic_in.iColorFormat = videoFormatI420;
	m_pic_in.iStride[0] = m_SVCParam.iPicWidth;
	m_pic_in.iStride[1] = (m_SVCParam.iPicWidth >> 1);
	m_pic_in.iStride[2] = m_pic_in.iStride[1];

	return ERR_NONE;
}

E_ISWH264ENC_RET Openh264Encoder::Close()
{
	if (m_pSVCEncoder)
	{
		m_pSVCEncoder->Uninitialize();
		WelsDestroySVCEncoder(m_pSVCEncoder);
		m_pSVCEncoder = NULL;
	}
	else
	{
		return ERR_ENCODER_NOT_OPENED;
	}
	return ERR_NONE;
}

E_ISWH264ENC_RET Openh264Encoder::RequestEncodedFrame(
	Openh264 *in_buf,	//NULL for flush , not NULL for encode
	Openh264 *out_ppData[8],
	size_t out_linesize[8],
	bool in_forceKey)
{
	if (!m_pSVCEncoder)
		return ERR_ENCODER_NOT_OPENED;

	if (out_linesize == nullptr || out_ppData == nullptr)
	{
		return ERR_INVALID_INPUT_DATA;
	}

	if (in_buf != NULL)
	{
		m_pic_in.pData[0] = in_buf;
		m_pic_in.pData[1] = in_buf + m_pic_in.iPicWidth*m_pic_in.iPicHeight;
		m_pic_in.pData[2] = m_pic_in.pData[1] + ((m_pic_in.iPicHeight*m_pic_in.iPicWidth) >> 2);
	}

	SFrameBSInfo info;
	int ret;
	KEY_FRAME_REQUEST_TYPE type = IDR_RECOVERY_REQUEST;
	if (in_forceKey && in_buf != NULL)
		m_pSVCEncoder->SetOption(ENCODER_LTR_RECOVERY_REQUEST, &type);

	if (in_buf != NULL && m_pSVCEncoder)
		ret = m_pSVCEncoder->EncodeFrame(&m_pic_in, &info);
	else if (m_pSVCEncoder)
		ret = m_pSVCEncoder->EncodeFrame(NULL, &info);

	if (ret != cmResultSuccess && in_buf)
		return ERR_ENCODER_FAILED_ENCODE;

	int layer = 0;
	int nal_index = 0;
	if (info.eFrameType != videoFrameTypeSkip
		&& info.eFrameType != videoFrameTypeIPMixed
		&& info.eFrameType != videoFrameTypeInvalid)
	{
		while (layer < info.iLayerNum)
		{
			const SLayerBSInfo* layer_bs_info = &info.sLayerInfo[layer];

			if (layer_bs_info != NULL)
			{
				int layer_size = 0;
				int nal_begin = 0;
				// it is sequential actually
				for (int i = 0; i < layer_bs_info->iNalCount; i++, nal_index++)
				{
					//out_ppData[nal_index] = layer_bs_info->pBsBuf + nal_begin;

					//out_linesize[nal_index] = layer_bs_info->pNalLengthInByte[i];

					//layer_size += layer_bs_info->pNalLengthInByte[i];

					//nal_begin = layer_size;

					out_linesize[nal_index] = layer_bs_info->pNalLengthInByte[i];

					out_ppData[nal_index] = new uint8_t[out_linesize[nal_index]];

					memcpy(out_ppData[nal_index], layer_bs_info->pBsBuf + nal_begin, out_linesize[nal_index]);
					//out_ppData[nal_index] = layer_bs_info->pBsBuf + nal_begin;

					layer_size += layer_bs_info->pNalLengthInByte[i];

					nal_begin = layer_size;
				}
			}
			layer++;
		}
	}
	out_ppData[nal_index] = nullptr;
	if (nal_index == 0)
	{
		return ERR_NO_MORE_DATA;
	}

	return ERR_NONE;
}

E_ISWH264ENC_RET Openh264Encoder::RequestEncodedFrame(
	Openh264 *in_buf[3],
	Openh264 *out_ppData[8],
	size_t out_linesize[8],
	bool in_forceKey)
{
	if (!m_pSVCEncoder)
		return ERR_ENCODER_NOT_OPENED;

	if (out_linesize == nullptr || out_ppData == nullptr)
	{
		return ERR_INVALID_INPUT_DATA;
	}

	if (in_buf != NULL)
	{
		m_pic_in.pData[0] = in_buf[0];
		m_pic_in.pData[1] = in_buf[1];
		m_pic_in.pData[2] = in_buf[2];
	}

	SFrameBSInfo info;
	int ret;
	KEY_FRAME_REQUEST_TYPE type = IDR_RECOVERY_REQUEST;
	if (in_forceKey && in_buf != NULL)
		m_pSVCEncoder->SetOption(ENCODER_LTR_RECOVERY_REQUEST, &type);

	if (in_buf != NULL && m_pSVCEncoder)
		ret = m_pSVCEncoder->EncodeFrame(&m_pic_in, &info);
	else if (m_pSVCEncoder)
		ret = m_pSVCEncoder->EncodeFrame(NULL, &info);

	if (ret != cmResultSuccess && in_buf)
		return ERR_ENCODER_FAILED_ENCODE;

	int layer = 0;
	int nal_index = 0;
	if (info.eFrameType != videoFrameTypeSkip
		&& info.eFrameType != videoFrameTypeIPMixed
		&& info.eFrameType != videoFrameTypeInvalid)
	{
		while (layer < info.iLayerNum)
		{
			const SLayerBSInfo* layer_bs_info = &info.sLayerInfo[layer];

			if (layer_bs_info != NULL)
			{
				int layer_size = 0;
				int nal_begin = 0;
				// it is sequential actually
				for (int i = 0; i < layer_bs_info->iNalCount; i++, nal_index++)
				{
					out_linesize[nal_index] = layer_bs_info->pNalLengthInByte[i];

					out_ppData[nal_index] = new uint8_t[out_linesize[nal_index]];

					memcpy(out_ppData[nal_index], layer_bs_info->pBsBuf + nal_begin, out_linesize[nal_index]);
					//out_ppData[nal_index] = layer_bs_info->pBsBuf + nal_begin;

					layer_size += layer_bs_info->pNalLengthInByte[i];

					nal_begin = layer_size;
				}
			}
			layer++;
		}
	}
	out_ppData[nal_index] = NULL;
	if (nal_index == 0)
	{
		return ERR_NO_MORE_DATA;
	}

	return ERR_NONE;
}