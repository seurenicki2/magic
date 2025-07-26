// cMultiLineText.cpp: implementation of the cMultiLineText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cMultiLineText.h"
#include "GameResourceManager.h"
#include "cFont.h"
#include "cScriptManager.h"
#include "ItemManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(cMultiLineText)
cMultiLineText::cMultiLineText()
{
	m_fgColor  =0x00ffffff;
	m_imgColor = 0x00ffffff;
	m_line_idx = 0;
	topLine = NULL;
	m_wFontIdx = 0;
	m_max_line_width = 0;
	m_fValid = FALSE;
	SetRect(&m_m_leftTopPos,0,0,0,0);
//	m_fSurvive = FALSE;
	m_alpha = 255;
	m_dwOptionAlpha = 100;
	m_bNamePannel = FALSE;

	// 070209 LYW --- cMultiLineText : Add variable.
	m_bMovedText = FALSE ;

	// 070328 LYW --- cMultiLineText : Initialize m_bTextAlpha.
	m_bTextAlpha = FALSE ;

}

cMultiLineText::~cMultiLineText()
{
	Release();
}

void cMultiLineText::Init(WORD fontIdx, DWORD fgColor, cImage * bgImage, DWORD imgColor, BOOL bModified)
{
	Release();
	m_wFontIdx = fontIdx;
	m_fgColor = fgColor;
	if( bgImage )
	m_bgImage = *bgImage;
	m_imgColor = imgColor;

	// 070209 LYW --- Initialize rect.
	m_renderRect = m_m_leftTopPos;
	m_ItemTip = bModified;

}

void cMultiLineText::Release()
{
	LINE_NODE * curLineNode = topLine;
	LINE_NODE * tmpNode = NULL;
	
	// 091208 pdy �޸� �ҰŰ� �ȵǰ� �־� ����
	while(curLineNode)
	{
		tmpNode = curLineNode;
		curLineNode = curLineNode->nextLine;

		LINE_NODE * PatialNode = tmpNode->nextPatial;
		while( PatialNode )
		{
			LINE_NODE * tmpNode2 = PatialNode;
			PatialNode = PatialNode->nextPatial ;
			SAFE_DELETE(tmpNode2);		
		}

		SAFE_DELETE(tmpNode);		
	}

	topLine = NULL;
	m_fValid = FALSE;
	m_line_idx = 0;
	m_max_line_width = 0;
	m_bNamePannel = FALSE;

	//Alemuri for all the 15 elements in the array set an empty image to reset it---
	for (int i = 0; i < 15; i++)
	{
		cImage image;
		toolTipImageArray[i] = image;
	}
	//------------------------------------------------------------------------------
}

void cMultiLineText::Render()
{
	LINE_NODE * curLineNode = topLine;

	VECTOR2 scaleRate, Pos, scaleRate2, Pos2; //Pos2 and scaleRate2 has been added
	//--------------------------------------------------------------------------------------------------------
	scaleRate2.x = 0.7;
	scaleRate2.y = 0.7;
	
	//cImage *image = new(cImage);
	//ITEMMGR->GetIconImage(30000548, image);
	//--------------------------------------------------------------------------------------------------------
	// 070209 LYW --- cMultiLineText : Modified render part.
	if (m_bMovedText)
	{
		SetRenderArea(&scaleRate, &Pos);
		m_bMovedText = FALSE;
	}
	if (!m_bgImage.IsNull())
	{
		if (m_ItemTip)
		{
			scaleRate.x = 1; scaleRate.y = 1;
			m_bgImage.RenderSprite(&scaleRate, NULL, 0, &Pos, RGBA_MERGE(m_imgColor, m_alpha * m_dwOptionAlpha / 100));
		}
		else
			m_bgImage.RenderSprite(&scaleRate, NULL, 0, &Pos, RGBA_MERGE(m_imgColor, m_alpha * m_dwOptionAlpha / 100));

	}

	//--------------------------------------------------------------------------------------------------------
	Pos2 = Pos;
	Pos2.x -= 27;
	int arrCount = 0;
	//--------------------------------------------------------------------------------------------------------
	while (curLineNode)
	{
		m_renderRect.right = m_renderRect.left + 1;
		m_renderRect.bottom = m_renderRect.top + 1;

		//--------------------------------------------------------------------------------------------------------
		if ((curLineNode->len > 0) && (!toolTipImageArray[arrCount].IsNull()))
		{
			//if (strcmp(topLine->line, "Item Drop") != 0) //Item Drop is the tooltip of the button, it change, it need to be updated
			//{
				toolTipImageArray[arrCount].RenderSprite(&scaleRate2, NULL, 0, &Pos2, RGBA_MAKE(255, 255, 255, 255));
				Pos2.y += 29.8;
				arrCount += 1;
			//}
		}

		//--------------------------------------------------------------------------------------------------------

		// 070328 LYW --- cMultiLineText : Modified render part.
		//CFONT_OBJ->RenderFont(m_wFontIdx,curLineNode->line,curLineNode->len,&m_renderRect,RGBA_MERGE(curLineNode->color, m_alpha * m_dwOptionAlpha / 100 ) );
		if (m_bTextAlpha)
		{
			CFONT_OBJ->RenderFontAlpha(m_wFontIdx, curLineNode->line, curLineNode->len, &m_renderRect, RGBA_MERGE(curLineNode->color, m_alpha * m_dwOptionAlpha / 100));
		}
		else
		{
			CFONT_OBJ->RenderFont(m_wFontIdx, curLineNode->line, curLineNode->len, &m_renderRect, RGBA_MERGE(curLineNode->color, m_alpha * m_dwOptionAlpha / 100));
		}

		if (curLineNode->nextPatial)
		{
			LINE_NODE * PatialLine = curLineNode->nextPatial;

			RECT renderRect = m_renderRect;

			LONG TextWidth = CFONT_OBJ->GetTextWidth(m_wFontIdx);

			while (PatialLine)
			{
				if (m_bTextAlpha)
				{
					renderRect.left = renderRect.right - TextWidth / 4;
					renderRect.right = renderRect.left + 1;

					CFONT_OBJ->RenderFontAlpha(m_wFontIdx, PatialLine->line, PatialLine->len, &renderRect, RGBA_MERGE(PatialLine->color, m_alpha * m_dwOptionAlpha / 100));
				}
				else
				{
					renderRect.left = renderRect.right - TextWidth;
					renderRect.right = renderRect.left + 1;

					CFONT_OBJ->RenderFont(m_wFontIdx, PatialLine->line, PatialLine->len, &renderRect, RGBA_MERGE(PatialLine->color, m_alpha * m_dwOptionAlpha / 100));
				}

				PatialLine = PatialLine->nextPatial;
			}
		}

		m_renderRect.top += LINE_HEIGHT;
		curLineNode = curLineNode->nextLine;
	}
}

// 070209 LYW --- cMultiLineText : Add function to setting area of render part.
void cMultiLineText::SetRenderArea(VECTOR2* scaleRate, VECTOR2* Pos)
{
	m_renderRect = m_m_leftTopPos;

	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();

	if( m_renderRect.left + m_max_line_width > (int)(dispInfo.dwWidth) ||
		m_renderRect.top + (m_line_idx+1)*LINE_HEIGHT > (int)(dispInfo.dwHeight) )
	{
		if( m_renderRect.left - ( TOOLTIP_MOUSESIZE_X+4 + m_max_line_width ) >= 0 )
			m_renderRect.left -= TOOLTIP_MOUSESIZE_X+4 + m_max_line_width;
		else
			m_renderRect.left -= TOOLTIP_MOUSESIZE_X;

		if( m_renderRect.top - ( TOOLTIP_MOUSESIZE_Y + (m_line_idx+1)*LINE_HEIGHT ) >= 0 )
			m_renderRect.top -= TOOLTIP_MOUSESIZE_Y+4 + (m_line_idx+1)*LINE_HEIGHT;
		else 
			m_renderRect.top -= TOOLTIP_MOUSESIZE_Y-4;
	}

	if( m_bNamePannel )
	{
		scaleRate->x = (float)((m_max_line_width+5)>>2);
		scaleRate->y = (float)(((m_line_idx+1)*LINE_HEIGHT+1)>>2);
		Pos->x = (float)m_renderRect.left-2;
		Pos->y = (float)m_renderRect.top-3;
	}
	else
	{
		scaleRate->x = (float)((m_max_line_width+10)>>2);
		scaleRate->y = (float)(((m_line_idx+1)*LINE_HEIGHT+10)>>2);
		Pos->x = (float)m_renderRect.left-5;
		Pos->y = (float)m_renderRect.top-5;
	}
}


void cMultiLineText::SetText( const char* text )
{
	if(0 == _tcslen(text))
	{
		return;
	}
	else if(_tcslen(text) > sizeof(topLine->line) / sizeof(*topLine->line))
	{
		text = _T("*text is too long");
	}

	if( topLine )
	{
		Release();
	}
	
	m_line_idx	= 0;
	m_max_line_width = 0; //KES �ʱ�ȭ

	LINE_NODE * curLineNode = topLine = new LINE_NODE;
	curLineNode->nextLine = NULL;
	curLineNode->color = m_fgColor;

	char * cur_line = topLine->line;

	while(*text != 0)
	{
//		if(*text & 0x80)
		if( IsDBCSLeadByte(*text) )
		{
			*cur_line++ = *text++;
			*cur_line++ = *text++;
		}
		else
		{
			switch(*text)
			{
			case TEXT_DELIMITER:
				{
					++text;
					char tmp = *text; //
					switch(tmp)
					{
					case TEXT_NEWLINECHAR:	// new line
						{
							*cur_line = 0;
							curLineNode->len = strlen(curLineNode->line);

							int real_len = CFONT_OBJ->GetTextExtentEx(m_wFontIdx, curLineNode->line, curLineNode->len); 
							if(m_max_line_width < real_len)
							{
								m_max_line_width = real_len;
							}
							m_line_idx++;
							//while(curLineNode && curLineNode->nextLine)
							//{
							//	curLineNode = curLineNode->nextLine;
							//}
							curLineNode = curLineNode->nextLine = new LINE_NODE;
							curLineNode->nextLine = NULL;
							cur_line = curLineNode->line;
							curLineNode->color = m_fgColor;
						}
						break;
					case TEXT_TABCHAR:
						{
						}
						break;
					case TEXT_SPACECHAR:
						{	
							*cur_line = ' ';
							++cur_line;
						}
						break;
					}// - switch()
				}
				break;

			default:
				{
					*cur_line = *text;
					++cur_line;
				}
				break;
			}//- switch()
			++text;
		}
	}
	
//	*cur_line = 0;	//KES �߰�
	curLineNode->len = strlen(curLineNode->line);
	curLineNode->nextLine = NULL;
//	int real_len = CFONT_OBJ->GetTextExtent(m_wFontIdx, curLineNode->line, curLineNode->len);
	int real_len = CFONT_OBJ->GetTextExtentEx(m_wFontIdx, curLineNode->line, curLineNode->len); 
	if(m_max_line_width < real_len)
	{
		m_max_line_width = real_len;
	}

	m_fValid = TRUE;
}

void cMultiLineText::AddLine(const char* text, DWORD dwColor, DWORD wItemIdx, int imageIndexArray)
{
	if (text == NULL) return;

	LINE_NODE* pLineNode = new LINE_NODE;
	pLineNode->nextLine = NULL;
	pLineNode->color = dwColor;
	strcpy(pLineNode->line, text);
	pLineNode->len = strlen(pLineNode->line);
	m_fValid = TRUE;

	LINE_NODE* pTail = topLine;
	if (pTail)
	{
		while (pTail->nextLine)
			pTail = pTail->nextLine;

		pTail->nextLine = pLineNode;
		++m_line_idx;
	}
	else
	{
		topLine = pLineNode;
	}

	int	real_len = CFONT_OBJ->GetTextExtentEx(m_wFontIdx, pLineNode->line, pLineNode->len);

	if (m_max_line_width < real_len)
	{
		m_max_line_width = real_len;
	}

	if ((wItemIdx != 0) && (imageIndexArray != 1000))
	{
		cImage image;
		ITEMMGR->GetIconImage(wItemIdx, &image);
		toolTipImageArray[imageIndexArray] = image;
	}

}

// 091012 pdy ���ٿ� ������ ADD�Ǵ� ���� �߰�  
void cMultiLineText::AddPatialLine(const char* text, DWORD dwColor, BOOL bNewLine )
{
	if( text == NULL ) return;

	if( topLine == NULL && bNewLine == FALSE )
	{
		// ������ �ϳ��� ���µ� �κ� �߰� �õ� 
#ifdef _GMTOOL_
		char buf[128] = {0,};
		sprintf( buf, "cMultiLineText::AddPatialLine Warning!! Not Have TopLine");
		MessageBox(NULL,buf,0,0);
#endif
	}

	static int real_len = 0;


	if( bNewLine == TRUE )
	{
		// ���� �����߰��� real_len�� �ʱ�ȭ 
		real_len = 0;
	}

	LINE_NODE* pNewLine	= new LINE_NODE;
	pNewLine->nextLine		= NULL;
	pNewLine->color		= dwColor;
	strcpy( pNewLine->line, text );
	pNewLine->len			= strlen(pNewLine->line);
	m_fValid = TRUE;

	if( topLine == NULL )
	{
		topLine = pNewLine ;
	}
	else
	{
		LINE_NODE* pLastLine = topLine ;

		while( pLastLine->nextLine )
			pLastLine = pLastLine->nextLine;

		if( bNewLine == TRUE )
		{
			//���� �����߰��� ������ ��忡 ����

			pLastLine->nextLine = pNewLine ;
			++m_line_idx;
		}
		else
		{
			//������ ����� ���� ����Ʈ�� ���� 
			if( pLastLine->nextPatial )
			{
				LINE_NODE* pLastPatial = pLastLine->nextPatial;
				while( pLastPatial->nextPatial )
					pLastPatial = pLastPatial->nextPatial;

				pLastPatial->nextPatial = pNewLine ;
			}
			else
			{
				pLastLine->nextPatial = pNewLine ;
			}
		}
	}

	real_len += CFONT_OBJ->GetTextExtentEx( m_wFontIdx, pNewLine->line, pNewLine->len );

	if(m_max_line_width < real_len)
	{
		m_max_line_width = real_len;
	}
}

void cMultiLineText::AddNamePannel( DWORD dwLength )
{
	LINE_NODE* pLineNode	= new LINE_NODE;
	pLineNode->nextLine		= NULL;
	pLineNode->color		= 0xffffffff;
	pLineNode->len			= strlen(pLineNode->line);
	m_fValid = TRUE;

	LINE_NODE* pTail		= topLine;
	if( pTail )
	{
		while( pTail->nextLine )
			pTail = pTail->nextLine;

		pTail->nextLine = pLineNode;
		++m_line_idx;
	}
	else
	{
		topLine = pLineNode;
	}
	
	char buf[MAX_NAME_LENGTH+1];
	strncpy(buf, "AAAAAAAAAAAAAAAA", MAX_NAME_LENGTH);
	int real_len = CFONT_OBJ->GetTextExtentEx( m_wFontIdx, buf, dwLength ); 

	if(m_max_line_width < real_len)
	{
		m_max_line_width = real_len;
	}

	m_bNamePannel = TRUE;
}

// �ӽ÷� ���۵� �Լ� //��� ���� �����ΰ�...--;
BOOL cMultiLineText::SetText(VECTOR3 * pPos3, const char * szText)
{	
	VECTOR3 OutPos3;
	GetScreenXYFromXYZ(g_pExecutive->GetGeometry(), 0, pPos3, &OutPos3);
	if(OutPos3.x < 0 || OutPos3.x > 1 || OutPos3.y < 0 || OutPos3.y > 1)
		return FALSE;

	SetXY((long)(GAMERESRCMNGR->m_GameDesc.dispInfo.dwWidth*OutPos3.x-30), (long)(GAMERESRCMNGR->m_GameDesc.dispInfo.dwHeight*OutPos3.y));

	SetText(szText);

	return TRUE;
}
