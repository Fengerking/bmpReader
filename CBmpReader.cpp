#include "CBmpReader.h"
#include "stdio.h"
#include "tchar.h"

CBmpReader::CBmpReader() {
	m_pBmpBuff = NULL;
	m_hMemDC = NULL;
	m_hBmpFile = NULL;
	memset(&m_bmpHeader, 0, sizeof(m_bmpHeader));
}

CBmpReader::~CBmpReader() {
	releaseBmp();
}

void CBmpReader::releaseBmp(void) {
	if (m_pBmpBuff != NULL)
		delete[]m_pBmpBuff;
	m_pBmpBuff = NULL;
	if (m_hMemDC != NULL)
		DeleteDC(m_hMemDC);
	m_hMemDC = NULL;
	if (m_hBmpFile != NULL)
		DeleteObject(m_hBmpFile);
	m_hBmpFile = NULL;
}

int	CBmpReader::OpenFile(const TCHAR * pBmpFile) {
	FILE * hFile = _wfopen(pBmpFile, _T("r"));
	if (hFile == NULL)
		return -1;
	releaseBmp();
	BITMAPFILEHEADER bmpFileInfo;
	memset(&bmpFileInfo, 0, sizeof(bmpFileInfo));
	fread(&bmpFileInfo, 1, sizeof(bmpFileInfo), hFile);
	memset(&m_bmpHeader, 0, sizeof(m_bmpHeader));
	fread(&m_bmpHeader, 1, sizeof(m_bmpHeader), hFile);
	if (m_pBmpBuff != NULL)
		delete[]m_pBmpBuff;
	m_pBmpBuff = new BYTE[m_bmpHeader.biSizeImage];
	fseek(hFile, bmpFileInfo.bfOffBits, SEEK_SET);
	fread(m_pBmpBuff, 1, m_bmpHeader.biSizeImage, hFile);
	fclose(hFile);
	return 0;
}

int CBmpReader::DrawBmp(HDC hDC, int nW, int nH) {
	if (m_pBmpBuff == NULL)
		return -1;

	if (m_hMemDC == NULL) {
		m_hMemDC = CreateCompatibleDC(hDC);
		void * pBmpBuff = NULL;
		m_hBmpFile = CreateDIBSection(m_hMemDC, (BITMAPINFO *)&m_bmpHeader, DIB_RGB_COLORS, (void **)&pBmpBuff, NULL, 0);
		SelectObject(m_hMemDC, m_hBmpFile);
		if (pBmpBuff == NULL)
			return -1;
		memcpy(pBmpBuff, m_pBmpBuff, m_bmpHeader.biSizeImage);
	}
	SetStretchBltMode(hDC, HALFTONE);
	StretchBlt(hDC, 0, 0, nW, nH, m_hMemDC, 0, 0, m_bmpHeader.biWidth, m_bmpHeader.biHeight, SRCCOPY);
	return 0;
}
