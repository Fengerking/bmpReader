#pragma once
#include "windows.h"

class CBmpReader
{
public:
	CBmpReader();
	virtual ~CBmpReader();

	virtual int		OpenFile(const TCHAR * pBmpFile);
	void			Convolute();
	virtual int		DrawBmp(HDC hDC, int nW, int nH);

protected:
	virtual void	releaseBmp(void);


protected:
	BITMAPINFOHEADER	m_headFile;
	unsigned char *		m_pBmpFile;

	BITMAPINFOHEADER	m_headData;
	unsigned char *		m_pBmpData;
	double				m_dKernelSmooth[3][3];
	int					m_nKernelX[3][3];
	int					m_nKernelY[3][3];

	HDC					m_hMemDC;
	HBITMAP				m_hBmpFile;
	HBITMAP				m_hBmpData;
};

