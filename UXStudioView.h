
// UXStudioView.h: CUXStudioView 클래스의 인터페이스
//

#pragma once

#include "Common/directx/CSCD2Context/SCD2Context.h"
#include "Common/directx/CSCD2Image/SCD2Image.h"
#include <dwrite.h>
#pragma comment(lib, "dwrite")


class CUXStudioView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CUXStudioView() noexcept;
	DECLARE_DYNCREATE(CUXStudioView)

	CSCD2Context			m_d2dc;
	ID2D1SolidColorBrush*	m_brush = NULL;
	ID2D1SolidColorBrush*	m_br_grid = NULL;
	IDWriteFactory*			m_WriteFactory = NULL;
	IDWriteTextFormat*		m_WriteFormat = NULL;

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_UXSTUDIO_FORM };
#endif

// 특성입니다.
public:
	CUXStudioDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CUXStudioView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* /*pDC*/);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // UXStudioView.cpp의 디버그 버전
inline CUXStudioDoc* CUXStudioView::GetDocument() const
   { return reinterpret_cast<CUXStudioDoc*>(m_pDocument); }
#endif

