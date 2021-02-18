// LzwCom.cpp: implementation of the CLzwCom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "trans30.h"
#include "LzwCom.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CLzwCom, CObject)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLzwCom::CLzwCom()
{
	sp = 0;
	// Initialize code table
	string_tab.SetSize(TABSIZE);
	stack.SetSize(TABSIZE);
	init_tab();
}

CLzwCom::~CLzwCom()
{
	string_tab.RemoveAll();
	stack.RemoveAll();
}

int CLzwCom::pop()
{
	if (sp > 0) 
	{
		// Push leaves sp pointing to next empty slot
		--sp;
		// Make sure pop returns char
		return ((int) stack[sp]);
	}
	else
		return EMPTY;
}

int CLzwCom::push(int c)
{
	// Coerce passed integer into a character
	stack[sp] = ((char) c);
	++sp;
	if (sp >= TABSIZE)
	{
		return -1;
	}
	return 0;
}

void CLzwCom::init_tab()
{
	register unsigned int i;
	for (i = 0; i <= 255; i++) upd_tab(NO_PRED,i);
}

void CLzwCom::upd_tab(unsigned int pred, unsigned int foll)
{
	unsigned int i = hash(pred,foll,TRUE);

	string_tab[i].used = TRUE;
	string_tab[i].next = 0;
	string_tab[i].predecessor = pred;
	string_tab[i].follower = foll;
}

unsigned int CLzwCom::eolist(unsigned int index)
{
	// Returns last element in a collision list
	register unsigned int temp;
	while ( (temp = string_tab[index].next) != 0)
	  index = temp;
	return index;
}

unsigned int CLzwCom::h(unsigned int pred, unsigned int foll)
{
	// Returns the mid square of pred + foll
	// h uses the 'mid-square' algorithm. I.E. for a hash val of n bits
	// hash = middle binary digits of (key * key).  Upon collision, hash
	// searches down linked list of keys that hashed to that key already.
	// It will NOT notice if the table is full. This must be handled elsewhere.
	long temp, local;				// 32 bit receiving field for local^2
    local = (pred + foll) | 0x8000;
    temp = local * local;
    return ((temp >> 8) & 0xFFFF);  // middle 16 bits of result
}

unsigned int CLzwCom::hash(unsigned int pred, unsigned int foll, BOOL update)
{
	register unsigned int i, local, tempnext;

	local = h(pred,foll);

	if ( !string_tab[local].used )
	{
		return local;
	}
	else
	{
		// if collision has occured
		local = eolist(local);
		// search for free entry from local + 101
		tempnext = (local + 101) & 0xFFFF;
		// initialize pointer
		i = tempnext;
		while ( string_tab[i].used )
		{
			++tempnext;
			if ( tempnext == TABSIZE )
			{
				tempnext = 0;	// handle wrap to beginning of table
				i = 0;			// address of first element of table
			} else
				i++;			// point to next element in table
		}

		// put new tempnext into last element in collision list
		if ( update )				// if update requested
		{
			string_tab[local].next = tempnext;
		}
		return tempnext;
	}
}

unsigned int CLzwCom::unhash(unsigned int pred, unsigned int foll)
{
	// unhash uses the 'next' field to go down the collision tree to find
	// the entry corresponding to the passed key
	// passed key and returns either the matching entry # or NOT_FND
	register unsigned int local;

	local = h(pred,foll);       // initial hash

	loop:
		if ((string_tab[local].predecessor == pred) && 
			(string_tab[local].follower == foll)) 
		{
			return local;
		}
		if ( string_tab[local].next == 0 )
		{
			return NOT_FND;
		}
		local = string_tab[local].next;
	goto loop;
}

unsigned int CLzwCom::getcode(FILE * fd)
{
	register unsigned int c0, c1;

	/* H L1 byte */
	if ((c1 = fgetc(fd)) == EOF)
	{
		return EOF;
	}
	/* H L0 next */
	if ((c0 = fgetc(fd)) == EOF)
	{
		return EOF;
	}
	return (((c1 << 8) & 0xFF00) + (c0 & 0x00FF));
}

void CLzwCom::putcode(FILE * fd, unsigned int code)
{
	/* H L1 byte */
	fputc((code >> 8) & 0x00FF, fd);
	/* H L0 next */
	fputc((code & 0x00FF), fd);
}

int CLzwCom::compress(LPCTSTR infile, LPCTSTR outfile)
{
	CString strErr;
	CString strCapt;
	register unsigned int c, code, localcode;
	long n, fsize;
	char unknown = FALSE;
	int  code_count = TABSIZE - 256;
	int  iPos;

	if ((infd = fopen( infile, "rb" )) == NULL)
	{
		strErr.LoadString(IDS_MS_COMPRESS_DATA);
		strErr += _T(": ");
		strErr += GetErrMsg(ER_OPEN_FILE, infile);
		// Запись текста ошибки в файл
		if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
		// Показ сообщения об ошибке
		if (TrnParm.iShowErr)
		{
			MessageBeep(MB_ICONEXCLAMATION);
			AfxMessageBox(strErr, MB_OK|MB_ICONEXCLAMATION);
		}
		return FALSE;
	}
	if ((outfd = fopen(outfile, "wb")) == NULL)
	{
		fclose(infd);
		strErr.LoadString(IDS_MS_COMPRESS_DATA);
		strErr += _T(": ");
		strErr += GetErrMsg(ER_WRITE_FILE, outfile);
		// Запись текста ошибки в файл
		if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
		// Показ сообщения об ошибке
		if (TrnParm.iShowErr)
		{
			MessageBeep(MB_ICONEXCLAMATION);
			AfxMessageBox(strErr, MB_OK|MB_ICONEXCLAMATION);
		}
		return FALSE;
	}
	// Count input file size
	fseek(infd, 0L, SEEK_END);
	fsize = ftell(infd);
	fclose(infd);
	// Open input file again
	infd = fopen( infile, "rb");

	AfxGetApp()->BeginWaitCursor();
	// Create message window
	pDlg = new CProgressDlg;
	pDlg->Create(IDD_PROGRESSDLG, AfxGetApp()->m_pMainWnd);
	pDlg->SetBreakEnable(TRUE);
	pDlg->SetRange(0, 100);
	pDlg->SetPos(0);
	strCapt.LoadString(IDS_MS_COMPRESS_DATA);
	pDlg->SetWindowText(strCapt);
	strCapt.FormatMessage(IDS_MS_COMPRESS1, infile);
	pDlg->SetMessageText(strCapt);
	strCapt.FormatMessage(IDS_MS_FILE2, outfile);
	pDlg->SetSubText(strCapt);
	n = iPos = 0;

	// Put magic code
	fputc((int)((LZW_MAGIC>>24) & 0xFF), outfd);
	fputc((int)((LZW_MAGIC>>16) & 0xFF), outfd);
	fputc((int)((LZW_MAGIC>> 8) & 0xFF), outfd);
	fputc((int)((LZW_MAGIC    ) & 0xFF), outfd);
	// Initial code for table
	c = fgetc(infd);
	code = unhash(NO_PRED,c);

	while ((c = fgetc(infd)) != UEOF)
	{
		n++;
		if (bCancel) goto end;
		// Check status
		pDlg->CheckState();
		if (pDlg->IsBreaked())
		{
			bCancel = TRUE;
			goto end;
		}
		// Set position
		if ((int)(n*100/fsize) > iPos)
		{
			iPos = (int)(n*100/fsize);
			pDlg->SetPos(iPos);
		}
		if ((localcode = unhash(code,c)) != NOT_FND)
		{
			code = localcode;
			continue;
		}
		// when the above clause comes false,
		// you have found the last known code
		// only update table if table isn't full
		putcode(outfd,code);
		if ( code_count ) 
		{
			upd_tab(code,c);
			--code_count;
		}
		// start loop again with the char
		// that didn't fit into last string
		code = unhash(NO_PRED,c);
	}
	// once EOF reached, always
	// one code left unsent
	putcode(outfd,code);

end:// Make sure everything gets closed
	fclose(infd);
	fclose(outfd);
	// Destroy dialog window
	AfxGetApp()->EndWaitCursor();
	if (pDlg != NULL)
	{
		pDlg->DestroyWindow();
		delete pDlg;
		pDlg = NULL;
	}
	return TRUE;
}

int CLzwCom::extract(LPCTSTR infile, LPCTSTR outfile)
{
	CString strErr;
	CString strCapt;
	register unsigned int c, finchar, lastchar;
	register unsigned int tempc, code, oldcode, incode;
	long magic, n, fsize;
	char unknown = FALSE;
	int  code_count = TABSIZE - 256;
	int  iPos;

	if ((infd = fopen( infile, "rb" )) == NULL)
	{
		strErr.LoadString(IDS_MS_EXTRACT_DATA);
		strErr += _T(": ");
		strErr += GetErrMsg(ER_OPEN_FILE, infile);
		goto err;
	}
	if ((outfd = fopen(outfile, "wb")) == NULL)
	{
		strErr.LoadString(IDS_MS_EXTRACT_DATA);
		strErr += _T(": ");
		strErr += GetErrMsg(ER_WRITE_FILE, outfile);
		goto err;
	}
	// Count input file size
	fseek(infd, 0L, SEEK_END);
	fsize = ftell(infd);
	fclose(infd);
	// Open input file again
	infd = fopen( infile, "rb");

    // Check magic code
	magic  = fgetc(infd); magic = magic<<8;
	magic |= fgetc(infd); magic = magic<<8;
	magic |= fgetc(infd); magic = magic<<8;
	magic |= fgetc(infd);
	if (magic != LZW_MAGIC)
	{
		strErr.FormatMessage(IDS_ER_LZW_MAGIC, 
			infile, magic, LZW_MAGIC);
		goto err;
	}

	AfxGetApp()->BeginWaitCursor();
	// Create message window
	pDlg = new CProgressDlg;
	pDlg->Create(IDD_PROGRESSDLG, AfxGetApp()->m_pMainWnd);
	pDlg->SetBreakEnable(TRUE);
	pDlg->SetRange(0, 100);
	pDlg->SetPos(0);
	strCapt.LoadString(IDS_MS_EXTRACT_DATA);
	pDlg->SetWindowText(strCapt);
	strCapt.FormatMessage(IDS_MS_EXTRACT1, infile);
	pDlg->SetMessageText(strCapt);
	strCapt.FormatMessage(IDS_MS_FILE2, outfile);
	pDlg->SetSubText(strCapt);
	n = iPos = 0;

	// First code always known
	code = oldcode = getcode(infd);
	c = string_tab[code].follower;
	fputc(c, outfd);
	finchar = c;

	while ((code = incode = getcode(infd)) != UEOF)
	{
		n++;
		if (bCancel) goto end;
		// Check status
		pDlg->CheckState();
		if (pDlg->IsBreaked())
		{
			bCancel = TRUE;
			goto end;
		}
		// Set position
		if ((int)(n*100/fsize) > iPos)
		{
			iPos = (int)(n*100/fsize);
			pDlg->SetPos(iPos);
		}

		if (!string_tab[code].used)	// if code isn't known
		{
			lastchar = finchar;
			code = oldcode;
			unknown = TRUE;
		}

		while (string_tab[code].predecessor != NO_PRED)
		{
			// decode string backwards into stack
			if (push( string_tab[code].follower ) < 0)
			{
				strErr.LoadString(IDS_MS_ER_STACK_OVF);
				goto err;
			}
			code = string_tab[code].predecessor;
		}

		finchar = string_tab[code].follower;
		// Above loop terminates, one way or another, with
		// String_tab[code].follower = first char in string

		fputc(finchar, outfd);
		// Pop anything stacked during code parsing
		while ((tempc = pop()) != EMPTY)
		{
			fputc(tempc, outfd);
		}
		// if code isn't known
		// the follower char of last
		if ( unknown )
		{
			fputc(finchar = lastchar, outfd);
			unknown = FALSE;
		}
		if ( code_count ) 
		{
			upd_tab(oldcode,finchar);
			--code_count;
		}
		oldcode = incode;
	}

end:// Make sure everything gets closed
	fclose(infd);
	fclose(outfd);
	// Destroy dialog window
	AfxGetApp()->EndWaitCursor();
	if (pDlg != NULL)
	{
		pDlg->DestroyWindow();
		delete pDlg;
		pDlg = NULL;
	}
	return TRUE;

err:
	// Запись текста ошибки в файл
	if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
	// Показ сообщения об ошибке
	if (TrnParm.iShowErr)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(strErr, MB_OK|MB_ICONEXCLAMATION);
	}
	// Make sure everything gets closed
	fclose(infd);
	fclose(outfd);
	// Destroy dialog window
	AfxGetApp()->EndWaitCursor();
	if (pDlg != NULL)
	{
		pDlg->DestroyWindow();
		delete pDlg;
		pDlg = NULL;
	}
	return FALSE;
}
