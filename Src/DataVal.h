// DataObj.h: interface for the CDataValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DATAVALUE_H)
#define _DATAVALUE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const char chHexDigits[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

class CDataValue : public CObject  
{
	
// Construction/Destruction
public:
	CDataValue();
	CDataValue(const int val);
	CDataValue(const TCHAR val);
	CDataValue(const short val);
	CDataValue(const long val);
	CDataValue(const float val);
	CDataValue(const double val);
	CDataValue(const TCHAR* pstr);
	CDataValue(const CString &str);
	CDataValue(const DATE_STRUCT* pdate);
	CDataValue(const TIME_STRUCT* ptime);
	CDataValue(const TIMESTAMP_STRUCT* pts);
	CDataValue(const CDataValue &val);
	virtual ~CDataValue();

// Implementation
public:
	BOOL Clear();
	void* InitType(int iNativeType, long iLength = 0);
	void* GetBuffer(int iSQLType, int iSize, BOOL bInit = FALSE);
	BOOL ConvertInto(const int val);
	BOOL ConvertInto(const TCHAR val);
	BOOL ConvertInto(const short val);
	BOOL ConvertInto(const long val);
	BOOL ConvertInto(const float val);
	BOOL ConvertInto(const double val);
	BOOL ConvertInto(const TCHAR* pstr);
	BOOL ConvertInto(const BYTE* pbin, long iSize);
	BOOL ConvertInto(const DATE_STRUCT* pdate);
	BOOL ConvertInto(const TIME_STRUCT* ptime);
	BOOL ConvertInto(const TIMESTAMP_STRUCT* pts);
	BOOL ConvertInto(const CDataValue &val);

// Operations
protected:
	LPCTSTR ToHex(const BYTE* pbin, long iSize) const;
	BOOL CopyIntoStr(const TCHAR* pstr);
	BOOL CopyIntoBin(const TCHAR* pstr);

// Properties
public:
	int GetBIT() const;
	BOOL SetBIT(const int val);
	TCHAR GetTINYINT() const;
	BOOL SetTINYINT(const TCHAR val);
	short GetSHORT() const;
	BOOL SetSHORT(const short val);
	long GetLONG() const;
	BOOL SetLONG(const long val);
	float GetFLOAT() const;
	BOOL SetFLOAT(const float val);
	double GetDOUBLE() const;
	BOOL SetDOUBLE(const double val);
	CString GetString(LPCTSTR szFmt = NULL) const;
	LPCTSTR GetCHAR() const;
	BOOL SetCHAR(const TCHAR* pstr);
	BYTE* GetBINARY() const;
	BOOL SetBINARY(const BYTE* pbin, long iSize);
	DATE_STRUCT GetDATE() const;
	BOOL SetDATE(const DATE_STRUCT* pdate);
	TIME_STRUCT GetTIME() const;
	BOOL SetTIME(const TIME_STRUCT* ptime);
	TIMESTAMP_STRUCT GetTIMESTAMP() const;
	BOOL SetTIMESTAMP(const TIMESTAMP_STRUCT* pts);
	CDataValue GetValue() const;
	BOOL SetValue(const CDataValue &val);
	BOOL SetDefault();
	BOOL SetNull();

// Operators
public:
	operator BYTE()    	{ return GetTINYINT(); }
	operator short()   	{ return GetSHORT(); }
	operator long()    	{ return GetLONG(); }
	operator float()   	{ return GetFLOAT(); }
	operator double()  	{ return GetDOUBLE(); }
	operator CString() 	{ return GetString(); }
	operator LPCTSTR() 	{ return GetCHAR(); }
	CDataValue & operator=(const int val);
	CDataValue & operator=(const TCHAR val);
	CDataValue & operator=(const short val);
	CDataValue & operator=(const long val);
	CDataValue & operator=(const float val);
	CDataValue & operator=(const double val);
	CDataValue & operator=(const TCHAR* pstr);
	CDataValue & operator=(const CString &str);
	CDataValue & operator=(const DATE_STRUCT* date);
	CDataValue & operator=(const TIME_STRUCT* ptime);
	CDataValue & operator=(const TIMESTAMP_STRUCT* pts);
	CDataValue & operator=(const CDataValue &val);
	BOOL operator==(const CDataValue &val) const;
	BOOL operator!=(const CDataValue &val) const;
	BOOL operator>(const CDataValue &val) const;
	BOOL operator<(const CDataValue &val) const;
	BOOL operator>=(const CDataValue &val) const;
	BOOL operator<=(const CDataValue &val) const;

// Data members
public:
	unsigned int		m_iType;	// C-Type
	unsigned long		m_length;	// Length
	long				m_lIndex;	// Index
	bool				m_bLong;	// Is long
	int					m_bVal;		// Boolean
	TCHAR				m_cVal;		// Byte
	short				m_iVal;		// Short Int
	long				m_lVal;		// Long Int
	float				m_fVal;		// Float
	double				m_dVal;		// Double
	TCHAR*				m_pstr;		// String
	BYTE*				m_pbin;		// Binary
	DATE_STRUCT*		m_pdate;	// Date
	TIME_STRUCT*		m_ptime;	// Time
	TIMESTAMP_STRUCT*	m_pts;		// Timestamp
	void*				m_pval;		// Buffer
};

#endif // !defined(_DATAVALUE_H)
