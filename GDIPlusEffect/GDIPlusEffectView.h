
// GDIPlusEffectView.h : interface of the CGDIPlusEffectView class
//

#pragma once

#include "GDIPlusEffectComm.h"
#include <memory>
#include <vector>

class CGDIPlusEffectDoc;

class CGDIPlusEffectView : public CView
{
protected: // create from serialization only
	CGDIPlusEffectView() noexcept;
	DECLARE_DYNCREATE(CGDIPlusEffectView)

// Attributes
public:
	CGDIPlusEffectDoc* GetDocument() const;

// Operations
public:

	void AddEffect(GDIPlusEffect* pEffect);

	void ClearEffect();
	bool HasEffect() const;

	bool RemoveEffect(int nIndex);

	bool IsEffectEnabled(int nIndex) const;
	void EnableEffect(int nIndex, bool bEnable);

	static CGDIPlusEffectView* GetActiveView();

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CGDIPlusEffectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	std::unique_ptr<Gdiplus::Bitmap> m_pBmp;
	std::vector<std::unique_ptr<GDIPlusEffect>> m_vEffects;

// Generated message map functions
protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

	afx_msg void OnEditPaste();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GDIPlusEffectView.cpp
inline CGDIPlusEffectDoc* CGDIPlusEffectView::GetDocument() const
   { return reinterpret_cast<CGDIPlusEffectDoc*>(m_pDocument); }
#endif

