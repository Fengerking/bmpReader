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
	BITMAPINFOHEADER	m_bmpHeader;
	LPBYTE				m_pBmpBuff;

	HDC					m_hMemDC;
	HBITMAP				m_hBmpFile;


};

