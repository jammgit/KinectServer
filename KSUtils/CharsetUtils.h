/***************************************************
 * Copyright (C) 2014, All right reserved.
 * file SLLangUtils.h
 * version 1.0

 * date 2014 ʮһ�� 26 12:55
 * brief
	�ַ����빤����
 * detail:
	windowsapiʵ�ֵ��ַ����빤���࣬��ʵ��ansi����������ҳ����unicode��utf8֮����ַ�������ת��
 * Function List:
	UnicodeStringToUTF8String
	UnicodeStringToANSIString
	UTF8StringToUnicodeString
	UTF8StringToANSIString
	ANSIStringToUnicodeString
	ANSIStringToUTF8String

 **************************************************/

#ifndef CharsetUtils_h__
#define CharsetUtils_h__

#include <string>
#include <windows.h>

class CharsetUtils
{
public:
	/*

	*Summary: ��Unicode�ַ���ת��UTF8�ַ���

	*Info��
	*Parameters:
	*     strSource��Ҫת����Unicode�ַ���
	*     o_strDest�����ת��������ַ���

	*Return :�ɹ�����true�����򷵻�false

	*/
	static bool UnicodeStringToUTF8String(const std::wstring &strSource, std::string &o_strDest)
	{
		//����ת������Ļ�������С
		int nLen = WideCharToMultiByte(CP_UTF8, 0, strSource.c_str(), -1, NULL, 0, NULL, FALSE);
		if (0 == nLen)
		{
			return false;
		}
		char *pBuf = new char[nLen];
		ZeroMemory(pBuf, nLen);
		//ת������
		nLen = WideCharToMultiByte(CP_UTF8, 0, strSource.c_str(), -1, pBuf, nLen, NULL, FALSE);
		if (nLen > 0)
		{
			o_strDest.assign(pBuf);
		}
		delete[]pBuf;
		return nLen > 0;
	}

	/*

	*Summary: ��Unicode�ַ���ת��UTF8�ַ���

	*Info��
	*Parameters:
	*     strSource��Ҫת����Unicode�ַ���

	*Return :ת��������ַ���

	*/
	static std::string UnicodeStringToUTF8String(const std::wstring &strSource)
	{
		std::string strDest;
		UnicodeStringToUTF8String(strSource, strDest);
		return strDest;
	}

	/*

	*Summary: ��Unicode�ַ���ת��ANSI����������ҳ���ַ���

	*Info��
	*Parameters:
	*     strSource��Ҫת����Unicode�ַ���
	*     o_strDest�����ת��������ַ���

	*Return :�ɹ�����true�����򷵻�false

	*/
	static bool UnicodeStringToANSIString(const std::wstring &strSource, std::string &o_strDest)
	{
		//����ת������Ļ�������С
		int nLen = WideCharToMultiByte(CP_ACP, 0, strSource.c_str(), -1, NULL, 0, NULL, FALSE);
		if (0 == nLen)
		{
			return false;
		}
		char *pBuf = new char[nLen];
		ZeroMemory(pBuf, nLen);
		//ת������
		nLen = WideCharToMultiByte(CP_ACP, 0, strSource.c_str(), -1, pBuf, nLen, NULL, FALSE);
		if (nLen > 0)
		{
			o_strDest.assign(pBuf);
		}
		delete[]pBuf;
		return nLen > 0;
	}

	/*

	*Summary: ��Unicode�ַ���ת��ANSI����������ҳ���ַ���

	*Info��
	*Parameters:
	*     strSource��Ҫת����Unicode�ַ���

	*Return :ת��������ַ���

	*/
	static std::string UnicodeStringToANSIString(const std::wstring &strSource)
	{
		std::string strDest;
		UnicodeStringToANSIString(strSource, strDest);
		return strDest;
	}

	/*

	*Summary: ��UTF8�ַ���ת��Unicode�ַ���

	*Info��
	*Parameters:
	*     strSource��Ҫת����UTF8�ַ���
	*     o_strDest�����ת�������Unicode�ַ���

	*Return :�ɹ�����true�����򷵻�false

	*/
	static bool UTF8StringToUnicodeString(const std::string &strSource, std::wstring &o_strDest)
	{
		//����ת������Ļ�������С
		int nLen = MultiByteToWideChar(CP_UTF8, 0, strSource.c_str(), -1, NULL, NULL);
		if (0 == nLen)
		{
			return false;
		}
		wchar_t *pBuf = new wchar_t[nLen];
		ZeroMemory(pBuf, nLen);
		//ת������
		nLen = MultiByteToWideChar(CP_UTF8, 0, strSource.c_str(), -1, pBuf, nLen);
		if (nLen > 0)
		{
			o_strDest.assign(pBuf);
		}
		delete[]pBuf;
		return nLen > 0;
	}

	/*

	*Summary: ��UTF8�ַ���ת��Unicode�ַ���

	*Info��
	*Parameters:
	*     strSource��Ҫת����UTF8�ַ���

	*Return :ת��������ַ���

	*/
	static std::wstring UTF8StringToUnicodeString(const std::string &strSource)
	{
		std::wstring strDest;
		UTF8StringToUnicodeString(strSource, strDest);
		return strDest;
	}

	/*

	*Summary: ��UTF8�ַ���ת��ANSI����������ҳ���ַ���

	*Info��
	*Parameters:
	*     strSource��Ҫת����UTF8�ַ���
	*     o_strDest�����ת��������ַ���

	*Return :�ɹ�����true�����򷵻�false

	*/
	static bool UTF8StringToANSIString(const std::string & strSource, std::string &o_strDest)
	{
		std::wstring unicodeStr;
		//windows��api����ֱ����utf8��ansi֮��ת����Ҫ�Ȱ�utf8ת����unicode����ת����ansi��
		if (UTF8StringToUnicodeString(strSource, unicodeStr) && UnicodeStringToANSIString(unicodeStr, o_strDest))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/*

	*Summary: ��UTF8�ַ���ת��ANSI����������ҳ���ַ���

	*Info��
	*Parameters:
	*     strSource��Ҫת����UTF8�ַ���

	*Return :����ת��������ַ���

	*/
	static std::string UTF8StringToANSIString(const std::string & strSource)
	{
		std::string strDest;
		UTF8StringToANSIString(strSource, strDest);
		return strDest;
	}

	/*

	*Summary: ��ANSI�ַ���ת��Unicode�ַ���

	*Info��
	*Parameters:
	*     strSource��Ҫת�����ַ���
	*     o_strDest�����ת��������ַ���

	*Return :�ɹ�����true�����򷵻�false

	*/
	static bool ANSIStringToUnicodeString(const std::string &strSource, std::wstring &o_strDest)
	{
		//����ת������Ļ�������С
		int nLen = MultiByteToWideChar(CP_ACP, 0, strSource.c_str(), -1, NULL, 0);
		if (0 == nLen)
		{
			return false;
		}
		wchar_t *pBuf = new wchar_t[nLen];
		ZeroMemory(pBuf, nLen);
		//ת������
		MultiByteToWideChar(CP_ACP, 0, strSource.c_str(), -1, pBuf, nLen);
		if (nLen > 0)
		{
			o_strDest.assign(pBuf);
		}
		delete[]pBuf;
		return nLen > 0;
	}

	/*

	*Summary: ��ANSI�ַ���ת��Unicode�ַ���

	*Info��
	*Parameters:
	*     strSource��Ҫת�����ַ���

	*Return :����ת��������ַ���

	*/
	static std::wstring ANSIStringToUnicodeString(const std::string &strSource)
	{
		std::wstring strDest;
		ANSIStringToUnicodeString(strSource, strDest);
		return strDest;
	}

	/*

	*Summary: ��ANSI�ַ���ת��Unicode�ַ���

	*Info��
	*Parameters:
	*     strSource��Ҫת�����ַ���
	*     o_strDest�����ת��������ַ���

	*Return :�ɹ�����true�����򷵻�false

	*/
	static bool ANSIStringToUTF8String(const std::string &strSource, std::string &o_strDest)
	{
		std::wstring unicodeStr;
		//windows��api����ֱ����utf8��ansi֮��ת����Ҫ�Ȱ�ansiת����unicode����ת����utf8i��
		if (ANSIStringToUnicodeString(strSource, unicodeStr) && UnicodeStringToUTF8String(unicodeStr, o_strDest))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/*

	*Summary: ��ANSI�ַ���ת��Unicode�ַ���

	*Info��
	*Parameters:
	*     strSource��Ҫת�����ַ���

	*Return :����ת��������ַ���

	*/
	static std::string ANSIStringToUTF8String(const std::string &strSource)
	{
		std::string strDest;
		ANSIStringToUTF8String(strSource, strDest);
		return strDest;
	}
};
#endif // CharsetUtils_h__