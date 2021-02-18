// LzwCom.h: interface for the CLzwCom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_LZWCOM_H)
#define _LZWCOM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Progress.h"

#define TABSIZE  65536
#define NO_PRED  0xFFFF
#define NOT_FND  0xFFFF 
#define EMPTY    0xFFFF 
#define LZW_MAGIC 'LZW!'
#define UEOF ((unsigned)EOF)

typedef struct _ENTRY
{
	BOOL used;
	unsigned int next;         /* hi bit is 'used' flag */
	unsigned int predecessor;  /* 16 bit code           */
	unsigned int follower;
} entry;

// Определение массива
typedef CArray<entry,entry&> CEntryArray;

class CLzwCom : public CObject  
{
    DECLARE_DYNCREATE(CLzwCom)

// Construction
public:
	CLzwCom();
	virtual ~CLzwCom();

// Implementation
public:
	int compress(LPCTSTR infile, LPCTSTR outfile);
	int extract(LPCTSTR infile, LPCTSTR outfile);

// Operations
protected:
	int pop();
	int push(int c);
	void init_tab();
	void upd_tab(unsigned int pred, unsigned int foll);
	unsigned int eolist(unsigned int index);
	unsigned int h(unsigned int pred, unsigned int foll);
	unsigned int hash(unsigned int pred, unsigned int foll, BOOL update);
	unsigned int unhash(unsigned int pred, unsigned int foll);
	unsigned int getcode(FILE * fd);
	void putcode(FILE * fd, unsigned int code);

// Properties
protected:
	FILE * infd;
	FILE * outfd;
	CEntryArray string_tab;
	CByteArray stack;			// stack for characters
	unsigned int sp;			// current stack pointer
	CProgressDlg* pDlg;			// Dialog window
};

#endif // !defined(_LZWCOM_H)