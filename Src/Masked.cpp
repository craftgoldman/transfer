/////////////////////////////////////////////////////////////////////////////
// MaskEd.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MaskEd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMaskEdit class

IMPLEMENT_DYNAMIC(CMaskEdit, CEdit)

BEGIN_MESSAGE_MAP(CMaskEdit, CEdit)
	//{{AFX_MSG_MAP(CMaskEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMaskEdit::CMaskEdit()
{
	m_bUseMask	= FALSE;
	m_bUseGroup = TRUE;
	m_bSigned	= TRUE;
	m_nMaskType	= MASK_STRING;
	m_nLen		= 0;
	m_nDec		= 0;
	m_cPrompt	= ' ';
	m_cDecimal	= '.';
	m_cThousand	= ',';
	m_cDate		= '.';
	m_cTime		= ':';
	m_strMask	= _T("");
	m_bMaskKeyInProgress = FALSE;
}

void CMaskEdit::SendChar(UINT nChar) 
{
	m_bMaskKeyInProgress = TRUE;
	#ifdef WIN32
		AfxCallWndProc(this, m_hWnd, WM_CHAR, nChar, 1);
	#else
		SendMessage(WM_CHAR, nChar, 1);
	#endif
	m_bMaskKeyInProgress = FALSE;
}

BOOL CMaskEdit::IsValidChar(UINT nChar, int nPos, CString strText, 
								   UINT nMaskType, CString strMask, 
								   int nLen) 
{
	switch (strMask.GetAt(nPos))
	{
	case '0':		// digit only
	{
		if(isdigit(nChar))
		{
			BOOL bValid = TRUE;

			// Проверка даты
			if (nMaskType == MASK_DATE ||
				nMaskType == MASK_DATETIME)
			{
				// Число
				if(nPos == 0)
				{
					if(nChar > '3')
						bValid = FALSE;
				}		
				if(nPos == 1)
				{
					if(strText.GetAt(0) == '3')
					{
						if(nChar > '1')
							bValid = FALSE;
					}
				}
				// Месяц
				if(nPos == 3)
				{
					if(nChar > '1')
						bValid = FALSE;
				}
				if(nPos == 4)
				{
					if(strText.GetAt(3) == '1')
					{
						if(nChar > '2')
							bValid = FALSE;
					}
				}
			}
			// Проверка времени
			else if (nMaskType == MASK_TIME ||
					 nMaskType == MASK_DATETIME)
			{
				// Часы
				int i = 0;
				switch (nLen)
				{
				case 5:  case 8:  i = 0;  break;
				case 14: case 17: i = 9;  break;
				case 16: case 19: i = 11; break;
				}
				if (nPos == i)
				{
					if(nChar > '2')
						bValid = FALSE;
				}
				if (nPos == (i + 1))
				{
					if(strText.GetAt(i) == '2')
					{
						if(nChar > '3')
							bValid = FALSE;
					}
				}
				// Минуты
				i += 3;
				if (nPos == i)
				{
					if(nChar > '5')
						bValid = FALSE;
				}
				if(nPos == (i + 1))
				{
					if(strText.GetAt(i) == '5')
					{
						if(nChar > '9')
							bValid = FALSE;
					}
				}
				// Секунды
				if (nLen == 8  ||
					nLen == 17 ||
					nLen == 19)
				{
					i += 3;
					if (nPos == i)
					{
						if(nChar > '5')
							bValid = FALSE;
					}
					if(nPos == (i + 1))
					{
						if(strText.GetAt(i) == '5')
						{
							if(nChar > '9')
								bValid = FALSE;
						}
					}
				}
			}
			return bValid;
		}
		break;
	}
	case '9':		// digit or space
	{
		if (isdigit(nChar)) return TRUE;
		if (nChar == VK_SPACE) return TRUE;
		break;
	}
	case '#':		// digit or space or '+' or '-'
	{
		if (isdigit(nChar)) return TRUE;
		if (nChar == VK_SPACE) return TRUE;
		if (nChar == '+' || 
			nChar == '-')
			return TRUE;
		break;
	}
	case '+':		// space or '+' or '-'
	{
		if (nChar == VK_SPACE) return TRUE;
		if (nChar == '+' ||
			nChar == '-')
			return TRUE;
		break;
	}
	case 'L':		// alpha only
	{
		if (isalpha(nChar)) return TRUE;
		break;
	}
	case '?':		// alpha or space
	{
		if (isalpha(nChar)) return TRUE;
		if (nChar == VK_SPACE) return TRUE;
		break;
	}
	case 'A':		// alpha numeric only
	{
		if (isalnum(nChar)) return TRUE;
		break;
	}
	case 'a':		// alpha numeric or space
	{
		if (isalnum(nChar)) return TRUE;
		if (nChar == VK_SPACE) return TRUE;
		break;
	}
	case '&':		// all print character only
	{
		if (isprint(nChar)) return TRUE;
		break;
	}
	}
	return FALSE;
}

BOOL CMaskEdit::CheckChar(UINT nChar, CString strText) 
{
	// do not use mask
	if (!m_bUseMask || m_strMask.IsEmpty())
		return TRUE;

	// control character, OK
	if (!isprint(nChar)) return TRUE;

	// unselect all selections, if any
	int startPos, endPos;
	GetSel(startPos, endPos);
	if (startPos != endPos)
	{
		SetSel(-1, 0);
		// Установка пустого значения
		if (m_nMaskType == MASK_NUMERIC)
			SetNumber(0);
		else
			SetText(_T(""));
	}
	GetSel(startPos, endPos);

	if (m_nMaskType == MASK_NUMERIC)
	{
		// Символ смены знака
		if (m_bSigned && nChar == '-')
		{
			// Установка знака
			UINT c = strText.GetAt(0);
			if (c == '-')	strText.SetAt(0, ' ');
			else			strText.SetAt(0, '-');
			// Установка текста
			SetWindowText(strText);
			SetSel(startPos, endPos);
			return FALSE;
		}
		// Если знак десятичного разделителя, 
		// перейти к дробной части
		if (nChar == m_cDecimal)
		{
			int nPos = strText.Find(m_cDecimal)+1;
			if (nPos >= 0)
				SetSel(nPos, nPos);
			else
				MessageBeep((UINT)-1);

			return FALSE;
		}
		// Проверка на цифру
		if (!isdigit(nChar))
		{
			MessageBeep((UINT)-1);
			return FALSE;
		}
		// Перейти к следующему символу,
		// если позиция на символе знака
		if (m_bSigned && startPos == 0)
		{
			SetSel(startPos+1, startPos+1);
			GetSel(startPos, endPos);
		}
	}

	// make sure the string is not longer than the mask
	// or prevent expanding the decimal fraction of number
	int	nMaskLen = m_strMask.GetLength();
	int	nTextLen = strText.GetLength();
	if (m_nDec == 0 && startPos >= nMaskLen ||
		m_nDec > 0 && startPos >= nTextLen)
	{
		MessageBeep((UINT)-1);
		return FALSE;
	}

	// check to see if a literal is in this position
	if (nTextLen == nMaskLen &&
		m_strMask.Mid(endPos, 1).FindOneOf(STR_FMT_CHAR) < 0)
	{
		SendChar(m_strMask.GetAt(endPos));
		GetSel(startPos, endPos);
	}

	// check the key against the mask
	if (m_nMaskType != MASK_NUMERIC)
		if (!IsValidChar(nChar, endPos, strText, 
			m_nMaskType, m_strMask, m_nLen))
		{
			MessageBeep((UINT)-1);
			return FALSE;
		}

	return TRUE;
}

void CMaskEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CString strText;
	GetWindowText(strText);

	if (!m_bMaskKeyInProgress)
		if (!CheckChar(nChar, strText))
			return;

	int startPos, endPos;
	GetSel(startPos, endPos);

	if (m_bUseMask)
	{
		// Обработка числовой маски
		if (m_nMaskType == MASK_NUMERIC)
		{
			// Длина текста
			int nTextLen = LineLength();
			// Позиция начала целой части числа
			int nBase = (m_nDec>0)? nTextLen-m_nDec-1: nTextLen;
			// Минимальная длина целой части
			int nLMin = (m_bSigned)? 2: 1;

			if (isprint(nChar))
			{
				// если длина текста короче длины маски
				if (nTextLen < m_strMask.GetLength())
				{
					// Замена символа справа, если позиция:
					// - внутри дробной части;
					// - на первом символе целой части, 
					//   а длина целой части - минимальная
					if (startPos >= nBase+1 && m_nDec > 0 ||
						startPos == nBase-1 && nBase == nLMin)
					{
						SetSel(startPos, endPos+1);
						ReplaceSel(CString((char)nChar));
					}
					// Замена символа слева, если позиция
					// в конце минимальной целой части,
					// а целая часть равна нулю
					else if (startPos == nBase && nBase == nLMin &&
						strText[nBase-1] == '0' && nChar != 0)
					{
						SetSel(startPos-1, endPos);
						ReplaceSel(CString((char)nChar));
					}
					// Добавление символа слева
					else
					{
						ReplaceSel(CString((char)nChar));
						// Установка нового значения
						SetNumber(GetNumber());
						// Установка текущей позиции
						startPos += LineLength()-nTextLen;
						SetSel(startPos, startPos);
					}
					return;
				}
				else
				{
					SetSel(startPos, endPos+1);
					Clear();
				}
			}
			else if (nChar == VK_BACK)
			{
				// Выделение символа слева
				SetSel(startPos-1, endPos);
				// Удаление символа слева, если
				// позиция внутри целой части и
				// длина целой части больше мнимальной
			    if (startPos <= nBase && nBase > nLMin)
				{
					if (strText[startPos-1] == (TCHAR)m_cThousand)
						SetSel(startPos-2, endPos);
					Clear();
					// Установка нового значения
					SetNumber(GetNumber());
					startPos -= nTextLen-LineLength();
				}
				// Замена символа слева, если
				// позиция больше начала целой части
				else if (startPos >= nBase)
				{
					if (m_nDec > 0 && startPos == nBase+1)
						ReplaceSel(CString((char)m_cDecimal));
					else
						ReplaceSel(CString((char)m_cPrompt));
					startPos--;
				}
				// Изменение позиции
				else
					startPos--;
				// Установка текущей позиции
				SetSel(startPos, startPos);
				return;
			}
		}

		// Обработка маски для текста
		// с фиксированной длиной
		else
		{
			if (isprint(nChar))
			{
				SetSel(startPos, endPos+1);
				Clear();
			}
			else if (nChar == VK_BACK)
			{
				// sanity range check
				if (startPos == endPos && startPos >= 1)
				{
					// back space the cursor
					SendMessage(WM_KEYDOWN, VK_LEFT, 0);
					// update the char backspacing over
					if (m_strMask.Mid(startPos - 1, 
						1).FindOneOf(STR_FMT_CHAR) < 0)
						SendChar(m_strMask.GetAt(startPos - 1));
					else
						SendChar(m_cPrompt);
					// back space the cursor again
					SendMessage(WM_KEYDOWN, VK_LEFT, 0);
				}
				// out of range or 
				// have more than one char selected
				else
					MessageBeep((UINT)-1);
				return;
			}
		}
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	if (!m_bMaskKeyInProgress &&
		!m_strMask.IsEmpty() && m_bUseMask)
	{
		// Если позиция меньше длины маски
		if (endPos < m_strMask.GetLength() - 1)
		{
			// добавление литерала из маски
			if (m_strMask.Mid(endPos + 1, 
				1).FindOneOf(STR_FMT_CHAR) < 0)
				SendChar(m_strMask[endPos + 1]);
		}
	}
}

void CMaskEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// si un masque existe, tester les touches spйciales
	if (m_bUseMask)
	{
	switch (nChar)
	{
		case VK_INSERT: return;
		case VK_DELETE:
		{
			// Отмена любого выделения
			int startPos, endPos;
			GetSel(startPos, endPos);
			if (startPos != endPos)
			{
				SetSel(-1, 0);
				// Установка пустого значения
				if (m_nMaskType == MASK_NUMERIC)
					SetNumber(0);
				else
					SetText(_T(""));
				return;
			}

			if (endPos < LineLength())
			{
				if (m_nMaskType == MASK_NUMERIC)
				{
					// Перейти к следующему символу,
					// если позиция на символе знака
					if (m_bSigned && startPos == 0)
					{
						SetSel(startPos+1, startPos+1);
						return;
					}
					// Длина текста
					CString strText;
					GetWindowText(strText);
					int nTextLen = LineLength();
					// Позиция начала целой части числа
					int nBase = (m_nDec>0)? nTextLen-m_nDec-1: nTextLen;
					// Минимальная длина целой части
					int nLMin = (m_bSigned)? 2: 1;
					// Выделение символа справа
					SetSel(startPos, endPos+1);
					// Удаление символа справа, если
					// позиция внутри целой части
				    if (startPos < nBase && nBase > nLMin)
					{
						if (strText[startPos] == (TCHAR)m_cThousand)
							SetSel(startPos, endPos+2);
						Clear();
						// Установка нового значения
						SetNumber(GetNumber());
					}
					// Замена символа справа
					else if (startPos == nBase-1 ||
							 startPos >= nBase+1)
					{
						ReplaceSel(CString((char)m_cPrompt));
						startPos++;
					}
					// Установка текущей позиции
					SetSel(startPos, startPos);
				}
				else
				{
					// update the char backspacing over
					if (m_strMask.Mid(startPos, 
						1).FindOneOf(STR_FMT_CHAR) < 0)
						SendChar(m_strMask.GetAt(startPos));
					else
						SendChar(m_cPrompt);
				}
			}
			// out of range
			else
				MessageBeep((UINT)-1);
			return;
		}
	}
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

CString CMaskEdit::FormatText(CString strText, UINT nMaskType, 
							  CString strMask, int nLen, 
							  UINT cPrompt, BOOL bFromLeft)
{
	// Форматирование текста по маске
	CString	strOut;
	int		nTextLen = strText.GetLength();
	int		nMaskLen = strMask.GetLength();
	int		i = bFromLeft? 0 : nTextLen - 1;
	int		m = bFromLeft? 0 : nMaskLen - 1;
	int		n = bFromLeft? 1 : -1;

	for (; bFromLeft?(m < nMaskLen):(m >= 0); m += n)
	{
		// Неформатируемый символ маски
		if (strMask.Mid(m, 1).FindOneOf(STR_FMT_CHAR) < 0)
			strOut.Insert((bFromLeft? nMaskLen:0),
							strMask.GetAt(m));
		// Символ внутри текста
		else if (bFromLeft?(i < nTextLen):(i >= 0))
		{
			// Неверный символ заменяется на
			// пустой символ
			if (!IsValidChar(strText.GetAt(i), m, strText, 
				nMaskType, strMask, nLen))
				strOut.Insert((bFromLeft? nMaskLen:0),
					cPrompt);
			// Правильный символ текста
			else
				strOut.Insert((bFromLeft? nMaskLen:0),
					strText.GetAt(i));
			// Изменение позиции в тексте
			i += n;
		}
		// Заполнение остатка текста
		else
			strOut.Insert((bFromLeft? nMaskLen:0),
				cPrompt);
	}
	return strOut;
}

CString CMaskEdit::FormatNumber(double dValue, int nDec, 
								UINT cDecimal, UINT cThousand, 
								BOOL bUseGroup, BOOL bSigned)
{
	// Преобразование числа в строку
	// с точностью nDec знаков после запятой
	int	iDec, iSign;
	CString strOut = _fcvt(dValue, nDec, &iDec, &iSign);
	// Установка нулевого значения
	if (strOut.IsEmpty()) strOut = _T("0");
	// Установка десятичного разделителя
	if (nDec > 0)
	{
		strOut.Insert(iDec, cDecimal);
		if (iDec <= 0) strOut.Insert(0, '0');
	}
	// Добавление разделителей групп
	if (bUseGroup)
	{
		for (int i=iDec, j=0, n=0; i > 0; i--,j++)
		{
			if (bUseGroup && j == 3*(n + 1))
			{
				n++;
				strOut.Insert(i, cThousand);
			}
		}
	}
	// Установка символа знака
	if (bSigned)
	{
		if (iSign == 0)	strOut.Insert(0, ' ');
		else			strOut.Insert(0, '-');
	}
	return strOut;
}

CString CMaskEdit::FormatDateTime(COleDateTime& DateTime, 
								  UINT nMaskType, int nLen, 
								  UINT cDate, UINT cTime)
{
	// Преобразование даты в строку
	if (DateTime.m_dt == 0) return CString();

	CString	sD = CString((char)cDate);
	CString	sT = CString((char)cTime);
	CString sFmt = "%d"+sD+"%m"+sD+"%Y %H"+sT+"%M";
	switch (nMaskType)
	{
	case MASK_DATE:
		switch (nLen)
		{
		case 8:	sFmt = "%d"+sD+"%m"+sD+"%y";	break;
		case 10:sFmt = "%d"+sD+"%m"+sD+"%Y";	break;
		default:sFmt = "%d"+sD+"%m"+sD+"%Y";	break;
		}
		break;
	case MASK_TIME:
		switch (nLen)
		{
		case 5:	sFmt = "%H"+sT+"%M";			break;
		case 8:	sFmt = "%H"+sT+"%M"+sT+"%S";	break;
		default:sFmt = "%H"+sT+"%M";			break;
		}
		break;
	case MASK_DATETIME:
		switch (nLen)
		{
		case 14:sFmt = "%d"+sD+"%m"+sD+"%y %H"+sT+"%M";
				break;
		case 16:sFmt = "%d"+sD+"%m"+sD+"%Y %H"+sT+"%M";
				break;
		case 17:sFmt = "%d"+sD+"%m"+sD+"%y %H"+sT+"%M"+sT+"%S";
				break;
		case 19:sFmt = "%d"+sD+"%m"+sD+"%Y %H"+sT+"%M"+sT+"%S";
				break;
		default:sFmt = "%d"+sD+"%m"+sD+"%Y %H"+sT+"%M";
				break;
		}
		break;
	}
	return DateTime.Format(sFmt);
}

CString CMaskEdit::ExtractText(CString strIn, BOOL bFromLeft)
{
	// Выделение текста из маскированного текста
	if (!m_bUseMask || m_strMask.IsEmpty())
		return strIn;

	CString	strText;
	int		nTextLen = strIn.GetLength();
	int		nMaskLen = m_strMask.GetLength();
	int		i = bFromLeft? 0 : nTextLen - 1;
	int		m = bFromLeft? 0 : nMaskLen - 1;
	int		n = bFromLeft? 1 : -1;

	for (; bFromLeft?(i < nTextLen):(i >= 0); i += n, m += n)
	{
		// Форматируемый символ маски
		if (m_strMask.Mid(m, 1).FindOneOf(STR_FMT_CHAR) >= 0)
		{
			// Неверный символ заменяется на
			// пустой символ
			if (!IsValidChar(strIn.GetAt(i), m, strIn, 
				m_nMaskType, m_strMask, m_nLen))
				strText.Insert((bFromLeft? nMaskLen:0),
							m_cPrompt);
			// Правильный символ текста
			else
				strText.Insert((bFromLeft? nMaskLen:0),
					strIn.GetAt(i));
		}
	}
	return strText;
}

UINT CMaskEdit::GetMaskType() const
{
	return m_nMaskType;
}

void CMaskEdit::SetMask(LPCTSTR lpMask, UINT cPrompt)
{
	// Установка маски ввода строки
	m_bUseMask	= FALSE;
	m_nMaskType	= MASK_STRING;
	m_strMask	= _T("");
	m_cPrompt	= ' ';
	m_nLen		= 0;
	m_nDec		= 0;
	if (lpMask == NULL) return;
	m_strMask = lpMask;
	if (m_strMask.IsEmpty()) return;
	m_cPrompt	= cPrompt;
	m_bUseMask	= TRUE;
	// Формирование и установка текста
	CString	strText;
	strText = FormatText(_T(""), m_nMaskType, 
			m_strMask, m_nLen, m_cPrompt);
	SetWindowText(strText);
}

void CMaskEdit::SetMask(UINT nMaskType, UINT nLen, UINT nDec) 
{
	// Установка специальной маски ввода
	m_bUseMask	= FALSE;
	m_nMaskType = nMaskType;
	m_strMask	= _T("");
	m_cPrompt	= ' ';
	m_nLen		= 0;
	m_nDec		= 0;

	CString	strText;
	switch (nMaskType)
	{
		case MASK_STRING:
			break;
		case MASK_NUMERIC:
		{
			// Установка длины по умолчанию
			if (nLen == 0) nLen = 10;
			// Формирование маски числа справа
			// Маска дробной части
			UINT i, n;
			if (nDec > 0)
			{
				for (i = 0; i < nDec; i++)
				{
					strText.Insert(0, '0');
					m_strMask.Insert(0, '0');
				}
				strText.Insert(0, m_cDecimal);
				m_strMask.Insert(0, m_cDecimal);
			}
			strText.Insert(0, " 0");
			// Маска целой части
			for (i = 0, n = 0; i < nLen; i++)
			{
				// Установка разделителя триад
				if (m_bUseGroup && i == 3*(n + 1))
				{
					n++;
					m_strMask.Insert(0, m_cThousand);
				}
				m_strMask.Insert(0, '0');
			}
			// Маска для знака числа
			if (m_bSigned)
				m_strMask.Insert(0, '+');

			m_nLen = nLen;
			m_nDec = nDec;
			m_cPrompt = '0';
			m_bUseMask = TRUE;
			break;
		}
		case MASK_DATE:
		{
			CString	sD = CString((char)m_cDate);
			switch (nLen)
			{
				case 8:	m_strMask = "00"+sD+"00"+sD+"00";	break;
				case 10:m_strMask = "00"+sD+"00"+sD+"0000";	break;
				default:m_strMask = "00"+sD+"00"+sD+"0000";
						nLen = 10;
						break;
			}
			m_nLen = nLen;
			m_cPrompt = '0';
			m_bUseMask = TRUE;
			strText = m_strMask;
			break;
		}
		case MASK_TIME:
		{
			CString	sT = CString((char)m_cTime);
			switch (nLen)
			{
				case 5: m_strMask = "00"+sT+"00";			break;
				case 8:	m_strMask = "00"+sT+"00"+sT+"00";	break;
				default:m_strMask = "00"+sT+"00";
						nLen = 5;
						break;
			}
			m_nLen = nLen;
			m_cPrompt = '0';
			m_bUseMask = TRUE;
			strText = m_strMask;
			break;
		}
		case MASK_DATETIME:
		{
			CString	sD = CString((char)m_cDate);
			CString	sT = CString((char)m_cTime);
			switch (nLen)
			{
				case 14:m_strMask = "00"+sD+"00"+sD+"00 "+
									"00"+sT+"00";
						break;
				case 16:m_strMask = "00"+sD+"00"+sD+"0000 "+
									"00"+sT+"00";
						break;
				case 17:m_strMask = "00"+sD+"00"+sD+"00 "+
									"00"+sT+"00"+sT+"00";
						break;
				case 19:m_strMask = "00"+sD+"00"+sD+"0000 "+
									"00"+sT+"00"+sT+"00";
						break;
				default:m_strMask = "00"+sD+"00"+sD+"00 "+
									"00"+sT+"00";
						nLen = 14;
						break;
			}
			m_nLen = nLen;
			m_cPrompt = '0';
			m_bUseMask = TRUE;
			strText = m_strMask;
			break;
		}
		default:
			m_nMaskType = MASK_STRING;
			break;
	}
	SetWindowText(strText);
}

BOOL CMaskEdit::SetText(LPCTSTR lpText) 
{
	// Установка текста
	CString	strText = lpText;
	if (m_bUseMask && !m_strMask.IsEmpty())
		strText = FormatText(strText, 
			m_nMaskType, m_strMask, 
			m_nLen, m_cPrompt);
	SetWindowText(strText);

	return TRUE;
}

CString CMaskEdit::GetText() 
{
	// Считывание текста
	CString	strText;
	GetWindowText(strText);
	switch (m_nMaskType)
	{
	case MASK_STRING:
		// Выделяем "чистый" текст, 
		// если имеется маска ввода
		strText = ExtractText(strText, TRUE);
		break;
	case MASK_NUMERIC:
		// Удаление разделителей групп
		strText.Remove(m_cThousand);
		// Преобразование десятичного разделителя
		// к стандартному виду
		strText.Replace(m_cDecimal, '.');
		// Удаление пробела слева
		strText.TrimLeft();
		break;
	case MASK_DATE:
	case MASK_TIME:
	case MASK_DATETIME:
		// Оставляем текст как есть
		break;
	}
	return strText;
}

BOOL CMaskEdit::SetNumber(double dValue) 
{
	// Установка числа
	if (m_bUseMask && 
		m_nMaskType != MASK_NUMERIC)
		return FALSE;

	// Форматирование и установка текста
	SetWindowText(FormatNumber(dValue, m_nDec, 
				m_cDecimal, m_cThousand, 
				m_bUseGroup, m_bSigned));

	return TRUE;
}

double CMaskEdit::GetNumber()  const
{
	// Считывание числа
	double	dValue = 0;
	CString	strText;
	if (m_bUseMask && 
		m_nMaskType == MASK_NUMERIC)
	{
		// Получение текста
		GetWindowText(strText);
		// Удаление разделителей групп
		strText.Remove(m_cThousand);
		// Преобразование десятичного разделителя
		// к стандартному виду
		strText.Replace(m_cDecimal, '.');
		// Преобразование в число
		dValue = strtod(strText, NULL);
	}

	return dValue;
}

BOOL CMaskEdit::SetDateTime(COleDateTime DateTime)
{
	if (m_bUseMask && 
		m_nMaskType != MASK_DATE &&
		m_nMaskType != MASK_TIME &&
		m_nMaskType != MASK_DATETIME)
		return FALSE;

	// Форматирование и установка текста
	SetWindowText(FormatDateTime(DateTime, 
	m_nMaskType, m_nLen, m_cDate, m_cTime));
	
	return TRUE;
}

COleDateTime CMaskEdit::GetDateTime()  const
{
	COleDateTime DateTime;
	if (m_bUseMask && 
		(m_nMaskType == MASK_DATE ||
		 m_nMaskType == MASK_TIME ||
		 m_nMaskType == MASK_DATETIME))
	{
		CString strText;
		GetWindowText(strText);
		DateTime.ParseDateTime(strText);
	}

	return DateTime;
}
