
// UXStudioView.h: CUXStudioView 클래스의 인터페이스
//

#pragma once

#include "Common/CEdit/SCEdit/SCEdit.h"
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

	CSCD2Context					m_d2dc;

	ComPtr<ID2D1SolidColorBrush>	m_br_canvas;
	ComPtr<ID2D1SolidColorBrush>	m_br_grid;
	ComPtr<ID2D1SolidColorBrush>	m_br_draw;
	ComPtr<ID2D1SolidColorBrush>	m_br_align_fit;
	ComPtr<ID2D1SolidColorBrush>	m_br_order;
	ComPtr<ID2D1SolidColorBrush>	m_br_item;
	ComPtr<ID2D1SolidColorBrush>	m_br_hover;
	ComPtr<ID2D1SolidColorBrush>	m_br_selected;
	ComPtr<ID2D1SolidColorBrush>	m_br_multi_selected;
	ComPtr<ID2D1SolidColorBrush>	m_br_label;

	ComPtr<ID2D1StrokeStyle>		m_stroke_style;


	IDWriteFactory*					m_WriteFactory = NULL;
	IDWriteTextFormat*				m_WriteFormat = NULL;

	//CSize							m_sz_grid;

	bool							m_lbutton_down = false;
	CPoint							m_pt_lbutton_down = CPoint(-1, -1);
	CPoint							m_pt_cur = CPoint(-1, -1);

	//개발 단계에서 확인용으로 각 항목의 좌표값 표시 여부
	bool							m_show_element_coord = true;

	int								m_handle_index = -1;
	CRect							m_resize_handle[9];
	void							draw_resize_handle(ID2D1DeviceContext* d2dc);
	//이동 또는 크기이동 중...
	bool							m_is_resizing = false;
	void							move_or_resize_item(CPoint pt);	//마우스를 이용한 이동, 크기조정
	void							move_or_resize_item(int key);		//방향키를 이용한 이동, 크기조정

	//max rect of all selected items
	Gdiplus::RectF					m_r_selected;
	std::deque<CSCUIElement*>		get_selected_items();
	void							select_item(CSCUIElement* item);

	//선택된 모든 항목의 최대 사각형인 m_r_selected를 구한다. new_rect가 NULL이 아니면 이것까지 포함해서 구한다.
	void							get_bound_selected_rect(Gdiplus::RectF* new_rect = NULL);
	//모든 항목을 선택 또는 해제한다.
	void							select_all(bool select);
	void							delete_selected_items();

	bool							m_in_editing = false;
	CSCEdit							m_edit;
	LRESULT							on_message_CSCEdit(WPARAM wParam, LPARAM lParam);
	void							edit_end(bool valid = true);

	void							apply_changed_property(CSCUIElement* item);
	void							apply_canvas_property_changed(int canvas_cx, int canvas_cy, Gdiplus::Color cr_canvas, int grid_cx, int grid_cy, Gdiplus::Color cr_grid);

	CSCUIElement*					m_item_hover = NULL;
	CSCUIElement*					m_item_selected = NULL;
	CSCUIElement*					m_item_current = NULL;
	CSCUIElement*					m_item_copy_src = NULL;

	//move or resize시에 일치된 항목과 일치 인덱스를 기억해서 OnDraw()에서 그려줘야 한다.
	std::vector<D2D1_POINT_2F>		m_pt_align_fit;
	void							push_align_fit_info();

	CSCUIElement*					get_hover_item(CPoint pt);

	//move, resize시에 다른 항목과의 일치되는 위치로 자동 보정
	void							get_fit_others(int index, CSCUIElement* el);

	//가장 가까운 grid 좌표를 리턴한다.
	//스크롤을 하면 grid 또한 함께 스크롤되므로 pt는 이미 스크롤 오프셋이 적용된 값으로 전달되어야 한다.
	//grid로 나누고 다시 grid를 곱하는 과정에서 항상 버림만 발생하므로 문제가 있다.
	//float으로 나누고 0.5를 넘으면 올림해야 한다.
	template <class T> T get_near_grid(T src, bool horz_grid = true)
	{
		if constexpr (std::is_same_v<T, float>)
			//if (typeid(T) == typeid(CPoint))	//compile error
		{
			float* f = reinterpret_cast<float*>(&src);
			if (horz_grid)
				*f = (int)(*f / pDoc->m_sz_grid.cx) * pDoc->m_sz_grid.cx;
			else
				*f = (int)(*f / pDoc->m_sz_grid.cy) * pDoc->m_sz_grid.cy;

			return *f;
		}
		else if constexpr (std::is_same_v<T, CPoint>)
		//if (typeid(T) == typeid(CPoint))	//compile error
		{
			CPoint* pt = reinterpret_cast<CPoint*>(&src);
			pt->x = (pt->x / pDoc->m_sz_grid.cx) * pDoc->m_sz_grid.cx;
			pt->y = (pt->y / pDoc->m_sz_grid.cy) * pDoc->m_sz_grid.cy;

			return *pt;
		}
		else if constexpr (std::is_same_v<T, Gdiplus::PointF>)
		//else if (typeid(T) == typeid(Gdiplus::PointF))
		{
			//Gdiplus::PointF* pt = reinterpret_cast<Gdiplus::PointF*>&src;
			//pt->X = (int)(pt->X / pDoc->m_sz_grid.cx) * pDoc->m_sz_grid.cx;
			//pt->Y = (int)(pt->Y / pDoc->m_sz_grid.cy) * pDoc->m_sz_grid.cy;
			Gdiplus::PointF* pt = reinterpret_cast<Gdiplus::PointF*>(&src);
			pt->X = (int)(pt->X / pDoc->m_sz_grid.cx) * pDoc->m_sz_grid.cx;
			pt->Y = (int)(pt->Y / pDoc->m_sz_grid.cy) * pDoc->m_sz_grid.cy;

			return *pt;
		}
		else if constexpr (std::is_same_v<T, Gdiplus::RectF>)
		//else if (typeid(T) == typeid(Gdiplus::RectF))
		{
			Gdiplus::RectF* r = reinterpret_cast<Gdiplus::RectF*>(&src);
			//float div = r->X / pDoc->m_sz_grid.cx;
			//div = ROUND(div, 0) * pDoc->m_sz_grid.cx;
			r->X = ROUND(r->X / pDoc->m_sz_grid.cx, 0) * pDoc->m_sz_grid.cx;
			r->Y = ROUND(r->Y / pDoc->m_sz_grid.cy, 0) * pDoc->m_sz_grid.cy;
			//r->X = int(r->X / pDoc->m_sz_grid.cx) * pDoc->m_sz_grid.cx;
			//r->Y = int(r->Y / pDoc->m_sz_grid.cy) * pDoc->m_sz_grid.cy;

			//TRACE(_T("src->X = %.2f, div = %.2f, r->X = %.2f\n"), src.X, div, r->X);

			return *r;
		}

		return src;
	}
	//Gdiplus::PointF					get_near_grid(Gdiplus::PointF pt);


	//
	bool							m_is_context_menu_displaying = false;

	//spacebar를 누른 채 화면을 드래그하면 스크롤된다.
	bool							m_spacebar_down = false;

	//point, rect 등을 현재 스크롤바 위치만큼 보정한다.
	//invert = false이면 스크롤 오프셋만큼 더하는 것이고 true이면 적용했던 오프셋을 다시 빼준다.
	template <class T> T adjust_scroll_offset(T& value, bool near_grid = true, bool invert = false)
	{
		CPoint sp((invert ? -1 : 1) * GetScrollPos(SB_HORZ), (invert ? -1 : 1) * GetScrollPos(SB_VERT));

		//((CPoint)value).Offset(sp);	//이렇게 하면 value가 변경되지 않는다.
		if constexpr (std::is_same_v<T, CPoint>)
		{
			CPoint pt = value;
			pt.Offset(sp);
			if (near_grid)
				pt = get_near_grid(pt);
			value = pt;
			return value;
		}
		else if constexpr (std::is_same_v<T, D2D1_POINT_2F>)
		{
			D2D1_POINT_2F pt = value;
			pt.x += sp.x;
			pt.y += sp.y;

			if (near_grid)
				pt = get_near_grid(pt);

			value = pt;
			return value;
		}
		else if constexpr (std::is_same_v<T, Gdiplus::Rect>)
		{
			Gdiplus::Rect r = value;
			r.Offset(sp.x, sp.y);
			if (near_grid)
				r = get_near_grid(r);
			value = r;
			return value;
		}
		else if constexpr (std::is_same_v<T, Gdiplus::RectF>)
		{
			Gdiplus::RectF r = value;
			r.Offset(sp.x, sp.y);
			if (near_grid)
				r = get_near_grid(r);

			value = r;
			return value;
		}

		return value;
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
	CUXStudioDoc*	pDoc = NULL;

	enum TIMER_ID
	{
		timer_doc_modified,
	};

	//항목이 변경되면 undo에 기록하고 속성창에도 이를 반영한다.
	//단 mouse 액션은 너무 빈번하게 발생되므로 timer를 적용하여 반영한다.
	void			set_property(bool doc_modified = true);

	std::vector<std::deque<CSCUIElement*>*> m_undo;
	std::vector<std::deque<CSCUIElement*>*>::iterator m_undo_iter;
	void			push_undo();
	void			undo();
	//after = true이면 현재 선택된 항목의 뒤에 추가한다.
	void			insert(CSCUIElement* new_item, bool after = true);

	//현재 항목의 index 리턴.
	int				get_index(CSCUIElement* cur);

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
	afx_msg void OnMenuViewSendToTop();
	afx_msg void OnMenuViewSendToFore();
	afx_msg void OnMenuViewSendToBack();
	afx_msg void OnMenuViewSendToBottom();
	afx_msg void OnMenuViewCopy();
	afx_msg void OnMenuViewPaste();
	afx_msg void OnMenuViewLabelEdit();
	afx_msg void OnMenuViewDelete();
	afx_msg void OnEditUndo();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnMenuViewShowCoord();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

#ifndef _DEBUG  // UXStudioView.cpp의 디버그 버전
inline CUXStudioDoc* CUXStudioView::GetDocument() const
   { return reinterpret_cast<CUXStudioDoc*>(m_pDocument); }
#endif

