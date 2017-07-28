#include "gui_precomp.h"
#include "gui_font.h"
#include <math.h>

START_GUI

#define GUI_FONT_TEXT_COLOR	RGB(255,255,255)
#define GUI_FONT_BG_COLOR	RGB(0,0,0)

CGuiFont::CGuiFont()
{
	m_hFont = NULL;
	m_nPtHeight = 0;		
	m_nPtWidth = 0;			
	m_nAttributes = 0;	
	m_bLogRes = false;
	m_dwWidth = 0;
	m_dwHeight = 0;

	m_nRefCount = 0;
}

CGuiFont::~CGuiFont()
{
}

/**
* \brief Gui ��Ʈ �ε�
* \param hDC			(HDC) DC�� �ڵ�
* \param pFontName		(const char*) Font�� �̸�
* \param strKey			(std::string&) Font�� Ű
* \param nWidth			(int) ��Ʈ�� ����
* \param nHeight		(int) ��Ʈ�� ����
* \param nAttribute		(int) ��Ʈ�� Ư��
* \param bLogRes		(bool) 
*/
bool CGuiFont::Load(HDC hDC, const char *pFontName, std::string& strKey, int nWidth, int nHeight, int nAttribute, bool bLogRes)
{
	// ���� ī��Ʈ�� 0 �϶��� ��Ʈ�� �����ϰ� ���� ī��Ʈ�� ������Ų��.
	// ���� ī��Ʈ�� 1 �̻��϶��� ���� ī��Ʈ�� ������Ų��.
	if(m_nRefCount)
	{
		AddRef();
		return true;
	}
	else
	{
		if(nHeight <= 0)
			return false;
		
		float fDpiCx, fDpiCy;
		
		HFONT hFont, hOldFont;
		LOGFONT lf;
		POINT pt;
		TEXTMETRIC tm;

		SetGraphicsMode(hDC, GM_ADVANCED) ;
		ModifyWorldTransform(hDC, NULL, MWT_IDENTITY) ;
		SetViewportOrgEx(hDC, 0, 0, NULL) ;
		SetWindowOrgEx(hDC, 0, 0, NULL) ;

		if (bLogRes)
		{
			fDpiCx = (float) GetDeviceCaps (hDC, LOGPIXELSX) ;				// ���ι������� ������ ��ġ�� �ȼ���
			fDpiCy = (float) GetDeviceCaps (hDC, LOGPIXELSY) ;				// ���ι������� ������ ��ġ�� �ȼ���
		}
		else
		{
			// HORZRES = �����ȼ���, HORZSIZE = �и������� ����ũ��
			// VERTRES = �����ȼ���, VERTSIZE = �и������� ����ũ��
			fDpiCx = (float) (25.4 * GetDeviceCaps (hDC, HORZRES) / GetDeviceCaps (hDC, HORZSIZE)) ;
	    	fDpiCy = (float) (25.4 * GetDeviceCaps (hDC, VERTRES) / GetDeviceCaps (hDC, VERTSIZE)) ;
		}

		pt.x = (int) (nWidth  * fDpiCx / 72);
		pt.y = (int) (nHeight * fDpiCy / 72);
	
		DPtoLP (hDC, &pt, 1) ;

		lf.lfHeight         = - (int) (fabs((float)pt.y) / 10.0 + 0.5) ;
		lf.lfWidth          = 0 ;
		lf.lfEscapement     = 0 ;
		lf.lfOrientation    = 0 ;
		lf.lfWeight         = nAttribute & EZ_ATTR_BOLD      ? 700 : 0 ;		///< BOLD�� ����Ǿ� �ִٸ� 700 �ƴϸ� 0
		lf.lfItalic         = nAttribute & EZ_ATTR_ITALIC    ?   1 : 0 ;		///< ITRALIC�� ����Ǿ� �ִٸ� 1 �ƴϸ� 0
		lf.lfUnderline      = nAttribute & EZ_ATTR_UNDERLINE ?   1 : 0 ;		///< UNDERLINE�� ����Ǿ� �ִٸ� 1 �ƴϸ� 0
		lf.lfStrikeOut      = nAttribute & EZ_ATTR_STRIKEOUT ?   1 : 0 ;		///< STRIKEOUT�� ����Ǿ� �ִٸ� 1 �ƴϸ� 0
		lf.lfCharSet        = DEFAULT_CHARSET ;									///< �⺻ ���� ��Ʈ
		lf.lfOutPrecision   = 0 ;
		lf.lfClipPrecision  = 0 ;
		lf.lfQuality        = ANTIALIASED_QUALITY ;								///< �⺻ 4
		lf.lfPitchAndFamily = DEFAULT_PITCH;//VARIABLE_PITCH  ;					///< �⺻ 0

		WCHAR wszUnicode[1024];
		memset((char*)wszUnicode, 0, sizeof(WCHAR) * (1024));
		::MultiByteToWideChar(GetACP(), 0, pFontName, -1, wszUnicode, 1024);
		lstrcpy (lf.lfFaceName, wszUnicode) ;

		hFont = CreateFontIndirect (&lf) ;
     
		if (nWidth != 0)
		{
			hOldFont = (HFONT) SelectObject (hDC, hFont) ;
			GetTextMetrics (hDC, &tm) ;
			::DeleteObject (SelectObject (hDC, hOldFont)) ;

			lf.lfWidth = (int) (tm.tmAveCharWidth *	fabs ((float)pt.x) / fabs ((float)pt.y) + 0.5) ;

			hFont = CreateFontIndirect (&lf);
		}
		
		hOldFont = (HFONT)SelectObject (hDC, hFont);
		GetTextMetrics (hDC, &tm);
		SelectObject( hDC, hOldFont );
						
		m_strKey = strKey;
		m_nPtWidth = nWidth;		// points
		m_nPtHeight = nHeight;		// points
		m_nAttributes = nAttribute;	// attributes
		m_bLogRes = bLogRes;						
 		m_dwWidth = tm.tmAveCharWidth;
		m_dwHeight = tm.tmHeight;// + tm.tmInternalLeading;
		m_dwInternal = 0;//tm.tmInternalLeading;

		m_hDC = hDC;
		m_hFont = hFont;
		
		m_strFontName = pFontName;		

		AddRef();
	}

	return true;
}

VOID CGuiFont::Unload(HDC hDC)
{
	ReleaseRef();
	if(m_nRefCount == 0 && m_hFont)
	{
		::DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

VOID CGuiFont::AddRef(VOID)
{
	m_nRefCount++;
}

VOID CGuiFont::ReleaseRef(VOID)
{
	if(--m_nRefCount < 0)
		m_nRefCount = 0;
}

std::string CGuiFont::Getkey(VOID) const
{
	return m_strKey;
}

std::string CGuiFont::GetFontName(VOID) const
{
	return m_strFontName;
}

int CGuiFont::GetWidth(VOID) const
{
	return (int)m_dwWidth;
}

int CGuiFont::GetHeight(VOID) const
{
	return (int)m_dwHeight;
}

int CGuiFont::GetInternal(VOID) const
{
	return (int)m_dwInternal;
}

CPos CGuiFont::GetTextSize(const char *pString, int nLen)
{
	SIZE size;

	HFONT hFont = (HFONT)SelectObject(m_hDC, m_hFont);

	WCHAR wszUnicode[1024];
	memset((char*)wszUnicode, 0, sizeof(WCHAR) * (1024));
	::MultiByteToWideChar(GetACP(), 0, pString, -1, wszUnicode, 1024);
	nLen = (int)lstrlen(wszUnicode);
	::GetTextExtentPoint32( m_hDC, wszUnicode, nLen, &size );

	SelectObject(m_hDC, hFont);

	return CPos(size.cx, size.cy); 
}

CPos CGuiFont::GetTextWSize(const WCHAR *pString, int nLen) 
{
	SIZE size;

	HFONT hFont = (HFONT)SelectObject(m_hDC, m_hFont);
	
	::GetTextExtentPoint32( m_hDC, pString, nLen, &size );

	SelectObject(m_hDC, hFont);

	return CPos(size.cx, size.cy); 
}

CPos CGuiFont::GetBlankTextSize(VOID)	
{
	static char temp[2] = {32,0};
	return GetTextSize(temp, 1);
}

CRectangle CGuiFont::TextOut(const char *pString, int nX /* = 0 */, int nY /* = 0 */)
{
	CRectangle rect;

	int nLen = (int)strlen(pString);

	HFONT hFont = (HFONT)SelectObject(m_hDC, m_hFont);

	SIZE size;

	WCHAR wszUnicode[1024];

	memset((char*)wszUnicode, 0, sizeof(WCHAR) * (1024));
	::MultiByteToWideChar(GetACP(), 0, pString, -1, wszUnicode, 1024);
	nLen = (int)lstrlen(wszUnicode);
	::GetTextExtentPoint32( m_hDC, wszUnicode, nLen, &size );

	rect.left = nX;
	rect.top = nY;
	rect.right = nX+size.cx;
	rect.bottom = nY+size.cy;

	HBRUSH hBrush = CreateSolidBrush(GUI_FONT_BG_COLOR);
	::FillRect(m_hDC, (LPRECT)rect, hBrush);
	::DeleteObject(hBrush);	

	SetTextColor(m_hDC, GUI_FONT_TEXT_COLOR);
	::TextOut(m_hDC, nX, nY, wszUnicode, nLen);
	
	SelectObject(m_hDC, hFont);

	return rect;
}

CRectangle CGuiFont::TextOutW(const WCHAR *pString,int nX /* = 0 */, int nY /* = 0 */)
{
	CRectangle rect;

	int nLen = (int)lstrlen(pString);

	rect = TextOutW(nLen, pString, nX, nY);

	return rect;
}

CRectangle CGuiFont::TextOutW(int nLen, const WCHAR *pString, int nX /* = 0 */, int nY /* = 0 */)
{
	CRectangle rect;

	HFONT hFont = (HFONT)SelectObject(m_hDC, m_hFont);

	SIZE size;

	::GetTextExtentPoint32( m_hDC, pString, nLen, &size );

	rect.left = nX;
	rect.top = nY;
	rect.right = nX+size.cx;
	rect.bottom = nY+size.cy;

	if( m_nAttributes & EZ_ATTR_UNDERLINE )
		rect.bottom += TEXT_UNDERLINE_SIZE;

	HBRUSH hBrush = CreateSolidBrush(GUI_FONT_BG_COLOR);
	::FillRect(m_hDC, (LPRECT)rect, hBrush);
	::DeleteObject(hBrush);
	
	SetTextColor(m_hDC, GUI_FONT_TEXT_COLOR);
	::TextOut(m_hDC, nX, nY, pString, nLen);

	SelectObject(m_hDC, hFont);

	return rect;
}

bool CGuiFont::IsValid(VOID)
{
	return (m_hFont) ? true : false;
}

HDC CGuiFont::GetHdc(VOID)
{
	return m_hDC;
}

HFONT CGuiFont::GetHFont(VOID)
{
	return m_hFont;
}

END_GUI