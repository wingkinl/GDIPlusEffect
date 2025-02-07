
#pragma once

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CPropertiesWnd : public CDockablePane
{
// Construction
public:
	CPropertiesWnd() noexcept;

	void AdjustLayout();

// Attributes
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

protected:
	CFont m_fntPropList;
	CPropertiesToolBar m_wndToolBar;
	CMFCPropertyGridCtrl m_wndPropList;

// Implementation
public:
	virtual ~CPropertiesWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAddEffect();
	afx_msg void OnUpdateAddEffect(CCmdUI* pCmdUI);

	afx_msg void OnClearEffect();
	afx_msg void OnUpdateClearEffect(CCmdUI* pCmdUI);

	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	afx_msg void OnUpdateEffectType(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEffectCmdRemove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEffectCmdEnable(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();
};

