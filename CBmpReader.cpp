#include "CBmpReader.h"
#include "stdio.h"
#include "tchar.h"

CBmpReader::CBmpReader() {
	m_pBmpFile = NULL;
	m_pBmpData = NULL;
	m_hMemDC = NULL;
	m_hBmpFile = NULL;
	m_hBmpData = NULL;
	memset(&m_headFile, 0, sizeof(m_headFile));
	memset(&m_headData, 0, sizeof(m_headData));
}

CBmpReader::~CBmpReader() {
	releaseBmp();
}

void CBmpReader::releaseBmp(void) {
	if (m_pBmpFile != NULL)
		delete[]m_pBmpFile;
	m_pBmpFile = NULL;
	if (m_pBmpData != NULL)
		delete[]m_pBmpData;
	m_pBmpData = NULL;
	if (m_hMemDC != NULL)
		DeleteDC(m_hMemDC);
	m_hMemDC = NULL;
	if (m_hBmpFile != NULL)
		DeleteObject(m_hBmpFile);
	m_hBmpFile = NULL;
	if (m_hBmpData != NULL)
		DeleteObject(m_hBmpData);
	m_hBmpData = NULL;
}

int	CBmpReader::OpenFile(const TCHAR * pBmpFile) {
	FILE * hFile = _wfopen(pBmpFile, _T("r"));
	if (hFile == NULL)
		return -1;
	releaseBmp();
	BITMAPFILEHEADER bmpFileInfo;
	memset(&bmpFileInfo, 0, sizeof(bmpFileInfo));
	fread(&bmpFileInfo, 1, sizeof(bmpFileInfo), hFile);
	memset(&m_headFile, 0, sizeof(m_headFile));
	fread(&m_headFile, 1, sizeof(m_headFile), hFile);
	m_pBmpFile = new BYTE[m_headFile.biSizeImage];
	fseek(hFile, bmpFileInfo.bfOffBits, SEEK_SET);
	fread(m_pBmpFile, 1, m_headFile.biSizeImage, hFile);
	fclose(hFile);

	memcpy(&m_headData, &m_headFile, sizeof(m_headData));
	m_headData.biBitCount = 32;
	m_headData.biSizeImage = m_headData.biWidth * m_headData.biHeight * 4;
	m_pBmpData = new int[m_headData.biSizeImage];
	memset(m_pBmpData, 0, m_headData.biSizeImage);
	unsigned char * pFile = m_pBmpFile;
	unsigned char * pData = (unsigned char *)m_pBmpData;
	for (int h = 0; h < m_headFile.biHeight; h++) {
		for (int w = 0; w < m_headFile.biWidth; w++) {
			*pData++ = *pFile++; // B
			*pData++ = *pFile++; // G
			*pData++ = *pFile++; // R
			*pData++;			 // Alpha
		}
	}
	return 0;
}

int CBmpReader::DrawBmp(HDC hDC, int nW, int nH) {
	if (m_pBmpFile == NULL)
		return -1;

	if (m_hMemDC == NULL) {
		m_hMemDC = CreateCompatibleDC(hDC);
		void * pBmpBuff = NULL;
		m_hBmpFile = CreateDIBSection(m_hMemDC, (BITMAPINFO *)&m_headFile, DIB_RGB_COLORS, (void **)&pBmpBuff, NULL, 0);
		if (pBmpBuff == NULL)
			return -1;
		memcpy(pBmpBuff, m_pBmpFile, m_headFile.biSizeImage);
		m_hBmpData = CreateDIBSection(m_hMemDC, (BITMAPINFO *)&m_headData, DIB_RGB_COLORS, (void **)&pBmpBuff, NULL, 0);
		if (pBmpBuff == NULL)
			return -1;
		memcpy(pBmpBuff, m_pBmpData, m_headData.biSizeImage);

	}
	SetStretchBltMode(hDC, HALFTONE);
	SelectObject(m_hMemDC, m_hBmpFile);
	StretchBlt(hDC, 0, 0, nW/2, nH, m_hMemDC, 0, 0, m_headFile.biWidth, m_headFile.biHeight, SRCCOPY);

	SelectObject(m_hMemDC, m_hBmpData);
	StretchBlt(hDC, nW/2, 0, nW/2, nH, m_hMemDC, 0, 0, m_headData.biWidth, m_headData.biHeight, SRCCOPY);

	return 0;
}
