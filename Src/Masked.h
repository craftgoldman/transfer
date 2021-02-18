/////////////////////////////////////////////////////////////////////////////
// MaskEd.h : header file
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CMaskEdit class

#ifndef _MASKDEFINE_
#define _MASKDEFINE_

#define	STR_FMT_CHAR	"+09#L?Aa&"

// Типы масок
enum MASK_TYPE {
	MASK_STRING,
	MASK_NUMERIC,
	MASK_DATE,
	MASK_TIME,
	MASK_DATETIME
};

class CMaskEdit : public CEdit
{
  DECLARE_DYNAMIC(CMaskEdit)

	// Constructors
public:
	CMaskEdit();
  
	// Attributes
	BOOL m_bUseMask;
	BOOL m_bUseGroup;
	BOOL m_bSigned;
	UINT m_cPrompt;
	UINT m_cDecimal;
	UINT m_cThousand;
	UINT m_cDate;
	UINT m_cTime;

	// Operations
	static BOOL IsValidChar(UINT nChar, int nPos, CString strText, 
		UINT nMaskType, CString strMask, int nLen);
	static CString FormatText(CString strText, UINT nMaskType, 
		CString strMask, int nLen, UINT cPrompt, BOOL bFromLeft = TRUE);
	static CString FormatNumber(double dValue, int nDec = 0, 
		UINT cDecimal = '.', UINT cThousand = ',', 
		BOOL bUseGroup = TRUE, BOOL bSigned = TRUE);
	static CString FormatDateTime(COleDateTime& DateTime, 
		UINT nMaskType = MASK_DATE, int nLen = 10, 
		UINT cDate = '.', UINT cTime = ':');
	UINT GetMaskType() const;
	void SetMask(LPCTSTR lpMask, UINT cPrompt = ' ');
	void SetMask(UINT nMaskType, UINT nLen = 0, UINT nDec = 0);
	BOOL SetText(LPCTSTR lpText);
	CString GetText();
	BOOL SetNumber(double dValue);
	double GetNumber() const;
	BOOL SetDateTime(COleDateTime DateTime);
	COleDateTime GetDateTime() const;

	// Implementation
public:

protected:
	void SendChar(UINT nChar);
	BOOL CheckChar(UINT nChar, CString strText);
	CString ExtractText(CString strIn, BOOL bFromLeft = TRUE);
	// Generated message map functions
	//{{AFX_MSG(CMaskEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL	m_bMaskKeyInProgress;
	int		m_nMaskType;
	int		m_nLen;
	int		m_nDec;
	CString	m_strMask;
};

#endif
