#pragma once

/* ���� Ÿ��Ʋ */
struct FUNC
{
	void (*_func)(LONG lId, void * p, DWORD we);
	const char * _funcName;
};
FUNC g_mt_func[];
int FUNCSEARCH(char * funcName);

// �α���
void MT_LogInOkBtnFunc(LONG lId, void* p, DWORD we);
void MT_ExitBtnFunc(LONG lId, void* p, DWORD we);
void MT_EditReturnFunc(LONG lId, void* p, DWORD we);
void MT_DynamicBtnFunc(LONG lId, void* p, DWORD we);
void MT_DynamicEditReturnFunc(LONG lId, void* p, DWORD we);

// �÷��̾� ����
void CS_BtnFuncCreateChar(LONG lId, void* p, DWORD we);
void CS_BtnFuncDeleteChar(LONG lId, void* p, DWORD we);	 
void CS_BtnFuncFirstChar(LONG lId, void* p, DWORD we);	 
void CS_BtnFuncSecondChar(LONG lId, void* p, DWORD we);	 
void CS_BtnFuncThirdChar(LONG lId, void* p, DWORD we);	 
void CS_BtnFuncFourthchar(LONG lId, void* p, DWORD we);	 
void CS_BtnFuncFifthchar(LONG lId, void* p, DWORD we);	 
void CS_BtnFuncEnter(LONG lId, void* p, DWORD we);
void CS_FUNC_OkISee(LONG lId, void* p, DWORD we);
void CS_BtnFuncLogOut(LONG lId, void* p, DWORD we);
void CS_BtnFuncChangeServer( LONG lId, void* p, DWORD we );
void CTF_CBFunc( LONG lId, void* p, DWORD we ) ;
void CM_CharMakeBtnFunc(LONG lId, void* p, DWORD we);
void CM_CharCancelBtnFunc(LONG lId, void* p, DWORD we);
void MI_CharBtnFunc(LONG lId, void* p, DWORD we);
void MI_ExchangeBtnFunc(LONG lId, void* p, DWORD we);
void MI_InventoryBtnFunc(LONG lId, void* p, DWORD we);
void MI_SkillBtnFunc(LONG lId, void* p, DWORD we);
/*Inventory interface*/
void IN_DlgFunc(LONG lId, void * p, DWORD we);
/*Skill Dialog Interface*/
void MGI_DlgFunc(LONG lId, void * p, DWORD we);
void MGI_SkillDlgFunc(LONG lId, void * p, DWORD we);
void STD_SkillTreeDlgFunc(LONG lId, void * p, DWORD we);
void GradeClassDlgEventHandler(LONG lId, void * p, DWORD we);
void CI_DlgFunc(LONG lId, void * p, DWORD we);
void CI_AddExpPoint(LONG lId, void * p, DWORD we);

// 070111 LYW --- Add function to process events.
void CI_ActionEventFun( LONG lId, void* p, DWORD we ) ;
/*Quick Dialog Interface */
void QI_QuickDlgFunc(LONG lId, void * p, DWORD we);

// LYJ 051017 ���Գ����� �߰�
void SO_DlgFunc(LONG lId, void * p, DWORD we);

void BRS_DlgFunc(LONG lId, void * p, DWORD we);
void BRS_CloseFunc(LONG lId, void * p, DWORD we);
void BRS_DeleteFunc(LONG lId, void * p, DWORD we);
void BRS_TypeListFunc(LONG lId, void * p, DWORD we);
void BRS_ItemListFunc(LONG lId, void * p, DWORD we);
void BRS_ClassListFunc(LONG lId, void * p, DWORD we);
void BRS_REGBtnFunc(LONG lId, void * p, DWORD we);

void BS_DlgFunc(LONG lId, void * p, DWORD we);
void BS_TitleEditBoxFunc(LONG lId, void * p, DWORD we);
void BS_SellBtnFunc(LONG lId, void * p, DWORD we);
/*StreetStall interface*/
void SSI_DlgFunc(LONG lId, void * p, DWORD we);
void SSI_TitleEditBoxFunc(LONG lId, void * p, DWORD we);
void SSI_BuyBtnFunc(LONG lId, void * p, DWORD we);
void SSI_EditBtnFunc(LONG lId, void * p, DWORD we);	//6->34

/*NPC ���â*/
void NSI_HyperLinkFunc(LONG lId, void * p, DWORD we);

/*����â*/
void CMI_MoneyOkFunc(LONG lId, void * p, DWORD we);
/*����â*/
void DE_DlgFunc(LONG lId, void * p, DWORD we);
void DE_BuyItemFunc(LONG lId, void * p, DWORD we);
void DE_DailyItemFunc(LONG lId, void * p, DWORD we); //Alemuri Calendar Daily Item
// 070503 LYW --- GlobalEventFunc : Add function to cancel sell dialog.
void DE_CancelFunc(LONG lId, void* p, DWORD we) ;
void DE_SellItemFunc(LONG lId, void * p, DWORD we);
void DE_CloseHideBtn(LONG lId, void * p, DWORD we); //7->41
/*��Ƽ*/
void PA_BtnFunc(LONG lId, void * p, DWORD we);

//080318 LUJ, ��Ƽ�� ��� ��ų ������ ǥ�� ����
void PartyMemberFunc( LONG lId, void * p, DWORD we );

/*���*/
void GD_WarehouseFunc( LONG id, void* p, DWORD we );
void GD_DlgFunc( LONG id, void* p, DWORD we );
void GDMARK_DlgFunc( LONG id, void* p, DWORD we );
void GDLEVEL_DlgFunc( LONG id, void* p, DWORD we );
void GDRANK_DlgFunc( LONG id, void*, DWORD we );
void GDCREATE_DlgFunc( LONG id, void*, DWORD we );
void GDINVITE_DlgFunc( LONG id, void*, DWORD we );
void GDWHRANK_DlgFunc( LONG id, void*, DWORD we );
void GDREVIVAL_DlgFunc( LONG id, void*, DWORD we );
void GDUnion_DlgFunc( LONG id, void*, DWORD we );
void GDNICK_DlgFunc( LONG id, void*, DWORD we );

/*����/����/��ȭ/��æƮ�� ���� ���� ���� â*/
void PROGRESS_DlgFunc( LONG id, void*, DWORD we );

/*�йи�*/
void FD_DlgFunc( LONG lId, void* p, DWORD we );

/*ǥ��*/
void PYO_DlgBtnFunc(LONG lId, void * p, DWORD we);
void Note_DlgBtnFunc(LONG lId, void * p, DWORD we);
void NoteWrite_DlgBtnFunc(LONG lId, void * p, DWORD we);
void BrowserBtnFunc(LONG lId, void * p, DWORD we);
//100118 ONS ģ������ ���̾�α׿� PC���������� ����߰�
void Friend_DlgFunc(LONG lId, void * p, DWORD we);

/*��ü��ġ��*/
void Shout_DlgBtnFunc(LONG lId, void * p, DWORD we);

/*���ýý���*/
void Fishing_DlgBtnFunc(LONG lId, void * p, DWORD we);

/*ĳ���� ���*/
void CR_DlgBtnFunc(LONG lId, void * p, DWORD we);
/*����Ʈ*/
//void QUE_RegistQuestNotice(BOOL bManual);	// 080304 ggomgrak --- ����Ʈ�ڵ��˸��� ó���� ���� �߰� (���� ������ ����)
void QUE_QuestDlgFunc( LONG lId, void* p, DWORD we );
void GAMEIN_ChatFunc(LONG lId, void * p, DWORD we);
/*�ɼ�â*/
void OTI_DlgFunc(LONG lId, void * p, DWORD we);
/*����â*/
void EXT_DlgFunc(LONG lId, void * p, DWORD we);
// S ����ý��� �߰� added by hseos 2007.08.24	2007.09.10	2007.10.23
void FARM_BUY_DlgFunc(LONG lId, void * p, DWORD we);
void FARM_UPGRADE_DlgFunc(LONG lId, void * p, DWORD we);
void FARM_MANAGE_DlgFunc(LONG lId, void * p, DWORD we);
// E ����ý��� �߰� added by hseos 2007.08.24	2007.09.10	2007.10.23

// desc_hseos_����Ʈ ��_01
// S ����Ʈ �� �߰� added by hseos 2007.11.14  2007.11.15	2008.01.24
void DATE_ZONE_LIST_DlgFunc(LONG lId, void * p, DWORD we);
void CHALLENGE_ZONE_LIST_DlgFunc(LONG lId, void * p, DWORD we);
void CHALLENGE_ZONE_CLEARNO1_DlgFunc(LONG lId, void * p, DWORD we);
// E ����Ʈ �� �߰� added by hseos 2007.11.14  2007.11.15	2008.01.24

/*��ȯâ*/
void XCI_DlgFunc(LONG lId, void * p, DWORD we);
/*PKLOOTING*/
void PLI_DlgFunc( LONG lId, void* p, DWORD we );
/*CNA_BtnOKFunc*/
void CNA_BtnOkFunc( LONG lId, void* p, DWORD we );
void MessageBox_Func(LONG lId, void * p, DWORD we);
void DIVIDEBOX_Func(LONG lId, void * p, DWORD we);
void MONEYDIVIDEBOX_Func(LONG lId, void * p, DWORD we);
/*�̴ϸ�*/
void MNM_DlgFunc(LONG lId, void * p, DWORD we);

/*��޾� â*/
void ITD_DlgFunc(LONG lId, void * p, DWORD we);
/*���� â*/
void ITMD_DlgFunc(LONG lId, void * p, DWORD we);
/* ��ȭ â*/
void ITR_DlgFunc(LONG lId, void * p, DWORD we);

// 080215 LUJ, �ɼ� ���� â
void ApplyOptionDialogFunc(LONG lId, void * p, DWORD we);
// 080916 LUJ, �ռ� â
void ComposeDialogFunc(LONG lId, void * p, DWORD we);
/*ä�� â*/
void CHA_DlgBtnFunc(LONG lId, void * p, DWORD we);
// ServerList
void SL_DlgBtnFunc( LONG lId, void* p, DWORD we );

// guildfieldwar
void GFW_DlgFunc( LONG lId, void* p, DWORD we );
// Guild Tournament
void GDT_DlgFunc(LONG lId, void * p, DWORD we);
void CG_DlgFunc(LONG lId, void *p, DWORD we);
// 06. 02. ��ȭ ��� �ȳ� �������̽� �߰� - �̿���
void RFGUIDE_DLGFunc(LONG lId, void *p, DWORD we);
void GN_DlgBtnFunc(LONG lId, void *p, DWORD we);
void AN_DlgBtnFunc(LONG lId, void *p, DWORD we);

// 06. 03. ���İ��� - �̿���
void GNotice_DlgBtnFunc(LONG lId, void *p, DWORD we);

void GuildPlusTime_DlgFunc(LONG IId, void* p, DWORD we);//Add 060803 by wonju
// 061217 LYW --- Add function to call callback function.
void CM_CMNCbFunc( LONG lId, void* p, DWORD we) ;
void MDS_CBFunc( LONG lId, void* p, DWORD we ) ;
void CharMain_BtnFunc( LONG lId, void* p, DWORD we ) ;
void JO_DlgFunc(LONG lId, void* p, DWORD we);
void DMD_LogInOkBtnFunc( LONG lId, void* p, DWORD we ) ;		// ��Ī ���̾�α� �׽�Ʈ 2007/03/15 ������
void DMD_DateMatchingDlgFunc( LONG lId, void* p, DWORD we ) ;		// ��Ī ���̾�α� �׽�Ʈ 2007/03/16 ������
void DMD_PartnerDlgFunc( LONG lId, void* p, DWORD we ) ;

// 070606 LYW --- GlovalEventFunc : Add function to call callback function of identification dialog.
void DMD_IDT_Func(LONG lId, void* p, DWORD we) ;
// 070607 LYW --- GlovalEventFunc : Add function to call callback function of favor icon dialog.
void DMD_FID_Func(LONG lId, void* p, DWORD we) ;

// 070607 LYW --- GlovalEventFunc : Add function to call callback function of my info dialog.
void DMD_MID_Func(LONG lId, void* p, DWORD we) ;
// 070607 LYW --- GlovalEventFunc : Add function to call callback function of record info dialog.
void DMD_RID_Func(LONG lId, void* p, DWORD we) ;

// 070618 LYW --- GlovalEventFunc : Add function to call callback function of key setting dialog.
void KS_Func(LONG lId, void* p, DWORD we) ;

void SkillTrainDlgFunc( LONG lId, void* p, DWORD we );
void SkillTrainDlgFuncJct( LONG lId, void* p, DWORD we );//JobTree
// desc_hseos_�йи�01
// S �йи� �߰� added by hseos 2007.07.02
void FD_GuildDlg(LONG IId, void* p, DWORD we);
// E �йи� �߰� added by hseos 2007.07.02

// 071018 LYW --- GlovalEventFunc : Add event function for tutorial dialog.
void TTR_DlgFunc( LONG id, void* p, DWORD we ) ;

// 071023 LYW --- GlovalEventFunc : Add event function for tutorial button dialog.
void TTR_BTNDlgFunc( LONG id, void* p, DWORD we ) ;

// 071024 LYW --- GlovalEventFunc : Add event function for helper dialog.
void HD_DlgFunc(LONG lId, void* p, DWORD we ) ;

// 071130 LYW --- GlovalEventFunc : Add event function for map move dialog.
void MapMove_DlgFunc(LONG lId, void* p, DWORD we ) ;

// 071228 LYW --- GlovalEventFunc : Add event function for change name dialog.
void Change_Name_Func(LONG lId, void* p, DWORD we ) ;

// 080109 LYW --- GlovalEventFunc : ���� �е� ���̾�α� �Լ� �߰�.
void NumberPad_DlgFunc(LONG lId, void* p, DWORD we ) ;

//---KES AUTONOTE
void AutoNoteDlg_Func( LONG lId, void* p, DWORD we );
void AutoAnswerDlg_Func( LONG lId, void* p, DWORD we );
//---------------

//---KES �����˻�
void StoreSearchDlg_Func( LONG lId, void* p, DWORD we );
//---------------

// 080414 LUJ, �ܾ� ���� â
void BodyChangeFunc( LONG lId, void* p, DWORD we );


// 080403 LYW --- GlovalEventFunc : Add event functions for chatroom system.
void ChatRoomMainDlg_Func(LONG lId, void* p, DWORD we ) ;
void ChatRoomCreateDlg_Func(LONG lId, void* p, DWORD we ) ;
void ChatRoomDlg_Func(LONG lId, void* p, DWORD we ) ;
void ChatRoomGuestListDlg_Func(LONG lId, void* p, DWORD we ) ;
void ChatRoomJoinDlg_Func(LONG lId, void* p, DWORD we ) ;
void ChatRoomOptionDlg_Func(LONG lId, void* p, DWORD we ) ;
//void ChatRoomLobbyDlg_Func(LONG lId, void* p, DWORD we ) ;

// 080414 KTH -- ���� ���̾�α� �߰�
void Animal_DlgBtnFunc(LONG lId, void * p, DWORD we);

void PET_STATUS_DLG_FUNC(LONG lId, void* p, DWORD we ) ;
void PET_RES_DLG_FUNC(LONG lId, void* p, DWORD we ) ;

void PopupMenuDlgFunc(LONG lId, void* p, DWORD we ) ;

void COOK_DLG_FUNC(LONG lId, void* p, DWORD we ) ;
// 090422 ShinJS --- Ż���� ���� �˾� ���̾�α� �߰�
void RidePopupMenuDlgFunc(LONG lId, void* p, DWORD we );
// 090415 ONS ���� ���̵� ����
void GradeClassDlgFunc(LONG lId, void* p, DWORD we ) ;

//090323 pdy �Ͽ�¡ ��ġ UI�߰�
void HousingRevolDlgFunc(LONG lId, void* p, DWORD we );

//090330 pdy �Ͽ�¡ â�� UI�߰�
void Housing_WarehouseDlg_Func(LONG lId, void* p, DWORD we ) ;

//090409 pdy �Ͽ�¡ �Ͽ콺 �˻� UI�߰�
void HouseSearchDlgFunc(LONG lId, void* p, DWORD we ) ;

//090410 pdy �Ͽ�¡ �Ͽ콺 �̸����� UI�߰�
void HouseNameDlgFunc(LONG lId, void* p, DWORD we );

//090414 pdy �Ͽ�¡ �ٹ̱�����Ʈ ���� UI�߰�
void HousingDecoPointDlgFunc(LONG lId, void* p, DWORD we );

//090507 pdy �Ͽ�¡ �׼� �˾��Ŵ� UI�߰�
void HousingActionPopupMenuDlgFunc(LONG lId, void* p, DWORD we );

//090525 pdy �Ͽ�¡ ����â�� ��ư UI�߰�
void HousingWarehouseButtonDlgFunc(LONG lId, void* p, DWORD we );

// 090525 ShinJS --- Party �ٸ��ʿ� �ִ� ������ ���� �ʴ�� �̸��� �Է��ϴ� Dialog �߰�
void InputNameDlgFunc(LONG lId, void* p, DWORD we );

//090708 pdy �Ͽ�¡ �ٹ̱��� ��ư UI�߰�
void HousingDecoModeBtDlgFunc(LONG lId, void* p, DWORD we );

//091012 pdy �Ͽ�¡ �ٹ̱� ����Ʈ ��ư ��ȹ����
void HousingMainPointDlgFunc(LONG lId, void* p, DWORD we ) ;

//091223 NYJ �Ͽ�¡ �ٸ����湮
void HousingSearchDlgFunc(LONG lId, void* p, DWORD we ) ;

// 091210 ShinJS --- ��ȭ Dialog �߰�
void VideoCaptureDlgFunc(LONG lId, void* p, DWORD we );

// 100111 ONS �ΰ����� ��ư ���̾�α� �߰�
void AdditionalButtonDlgFunc( LONG lId, void* p, DWORD we );

// 100511 ONS �������� ���̾�α� �߰�
void ChangeClassDlgFunc( LONG lId, void* p, DWORD we );
// 100503 NYJ �ǸŴ���
void ConsignmentDlgFunc(LONG lId, void * p, DWORD we);

// ����Ÿ�� ���̾˷α�
void MonsterTargetDlgFunc(LONG lId, void * p, DWORD we);

// 100709 ONS ����Ʈ�˸��� ������ư ó�� �߰�
void QuickView_DlgFunc(LONG lId, void * p, DWORD we);

// 100628 ShinJS ����� ���̾�α� �߰�
void WorldMapDlgFunc(LONG lId, void * p, DWORD we);

// 100714 ShinJS ��� ���̾�α� �߰�
void BigMapDlgFunc(LONG lId, void * p, DWORD we);

// 5 Sep 2013 Death Add AntiBot Dialog
void AntiBotFunc( LONG lId, void* p, DWORD we ) ;

// 6 Feb 2014 Death Add Marry Dialog
void MarryFunc( LONG lId, void* p, DWORD we ) ;
void Slot_DlgBtnFunc(LONG lId, void * p, DWORD we);

void Report_DlgBtnFunc(LONG lId, void * p, DWORD we);

void Chaptcha_DlgBtnFunc(LONG lId, void * p, DWORD we);
// ---- skr
void AA_weatheronoff( LONG lId, void * p, DWORD we);
void AA_JobInfoEX( LONG lId, void * p, DWORD we);

//inspect
void CG_TargetObjectBtnFunc(LONG lId, void* p, DWORD we);