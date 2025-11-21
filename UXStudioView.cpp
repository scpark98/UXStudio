
// UXStudioView.cpp: CUXStudioView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "UXStudio.h"
#endif

#include "MainFrm.h"
#include "UXStudioDoc.h"
#include "UXStudioView.h"

#include "Common/Functions.h"
#include "Common/MemoryDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUXStudioView

IMPLEMENT_DYNCREATE(CUXStudioView, CFormView)

BEGIN_MESSAGE_MAP(CUXStudioView, CFormView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CUXStudioView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
//	ON_WM_ACTIVATE()
//	ON_WM_ACTIVATEAPP()
ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// CUXStudioView 생성/소멸

CUXStudioView::CUXStudioView() noexcept
	: CFormView(IDD_UXSTUDIO_FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CUXStudioView::~CUXStudioView()
{
	SAFE_RELEASE(m_WriteFactory);
	SAFE_RELEASE(m_WriteFormat);

	for (int i = 0; i < pDoc->m_data.size(); i++)
	{
		CSCUIElement* el = pDoc->m_data[i];
		delete el;
	}
}

void CUXStudioView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CUXStudioView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	return CFormView::PreCreateWindow(cs);
}

void CUXStudioView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	ResizeParentToFit();

	pDoc = GetDocument();

	m_d2dc.init(m_hWnd);

	HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_WriteFactory));
	if (SUCCEEDED(hr))
	{
		m_WriteFactory->CreateTextFormat(_T("맑은 고딕"), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, _T("ko-kr"), &m_WriteFormat);
		m_WriteFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		m_WriteFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}

	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), m_br_draw.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(.5f, .5f, .5f, 0.6f), m_br_grid.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), m_br_item.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::RoyalBlue), m_br_hover.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::RoyalBlue), m_br_selected.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue), m_br_multi_selected.GetAddressOf());

	m_sz_grid.cx = AfxGetApp()->GetProfileInt(_T("setting"), _T("grid cx"), 8);
	m_sz_grid.cy = AfxGetApp()->GetProfileInt(_T("setting"), _T("grid cy"), 8);

	SetScrollSizes(MM_TEXT, CSize(1920, 1080));
}

// CUXStudioView 인쇄


void CUXStudioView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CUXStudioView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CUXStudioView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CUXStudioView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CUXStudioView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: 여기에 사용자 지정 인쇄 코드를 추가합니다.
}

void CUXStudioView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CUXStudioView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CUXStudioView 진단

#ifdef _DEBUG
void CUXStudioView::AssertValid() const
{
	CFormView::AssertValid();
}

void CUXStudioView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CUXStudioDoc* CUXStudioView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUXStudioDoc)));
	return (CUXStudioDoc*)m_pDocument;
}
#endif //_DEBUG


// CUXStudioView 메시지 처리기

void CUXStudioView::OnDraw(CDC* pDC)
{
	CRect rc;

	GetClientRect(rc);

	ID2D1DeviceContext* d2dc = m_d2dc.get_d2dc();
	D2D1_SIZE_F sz_dc = m_d2dc.get_size();

	int hs = GetScrollPos(SB_HORZ);
	int vs = GetScrollPos(SB_VERT);

	//TRACE(_T("size = %.0f x %.0f, vs = %d, hs = %d\n"), sz_dc.width, sz_dc.height, vs, hs);

	d2dc->BeginDraw();
	d2dc->SetTransform(D2D1::Matrix3x2F::Identity());

	//으로 칠한 후
	d2dc->Clear(D2D1::ColorF(0.99f, 0.99f, 0.99f));

	int ix = m_sz_grid.cx;
	int iy = m_sz_grid.cy;
	int x, y;

	for (x = ix; x < 1920; x += ix)
	{
		for (y = iy; y < 1080; y += iy)
		{
			d2dc->FillRectangle(D2D1::RectF(x - hs, y - vs, x - hs  + 1, y - vs + 1), m_br_grid.Get());
		}
	}


	//도형을 그리는 중에는 스크롤 보정된 실제 좌표이므로 보정을 하지 않은 좌표를 스크린에 그려줘야 한다.
	if (m_lbutton_down && !m_spacebar_down)
	{
		CPoint pt_lbutton_down = m_pt_lbutton_down;
		CPoint pt_cur = m_pt_cur;
		TRACE(_T("old = %d, %d\n"), pt_cur.x, pt_cur.y);
		adjust_scroll_offset(pt_lbutton_down, true);
		adjust_scroll_offset(pt_cur, true);
		TRACE(_T("new = %d, %d\n"), pt_cur.x, pt_cur.y);
		d2dc->DrawRectangle(D2D1::RectF(pt_lbutton_down.x, pt_lbutton_down.y, pt_cur.x, pt_cur.y), m_br_draw.Get());
	}

	for (int i = 0; i < pDoc->m_data.size(); i++)
	{
		CSCUIElement* el = pDoc->m_data[i];
		Gdiplus::RectF r = el->m_r;

		r.Offset(-hs, -vs);

		D2D1_RECT_F rf = { r.X, r.Y, r.GetRight(), r.GetBottom() };

		draw_rect(d2dc, rf, el->m_cr_stroke, el->m_cr_fill, (el == m_item_hover && !el->m_selected) ? 2.0f : 1.0f);
		CString text;
		text.Format(_T("%d. %s"), i, get_rect_info_str(gpRectF_to_CRect(el->m_r), 0));
		d2dc->DrawText(text, text.GetLength(), m_WriteFormat, gpRectF_to_d2Rect(el->m_r), m_br_selected.Get());
		/*
		if (el->m_cr_fill.GetValue() != Gdiplus::Color::Transparent)
		{
			m_br_item->SetColor(get_d2color(el->m_cr_fill));
			d2dc->FillRectangle(rf, m_br_item.Get());
		}

		if (el == m_item_hover)// || m_handle_index >= 0)
		{
			d2dc->DrawRectangle(rf, m_br_hover.Get(), 2.0f);
		}
		else if (el->m_cr_stroke.GetValue() != Gdiplus::Color::Transparent)
		{
			m_br_item->SetColor(get_d2color(el->m_cr_stroke));
			d2dc->DrawRectangle(rf, m_br_item.Get());
		}
		*/
	}

	if (m_item_selected)
	{
		Gdiplus::RectF r_selected = m_item_selected->m_r;
		r_selected.Offset(-hs, -vs);
		D2D1_RECT_F rf_selected = { r_selected.X, r_selected.Y, r_selected.GetRight(), r_selected.GetBottom() };
		d2dc->DrawRectangle(rf_selected, m_br_multi_selected.Get(), 2.0f);
		get_resizable_handle(r_selected, m_resize_handle);
		draw_resize_handle(d2dc);
	}

	HRESULT hr = d2dc->EndDraw();

	if (SUCCEEDED(hr))
		hr = m_d2dc.get_swapchain()->Present(0, 0);

	//for test dc drawing
	//if (m_lbutton_down)
	//	draw_line(pDC, m_pt_lbutton_down, m_pt_cur, red, 1, PS_SOLID, R2_XORPEN);
	//else
	//	draw_line(pDC, m_pt_lbutton_down, m_pt_cur, red, 1, PS_SOLID);

	//d2gdi->ReleaseDC(rc);
}

void CUXStudioView::draw_resize_handle(ID2D1DeviceContext* d2dc)
{
	//0번은 inside이므로 별도로 그려주는 것이 없으므로 1번부터 그린다.
	//요즘은 left, top, right, bottom에는 handle을 그리지 않고 resize커서로만 표시하는게 깔끔하다.
	for (int i = 1; i < RECT_RESIZE_HANDLE_COUNT; i++)
	{
		CRect r = m_resize_handle[i];
		draw_rect(d2dc, CRect_to_gpRectF(r), Gdiplus::Color::RoyalBlue, Gdiplus::Color::White);
	}
}

void CUXStudioView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_d2dc.get_d2dc())
		return;

	m_d2dc.on_size_changed(cx, cy);
}

BOOL CUXStudioView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return FALSE;
	return CFormView::OnEraseBkgnd(pDC);
}

void CUXStudioView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	Invalidate();
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CUXStudioView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	Invalidate();
	CFormView::OnVScroll(nSBCode, nPos, pScrollBar);
}

//가장 가까운 grid 좌표를 리턴한다.
//스크롤을 하면 grid 또한 함께 스크롤되므로 pt는 이미 스크롤 오프셋이 적용된 값으로 전달되어야 한다.
CPoint CUXStudioView::get_near_grid(CPoint pt)
{
	CPoint res = pt;
	res.x = (res.x / m_sz_grid.cx) * m_sz_grid.cx;
	res.y = (res.y / m_sz_grid.cy) * m_sz_grid.cy;

	return res;
}

void CUXStudioView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	//hover인 항목을 클릭하면 편집모드로 전환된다.
	CPoint pt = point;
	//pt.Offset(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
	adjust_scroll_offset(pt);

	//선택된 항목들을 이동, 크기를 변경 모드 시작
	//if (!m_r_selected.IsEmptyArea() && m_handle_index >= corner_inside)
	if (m_item_selected && m_handle_index >= corner_inside)
	{
		m_is_resizing = true;
		trace(m_is_resizing);
		m_pt_lbutton_down = point;
		//adjust_scroll_offset(m_pt_lbutton_down);
		return;
	}
	else
	{
		m_is_resizing = false;
		trace(m_is_resizing);
	}

	if (m_item_hover)
	{
		if (m_item_hover->pt_in_rect(pt.x, pt.y))
		{
			if (!IsShiftPressed())
				select_all(false);

			m_item_hover->m_selected = true;
			//get_bound_selected_rect();
			//get_resizable_handle(m_r_selected, m_resize_handle);
			m_item_selected = m_item_hover;
			//get_resizable_handle(m_item_selected->m_r, m_resize_handle);
		}
		else
		{
			m_item_selected = NULL;
			m_r_selected.Width = 0;
		}

		Invalidate();
		return;
	}
	else
	{
		m_r_selected.Width = 0;
	}

	m_lbutton_down = true;
	m_pt_lbutton_down = get_near_grid(pt);

	CFormView::OnLButtonDown(nFlags, point);
}

void CUXStudioView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPoint pt = point;
	pt.Offset(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
	((CMainFrame*)(AfxGetApp()->m_pMainWnd))->set_cursor_info(pt);

	//trace(m_lbutton_down);
	//trace(m_spacebar_down);

	if (m_is_resizing)
	{
		resize_items(pt);
	}
	else if (m_lbutton_down)
	{
		if (m_spacebar_down)
		{
			SetScrollPos(SB_HORZ, m_pt_lbutton_down.x - point.x);
			SetScrollPos(SB_VERT, m_pt_lbutton_down.y - point.y);
		}
		else
		{
			m_pt_cur = get_near_grid(pt);
		}
		Invalidate();
	}
	else
	{
		m_item_hover = get_hover_item(pt);
		TRACE(_T("hover = %p\n"), m_item_hover);
		Invalidate();
	}

	CFormView::OnMouseMove(nFlags, point);
}

void CUXStudioView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	ReleaseCapture();

	if (m_is_resizing)
	{
		m_is_resizing = false;
		m_pt_lbutton_down = CPoint(-1, -1);
		//normalize_rect(m_item_selected->m_r);
	}

	if (m_lbutton_down)
	{
		m_lbutton_down = false;

		if (m_spacebar_down)
		{
			m_spacebar_down = false;
			return;
		}
		else
		{
			m_pt_cur = point;
			adjust_scroll_offset(m_pt_cur);
			m_pt_cur = get_near_grid(m_pt_cur);
			Gdiplus::RectF r(m_pt_lbutton_down.x, m_pt_lbutton_down.y, m_pt_cur.x - m_pt_lbutton_down.x, m_pt_cur.y - m_pt_lbutton_down.y);
			normalize_rect(r);

			if (r.Width < 20 && r.Height < 20)
				return;

			pDoc->m_data.push_back(new CSCUIElement(r));
		}
		Invalidate();
	}

	CFormView::OnLButtonUp(nFlags, point);
}

void CUXStudioView::resize_items(CPoint pt)
{
	CSCUIElement* item = NULL;

	auto res = std::find_if(pDoc->m_data.begin(), pDoc->m_data.end(),
		[&](const auto& el)
		{
			if (el->m_selected)
			{
				return el;
			}
		});

	if (res != pDoc->m_data.end())
		item = *res;

	if (!item)
		return;

	switch (m_handle_index)
	{
		case corner_inside:
			item->m_r.X += (pt.x - m_pt_lbutton_down.x);
			item->m_r.Y += (pt.y - m_pt_lbutton_down.y);
			//adjust_rect_range(m_screen_roi, CRect_to_gpRectF(m_r_display));
			m_pt_lbutton_down = pt;
			break;
		case corner_left:
			set_left(item->m_r, pt.x);
			break;
		case corner_right:
			item->m_r.Width = pt.x - item->m_r.X;
			break;
		case corner_top:
			set_top(item->m_r, pt.y);
			break;
		case corner_bottom:
			item->m_r.Height = pt.y - item->m_r.Y;
			break;
		case corner_topleft:
			set_top(item->m_r, pt.y);
			set_left(item->m_r, pt.x);
			break;
		case corner_topright:
			set_top(item->m_r, pt.y);
			item->m_r.Width = pt.x - item->m_r.X;
			break;
		case corner_bottomleft:
			item->m_r.Height = pt.y - item->m_r.Y;
			set_left(item->m_r, pt.x);
			break;
		case corner_bottomright:
			item->m_r.Height = pt.y - item->m_r.Y;
			item->m_r.Width = pt.x - item->m_r.X;
			break;
	}

	Invalidate();
}

CSCUIElement* CUXStudioView::get_hover_item(CPoint pt)
{
	m_item_hover = NULL;
	
	//기본적으로 rect안에 커서가 들어오면 hover로 인식하지만
	//rect안에 또 다른 rect가 있을 경우, 순서가 나중인 rect는 hover로 판정될 수 없다.
	//따라서 테두리 위에 커서가 위치하는 경우 먼저 hover 판정을 해야 하고
	//그러한 rect가 없다면 커서가 위치한 rect를 hover 판정해야 한다.
	auto res = std::find_if(pDoc->m_data.begin(), pDoc->m_data.end(),
		[&](const auto& el)
		{
			Gdiplus::RectF r = el->m_r;
			if (pt_in_rect_border(r, pt, 2))
			{
				return el;
			}
		});

	if (res == pDoc->m_data.end())
	{
		res = std::find_if(pDoc->m_data.begin(), pDoc->m_data.end(),
			[&](const auto& el)
			{
				Gdiplus::RectF r = el->m_r;
				r.Inflate(3, 3);
				if (pt_in_rect(r, pt))
				{
					return el;
				}
			});
	}

	if (res != pDoc->m_data.end())
		m_item_hover = *res;

	return m_item_hover;
}

BOOL CUXStudioView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
			case VK_SPACE:
				m_spacebar_down = true;
				break;
			case VK_DELETE:
				delete_selected_items();
				return true;
		}
	}
	else if (pMsg->message == WM_KEYUP)
	{
		switch (pMsg->wParam)
		{
			case VK_SPACE:
				m_spacebar_down = false;
				break;
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}

//선택된 모든 항목의 최대 사각형인 m_r_selected를 구한다. new_rect가 NULL이 아니면 이것까지 포함해서 구한다.
void CUXStudioView::get_bound_selected_rect(Gdiplus::RectF* new_rect)
{
	m_r_selected = Gdiplus::RectF();

	for (auto el : pDoc->m_data)
	{
		if (el->m_selected)
		{
			if (m_r_selected.IsEmptyArea())
				m_r_selected = el->m_r;
			else
				m_r_selected.Union(m_r_selected, m_r_selected, el->m_r);
		}
	}

	//adjust_scroll_offset(m_r_selected, true);
}

//모든 항목을 선택 또는 해제한다.
void CUXStudioView::select_all(bool select)
{
	for (auto el : pDoc->m_data)
		el->m_selected = select;

	get_bound_selected_rect();

	Invalidate();
}

void CUXStudioView::delete_selected_items()
{
	for (int i = pDoc->m_data.size() - 1; i >= 0; i--)
	{
		if (pDoc->m_data[i]->m_selected)
		{
			delete pDoc->m_data[i];
			pDoc->m_data.erase(pDoc->m_data.begin() + i);
		}
	}

	Invalidate();
}

BOOL CUXStudioView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPoint pt;
	CRect rc;

	GetCursorPos(&pt);
	ScreenToClient(&pt);

	m_handle_index = -1;

	for (int i = 0; i < RECT_RESIZE_HANDLE_COUNT; i++)
	{
		if (m_resize_handle[i].PtInRect(pt))
		{
			m_handle_index = i;
			break;
		}
	}

	//trace(m_handle_index);

	if (m_handle_index != -1)
	{
		//마우스 커서 모양을 변경한다.
		switch (m_handle_index)
		{
			case corner_inside :
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
				break;
			case corner_left :
			case corner_right :
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
				break;
			case corner_top :
			case corner_bottom :
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
				break;
			case corner_topleft :
			case corner_bottomright :
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE));
				break;
			case corner_topright :
			case corner_bottomleft :
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENESW));
				break;
		}

		return TRUE;
	}
	else
	{
		//SetCursor(m_cursor_cross);
		//return TRUE;
	}

	return CFormView::OnSetCursor(pWnd, nHitTest, message);
}
