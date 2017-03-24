/***************************************************
 * Copyright (C) 2014, All right reserved.
 * file SLLangUtils.h
 * version 1.0

 * date 2014 十一月 26 12:55
 * brief
	字符编码工具类
 * detail:
	windowsapi实现的字符编码工具类，可实现ansi（本机代码页）、unicode、utf8之间的字符串编码转换
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

	*Summary: 将Unicode字符串转成UTF8字符串

	*Info：
	*Parameters:
	*     strSource：要转换的Unicode字符串
	*     o_strDest：存放转换结果的字符串

	*Return :成功返回true，否则返回false

	*/
	static bool UnicodeStringToUTF8String(const std::wstring &strSource, std::string &o_strDest)
	{
		//计算转换所需的缓冲区大小
		int nLen = WideCharToMultiByte(CP_UTF8, 0, strSource.c_str(), -1, NULL, 0, NULL, FALSE);
		if (0 == nLen)
		{
			return false;
		}
		char *pBuf = new char[nLen];
		ZeroMemory(pBuf, nLen);
		//转换编码
		nLen = WideCharToMultiByte(CP_UTF8, 0, strSource.c_str(), -1, pBuf, nLen, NULL, FALSE);
		if (nLen > 0)
		{
			o_strDest.assign(pBuf);
		}
		delete[]pBuf;
		return nLen > 0;
	}

	/*

	*Summary: 将Unicode字符串转成UTF8字符串

	*Info：
	*Parameters:
	*     strSource：要转换的Unicode字符串

	*Return :转换结果的字符串

	*/
	static std::string UnicodeStringToUTF8String(const std::wstring &strSource)
	{
		std::string strDest;
		UnicodeStringToUTF8String(strSource, strDest);
		return strDest;
	}

	/*

	*Summary: 将Unicode字符串转成ANSI（本机代码页）字符串

	*Info：
	*Parameters:
	*     strSource：要转换的Unicode字符串
	*     o_strDest：存放转换结果的字符串

	*Return :成功返回true，否则返回false

	*/
	static bool UnicodeStringToANSIString(const std::wstring &strSource, std::string &o_strDest)
	{
		//计算转换所需的缓冲区大小
		int nLen = WideCharToMultiByte(CP_ACP, 0, strSource.c_str(), -1, NULL, 0, NULL, FALSE);
		if (0 == nLen)
		{
			return false;
		}
		char *pBuf = new char[nLen];
		ZeroMemory(pBuf, nLen);
		//转换编码
		nLen = WideCharToMultiByte(CP_ACP, 0, strSource.c_str(), -1, pBuf, nLen, NULL, FALSE);
		if (nLen > 0)
		{
			o_strDest.assign(pBuf);
		}
		delete[]pBuf;
		return nLen > 0;
	}

	/*

	*Summary: 将Unicode字符串转成ANSI（本机代码页）字符串

	*Info：
	*Parameters:
	*     strSource：要转换的Unicode字符串

	*Return :转换结果的字符串

	*/
	static std::string UnicodeStringToANSIString(const std::wstring &strSource)
	{
		std::string strDest;
		UnicodeStringToANSIString(strSource, strDest);
		return strDest;
	}

	/*

	*Summary: 将UTF8字符串转成Unicode字符串

	*Info：
	*Parameters:
	*     strSource：要转换的UTF8字符串
	*     o_strDest：存放转换结果的Unicode字符串

	*Return :成功返回true，否则返回false

	*/
	static bool UTF8StringToUnicodeString(const std::string &strSource, std::wstring &o_strDest)
	{
		//计算转换所需的缓冲区大小
		int nLen = MultiByteToWideChar(CP_UTF8, 0, strSource.c_str(), -1, NULL, NULL);
		if (0 == nLen)
		{
			return false;
		}
		wchar_t *pBuf = new wchar_t[nLen];
		ZeroMemory(pBuf, nLen);
		//转换编码
		nLen = MultiByteToWideChar(CP_UTF8, 0, strSource.c_str(), -1, pBuf, nLen);
		if (nLen > 0)
		{
			o_strDest.assign(pBuf);
		}
		delete[]pBuf;
		return nLen > 0;
	}

	/*

	*Summary: 将UTF8字符串转成Unicode字符串

	*Info：
	*Parameters:
	*     strSource：要转换的UTF8字符串

	*Return :转换结果的字符串

	*/
	static std::wstring UTF8StringToUnicodeString(const std::string &strSource)
	{
		std::wstring strDest;
		UTF8StringToUnicodeString(strSource, strDest);
		return strDest;
	}

	/*

	*Summary: 将UTF8字符串转成ANSI（本机代码页）字符串

	*Info：
	*Parameters:
	*     strSource：要转换的UTF8字符串
	*     o_strDest：存放转换结果的字符串

	*Return :成功返回true，否则返回false

	*/
	static bool UTF8StringToANSIString(const std::string & strSource, std::string &o_strDest)
	{
		std::wstring unicodeStr;
		//windows的api不能直接在utf8和ansi之间转换，要先把utf8转换到unicode，再转换到ansi。
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

	*Summary: 将UTF8字符串转成ANSI（本机代码页）字符串

	*Info：
	*Parameters:
	*     strSource：要转换的UTF8字符串

	*Return :返回转换结果的字符串

	*/
	static std::string UTF8StringToANSIString(const std::string & strSource)
	{
		std::string strDest;
		UTF8StringToANSIString(strSource, strDest);
		return strDest;
	}

	/*

	*Summary: 将ANSI字符串转成Unicode字符串

	*Info：
	*Parameters:
	*     strSource：要转换的字符串
	*     o_strDest：存放转换结果的字符串

	*Return :成功返回true，否则返回false

	*/
	static bool ANSIStringToUnicodeString(const std::string &strSource, std::wstring &o_strDest)
	{
		//计算转换所需的缓冲区大小
		int nLen = MultiByteToWideChar(CP_ACP, 0, strSource.c_str(), -1, NULL, 0);
		if (0 == nLen)
		{
			return false;
		}
		wchar_t *pBuf = new wchar_t[nLen];
		ZeroMemory(pBuf, nLen);
		//转换编码
		MultiByteToWideChar(CP_ACP, 0, strSource.c_str(), -1, pBuf, nLen);
		if (nLen > 0)
		{
			o_strDest.assign(pBuf);
		}
		delete[]pBuf;
		return nLen > 0;
	}

	/*

	*Summary: 将ANSI字符串转成Unicode字符串

	*Info：
	*Parameters:
	*     strSource：要转换的字符串

	*Return :返回转换结果的字符串

	*/
	static std::wstring ANSIStringToUnicodeString(const std::string &strSource)
	{
		std::wstring strDest;
		ANSIStringToUnicodeString(strSource, strDest);
		return strDest;
	}

	/*

	*Summary: 将ANSI字符串转成Unicode字符串

	*Info：
	*Parameters:
	*     strSource：要转换的字符串
	*     o_strDest：存放转换结果的字符串

	*Return :成功返回true，否则返回false

	*/
	static bool ANSIStringToUTF8String(const std::string &strSource, std::string &o_strDest)
	{
		std::wstring unicodeStr;
		//windows的api不能直接在utf8和ansi之间转换，要先把ansi转换到unicode，再转换到utf8i。
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

	*Summary: 将ANSI字符串转成Unicode字符串

	*Info：
	*Parameters:
	*     strSource：要转换的字符串

	*Return :返回转换结果的字符串

	*/
	static std::string ANSIStringToUTF8String(const std::string &strSource)
	{
		std::string strDest;
		ANSIStringToUTF8String(strSource, strDest);
		return strDest;
	}
};
#endif // CharsetUtils_h__