#pragma once
#include "windows.h"

class CBmpReader
{
public:
	CBmpReader();
	virtual ~CBmpReader();

	virtual int		OpenFile(const TCHAR * pBmpFile);
	virtual int		DrawBmp(HDC hDC, int nW, int nH);

protected:
	virtual void	releaseBmp(void);


protected:
	BITMAPINFOHEADER	m_headFile;
	unsigned char *		m_pBmpFile;

	BITMAPINFOHEADER	m_headData;
	int *				m_pBmpData;

	HDC					m_hMemDC;
	HBITMAP				m_hBmpFile;
	HBITMAP				m_hBmpData;
};

