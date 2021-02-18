// DataObj.cpp: implementation of the CDataValue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataVal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataValue::CDataValue() 
{
	Clear();
}

CDataValue::CDataValue(const int val)
{
	Clear();
	SetBIT(val);
}

CDataValue::CDataValue(const TCHAR val)
{
	Clear();
	SetTINYINT(val);
}

CDataValue::CDataValue(const short val)
{
	Clear();
	SetSHORT(val);
}

CDataValue::CDataValue(const long val)
{
	Clear();
	SetLONG(val);
}

CDataValue::CDataValue(const float val)
{
	Clear();
	SetFLOAT(val);
}

CDataValue::CDataValue(const double val)
{
	Clear();
	SetDOUBLE(val);
}

CDataValue::CDataValue(const TCHAR* pstr)
{
	Clear();
	SetCHAR(pstr);
}

CDataValue::CDataValue(const CString &str)
{
	Clear();
	SetCHAR((LPCTSTR)str);
}

CDataValue::CDataValue(const DATE_STRUCT* pdate)
{
	Clear();
	SetDATE(pdate);
}

CDataValue::CDataValue(const TIME_STRUCT* ptime)
{
	Clear();
	SetTIME(ptime);
}

CDataValue::CDataValue(const TIMESTAMP_STRUCT* pts)
{
	Clear();
	SetTIMESTAMP(pts);
}

CDataValue::CDataValue(const CDataValue &val)
{
	Clear();
	SetValue(val);
}

CDataValue::~CDataValue() 
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////

BOOL CDataValue::Clear() 
{
	if (m_iType == SQL_C_CHAR &&
		m_pstr  != NULL)
		delete [] m_pstr;
	if (m_iType == SQL_C_BINARY &&
		m_pbin  != NULL)
	{
		delete [] m_pbin;
	}
	if (m_iType == SQL_C_DATE &&
		m_pdate != NULL)
		delete (DATE_STRUCT*)m_pdate;
	if (m_iType == SQL_C_TIME &&
		m_ptime != NULL)
		delete (TIME_STRUCT*)m_ptime;
	if (m_iType == SQL_C_TIMESTAMP &&
		m_pts   != NULL)
		delete (TIMESTAMP_STRUCT*)m_pts;

	m_iType  = 0;
	m_length = 0;
	m_bLong  = 0;
	m_bVal   = 0;
	m_cVal   = 0;
	m_iVal   = 0;
	m_lVal   = 0;
	m_fVal   = 0;
	m_fVal   = 0;
	m_pstr   = NULL;
	m_pbin   = NULL;
	m_pdate  = NULL;
	m_ptime  = NULL;
	m_pts    = NULL;
	m_pval   = NULL;

	return TRUE;
}

void* CDataValue::InitType(int iNativeType, long iLength) 
{
	Clear();
	switch (iNativeType)
	{
	case SQL_C_BIT:
		m_iType  = SQL_C_BIT;
		m_length = sizeof(m_bVal);
		m_pval   = &m_bVal;
		break;
	case SQL_C_UTINYINT:
		m_iType  = SQL_C_UTINYINT;
		m_length = sizeof(m_cVal);
		m_pval   = &m_cVal;
		break;
	case SQL_C_SSHORT:
		m_iType  = SQL_C_SSHORT;
		m_length = sizeof(m_iVal);
		m_pval   = &m_iVal;
		break;
	case SQL_C_SLONG:
		m_iType  = SQL_C_SLONG;
		m_length = sizeof(m_lVal);
		m_pval   = &m_lVal;
		break;
	case SQL_C_FLOAT:
		m_iType  = SQL_C_FLOAT;
		m_length = sizeof(m_fVal);
		m_pval   = &m_fVal;
		break;
	case SQL_C_DOUBLE:
		m_iType  = SQL_C_DOUBLE;
		m_length = sizeof(m_dVal);
		m_pval   = &m_dVal;
		break;
	case SQL_C_CHAR:
		m_iType  = SQL_C_CHAR;
		m_length = iLength;
		m_pstr   = new TCHAR[iLength];
		m_pval   = m_pstr;
		break;
	case SQL_C_BINARY:
		m_iType  = SQL_C_BINARY;
		m_length = iLength;
		m_pbin   = new BYTE[iLength];
		memset(m_pbin, 0, m_length);
		m_pval   = m_pbin;
		break;
	case SQL_C_DATE:
		m_iType  = SQL_C_DATE;
		m_length = sizeof(DATE_STRUCT);
		m_pdate  = new DATE_STRUCT;
		m_pval   = m_pdate;
		break;
	case SQL_C_TIME:
		m_iType  = SQL_C_TIME;
		m_length = sizeof(TIME_STRUCT);
		m_ptime  = new TIME_STRUCT;
		m_pval   = m_ptime;
		break;
	case SQL_C_TIMESTAMP:
		m_iType  = SQL_C_TIMESTAMP;
		m_length = sizeof(TIMESTAMP_STRUCT);
		m_pts    = new TIMESTAMP_STRUCT;
		m_pval   = m_pts;
		break;
	}
	return m_pval;
}

void* CDataValue::GetBuffer(int iSQLType, int iSize, BOOL bInit) 
{
	switch (iSQLType)
	{
	case SQL_BIT:
		if (bInit) InitType(SQL_C_BIT);
		m_pval = &m_bVal;
		break;
	case SQL_TINYINT:
		if (bInit) InitType(SQL_C_UTINYINT);
		m_pval = &m_cVal;
		break;
	case SQL_SMALLINT:
		if (bInit) InitType(SQL_C_SSHORT);
		m_pval = &m_iVal;
		break;
	case SQL_INTEGER:
	case SQL_BIGINT:
		if (bInit) InitType(SQL_C_SLONG);
		m_pval = &m_lVal;
		break;
	case SQL_REAL:
		if (bInit) InitType(SQL_C_FLOAT);
		m_pval = &m_fVal;
		break;
	case SQL_FLOAT:
	case SQL_DOUBLE:
	case SQL_NUMERIC:
	case SQL_DECIMAL:
		if (bInit) InitType(SQL_C_DOUBLE);
		m_pval = &m_dVal;
		break;
	case SQL_CHAR:
	case SQL_VARCHAR:
		if (bInit) InitType(SQL_C_CHAR, iSize + 1);
		m_pval = m_pstr;
		break;
	case SQL_LONGVARCHAR:
		if (bInit) InitType(SQL_C_CHAR, 1);
		m_bLong  = TRUE;
		m_pval = m_pstr;
		break;
	case SQL_BINARY:
	case SQL_VARBINARY:
		if (bInit) InitType(SQL_C_BINARY, iSize);
		m_pval = m_pbin;
		break;
	case SQL_LONGVARBINARY:
		if (bInit) InitType(SQL_C_BINARY, 1);
		m_bLong  = TRUE;
		m_pval = m_pbin;
		break;
	case SQL_DATE:
	case SQL_TYPE_DATE:
		if (bInit) InitType(SQL_C_DATE);
		m_pval = m_pdate;
		break;
	case SQL_TIME:
	case SQL_TYPE_TIME:
		if (bInit) InitType(SQL_C_TIME);
		m_pval = m_ptime;
		break;
	case SQL_TIMESTAMP:
	case SQL_TYPE_TIMESTAMP:
		if (bInit) InitType(SQL_C_TIMESTAMP);
		m_pval = m_pts;
		break;
	}
	return m_pval;
}

BOOL CDataValue::ConvertInto(const int val) 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		m_bVal = val;				break;
	case SQL_C_UTINYINT:m_cVal = (TCHAR)val;		break;
	case SQL_C_SSHORT:	m_iVal = (short)val;		break;
	case SQL_C_SLONG:	m_lVal = (long)val;			break;
	case SQL_C_FLOAT:	m_fVal = (float)val;		break;
	case SQL_C_DOUBLE:	m_dVal = (double)val;		break;
	case SQL_C_CHAR:
	{
		CString str;
		str.Format("%i", val);
		CopyIntoStr(str);
		break;
	}
	case SQL_C_BINARY:	break;
	default:			return FALSE;
	}
	return TRUE;
}

BOOL CDataValue::ConvertInto(const TCHAR val) 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		m_bVal = (val>0)?1:0;	break;
	case SQL_C_UTINYINT:m_cVal = (TCHAR)val;	break;
	case SQL_C_SSHORT:	m_iVal = (short)val;	break;
	case SQL_C_SLONG:	m_lVal = (long)val;		break;
	case SQL_C_FLOAT:	m_fVal = (float)val;	break;
	case SQL_C_DOUBLE:	m_dVal = (double)val;	break;
	case SQL_C_CHAR:
	{
		CString str;
		str.Format("%i", val);
		CopyIntoStr(str);
		break;
	}
	case SQL_C_BINARY:
	{
		CString str;
		str.Format("%i", val);
		CopyIntoBin(str);
		break;
	}
	default:			return FALSE;
	}
	return TRUE;
}

BOOL CDataValue::ConvertInto(const short val) 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		m_bVal = (val>0)?1:0;	break;
	case SQL_C_UTINYINT:m_cVal = (TCHAR)val;	break;
	case SQL_C_SSHORT:	m_iVal = (short)val;	break;
	case SQL_C_SLONG:	m_lVal = (long)val;		break;
	case SQL_C_FLOAT:	m_fVal = (float)val;	break;
	case SQL_C_DOUBLE:	m_dVal = (double)val;	break;
	case SQL_C_CHAR:
	{
		CString str;
		str.Format("%d", val);
		CopyIntoStr(str);
		break;
	}
	case SQL_C_BINARY:	break;
	default:			return FALSE;
	}
	return TRUE;
}

BOOL CDataValue::ConvertInto(const long val) 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		m_bVal = (val>0)?1:0;	break;
	case SQL_C_UTINYINT:m_cVal = (TCHAR)val;	break;
	case SQL_C_SSHORT:	m_iVal = (short)val;	break;
	case SQL_C_SLONG:	m_lVal = (long)val;		break;
	case SQL_C_FLOAT:	m_fVal = (float)val;	break;
	case SQL_C_DOUBLE:	m_dVal = (double)val;	break;
	case SQL_C_CHAR:
	{
		CString str;
		str.Format("%ld", val);
		CopyIntoStr(str);
		break;
	}
	case SQL_C_BINARY:	break;
	default:			return FALSE;
	}
	return TRUE;
}

BOOL CDataValue::ConvertInto(const float val) 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		m_bVal = (val>0)?1:0;	break;
	case SQL_C_UTINYINT:m_cVal = (TCHAR)val;	break;
	case SQL_C_SSHORT:	m_iVal = (short)val;	break;
	case SQL_C_SLONG:	m_lVal = (long)val;		break;
	case SQL_C_FLOAT:	m_fVal = (float)val;	break;
	case SQL_C_DOUBLE:	m_dVal = (double)val;	break;
	case SQL_C_CHAR:
	{
		CString str;
		str.Format("%f", val);
		str.TrimRight('0');
		str.TrimRight('.');
		CopyIntoStr(str);
		break;
	}
	case SQL_C_BINARY:	break;
	default:			return FALSE;
	}
	return TRUE;
}

BOOL CDataValue::ConvertInto(const double val) 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		m_bVal = (val>0)?1:0;	break;
	case SQL_C_UTINYINT:m_cVal = (TCHAR)val;	break;
	case SQL_C_SSHORT:	m_iVal = (short)val;	break;
	case SQL_C_SLONG:	m_lVal = (long)val;		break;
	case SQL_C_FLOAT:	m_fVal = (float)val;	break;
	case SQL_C_DOUBLE:	m_dVal = (double)val;	break;
	case SQL_C_CHAR:
	{
		CString str;
		str.Format("%f", val);
		str.TrimRight('0');
		str.TrimRight('.');
		CopyIntoStr(str);
		break;
	}
	case SQL_C_BINARY:	break;
	default:			return FALSE;
	}
	return TRUE;
}

BOOL CDataValue::ConvertInto(const TCHAR* pstr) 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		m_bVal = (atoi(pstr)>0)?1:0;break;
	case SQL_C_UTINYINT:m_cVal = CString(pstr)[0];	break;
	case SQL_C_SSHORT:	m_iVal = atoi(pstr);		break;
	case SQL_C_SLONG:	m_lVal = atol(pstr);		break;
	case SQL_C_FLOAT:	m_fVal = (float)atof(pstr);	break;
	case SQL_C_DOUBLE:	m_dVal = atof(pstr);		break;
	case SQL_C_CHAR:	CopyIntoStr(pstr);			break;
	case SQL_C_BINARY:	CopyIntoBin(pstr);			break;
	case SQL_C_DATE:
	{
		COleDateTime t;
		if (!t.ParseDateTime(pstr, VAR_DATEVALUEONLY))
			return FALSE;
		m_pdate->year   = t.GetYear();
		m_pdate->month  = t.GetMonth();
		m_pdate->day    = t.GetDay();
		break;
	}
	case SQL_C_TIME:
	{
		COleDateTime t;
		if (!t.ParseDateTime(pstr, VAR_TIMEVALUEONLY))
			return FALSE;
		m_ptime->hour   = t.GetHour();
		m_ptime->minute = t.GetMinute();
		m_ptime->second = t.GetSecond();
		break;
	}
	case SQL_C_TIMESTAMP:
	{
		COleDateTime t;
		if (!t.ParseDateTime(pstr))
			return FALSE;
		m_pts->year     = t.GetYear();
		m_pts->month    = t.GetMonth();
		m_pts->day      = t.GetDay();
		m_pts->hour     = t.GetHour();
		m_pts->minute   = t.GetMinute();
		m_pts->second   = t.GetSecond();
		m_pts->fraction	= 0;
		break;
	}
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CDataValue::ConvertInto(const BYTE* pbin, long iSize) 
{
	switch (m_iType)
	{
	case SQL_C_BINARY:
	{
		if (pbin == NULL || m_pbin == NULL)
			return FALSE;

		if (iSize > 0)
		{
			if (m_bLong)	// SQL_LONGVARBINARY
			{
				// Инициализация буфера
				InitType(SQL_C_BINARY, iSize);
				m_bLong = TRUE;
			}
			// Копирование данных
			if (iSize < m_length)
			{
				::memset(m_pbin, 0, m_length);
				::memcpy(m_pbin, pbin, iSize);
			}
			else
			{
				::memcpy(m_pbin, pbin, m_length);
			}
		}
		else	// Установка NULL-данных
		{
			SetNull();
		}
		break;
	}
	case SQL_C_CHAR:
	{
		if (iSize > 0)
		{
			CopyIntoStr(ToHex(pbin, iSize));
		}
		else	// Установка NULL-данных
		{
			SetNull();
		}
		break;
	}
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CDataValue::ConvertInto(const DATE_STRUCT* pdate) 
{
	switch (m_iType)
	{
	case SQL_C_DATE:
	{
		m_pdate->year   = pdate->year;
		m_pdate->month  = pdate->month;
		m_pdate->day    = pdate->day;
		break;
	}
	case SQL_C_TIMESTAMP:
	{
		m_pts->year     = pdate->year;
		m_pts->month    = pdate->month;
		m_pts->day      = pdate->day;
		m_pts->hour     = 0;
		m_pts->minute   = 0;
		m_pts->second   = 0;
		m_pts->fraction	= 0;
		break;
	}
	case SQL_C_CHAR:
	{
		CopyIntoStr((LPCTSTR)CTime(pdate->year, pdate->month,
					pdate->day, 0, 0, 0, -1).Format("%d.%m.%Y"));
		break;
	}
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CDataValue::ConvertInto(const TIME_STRUCT* ptime) 
{
	switch (m_iType)
	{
	case SQL_C_TIME:
	{
		m_ptime->hour   = ptime->hour;
		m_ptime->minute = ptime->minute;
		m_ptime->second = ptime->second;
		break;
	}
	case SQL_C_TIMESTAMP:
	{
		m_pts->year     = 1970;
		m_pts->month    = 1;
		m_pts->day      = 1;
		m_pts->hour     = ptime->hour;
		m_pts->minute   = ptime->minute;
		m_pts->second   = ptime->second;
		m_pts->fraction	= 0;
		break;
	}
	case SQL_C_CHAR:
	{
		CopyIntoStr((LPCTSTR)CTime(1970, 1, 1, ptime->hour, 
					ptime->minute, ptime->second, 
					-1).Format("%H:%M"));
		break;
	}
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CDataValue::ConvertInto(const TIMESTAMP_STRUCT* pts) 
{
	switch (m_iType)
	{
	case SQL_C_TIMESTAMP:
	{
		m_pts->year     = pts->year;
		m_pts->month    = pts->month;
		m_pts->day      = pts->day;
		m_pts->hour     = pts->hour;
		m_pts->minute   = pts->minute;
		m_pts->second   = pts->second;
		m_pts->fraction	= 0;
		break;
	}
	case SQL_C_DATE:
	{
		m_pdate->year   = pts->year;
		m_pdate->month  = pts->month;
		m_pdate->day    = pts->day;
		break;
	}
	case SQL_C_TIME:
	{
		m_ptime->hour   = pts->hour;
		m_ptime->minute = pts->minute;
		m_ptime->second = pts->second;
		break;
	}
	case SQL_C_CHAR:
	{
		CopyIntoStr((LPCTSTR)CTime(pts->year, pts->month, 
					pts->day, pts->hour, pts->minute, 
					pts->second, -1).Format("%d.%m.%Y %H:%M"));
		break;
	}
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CDataValue::ConvertInto(const CDataValue &val) 
{
	// Тип данных источника
	switch (val.m_iType)
	{
	case SQL_C_BIT:		return ConvertInto(val.GetBIT());
	case SQL_C_UTINYINT:return ConvertInto(val.GetTINYINT());
	case SQL_C_SSHORT:	return ConvertInto(val.GetSHORT());
	case SQL_C_SLONG:	return ConvertInto(val.GetLONG());
	case SQL_C_FLOAT:	return ConvertInto(val.GetFLOAT());
	case SQL_C_DOUBLE:	return ConvertInto(val.GetDOUBLE());
	case SQL_C_CHAR:	return ConvertInto(val.GetCHAR());
	case SQL_C_DATE:	return ConvertInto(&val.GetDATE());
	case SQL_C_TIME:	return ConvertInto(&val.GetTIME());
	case SQL_C_TIMESTAMP:return ConvertInto(&val.GetTIMESTAMP());
	case SQL_C_BINARY:	return ConvertInto(val.GetBINARY(),val.m_length);
	}
	return FALSE;
}

LPCTSTR CDataValue::ToHex(const BYTE* pbin, long iSize) const 
{
	UINT	i;
	char*	p;
	CString	s;
	BYTE	b;
	for (i = 0, p = (char*)pbin; i < iSize; i++, p++)
	{
		b = *p & 0xF0;
		s += chHexDigits[b>>4];
		b = *p & 0x0F;
		s += chHexDigits[b];
	}
	s = (CString)_T("0x") + s;

	return LPCTSTR(s);

}

BOOL CDataValue::CopyIntoStr(const TCHAR* pstr) 
{
	if (m_bLong)	// SQL_LONGVARCHAR
	{
		// Инициализация буфера
		InitType(SQL_C_CHAR, strlen(pstr) + 1);
		m_bLong = TRUE;
	}
	// Копирование строки в буфер
	strncpy(m_pstr, pstr, m_length);
	// Дополнение NULL-знаком
	if (strlen(pstr) >= m_length)
	{
		m_pstr[m_length-1] = '\0';
	}
	return TRUE;
}

BOOL CDataValue::CopyIntoBin(const TCHAR* pstr) 
{
	if (strlen(pstr) > 0)
	{
		if (m_bLong)	// SQL_LONGBINARY
		{
			// Инициализация буфера
			InitType(SQL_C_CHAR, strlen(pstr) + 1);
			m_bLong = TRUE;
		}
		// Копирование блока данных
		if (strlen(pstr) < m_length)
		{
			::memset(m_pbin, 0, m_length);
			::memcpy(m_pbin, pstr, strlen(pstr));
		}
		else
		{
			::memcpy(m_pbin, pstr, m_length);
		}
	}
	else	// Установка NULL-данных
	{
		SetNull();
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Properties
//////////////////////////////////////////////////////////////////////

int CDataValue::GetBIT() const 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		return (int)m_bVal;
	case SQL_C_UTINYINT:return (int)m_cVal;
	case SQL_C_SSHORT:	return (int)m_iVal;
	case SQL_C_SLONG:	return (int)m_lVal;
	case SQL_C_FLOAT:	return (int)m_fVal;
	case SQL_C_DOUBLE:	return (int)m_dVal;
	case SQL_C_CHAR:
	case SQL_C_BINARY:
	case SQL_C_DATE:
	case SQL_C_TIME:
	case SQL_C_TIMESTAMP:
	default:			return 0;
	}
	return 0;
}

BOOL CDataValue::SetBIT(const int val) 
{
	if (m_iType != SQL_C_BIT)
		InitType(SQL_C_BIT);
	m_bVal = val;
	return TRUE;
}

TCHAR CDataValue::GetTINYINT() const 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		return (TCHAR)m_bVal;
	case SQL_C_UTINYINT:return m_cVal;
	case SQL_C_SSHORT:	return (TCHAR)m_iVal;
	case SQL_C_SLONG:	return (TCHAR)m_lVal;
	case SQL_C_FLOAT:	return (TCHAR)m_fVal;
	case SQL_C_DOUBLE:	return (TCHAR)m_dVal;
	case SQL_C_CHAR:
	case SQL_C_BINARY:
	case SQL_C_DATE:
	case SQL_C_TIME:
	case SQL_C_TIMESTAMP:
	default:			return 0;
	}
	return 0;
}

BOOL CDataValue::SetTINYINT(const TCHAR val) 
{
	if (m_iType != SQL_C_UTINYINT)
		InitType(SQL_C_UTINYINT);
	m_cVal = val;
	return TRUE;
}

short CDataValue::GetSHORT() const 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		return (short)m_bVal;
	case SQL_C_UTINYINT:return (short)m_cVal;
	case SQL_C_SSHORT:	return m_iVal;
	case SQL_C_SLONG:	return (short)m_lVal;
	case SQL_C_FLOAT:	return (short)m_fVal;
	case SQL_C_DOUBLE:	return (short)m_dVal;
	case SQL_C_CHAR:
	case SQL_C_BINARY:
	case SQL_C_DATE:
	case SQL_C_TIME:
	case SQL_C_TIMESTAMP:
	default:			return 0;
	}
	return 0;
}

BOOL CDataValue::SetSHORT(const short val) 
{
	if (m_iType != SQL_C_SHORT)
		InitType(SQL_C_SHORT);
	m_iVal = val;
	return TRUE;
}

long CDataValue::GetLONG() const 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		return (long)m_bVal;
	case SQL_C_UTINYINT:return (long)m_cVal;
	case SQL_C_SSHORT:	return (long)m_iVal;
	case SQL_C_SLONG:	return m_lVal;
	case SQL_C_FLOAT:	return (long)m_fVal;
	case SQL_C_DOUBLE:	return (long)m_dVal;
	case SQL_C_CHAR:
	case SQL_C_BINARY:
	case SQL_C_DATE:
	case SQL_C_TIME:
	case SQL_C_TIMESTAMP:
	default:			return 0;
	}
	return 0;
}

BOOL CDataValue::SetLONG(const long val) 
{
	if (m_iType != SQL_C_SLONG)
		InitType(SQL_C_SLONG);
	m_lVal = val;
	return TRUE;
}

float CDataValue::GetFLOAT() const 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		return (float)m_bVal;
	case SQL_C_UTINYINT:return (float)m_cVal;
	case SQL_C_SSHORT:	return (float)m_iVal;
	case SQL_C_SLONG:	return (float)m_lVal;
	case SQL_C_FLOAT:	return m_fVal;
	case SQL_C_DOUBLE:	return (float)m_dVal;
	case SQL_C_CHAR:
	case SQL_C_BINARY:
	case SQL_C_DATE:
	case SQL_C_TIME:
	case SQL_C_TIMESTAMP:
	default:			return 0;
	}
	return 0;
}

BOOL CDataValue::SetFLOAT(const float val) 
{
	if (m_iType != SQL_C_FLOAT)
		InitType(SQL_C_FLOAT);
	m_fVal = val;
	return TRUE;
}

double CDataValue::GetDOUBLE() const 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		return (double)m_bVal;
	case SQL_C_UTINYINT:return (double)m_cVal;
	case SQL_C_SSHORT:	return (double)m_iVal;
	case SQL_C_SLONG:	return (double)m_lVal;
	case SQL_C_FLOAT:	return (double)m_fVal;
	case SQL_C_DOUBLE:	return m_dVal;
	case SQL_C_CHAR:
	case SQL_C_BINARY:
	case SQL_C_DATE:
	case SQL_C_TIME:
	case SQL_C_TIMESTAMP:
	default:			return 0;
	}
	return 0;
}

BOOL CDataValue::SetDOUBLE(const double val) 
{
	if (m_iType != SQL_C_DOUBLE)
		InitType(SQL_C_DOUBLE);
	m_dVal = val;
	return TRUE;
}

CString CDataValue::GetString(LPCTSTR szFmt) const 
{
	switch (m_iType)
	{
	case SQL_C_BIT:
	{
		return (m_bVal)?CString("1"):CString("0");
	}
	case SQL_C_UTINYINT:
	{
		return CString(m_cVal);
	}
	case SQL_C_SSHORT:
	{
		if (szFmt == NULL) szFmt = "%d";
		CString str;
		str.Format(szFmt, m_iVal);
		return str;
	}
	case SQL_C_SLONG:
	{
		if (szFmt == NULL) szFmt = "%ld";
		CString str;
		str.Format(szFmt, m_lVal);
		return str;
	}
	case SQL_C_FLOAT:
	{
		if (szFmt == NULL) szFmt = "%f";
		CString str;
		str.Format(szFmt, m_fVal);
		str.TrimRight('0');
		str.TrimRight('.');
		return str;
	}
	case SQL_C_DOUBLE:
	{
		if (szFmt == NULL) szFmt = "%f";
		CString str;
		str.Format(szFmt, m_dVal);
		str.TrimRight('0');
		str.TrimRight('.');
		return str;
	}
	case SQL_C_CHAR:
	{
		return CString(m_pstr);
	}
	case SQL_C_BINARY:
	{
		return CString(ToHex(m_pbin, m_length));
	}
	case SQL_C_TIME:
	{
		if (szFmt == NULL) szFmt = "%H:%M";
		return CTime(1970, 1, 1, 
				m_ptime->hour, m_ptime->minute, 
				m_ptime->second, -1).Format(szFmt);
	}
	case SQL_C_DATE:
	{
		if (szFmt == NULL) szFmt = "%d.%m.%Y";
		if (m_pdate->year != 0)
			return CTime(m_pdate->year, 
					m_pdate->month, m_pdate->day, 
					0, 0, 0, -1).Format(szFmt);
		else
			return CString(_T(""));
	}
	case SQL_C_TIMESTAMP:
	{
		if (szFmt == NULL) szFmt = "%d.%m.%Y %H:%M";
		if (m_pts->year != 0)
			return CTime(m_pts->year, m_pts->month, m_pts->day, 
					m_pts->hour, m_pts->minute, m_pts->second, 
					-1).Format(szFmt);
		else
			return CString(_T(""));
	}
	default:
	{
		return CString(_T(""));
	}
	}
	return CString(_T(""));
}

LPCTSTR CDataValue::GetCHAR() const 
{
	switch (m_iType)
	{
	case SQL_C_CHAR:	return (LPCTSTR)m_pstr;
	default:			return NULL;
	}
	return NULL;
}

BOOL CDataValue::SetCHAR(const TCHAR* pstr) 
{
	if (m_iType != SQL_C_CHAR)
		InitType(SQL_C_CHAR, strlen(pstr)+1);

	return CopyIntoStr(pstr);
}

BYTE* CDataValue::GetBINARY() const 
{
	switch (m_iType)
	{
	case SQL_C_CHAR:	return (BYTE*)m_pstr;
	case SQL_C_BINARY:	return m_pbin;
	default:			return NULL;
	}
	return NULL;
}

BOOL CDataValue::SetBINARY(const BYTE* pbin, long iSize) 
{
	if (m_pbin == NULL) return FALSE;
	if (m_iType != SQL_C_BINARY)
		InitType(SQL_C_BINARY, iSize);
	// Копирование данных
	if (iSize < m_length)
	{
		::memset(m_pbin, 0, m_length);
		::memcpy(m_pbin, pbin, iSize);
	}
	else
	{
		::memcpy(m_pbin, pbin, m_length);
	}
	return TRUE;
}

DATE_STRUCT CDataValue::GetDATE() const 
{
	DATE_STRUCT date;
	if (m_iType == SQL_C_DATE)
	{
		date.year  = m_pdate->year;
		date.month = m_pdate->month;
		date.day   = m_pdate->day;
	}
	return date;
}

BOOL CDataValue::SetDATE(const DATE_STRUCT* pdate) 
{
	if (m_iType != SQL_C_DATE)
		InitType(SQL_C_DATE);
	m_pdate->year   = pdate->year;
	m_pdate->month  = pdate->month;
	m_pdate->day    = pdate->day;
	return TRUE;
}

TIME_STRUCT CDataValue::GetTIME() const 
{
	TIME_STRUCT time;
	if (m_iType == SQL_C_TIME)
	{
		time.hour   = m_ptime->hour;
		time.minute = m_ptime->minute;
		time.second = m_ptime->second;
	}
	return time;
}

BOOL CDataValue::SetTIME(const TIME_STRUCT* ptime) 
{
	if (m_iType != SQL_C_TIME)
		InitType(SQL_C_TIME);
	m_ptime->hour   = ptime->hour;
	m_ptime->minute = ptime->minute;
	m_ptime->second = ptime->second;
	return TRUE;
}

TIMESTAMP_STRUCT CDataValue::GetTIMESTAMP() const 
{
	TIMESTAMP_STRUCT ts;
	if (m_iType == SQL_C_TIMESTAMP)
	{
		ts.year		= m_pts->year;
		ts.month	= m_pts->month;
		ts.day		= m_pts->day;
		ts.hour		= m_pts->hour;
		ts.minute	= m_pts->minute;
		ts.second	= m_pts->second;
		ts.fraction	= m_pts->fraction;
	}
	return ts;
}

BOOL CDataValue::SetTIMESTAMP(const TIMESTAMP_STRUCT* pts) 
{
	if (m_iType != SQL_C_TIMESTAMP)
		InitType(SQL_C_TIMESTAMP);
	m_pts->year     = pts->year;
	m_pts->month    = pts->month;
	m_pts->day      = pts->day;
	m_pts->hour     = pts->hour;
	m_pts->minute   = pts->minute;
	m_pts->second   = pts->second;
	m_pts->fraction	= pts->fraction;
	return TRUE;
}

CDataValue CDataValue::GetValue() const 
{
	CDataValue val;
	switch (m_iType)
	{
	case SQL_C_BIT:			val.SetBIT(GetBIT());				break;
	case SQL_C_UTINYINT:	val.SetTINYINT(GetTINYINT());		break;
	case SQL_C_SSHORT:		val.SetSHORT(GetSHORT());			break;
	case SQL_C_SLONG:		val.SetLONG(GetLONG());				break;
	case SQL_C_FLOAT:		val.SetFLOAT(GetFLOAT());			break;
	case SQL_C_DOUBLE:		val.SetDOUBLE(GetDOUBLE());			break;
	case SQL_C_CHAR:		val.SetCHAR(GetCHAR());				break;
	case SQL_C_DATE:		val.SetDATE(&GetDATE());			break;
	case SQL_C_TIME:		val.SetTIME(&GetTIME());			break;
	case SQL_C_TIMESTAMP:	val.SetTIMESTAMP(&GetTIMESTAMP());	break;
	case SQL_C_BINARY:		val.SetBINARY(GetBINARY(),m_length);break;
	}
	return val;
}

BOOL CDataValue::SetValue(const CDataValue &val) 
{
	// Тип данных источника
	switch (val.m_iType)
	{
	case SQL_C_BIT:			return SetBIT(val.GetBIT());
	case SQL_C_UTINYINT:	return SetTINYINT(val.GetTINYINT());
	case SQL_C_SSHORT:		return SetSHORT(val.GetSHORT());
	case SQL_C_SLONG:		return SetLONG(val.GetLONG());
	case SQL_C_FLOAT:		return SetFLOAT(val.GetFLOAT());
	case SQL_C_DOUBLE:		return SetDOUBLE(val.GetDOUBLE());
	case SQL_C_CHAR:		return SetCHAR(val.GetCHAR());
	case SQL_C_DATE:		return SetDATE(&val.GetDATE());
	case SQL_C_TIME:		return SetTIME(&val.GetTIME());
	case SQL_C_TIMESTAMP:	return SetTIMESTAMP(&val.GetTIMESTAMP());
	case SQL_C_BINARY:		return SetBINARY(val.GetBINARY(),val.m_length);
	}
	return FALSE;
}

BOOL CDataValue::SetDefault() 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		m_bVal = 0;					break;
	case SQL_C_UTINYINT:m_cVal = 0;					break;
	case SQL_C_SSHORT:	m_iVal = 0;					break;
	case SQL_C_SLONG:	m_lVal = 0;					break;
	case SQL_C_FLOAT:	m_fVal = 0;					break;
	case SQL_C_DOUBLE:	m_dVal = 0;					break;
	case SQL_C_CHAR:	_strset(m_pstr,'\0');		break;
	case SQL_C_BINARY:	memset(m_pbin, 0, m_length);break;
	case SQL_C_TIME:
	{
		CTime  t = CTime::GetCurrentTime();
		m_ptime->hour	= t.GetHour();
		m_ptime->minute	= t.GetMinute();
		m_ptime->second	= t.GetSecond();
		break;
	}
	case SQL_C_DATE:
	{
		CTime  t = CTime::GetCurrentTime();
		m_pdate->year	= t.GetYear();
		m_pdate->month	= t.GetMonth();
		m_pdate->day		= t.GetDay();
		break;
	}
	case SQL_C_TIMESTAMP:
	{
		CTime  t = CTime::GetCurrentTime();
		m_pts->year		= t.GetYear();
		m_pts->month		= t.GetMonth();
		m_pts->day		= t.GetDay();
		m_pts->hour		= t.GetHour();
		m_pts->minute	= t.GetMinute();
		m_pts->second	= t.GetSecond();
		m_pts->fraction	= 0;
		break;
	}
	}
	return TRUE;
}

BOOL CDataValue::SetNull() 
{
	switch (m_iType)
	{
	case SQL_C_BIT:		m_bVal = 0;					break;
	case SQL_C_UTINYINT:m_cVal = 0;					break;
	case SQL_C_SSHORT:	m_iVal = 0;					break;
	case SQL_C_SLONG:	m_lVal = 0;					break;
	case SQL_C_FLOAT:	m_fVal = 0;					break;
	case SQL_C_DOUBLE:	m_dVal = 0;					break;
	case SQL_C_CHAR:
	{
		if (m_bLong)	// SQL_LONGVARCHAR
		{
			// Инициализация буфера
			InitType(SQL_C_CHAR, 1);
			_strset(m_pstr, '\0');
			m_bLong = TRUE;
			m_length = 0;
		}
		else
		{
			_strset(m_pstr, '\0');
		}
		break;
	}
	case SQL_C_BINARY:
	{
		if (m_bLong)	// SQL_LONGVARBINARY
		{
			// Инициализация буфера
			InitType(SQL_C_BINARY, 1);
			::memset(m_pbin, 0, 1);
			m_bLong = TRUE;
			m_length = 0;
		}
		else
		{
			::memset(m_pbin, 0, m_length);
		}
		break;
	}
	case SQL_C_TIME:
	{
		m_ptime->hour	= 0;
		m_ptime->minute	= 0;
		m_ptime->second	= 0;
		break;
	}
	case SQL_C_DATE:
	{
		m_pdate->year	= 1970;
		m_pdate->month	= 1;
		m_pdate->day		= 1;
		break;
	}
	case SQL_C_TIMESTAMP:
	{
		m_pts->year		= 1970;
		m_pts->month		= 1;
		m_pts->day		= 1;
		m_pts->hour		= 0;
		m_pts->minute	= 0;
		m_pts->second	= 0;
		m_pts->fraction	= 0;
		break;
	}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////

CDataValue & CDataValue::operator=(const int val)
{
	SetBIT(val);
    return *this;
}

CDataValue & CDataValue::operator=(const TCHAR val)
{
	SetTINYINT(val);
    return *this;
}

CDataValue & CDataValue::operator=(const short val)
{
	SetSHORT(val);
    return *this;
}

CDataValue & CDataValue::operator=(const long val)
{
	SetLONG(val);
    return *this;
}

CDataValue & CDataValue::operator=(const float val)
{
	SetFLOAT(val);
    return *this;
}

CDataValue & CDataValue::operator=(const double val)
{
	SetDOUBLE(val);
    return *this;
}

CDataValue & CDataValue::operator=(const TCHAR* pstr)
{
	SetCHAR(pstr);
    return *this;
}

CDataValue & CDataValue::operator=(const CString &str)
{
	SetCHAR((LPCTSTR)str);
    return *this;
}

CDataValue & CDataValue::operator=(const DATE_STRUCT* date)
{
	SetDATE(date);
    return *this;
}

CDataValue & CDataValue::operator=(const TIME_STRUCT* ptime)
{
	SetTIME(ptime);
    return *this;
}

CDataValue & CDataValue::operator=(const TIMESTAMP_STRUCT* pts)
{
	SetTIMESTAMP(pts);
    return *this;
}

CDataValue & CDataValue::operator=(const CDataValue &val) 
{
	SetValue(val);
    return *this;
}

BOOL CDataValue::operator==(const CDataValue &val) const 
{
	// Сравнение типов
	if (m_iType != val.m_iType) return FALSE;
	// Тип данных
	switch (m_iType)
	{
	case SQL_C_BIT:		return (m_bVal == val.m_bVal);
	case SQL_C_UTINYINT:return (m_cVal == val.m_cVal);
	case SQL_C_SSHORT:	return (m_iVal == val.m_iVal);
	case SQL_C_SLONG:	return (m_lVal == val.m_lVal);
	case SQL_C_FLOAT:	return (m_fVal == val.m_fVal);
	case SQL_C_DOUBLE:	return (m_dVal == val.m_dVal);
	case SQL_C_CHAR:	return (strcmp(m_pstr, val.m_pstr) == 0);
	case SQL_C_BINARY:	return FALSE;
	case SQL_C_DATE:
	{
		if (m_pdate->year	== val.m_pdate->year &&
			m_pdate->month	== val.m_pdate->month &&
			m_pdate->day		== val.m_pdate->day)
			return TRUE;
		else
			return FALSE;
	}
	case SQL_C_TIME:
	{
		if (m_ptime->hour	== val.m_ptime->hour &&
			m_ptime->minute	== val.m_ptime->minute &&
			m_ptime->second	== val.m_ptime->second)
			return TRUE;
		else
			return FALSE;
	}
	case SQL_C_TIMESTAMP:
	{
		if (m_pts->year		== val.m_pts->year &&
			m_pts->month		== val.m_pts->month &&
			m_pts->day		== val.m_pts->day &&
			m_pts->hour		== val.m_pts->hour &&
			m_pts->minute	== val.m_pts->minute &&
			m_pts->second	== val.m_pts->second)
			return TRUE;
		else
			return FALSE;
	}
	}
    return FALSE;
}

BOOL CDataValue::operator!=(const CDataValue &val) const 
{
    return !(*this == val);
}

BOOL CDataValue::operator>(const CDataValue &val) const 
{
	// Сравнение типов
	if (m_iType != val.m_iType) return FALSE;
	// Тип данных
	switch (m_iType)
	{
	case SQL_C_BIT:		return (m_bVal > val.m_bVal);
	case SQL_C_UTINYINT:return (m_cVal > val.m_cVal);
	case SQL_C_SSHORT:	return (m_iVal > val.m_iVal);
	case SQL_C_SLONG:	return (m_lVal > val.m_lVal);
	case SQL_C_FLOAT:	return (m_fVal > val.m_fVal);
	case SQL_C_DOUBLE:	return (m_dVal > val.m_dVal);
	case SQL_C_CHAR:	return (strcmp(m_pstr, val.m_pstr) > 0);
	case SQL_C_BINARY:	return FALSE;
	case SQL_C_DATE:
	{
		CTime t1 = CTime(m_pdate->year, m_pdate->month, 
						 m_pdate->day, 0, 0, 0, -1);
		CTime t2 = CTime(val.m_pdate->year, val.m_pdate->month, 
						 val.m_pdate->day, 0, 0, 0, -1);
		return (t1 > t2);
	}
	case SQL_C_TIME:
	{
		CTime t1 = CTime(1970, 1, 1, m_ptime->hour, 
						 m_ptime->minute, m_ptime->second, -1);
		CTime t2 = CTime(1970, 1, 1, val.m_ptime->hour, 
						 val.m_ptime->minute, val.m_ptime->second, -1);
		return (t1 > t2);
	}
	case SQL_C_TIMESTAMP:
	{
		CTime t1 = CTime(m_pdate->year, m_pdate->month, 
						 m_pdate->day, m_ptime->hour, 
						 m_ptime->minute, m_ptime->second, -1);
		CTime t2 = CTime(val.m_pdate->year, val.m_pdate->month, 
						 val.m_pdate->day, val.m_ptime->hour, 
						 val.m_ptime->minute, val.m_ptime->second, -1);
		return (t1 > t2);
	}
	}
    return FALSE;
}

BOOL CDataValue::operator<(const CDataValue &val) const 
{
	// Сравнение типов
	if (m_iType != val.m_iType) return FALSE;
	// Тип данных
	switch (m_iType)
	{
	case SQL_C_BIT:		return (m_bVal < val.m_bVal);
	case SQL_C_UTINYINT:return (m_cVal < val.m_cVal);
	case SQL_C_SSHORT:	return (m_iVal < val.m_iVal);
	case SQL_C_SLONG:	return (m_lVal < val.m_lVal);
	case SQL_C_FLOAT:	return (m_fVal < val.m_fVal);
	case SQL_C_DOUBLE:	return (m_dVal < val.m_dVal);
	case SQL_C_CHAR:	return (strcmp(m_pstr, val.m_pstr) < 0);
	case SQL_C_BINARY:	return FALSE;
	case SQL_C_DATE:
	{
		CTime t1 = CTime(m_pdate->year, m_pdate->month, 
						 m_pdate->day, 0, 0, 0, -1);
		CTime t2 = CTime(val.m_pdate->year, val.m_pdate->month, 
						 val.m_pdate->day, 0, 0, 0, -1);
		return (t1 < t2);
	}
	case SQL_C_TIME:
	{
		CTime t1 = CTime(1970, 1, 1, m_ptime->hour, 
						 m_ptime->minute, m_ptime->second, -1);
		CTime t2 = CTime(1970, 1, 1, val.m_ptime->hour, 
						 val.m_ptime->minute, val.m_ptime->second, -1);
		return (t1 < t2);
	}
	case SQL_C_TIMESTAMP:
	{
		CTime t1 = CTime(m_pdate->year, m_pdate->month, 
						 m_pdate->day, m_ptime->hour, 
						 m_ptime->minute, m_ptime->second, -1);
		CTime t2 = CTime(val.m_pdate->year, val.m_pdate->month, 
						 val.m_pdate->day, val.m_ptime->hour, 
						 val.m_ptime->minute, val.m_ptime->second, -1);
		return (t1 < t2);
	}
	}
	return FALSE;
}

BOOL CDataValue::operator>=(const CDataValue &val) const 
{
    return ((*this == val) || (*this > val));
}

BOOL CDataValue::operator<=(const CDataValue &val) const 
{
    return ((*this == val) || (*this < val));
}
