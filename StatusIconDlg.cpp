// StatusIconDlg.cpp: implementation of the CStatusIconDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StatusIconDlg.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cScriptManager.h"
#include "./Input/UserInput.h"
#include "./Interface/cFont.h"
#include "MHFile.h"
#include "ItemManager.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "Quest.h"
#include "QuestManager.h"
//#include "GuildManager.h"
#include "../[cc]skill/client/info/skillinfo.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[cc]skill/client/info/buffskillinfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLOBALTON(CStatusIconDlg)

// 080318 LUJ,	�ִ� ������ ǥ�� ���� �ʱ�ȭ �߰�
//				��� �ð� �ʱ�ȭ �߰�
CStatusIconDlg::CStatusIconDlg() :
mMaxIconQuantity( UINT_MAX ),
mElaspedTime( gCurTime )
{
	m_pDescriptionArray = NULL;
	m_pObject = NULL;
	memset(m_IconCount,0,sizeof(WORD)*eStatusIcon_Max);
	memset(m_StatusIcon,0,sizeof(cImage*)*eStatusIcon_Max);
	memset(m_IconInfo,0,sizeof(ICONRENDERINFO)*eStatusIcon_Max);

	m_CurIconNum = 0;

	memset(m_dwRemainTime,0,sizeof(DWORD)*eStatusIcon_Max);
	memset(m_dwStartTime,0,sizeof(DWORD)*eStatusIcon_Max);

	m_nQuestIconCount = 0;

	mStatusInfoTable.Initialize(10);

	// 080318 LUJ, ������ ũ�� �ʱ�ȭ
	{
		const float buffIconSize = 25;

		SetIconSize( buffIconSize, buffIconSize );
	}
}

// 080318 LUJ, �ߺ� �ڵ带 Release()�� ����Ͽ� ����
CStatusIconDlg::~CStatusIconDlg()
{
	if(m_pDescriptionArray)
	{
		delete [] m_pDescriptionArray;
	}

	Release();
}


void CStatusIconDlg::AddIcon(CObject* pObject,WORD StatusIconNum, DWORD ItemIdx, DWORD dwRemainTime )
{
	if(pObject != m_pObject)
		return;

	if(StatusIconNum >= eStatusIcon_Max)
		__asm int 3;
	
	++m_IconCount[StatusIconNum];

	if( m_IconCount[StatusIconNum] == 1 )
		++m_CurIconNum;

	m_dwRemainTime[StatusIconNum] = dwRemainTime;
	m_dwStartTime[StatusIconNum] = gCurTime;
	m_IconInfo[StatusIconNum].bAlpha = FALSE;

	if( ItemIdx )
	{
		m_IconInfo[StatusIconNum].ItemIndex = ItemIdx;
		m_IconInfo[StatusIconNum].bPlus = TRUE;
		m_IconInfo[StatusIconNum].bAlpha = FALSE;
		m_IconInfo[StatusIconNum].Alpha = 0;
	}
}

void CStatusIconDlg::AddQuestTimeIcon( CObject* pObject,WORD StatusIconNum )
{
	if( pObject != m_pObject )	return;
	if(StatusIconNum >= eStatusIcon_Max)
		__asm int 3;
	
	++m_IconCount[StatusIconNum];

	if( m_IconCount[StatusIconNum] == 1 )
		++m_CurIconNum;

//	m_IconCount[StatusIconNum] = 1;

//	m_dwRemainTime[StatusIconNum] = dwRemainTime;
//	m_dwStartTime[StatusIconNum] = dwQuestIdx;
	m_IconInfo[StatusIconNum].bAlpha = FALSE;
}

void CStatusIconDlg::RemoveIcon(CObject* pObject,WORD StatusIconNum, DWORD ItemIdx)
{
	if(pObject != m_pObject)
		return;

	if(StatusIconNum >= eStatusIcon_Max)
		__asm int 3;

	if(m_IconCount[StatusIconNum] == 0)
		return;
	
	--m_IconCount[StatusIconNum];

	if( m_IconCount[StatusIconNum] == 0 )
	{
		--m_CurIconNum;

		m_dwRemainTime[StatusIconNum] = 0;
		m_dwStartTime[StatusIconNum] = 0;
		m_IconInfo[StatusIconNum].bAlpha = FALSE;
	}

	if( ItemIdx )
	{
		m_IconInfo[StatusIconNum].ItemIndex = 0;
		m_IconInfo[StatusIconNum].bPlus = FALSE;
		m_IconInfo[StatusIconNum].bAlpha = FALSE;
		m_IconInfo[StatusIconNum].Alpha = 0;
	}
}

void CStatusIconDlg::RemoveQuestTimeIcon( CObject* pObject,WORD StatusIconNum )
{
	if(pObject != m_pObject)	return;

	if(StatusIconNum >= eStatusIcon_Max)
		__asm int 3;

	if(m_IconCount[StatusIconNum] == 0)	return;
	
	--m_IconCount[StatusIconNum];
	if( m_IconCount[StatusIconNum] == 0 )
	{
		--m_CurIconNum;
		m_dwRemainTime[StatusIconNum] = 0;
		m_dwStartTime[StatusIconNum] = 0;
		m_IconInfo[StatusIconNum].bAlpha = FALSE;
	}
}

void CStatusIconDlg::RemoveAllQuestTimeIcon()
{
	if( m_IconCount[eStatusIcon_QuestTime] > 0 )
	{
		--m_CurIconNum;
	}

	m_IconCount[eStatusIcon_QuestTime] = 0;
	m_dwRemainTime[eStatusIcon_QuestTime] = 0;
	m_dwStartTime[eStatusIcon_QuestTime] = 0;
	m_IconInfo[eStatusIcon_QuestTime].bAlpha = FALSE;
}

void CStatusIconDlg::LoadDescription()
{
	CMHFile file;
	if(file.Init("Data/Interface/Windows/buf_msg.bin","rb",MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR) == FALSE)
		return;
	
	char buf[256];
	m_MaxDesc = file.GetInt();
	file.GetLine(buf,256);	// read enter
	m_pDescriptionArray = new StaticString[m_MaxDesc+1];
	for(int n=1;n<=m_MaxDesc;++n)	// ������ ��ȣ�� 1���� �����Ѵ�.
	{
		file.GetLine(buf,256);
		m_pDescriptionArray[n] = buf;
	}

	file.Release();
}

void CStatusIconDlg::Init(CObject* pObject, VECTOR2* pDrawPosition, int MaxIconPerLine)
{
	memset(m_IconCount, 0, sizeof(WORD)*eStatusIcon_Max);
	memset(m_StatusIcon, 0, sizeof(cImage*)*eStatusIcon_Max);

	m_pObject = pObject;
	m_DrawPosition = *pDrawPosition;
	m_MaxIconPerLine = MaxIconPerLine;

	// image load
	for (int n = 1; n<eStatusIcon_Max; ++n)
	{
		SCRIPTMGR->GetImage(n - 1, &m_StatusIcon[n], PFT_BUFFPATH);
	}

	//Description Load
	if (m_pDescriptionArray == NULL)
	{
		LoadDescription();
	}

	cImage ToolTipImg;
	SCRIPTMGR->GetImage(0, &ToolTipImg, PFT_HARDPATH);
	m_toolTip.Init(0, TTTC_DEFAULT, &ToolTipImg, TTCLR_ITEM_CANEQUIP);
	m_toolTip.SetImageAlpha(TOOLTIP_ALPHA);

	m_CurIconNum = 0;

	if (m_nQuestIconCount > 0)
	{
		m_IconCount[eStatusIcon_QuestTime] = WORD(m_nQuestIconCount);
		++m_CurIconNum;
		m_nQuestIconCount = 0;
	}
}

void CStatusIconDlg::Release()
{
	m_pObject = NULL;
	m_CurIconNum = 0;
	memset(m_IconCount,0,sizeof(WORD)*eStatusIcon_Max);
	memset(m_StatusIcon,0,sizeof(cImage*)*eStatusIcon_Max);

	m_nQuestIconCount = 0;

	mStatusInfoTable.SetPositionHead();

	for(StatusInfo* pStatusInfo = mStatusInfoTable.GetData();
		0 < pStatusInfo;
		pStatusInfo = mStatusInfoTable.GetData())
	{
		delete pStatusInfo->Image;
		delete pStatusInfo;
	}

	mStatusInfoTable.RemoveAll();

	// 080318 LUJ, ������ ���� �����̳ʰ� ���� �޸� ����
	{
		for(
			ReservedIconList::iterator it = mReservedIconList.begin();
			mReservedIconList.end() != it;
			++it )
		{
			SAFE_DELETE( *it );
		}

		mReservedIconList.clear();
	}
}

void CStatusIconDlg::Render()
{
#ifdef _GMTOOL_
	if( WINDOWMGR->IsInterfaceHided() ) return;
#endif

	int mx = g_UserInput.GetMouse()->GetMouseX();
	int my = g_UserInput.GetMouse()->GetMouseY();
	
	int curx = 0;
	int cury = 0;
	VECTOR2 curpos;

	mStatusInfoTable.SetPositionHead();

	for(StatusInfo* pStatusInfo = mStatusInfoTable.GetData();
		0 < pStatusInfo;
		pStatusInfo = mStatusInfoTable.GetData())
	{
		// 080318 LUJ, ������ ũ�⸦ �������� ���������� ��
		curpos.x = m_DrawPosition.x + curx * mIconSize.x;
		curpos.y = m_DrawPosition.y + cury * mIconSize.y;

		//---KES �����Ӱ� �����ð�ǥ��

		DWORD dwRemain = pStatusInfo->Time - gCurTime;
		if( dwRemain < 60*1000 )
		{
			float fAlphaDiff = (gCurTime - mElaspedTime) * 0.2f;
            
			if( pStatusInfo->fAlpha > fAlphaDiff )
				pStatusInfo->fAlpha -= fAlphaDiff;
			else
				pStatusInfo->fAlpha = 255.f;

			pStatusInfo->Image->RenderSprite(&Scale,NULL,0,&curpos, RGBA_MAKE(255, 255, 255, (DWORD)(pStatusInfo->fAlpha) ) );

			char second[16];
			// 100613 ShinJS --- 10�� �̸��ΰ�� �Ҽ������� ǥ���Ѵ�.
			if( dwRemain < 10000 )
			{
				const float sec = dwRemain / 1000.f;
				sprintf( second, "%.1f", sec );
			}
			else
			{
				sprintf( second, "%2d", dwRemain / 1000 + 1 );
			}

			const int secondSize = (int)(sizeof( char ) * strlen(second));
			const int nRenderPosX = (int)( curpos.x + mIconSize.x - CFONT_OBJ->GetTextExtentWidth( 0, second, secondSize ) - 1 );
			const int nRenderPosY = (int)( curpos.y + mIconSize.y - CFONT_OBJ->GetTextHeight(0) + 2 );

			// 080318 LUJ, ��Ƽâ�� ǥ�õǴ� 50% ���� ũ���� �����ܿ� ���ڰ� ǥ�õǵ� �˾ƺ� ���� ����
			if( Scale.x > 0.7f )
			{
				RECT rc;
				SetRect( &rc, nRenderPosX, nRenderPosY, 1, 1 );
				CFONT_OBJ->RenderFontAlpha( 0, second, secondSize, &rc, RGBA_MAKE(10, 10, 10, (DWORD)(pStatusInfo->fAlpha) ) );

				SetRect( &rc, nRenderPosX - 1, nRenderPosY - 1, 1, 1 );
				CFONT_OBJ->RenderFontAlpha( 0, second, secondSize, &rc, RGBA_MAKE(255, 255, 0, (DWORD)(pStatusInfo->fAlpha) ) );
			}			
		}
		else
		{
			pStatusInfo->Image->RenderSprite(&Scale,NULL,0,&curpos, 0xFFFFFFFF );
		}

		if( pStatusInfo->Count )
		{
			TCHAR count[ MAX_NAME_LENGTH ] = { 0 };
			_stprintf( count, _T( "%d" ), pStatusInfo->Count );
			const size_t length = _tcslen( count );
            
			// 090204 LUJ, ���� �ʹ� Ŭ ��� ������ ǥ���Ѵ�
			switch( length )
			{
			case 1:
			case 2:
			case 3:
				{
					break;
				}
			case 4:
			case 5:
			case 6:
				{
					ZeroMemory( count, sizeof( count ) );
					_stprintf( count, _T( "%dK" ), pStatusInfo->Count / 1000 );
					break;
				}
			case 7:
			case 8:
			case 9:
				{
					ZeroMemory( count, sizeof( count ) );
					_stprintf( count, _T( "%dM" ), pStatusInfo->Count / 1000000 );
					break;
				}
			default:
				{
					ZeroMemory( count, sizeof( count ) );
					SafeStrCpy( count, _T( "***" ), sizeof( count ) / sizeof( *count ) );
					break;
				}
			}

			RECT rc;
			SetRect( &rc, static_cast<int>(curpos.x) + 2, static_cast<int>(curpos.y) + 2, 1, 1 );
			CFONT_OBJ->RenderFont( 0, count, length, &rc, ICONTEXT_USABLE );
		}

		//-----------------------------

		if(++curx >= m_MaxIconPerLine)
		{
			curx = 0;
			++cury;
		}

		if( !WINDOWMGR->IsMouseOverUsed() )
		{
			int dx = int(mx - curpos.x);
			int dy = int(my - curpos.y);

			if(0 < dx && dx < mIconSize.x && 0 < dy && dy < mIconSize.y )
			{
				m_toolTip.Release();

				cSkillInfo* pInfo = SKILLMGR->GetSkillInfo( pStatusInfo->Index );
				BUFF_SKILL_INFO* pBuffSkillInfo = NULL;
				if( pInfo )
					pBuffSkillInfo = ((cBuffSkillInfo*)pInfo)->GetSkillInfo();

				if( pBuffSkillInfo )
				{
					char line[MAX_PATH]={0,};
					char typeTxt[MAX_PATH]={0,};
					//m_toolTip.AddLine("");
					wsprintf(line, CHATMGR->GetChatMsg( 2094 ), pInfo->GetName(), pInfo->GetLevel() );
					m_toolTip.AddLine( line, TTTC_SKILLNAME );
					m_toolTip.AddLine( "" );

					// 100315 ShinJS --- ���� �ʿ�/���� �׸� �߰�
					// ����
					if( SKILLMGR->GetSkillWeaponTypeText( pBuffSkillInfo, typeTxt, MAX_PATH ) )
					{
						sprintf(line, CHATMGR->GetChatMsg( 2055 ), SKILLMGR->GetSkillWeaponTypeText( pBuffSkillInfo, typeTxt, MAX_PATH ) );
						m_toolTip.AddLine( line, TTTC_SKILLLEVEL );
					}
					// ���
					if( SKILLMGR->GetSkillArmorTypeText( pBuffSkillInfo, typeTxt, MAX_PATH ) )
					{
						sprintf(line, CHATMGR->GetChatMsg( 2049 ), SKILLMGR->GetSkillArmorTypeText( pBuffSkillInfo, typeTxt, MAX_PATH ) );
						m_toolTip.AddLine( line, TTTC_SKILLLEVEL );
					}

					// ����
					if( pBuffSkillInfo->mCondition.mType != BUFF_SKILL_INFO::Condition::TypeNone )
					{
						sprintf(line, CHATMGR->GetChatMsg( 2062 ), SKILLMGR->GetBuffConditionTypeText( pBuffSkillInfo, typeTxt, MAX_PATH ) );
						m_toolTip.AddLine( line, TTTC_SKILLLEVEL );
					}

					struct {
						void operator() ( eBuffSkillActiveRule eRule, cMultiLineText& tooltip, char* buf, const char* form )
						{
							switch( eRule )
							{
							case eBuffSkillActiveRule_Active:		return;
							case eBuffSkillActiveRule_Delete:		sprintf( buf, "%s%s", form, CHATMGR->GetChatMsg( 2063 ) );			break;
							case eBuffSkillActiveRule_Stop:			sprintf( buf, "%s%s", form, CHATMGR->GetChatMsg( 2064 ) );			break;
							}
							tooltip.AddLine( buf, TTTC_SKILLLEVEL );
						}
					}AddLineActiveRule;

					// ������
					AddLineActiveRule( pBuffSkillInfo->mRuleBattle, m_toolTip, line, CHATMGR->GetChatMsg( 2065 ) );

					// ��ȭ��
					AddLineActiveRule( pBuffSkillInfo->mRulePeace, m_toolTip, line, CHATMGR->GetChatMsg( 2066 ) );

					// �̵���
					AddLineActiveRule( pBuffSkillInfo->mRuleMove, m_toolTip, line, CHATMGR->GetChatMsg( 2067 ) );

					// ������
					AddLineActiveRule( pBuffSkillInfo->mRuleStop, m_toolTip, line, CHATMGR->GetChatMsg( 2068 ) );

					// �޽Ľ�
					AddLineActiveRule( pBuffSkillInfo->mRuleRest, m_toolTip, line, CHATMGR->GetChatMsg( 2069 ) );

					// �����
					switch( pBuffSkillInfo->Die  )
					{
					case 0:		sprintf( line, "%s%s", CHATMGR->GetChatMsg( 2070 ), CHATMGR->GetChatMsg( 2071 ) );			break;
					case 1:		sprintf( line, "%s%s", CHATMGR->GetChatMsg( 2070 ), CHATMGR->GetChatMsg( 2072 ) );			break;
					}
					m_toolTip.AddLine( line, TTTC_SKILLLEVEL );

					// �ʿ� ������
					for( int i=0 ; i < sizeof(pBuffSkillInfo->Item)/sizeof(*pBuffSkillInfo->Item) ; ++i )
					{
						const ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( pBuffSkillInfo->Item[i] );
						if( pItemInfo )
						{
							sprintf( line, CHATMGR->GetChatMsg( 2073 ), pItemInfo->ItemName, pBuffSkillInfo->ItemCount[i] );
							m_toolTip.AddLine( line, TTTC_SKILLLEVEL );
						}
					}

					// �Һ���
					if( pBuffSkillInfo->Money )
					{
						sprintf( line, CHATMGR->GetChatMsg( 2074 ), AddComma( pBuffSkillInfo->Money ) );
						m_toolTip.AddLine( line, TTTC_SKILLLEVEL );
					}

					// �Һ�MP
					if( pBuffSkillInfo->mMana.mPlus )
					{
						sprintf( line, CHATMGR->GetChatMsg( 2075 ), (int)pBuffSkillInfo->mMana.mPlus );
						m_toolTip.AddLine( line, TTTC_SKILLLEVEL );
					}

					// �Һ�HP
					if( pBuffSkillInfo->mLife.mPlus )
					{
						sprintf( line, CHATMGR->GetChatMsg( 2076 ), (int)pBuffSkillInfo->mLife.mPlus );
						m_toolTip.AddLine( line, TTTC_SKILLLEVEL );
					}


					const std::vector< std::string >& cutTooltipList = ITEMMGR->GetToolTipMsg( pInfo->GetTooltip() );
					for( std::vector< std::string >::const_iterator cutTooltipListIter = cutTooltipList.begin() ; cutTooltipListIter != cutTooltipList.end() ; ++cutTooltipListIter )
					{
						const std::string strTooltip = *cutTooltipListIter;
						m_toolTip.AddLine( strTooltip.c_str(), TTTC_DESCRIPTION );
					}

					// ���� ���� ���ǵ�
					struct _AddClearConditionTooltip{
						BOOL bIsAddedClearCondition;

						_AddClearConditionTooltip()
						{
							bIsAddedClearCondition = FALSE;
						}
						void operator() ( cMultiLineText& tooltip )
						{
							if( !bIsAddedClearCondition )
							{
								tooltip.AddLine( CHATMGR->GetChatMsg( 2095 ), TTTC_QUESTITEM );		// "[��������]"
								bIsAddedClearCondition = TRUE;
							}
						}
					}AddClearConditionTooltip;
					
					if( pStatusInfo->Time > 0 )
					{
						if( pStatusInfo->Time >= gCurTime )
						{
							AddClearConditionTooltip( m_toolTip );

							DWORD RemainTime = pStatusInfo->Time - gCurTime;
						
							if( RemainTime >= 60000 )
							{
								const DWORD sec = (RemainTime % 60000) / 1000;
								const DWORD min = RemainTime / 60000;

								sprintf( line, "%s%d%s %d%s", CHATMGR->GetChatMsg(793), min, CHATMGR->GetChatMsg(794), sec, CHATMGR->GetChatMsg(795) );
							}
							else
							{
								// 100613 ShinJS --- 10�� �̸��ΰ�� �Ҽ������� ǥ���Ѵ�.
								if( RemainTime < 10000 )
								{
									const float sec = RemainTime / 1000.f;
									sprintf( line, "%s%.1f%s", CHATMGR->GetChatMsg(793), sec, CHATMGR->GetChatMsg(795) );
								}
								else
								{
									sprintf( line, "%s%d%s", CHATMGR->GetChatMsg(793), RemainTime / 1000 + 1, CHATMGR->GetChatMsg(795) );
								}
							}

							m_toolTip.AddLine(line, TTTC_QUESTITEM);
						}
					}

					// Count ��������
					if( pBuffSkillInfo->CountType != eBuffSkillCountType_None )
					{
						AddClearConditionTooltip( m_toolTip );

						sprintf( line, SKILLMGR->GetBuffCountTypeText( pBuffSkillInfo, typeTxt, MAX_PATH, pStatusInfo->Count ) );
						m_toolTip.AddLine( line, TTTC_QUESTITEM );
					}

					
					m_toolTip.SetXY( mx+TOOLTIP_MOUSESIZE_X, my+TOOLTIP_MOUSESIZE_Y );
					WINDOWMGR->SetToolTipWindow( &m_toolTip );
				}
			}
		}
	}

	mElaspedTime = gCurTime;
}

// 080318 LUJ, �̹� ǥ�� ���̰ų� ���� ���� �������� ��� �Ҵ����� �ʰ� ��ü�Ѵ�.
// 090204 LUJ, ��ġ ������ Ȯ����
void CStatusIconDlg::AddSkillIcon( DWORD ID, DWORD Index, DWORD time, int count )
{
	// 080318 LUJ,  �̹� ǥ�� ���� �������̶�� ������ ��ü�ϰ� ������
	{
		StatusInfo* info = mStatusInfoTable.GetData( Index );

		if( info )
		{
			info->Time	= time;
			info->Count	= count;
			info->ID	= ID;

			return;
		}
	}

	// 080318 LUJ, �̹� ���� ���� �������̸� ������ ��ü�ϰ� ������
	for(
		ReservedIconList::iterator it = mReservedIconList.begin();
		mReservedIconList.end() != it;
		++it )
	{
		StatusInfo* info = *it;

		if( info->Index == Index )
		{
			info->Time	= time;
			info->Count	= count;
			info->ID	= ID;

			return;
		}
	}

	cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( Index );

	// 080804 LUJ, ��ų ������ ���� 0�� ���� �ƹ� �������̳� �������Ƿ� üũ������
	if( ! pSkillInfo ||
		! pSkillInfo->GetImage() )
	{
		return;
	}

	cImage* pImage = new cImage;
	SCRIPTMGR->GetImage( pSkillInfo->GetImage(), pImage, PFT_SKILLPATH );

	StatusInfo* pStatusInfo = new StatusInfo;

	pStatusInfo->Image = pImage;
	pStatusInfo->Index = Index;
	pStatusInfo->Time = time;
	pStatusInfo->ID = ID;
	pStatusInfo->Count = count;
	
	// 080318 LUJ, �ִ� ������ ������ �ʰ��� ��� �����ϰ� ������
	if( mStatusInfoTable.GetDataNum() >= mMaxIconQuantity )
	{
		mReservedIconList.push_back( pStatusInfo );
	}
	else
	{
		mStatusInfoTable.Add( pStatusInfo, pStatusInfo->Index );
	}
}

// 080318 LUJ, ���� ���� �����ܵ� ������ �� �ֵ��� ��
void CStatusIconDlg::RemoveSkillIcon( DWORD ID, DWORD Index )
{
	// 080318 LUJ, ���� ���� �������̸� ����
	for(
		ReservedIconList::iterator it = mReservedIconList.begin();
		mReservedIconList.end() != it;
		++it )
	{
		StatusInfo* info = *it;

		if( info->ID == ID )
		{
			SAFE_DELETE( info );
			mReservedIconList.erase( it );

			break;
		}
	}

	StatusInfo* pStatusInfo = mStatusInfoTable.GetData( Index );

	if( pStatusInfo && ID == pStatusInfo->ID )
	{
		delete pStatusInfo->Image;
		delete pStatusInfo;
	
		mStatusInfoTable.Remove( Index );
	}

	// 080318 LUJ, ����� �������� ���� ��� ǥ�� ���� �����̳ʷ� �̵���Ų��
	if( !	mReservedIconList.empty() &&
			mStatusInfoTable.GetDataNum() < mMaxIconQuantity )
	{
		StatusInfo* info = mReservedIconList.front();
		mReservedIconList.pop_front();

		if( info )
		{
			mStatusInfoTable.Add( info, info->Index );
		}
	}
}

void CStatusIconDlg::RemoveAllSkillIcon()
{
	for( ReservedIconList::iterator it = mReservedIconList.begin(); 
		mReservedIconList.end() != it; ++it )
	{
		StatusInfo* pInfo	=	*it;
		SAFE_DELETE( pInfo );
	}
	mReservedIconList.clear();

	mStatusInfoTable.RemoveAll();
}

// 080318 LUJ, ����� �������� ������ �����Ѵ�
void CStatusIconDlg::UpdateSkillTime( DWORD ID, DWORD Index, DWORD time )
{
	// 080318 LUJ, ���� ���� ������ ���� ����
	for(
		ReservedIconList::iterator it = mReservedIconList.begin();
		mReservedIconList.end() != it;
		++it )
	{
		StatusInfo* info = *it;

		if( info->ID == ID )
		{
			info->Time = time;

			break;
		}
	}

	StatusInfo* pStatusInfo = mStatusInfoTable.GetData( Index );

	if( pStatusInfo && ID == pStatusInfo->ID )
	{
		pStatusInfo->Time = time;
	}
}


// 080318 LUJ, ����� �������� ������ �����Ѵ�
// 090204 LUJ, ��ġ ������ Ȯ����
void CStatusIconDlg::UpdateSkillCount( DWORD ID, DWORD Index, int count )
{
	// 080318 LUJ, ���� ���� ������ ���� ����
	for(
		ReservedIconList::iterator it = mReservedIconList.begin();
		mReservedIconList.end() != it;
		++it )
	{
		StatusInfo* info = *it;

		if( info->ID == ID )
		{
			info->Count = count;

			break;
		}
	}

	StatusInfo* pStatusInfo = mStatusInfoTable.GetData( Index );

	if( pStatusInfo && ID == pStatusInfo->ID )
	{
		pStatusInfo->Count = count;
	}
}


// 080318 LUJ, ǥ�� ������ ũ�� ����
void CStatusIconDlg::SetIconSize( float width, float height )
{
	mIconSize.x	= width;
	mIconSize.y	= height;

	Scale.x	= width / DEFAULT_ICONSIZE;
	Scale.y	= height / DEFAULT_ICONSIZE;
}


// 080318 LUJ, ���� ������ ��ų�� �ڷᱸ���� �����Ѵ�
void CStatusIconDlg::Copy( std::list< StatusInfo >& skillList )
{
	// 080318 LUJ, ǥ�� ���� ��ų ����
	{
		mStatusInfoTable.SetPositionHead();

		for(StatusInfo* source = mStatusInfoTable.GetData();
			0 < source;
			source = mStatusInfoTable.GetData())
		{
			StatusInfo dest = { 0 };
			dest.ID		= source->ID;
			dest.Index	= source->Index;
			dest.Time	= source->Time;
			dest.Count	= source->Count;

			skillList.push_back( dest );
		}
	}	

	// 080318 LUJ, ���� ���� ��ų ����
	for(
		ReservedIconList::iterator it = mReservedIconList.begin();
		mReservedIconList.end() != it;
		++it )
	{
		StatusInfo* source = *it;
		
		if( source )
		{
			StatusInfo dest = { 0 };
			dest.ID		= source->ID;
			dest.Index	= source->Index;
			dest.Time	= source->Time;
			dest.Count	= source->Count;

			skillList.push_back( dest );
		}		
	}
}

BOOL CStatusIconDlg::IsHasBuff(DWORD buffIndex)
{
	buffIndex = buffIndex / 100;

	mStatusInfoTable.SetPositionHead();

	while(const StatusInfo* const statusInfo = mStatusInfoTable.GetData())
	{
		if(buffIndex == statusInfo->Index / 100)
		{
			return TRUE;
		}
	}

	return FALSE;
}