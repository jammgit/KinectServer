#pragma once

#include "stdint.h"
#include <cstring>

/* -------- param setting ------ */
typedef enum
{
	ERR_NONE = 1,
	ERR_INVALID_INPUT_DATA = -256,
	ERR_PARAM_NOT_SET,
	ERR_ENCODER_HAD_OPENED,
	ERR_ENCODER_NOT_OPENED,
	ERR_ENCODER_FAILED_OPEN,
	ERR_ENCODER_FAILED_ENCODE,
	ERR_SET_TIMESTAMP_BEFORE_ENCODE,
	ERR_NO_MORE_DATA
}E_ISWH264ENC_RET;

typedef enum
{
	PROFILE_BASELINE = 1,
	PROFILE_MAIN,
	PROFILE_HIGH,
	PROFILE_DEFAULT = PROFILE_MAIN
}E_ISWH264ENC_PROFILE;

typedef enum
{
	RC_AVR = 1,
	RC_CRF,
	RC_CQP,
	RC_DEFAULT = RC_CQP
}E_ISWH264ENC_RC;

typedef enum
{
	PRE_ULTRAFAST = 1,
	PRE_SUPERFAST,
	PRE_VERYFAST,
	PRE_FASTER,
	PRE_FAST,
	PRE_MEDIUM,
	PRE_SLOW,
	PRE_SLOWER,
	PRE_VERYSLOW,
	PRE_PLACEBO,
	PRE_DEFAULT = PRE_ULTRAFAST
}E_ISWH264ENC_PRESET;

typedef enum
{
	TUNE_FIIM = 1,
	TUNE_ANIMATION,
	TUNE_GRAIN,
	TUNE_STILLIMAGE,
	TUNE_PSNR,
	TUNE_SSIM,
	TUNE_FASTDECODE,
	TUNE_ZEROLATENCY,
	TUNE_DEFAULT = TUNE_ZEROLATENCY
}E_ISWH264ENC_TUNE;




typedef struct
{
	int iHeight;
	int iWidth;
	int iFrameRate;
	int iBitrate;					// bps(when use Openh264,set it 5000000)
	E_ISWH264ENC_PROFILE eProfile;
	E_ISWH264ENC_PRESET	 ePreset;
	E_ISWH264ENC_RC		 eRcMode;
	E_ISWH264ENC_TUNE	 eTune;
}S_ISWH264EncoderParam, *pS_ISWH264EncoderParam;

struct IMFSample;
typedef uint8_t		   X264;					// x264 encoder
typedef uint8_t		   Openh264;				// openh264 encoder
typedef IMFSample	   MediaFoundation;			// media foundation encoder

template <class TYPE>
class BasicH264Encoder
{
#define EncoderSafeRelease(p) if (p && *p){delete *p; *p=nullptr;}

public:
	BasicH264Encoder();
	virtual ~BasicH264Encoder() {};


	E_ISWH264ENC_RET SetParam(const S_ISWH264EncoderParam& param);

	virtual E_ISWH264ENC_RET Open() = 0;
	virtual E_ISWH264ENC_RET Close() = 0;
	virtual E_ISWH264ENC_RET RequestEncodedFrame(
		TYPE  *in_buf,
		TYPE  *out_ppData[8],
		size_t out_linesize[8],
		bool in_forceKey = false) = 0;

	virtual E_ISWH264ENC_RET RequestEncodedFrame(
		TYPE  *in_buf[3],
		TYPE  *out_ppData[8],
		size_t out_linesize[8],
		bool in_forceKey = false) = 0;

protected:

	S_ISWH264EncoderParam m_param;
};


template <class TYPE>
BasicH264Encoder<TYPE>::BasicH264Encoder()
{
	memset(&m_param, 0, sizeof(S_ISWH264EncoderParam));
}


template <class TYPE>
E_ISWH264ENC_RET BasicH264Encoder<TYPE>::SetParam(const S_ISWH264EncoderParam& param)
{
	if (param.iHeight <= 0 || param.iWidth <= 0
		|| param.iBitrate <= 0 || param.iFrameRate <= 0
		|| param.ePreset <= 0 || param.ePreset > PRE_DEFAULT
		|| param.eProfile <= 0 || param.eProfile > PROFILE_DEFAULT
		|| param.eRcMode <= 0 || param.eRcMode > RC_DEFAULT
		|| param.eTune <= 0 || param.eTune > TUNE_DEFAULT)
	{
		return ERR_INVALID_INPUT_DATA;
	}
	m_param = param;
	return ERR_NONE;
}




