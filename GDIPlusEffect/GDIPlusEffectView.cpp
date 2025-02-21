
// GDIPlusEffectView.cpp : implementation of the CGDIPlusEffectView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "GDIPlusEffect.h"
#endif

#include "GDIPlusEffectDoc.h"
#include "GDIPlusEffectView.h"

// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif



// CGDIPlusEffectView

IMPLEMENT_DYNCREATE(CGDIPlusEffectView, CView)

BEGIN_MESSAGE_MAP(CGDIPlusEffectView, CView)
	// Standard printing commands
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CGDIPlusEffectView::OnFilePrintPreview)
	ON_COMMAND(ID_EDIT_PASTE, &OnEditPaste)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CGDIPlusEffectView construction/destruction

CGDIPlusEffectView::CGDIPlusEffectView() noexcept
{
	// TODO: add construction code here

}

CGDIPlusEffectView::~CGDIPlusEffectView()
{
}

BOOL CGDIPlusEffectView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CGDIPlusEffectView drawing

void CGDIPlusEffectView::OnDraw(CDC* pDC)
{
	CGDIPlusEffectDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	Gdiplus::Graphics gg(pDC->GetSafeHdc());
	gg.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	CRect rect;
	GetClientRect(&rect);
	Gdiplus::Rect rc(rect.left, rect.top, rect.Width(), rect.Height());
	Gdiplus::SolidBrush br(Gdiplus::Color::White);
	gg.FillRectangle(&br, rc);

	if (!pDoc || !m_pBmp)
		return;

	auto cx = m_pBmp->GetWidth();
	auto cy = m_pBmp->GetHeight();
	auto pBitmap = m_pBmp->Clone(0, 0, cx, cy, m_pBmp->GetPixelFormat());

	for (auto& effect : m_vEffects)
	{
		if (effect->IsEnabled())
		{
			auto pEffect = effect->GetEffect();
			pBitmap->ApplyEffect(pEffect, nullptr);
		}
	}

	gg.DrawImage(pBitmap, 0, 0);

	delete pBitmap;
}




BOOL CGDIPlusEffectView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGDIPlusEffectView::OnPaint()
{
	CPaintDC dc(this);
	CMemDC dcMem(dc, this);
	auto& dcDraw = dcMem.GetDC();
	OnPrepareDC(&dcDraw);
	OnDraw(&dcDraw);
}

void CGDIPlusEffectView::OnEditPaste()
{
	if (!OpenClipboard())
	{
		AfxMessageBox(_T("Failed to open clipboard!"), MB_ICONERROR);
		return;
	}
	BOOL bOK = FALSE;
	auto hBmp = (HBITMAP)GetClipboardData(CF_BITMAP);
	if (hBmp)
	{
		//m_pBmp.reset(new Gdiplus::Bitmap(hBmp, nullptr));

		Gdiplus::Bitmap bmp(hBmp, nullptr);
		auto cx = bmp.GetWidth();
		auto cy = bmp.GetHeight();
		m_pBmp.reset(new Gdiplus::Bitmap(cx, cy, PixelFormat32bppARGB));

		Gdiplus::Graphics gg(m_pBmp.get());

		gg.DrawImage(&bmp, 0, 0);
	}
	else
	{
		AfxMessageBox(_T("Not a BMP format!"), MB_ICONERROR);
	}
	CloseClipboard();
	Invalidate();
	if (bOK)
	{
		
	}
}

// CGDIPlusEffectView printing


void CGDIPlusEffectView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CGDIPlusEffectView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGDIPlusEffectView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGDIPlusEffectView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CGDIPlusEffectView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CGDIPlusEffectView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CGDIPlusEffectView diagnostics

#ifdef _DEBUG
void CGDIPlusEffectView::AssertValid() const
{
	CView::AssertValid();
}

void CGDIPlusEffectView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGDIPlusEffectDoc* CGDIPlusEffectView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGDIPlusEffectDoc)));
	return (CGDIPlusEffectDoc*)m_pDocument;
}



#endif //_DEBUG


// CGDIPlusEffectView message handlers

void CGDIPlusEffectView::AddEffect(GDIPlusEffect* pEffect)
{
	m_vEffects.emplace_back(pEffect);
	Invalidate();
}

void CGDIPlusEffectView::ClearEffect()
{
	m_vEffects.clear();
	Invalidate();
}

bool CGDIPlusEffectView::HasEffect() const
{
	return !m_vEffects.empty();
}

bool CGDIPlusEffectView::RemoveEffect(int nIndex)
{
	if (nIndex < 0 || (size_t)nIndex >= m_vEffects.size())
		return false;
	m_vEffects.erase(m_vEffects.begin() + nIndex);
	Invalidate();
	return true;
}

bool CGDIPlusEffectView::IsEffectEnabled(int nIndex) const
{
	if (nIndex < 0 || (size_t)nIndex >= m_vEffects.size())
		return false;
	return m_vEffects[nIndex]->IsEnabled();
}

void CGDIPlusEffectView::EnableEffect(int nIndex, bool bEnable)
{
	if (nIndex < 0 || (size_t)nIndex >= m_vEffects.size())
		return;
	m_vEffects[nIndex]->Enable(bEnable);
	Invalidate();
}

CGDIPlusEffectView* CGDIPlusEffectView::GetActiveView()
{
	auto pMainFrame = (CFrameWnd*)AfxGetMainWnd();
	auto pView = (CGDIPlusEffectView*)pMainFrame->GetActiveView();
	if (!pView)
	{
		CWnd* pWnd = pMainFrame->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
		if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
			pView = (CGDIPlusEffectView*)pWnd;
	}
	return pView;
}
