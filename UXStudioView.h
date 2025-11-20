
// UXStudioView.h: CUXStudioView 클래스의 인터페이스
//

#pragma once

#include "Common/directx/CSCD2Context/SCD2Context.h"
#include "Common/directx/CSCD2Image/SCD2Image.h"
#include "Common/data_types/CSCUIElement/SCUIElement.h"
#include <dwrite.h>
#pragma comment(lib, "dwrite")


class CUXStudioView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CUXStudioView() noexcept;
	DECLARE_DYNCREATE(CUXStudioView)

	CSCD2Context			m_d2dc;

	ComPtr<ID2D1SolidColorBrush>	m_br_draw;
	ComPtr<ID2D1SolidColorBrush>	m_br_grid;
	ComPtr<ID2D1SolidColorBrush>	m_br_item;
	ComPtr<ID2D1SolidColorBrush>	m_br_hover;
	ComPtr<ID2D1SolidColorBrush>	m_br_selected;

	IDWriteFactory*					m_WriteFactory = NULL;
	IDWriteTextFormat*				m_WriteFormat = NULL;

	CSize							m_sz_grid;

	bool							m_lbutton_down = false;
	CPoint							m_pt_lbutton_down;
	CPoint							m_pt_cur;

	int								m_handle_index = -1;
	CRect							m_resize_handle[9];
	void							draw_resize_handle(ID2D1DeviceContext* d2dc);
	bool							m_is_resizing = false;

	//max rect of all selected items
	Gdiplus::RectF					m_r_selected;
	//선택된 모든 항목의 최대 사각형인 m_r_selected를 구한다. new_rect가 NULL이 아니면 이것까지 포함해서 구한다.
	void							get_bound_selected_rect(Gdiplus::RectF* new_rect = NULL);
	//m_r_selected를 clear하고 모든 항목의 선택 플래그도 리셋시킨다.
	void							set_selected_flag(bool selected);
	void							delete_selected_items();

	CSCUIElement*					m_item_hover = NULL;
	//std::vector<CSCUIElement*>		m_item_selected;
	CSCUIElement*					m_item_current = NULL;

	CSCUIElement*					get_hover_item(CPoint pt);
	CPoint							get_near_grid(CPoint pt);

//load, save

	//spacebar를 누른 채 화면을 드래그하면 스크롤된다.
	bool							m_spacebar_down = false;

	//point, rect 등을 현재 스크롤바 위치만큼 보정한다.
	template <class T> void	offset_scroll(T& value)
	{
		CPoint sp(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));

		if constexpr (std::is_same_v<T, CPoint>)
			((CPoint)value).Offset(sp);
		else if constexpr (std::is_same_v<T, Gdiplus::Rect>)
			((CPoint)value).Offset(sp);
	}

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
	CUXStudioDoc* pDoc = NULL;

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
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
//	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
//	virtual void OnActivateFrame(UINT nState, CFrameWnd* pDeactivateFrame);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};

#ifndef _DEBUG  // UXStudioView.cpp의 디버그 버전
inline CUXStudioDoc* CUXStudioView::GetDocument() const
   { return reinterpret_cast<CUXStudioDoc*>(m_pDocument); }
#endif

