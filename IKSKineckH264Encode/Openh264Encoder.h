#pragma once

#include <stdio.h>
#include <cstring>
#include "BasicH264Encoder.h"

#include "openh264/wels/codec_api.h"

#pragma comment(lib, "../lib/openh264/openh264.lib")

class Openh264Encoder :
	public BasicH264Encoder<Openh264>
{
public:
	Openh264Encoder();
	virtual ~Openh264Encoder();

	E_ISWH264ENC_RET Open() override;
	E_ISWH264ENC_RET Close() override;
	E_ISWH264ENC_RET RequestEncodedFrame(
		Openh264 *in_buf,
		Openh264 *out_ppData[8],
		size_t out_linesize[8],
		bool in_forceKey = false) override;

	E_ISWH264ENC_RET RequestEncodedFrame(
		Openh264 *in_buf[3],
		Openh264 *out_ppData[8],
		size_t out_linesize[8],
		bool in_forceKey = false) override;

protected:
	ISVCEncoder *m_pSVCEncoder;
	SEncParamExt m_SVCParam;

	SSourcePicture m_pic_in;
};
