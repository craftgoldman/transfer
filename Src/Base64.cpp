// Base64.cpp
//*********************************************************************
//* Base64 - a simple base64 encoder and decoder.
//*********************************************************************

#include "stdafx.h"
#include "Base64.h"

LPCTSTR Base64::encode(LPCTSTR buffer)
{
	char c;
	unsigned int i;
	unsigned int len = strlen(buffer);
	CString sRet;

	for (i = 0; i < len; ++i)
	{
		c = (buffer[i] >> 2) & 0x3f;
		sRet += cvt[c];
		c = (buffer[i] << 4) & 0x3f;
		if (++i < len)
			c |= (buffer[i] >> 4) & 0x0f;
		sRet += cvt[c];
		if (i < len)
		{
			c = (buffer[i] << 2) & 0x3f;
			if (++i < len)
				c |= (buffer[i] >> 6) & 0x03;
			sRet += cvt[c];
		}
		else
		{
			++i;
			sRet += fillchar;
		}
		if (i < len)
		{
			c = buffer[i] & 0x3f;
			sRet += cvt[c];
		}
		else
		{
			sRet += fillchar;
		}
	}
	return (LPCTSTR)sRet;
}

LPCTSTR Base64::decode(LPCTSTR buffer)
{
	char c, c1;
	unsigned int i;
	unsigned int len = strlen(buffer);
	CString sRet, svt(cvt);

	for (i = 0; i < len; ++i)
	{
		c = (char) svt.Find(buffer[i]);
		++i;
		c1 = (char) svt.Find(buffer[i]);
		c = (c << 2) | ((c1 >> 4) & 0x3);
		sRet += c;
		if (++i < len)
		{
			c = buffer[i];
			if (fillchar == c)
				break;
			c = (char) svt.Find(c);
			c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
			sRet += c1;
		}
		if (++i < len)
		{
			c1 = buffer[i];
			if (fillchar == c1)
				break;
			c1 = (char) svt.Find(c1);
			c = ((c << 6) & 0xc0) | c1;
			sRet += c;
		}
	}
	return (LPCTSTR)sRet;
}
