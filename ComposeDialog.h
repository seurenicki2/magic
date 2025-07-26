/*
080916 LUJ, �ߺ����� �ʴ� �� �������� �ռ��Ͽ� ���ο� �������� ����� ��� �������̽�
*/
#pragma once

#include "interface/cDialog.h"

class CItem;
class cListDialog;
class cIconDialog;
class cIconGridDialog;
class cButton;
class cStatic;

class CComposeDialog : public cDialog
{
public:
	CComposeDialog();
	virtual ~CComposeDialog();
	virtual BOOL FakeMoveIcon( LONG x, LONG y, cIcon* );
	virtual void SetActive( BOOL );
	void Linking();
	void OnActionEvent( LONG id, void* p, DWORD event );
	void SetKeyItem( const ITEMBASE& );
	// 080916 LUJ, �ռ� ���� �޽��� ó��
	void Proceed( const MSG_ITEM_COMPOSE_ACK& );
	// 080916 LUJ, �ռ� ���� �޽��� ó��
	void Proceed( const MSG_ITEM_COMPOSE_NACK& );
	// 080916 LUJ, ���α׷��� â���� ���� �ð��� ȣ���Ѵ�.
	void Send();

private:
	// 080916 LUJ, ��ϵ� �޸𸮸� �����Ѵ�
	void Release();
	// 080916 LUJ, ��Ʈ���� �ʱ�ȭ�Ѵ�
	void Clear( cIconDialog& ) const;
	void Clear( cIconGridDialog& ) const;
	void Clear( cIcon* ) const;
	// 080916 LUJ, ��� �������� ��ȯ�Ѵ�
	CItem* GetResultItem( POSTYPE );
	// 080916 LUJ, �־��� ITEMBASE ������ �������� �����Ѵ�
	void UpdateItem( CItem&, const ITEMBASE& ) const;
	// 080916 LUJ, ����� ������ ������ �����Ѵ�
	void UpdateResult( const ComposeScript&, int pageIndex );

private:
	typedef std::list< CItem* >	ItemList;
	ItemList					mItemList;

	ICONBASE				mKeyIconBase;
	cListDialog*			mOptionListDialog;
	cIconDialog*			mSourceIconDialog;
	cIconGridDialog*		mResultIconGridDialog;
	cButton*				mSubmitButton;
	cButton*				mPagePreviousButton;
	cButton*				mPageNextButton;
	cStatic*				mPageStatic;
	MSG_ITEM_COMPOSE_SYN	mComposeMessage;
};