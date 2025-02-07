
#include "pch.h"
#include "framework.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "GDIPlusEffect.h"
#include "GDIPlusEffectComm.h"
#include "GDIPlusEffectView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd() noexcept
{
	
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_ADD_EFFECT, OnAddEffect)
	ON_UPDATE_COMMAND_UI(ID_ADD_EFFECT, OnUpdateAddEffect)
	ON_COMMAND(ID_CLEAR_EFFECT, OnClearEffect)
	ON_UPDATE_COMMAND_UI(ID_CLEAR_EFFECT, OnUpdateClearEffect)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EFFECT_TYPE_0, ID_EFFECT_TYPE_0+(int)GDIPlusEffect::Type::Total, OnUpdateEffectType)
	ON_UPDATE_COMMAND_UI(ID_EFFECT_CMD_REMOVE, OnUpdateEffectCmdRemove)
	ON_UPDATE_COMMAND_UI(ID_EFFECT_CMD_ENABLE, OnUpdateEffectCmdEnable)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(nullptr, rectClient.left, rectClient.top + cyTlb, rectClient.Width(), rectClient.Height() -cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnAddEffect()
{
	CMenu menu;
	menu.CreatePopupMenu();

	for (int ii = 0; ii < (int)GDIPlusEffect::Type::Total; ++ii)
	{
		auto szText = GDIPlusEffect::GetTypeName((GDIPlusEffect::Type)ii);
		menu.AppendMenu(MF_STRING, ID_EFFECT_TYPE_0 +ii, szText);
	}

	CPoint point;
	GetCursorPos(&point);

	CCmdUI state;
	state.m_pMenu = &menu;
	state.m_nIndexMax = menu.GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; ++state.m_nIndex)
	{
		state.m_nID = menu.GetMenuItemID(state.m_nIndex);
		state.DoUpdate(this, FALSE);
	}

// 	theApp.GetContextMenuManager()->ShowPopupMenu(
// 		menu, point.x, point.y, this);

	int nMenuResult = theApp.GetContextMenuManager()->TrackPopupMenu(
		menu, point.x, point.y, this);

	if (!nMenuResult)
		return;

	auto nType = (GDIPlusEffect::Type)(nMenuResult - ID_EFFECT_TYPE_0);

	auto pEffect = GDIPlusEffect::CreateEffect(nType);
	if (!pEffect)
		return;

	auto pProps = pEffect->CreateProps();
	pEffect->AttachProps(pProps);
	m_wndPropList.AddProperty(pProps);

	auto pView = CGDIPlusEffectView::GetActiveView();
	pView->AddEffect(pEffect);
}

void CPropertiesWnd::OnUpdateAddEffect(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnUpdateEffectType(CCmdUI* pCmdUI)
{
}

void CPropertiesWnd::OnClearEffect()
{
	auto pView = CGDIPlusEffectView::GetActiveView();
	pView->ClearEffect();
}

void CPropertiesWnd::OnUpdateClearEffect(CCmdUI* pCmdUI)
{
	auto pView = CGDIPlusEffectView::GetActiveView();
	pCmdUI->Enable(pView->HasEffect());
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
	m_wndPropList.ScreenToClient(&point);
	CMFCPropertyGridProperty::ClickArea nArea = CMFCPropertyGridProperty::ClickExpandBox;
	auto pProp = m_wndPropList.HitTest(point, &nArea, TRUE);
	if (pProp)
	{
		CMenu menu;
		menu.CreatePopupMenu();

		int nn = 0;
		menu.AppendMenu(MF_STRING, ID_EFFECT_CMD_REMOVE, _T("&Remove"));
		menu.AppendMenu(MF_STRING, ID_EFFECT_CMD_ENABLE, _T("&Enable"));
		menu.CheckMenuItem(ID_EFFECT_CMD_ENABLE, TRUE);
		++nn;

		CPoint point;
		GetCursorPos(&point);

		CCmdUI state;
		state.m_pMenu = &menu;
		state.m_nIndexMax = menu.GetMenuItemCount();
		for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; ++state.m_nIndex)
		{
			state.m_nID = menu.GetMenuItemID(state.m_nIndex);
			state.DoUpdate(this, FALSE);
		}

		int nMenuResult = theApp.GetContextMenuManager()->TrackPopupMenu(
			menu, point.x, point.y, this);

		if (!nMenuResult)
			return;
		auto pView = CGDIPlusEffectView::GetActiveView();

		if (!pProp->IsGroup())
		{
			pProp = pProp->GetParent();
		}
		int nPropIndex = 0;
		int nCount = m_wndPropList.GetPropertyCount();
		for (int ii = 0; ii < nCount; ++ii)
		{
			auto pCurProp = m_wndPropList.GetProperty(ii);
			if (pCurProp == pProp)
			{
				nPropIndex = ii;
				break;
			}
		}

		switch (nMenuResult)
		{
		case ID_EFFECT_CMD_REMOVE:
			pView->RemoveEffect(nPropIndex);
			break;
		case ID_EFFECT_CMD_ENABLE:
			{
				bool bEnable = !pView->IsEffectEnabled(nPropIndex);
				pView->EnableEffect(nPropIndex, bEnable);
				pProp->Enable(bEnable);
				nCount = pProp->GetSubItemsCount();
				for (int ii = 0; ii < nCount; ++ii)
				{
					auto pSubProp = pProp->GetSubItem(ii);
					pSubProp->Enable(bEnable);
				}
			}
			break;
		}
	}
}

void CPropertiesWnd::OnUpdateEffectCmdRemove(CCmdUI* pCmdUI)
{

}

void CPropertiesWnd::OnUpdateEffectCmdEnable(CCmdUI* pCmdUI)
{
	auto pProp = m_wndPropList.GetCurSel();
	bool bEnable = false;
	if (pProp)
	{
		if (!pProp->IsGroup())
		{
			pProp = pProp->GetParent();
		}
		int nPropIndex = 0;
		int nCount = m_wndPropList.GetPropertyCount();
		for (int ii = 0; ii < nCount; ++ii)
		{
			auto pCurProp = m_wndPropList.GetProperty(ii);
			if (pCurProp == pProp)
			{
				nPropIndex = ii;
				break;
			}
		}
		auto pView = CGDIPlusEffectView::GetActiveView();
		bEnable = pView->IsEffectEnabled(nPropIndex);
	}
	pCmdUI->SetCheck(bEnable);
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}
