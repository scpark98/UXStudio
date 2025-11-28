
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
ON_COMMAND(ID_MENU_VIEW_SEND_TO_TOP, &CUXStudioView::OnMenuViewSendToTop)
ON_COMMAND(ID_MENU_VIEW_SEND_TO_FORE, &CUXStudioView::OnMenuViewSendToFore)
ON_COMMAND(ID_MENU_VIEW_SEND_TO_BACK, &CUXStudioView::OnMenuViewSendToBack)
ON_COMMAND(ID_MENU_VIEW_SEND_TO_BOTTOM, &CUXStudioView::OnMenuViewSendToBottom)
ON_COMMAND(ID_MENU_VIEW_COPY, &CUXStudioView::OnMenuViewCopy)
ON_COMMAND(ID_MENU_VIEW_PASTE, &CUXStudioView::OnMenuViewPaste)
ON_COMMAND(ID_MENU_VIEW_LABEL_EDIT, &CUXStudioView::OnMenuViewLabelEdit)
ON_REGISTERED_MESSAGE(Message_CSCEdit, &CUXStudioView::on_message_CSCEdit)
ON_COMMAND(ID_MENU_VIEW_DELETE, &CUXStudioView::OnMenuViewDelete)
ON_COMMAND(ID_EDIT_UNDO, &CUXStudioView::OnEditUndo)
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

	for (int i = 0; i < m_undo.size(); i++)
	{
		std::deque<CSCUIElement*>* data = m_undo[i];

		for (int j = 0; j < data->size(); j++)
			delete data->at(j);

		delete data;
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
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), m_br_label.GetAddressOf());

	SetScrollSizes(MM_TEXT, pDoc->m_sz_canvas);

	((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_propertyDlg.set_canvas_property(1920, 1080, pDoc->m_sz_grid.cx, pDoc->m_sz_grid.cy);
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
	//CSCUIElement* hover_item = get_hover_item(point);
	//선택되지 않은 항목에서 우클릭하면 선택상태로 만들어준 후 팝업메뉴를 표시한다.
	if (m_item_hover != m_item_selected)
	{
		select_all(false);
		m_item_selected = m_item_hover;
		m_item_selected->m_selected = true;
		Invalidate();
		Wait(10);
	}

	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CUXStudioView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
//#ifndef SHARED_HANDLERS
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_VIEW_CONTEXT, point.x, point.y, this, TRUE);
//#endif
	CMenu menu;
	CMenu* pMenu = NULL;

	menu.LoadMenu(IDR_MENU_VIEW_CONTEXT);
	pMenu = menu.GetSubMenu(0);

	m_is_context_menu_displaying = true;
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	m_is_context_menu_displaying = false;
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

	int ix = pDoc->m_sz_grid.cx;
	int iy = pDoc->m_sz_grid.cy;
	int x, y;

	for (x = ix; x < 1920; x += ix)
	{
		for (y = iy; y < 1080; y += iy)
		{
			d2dc->FillRectangle(D2D1::RectF(x - hs, y - vs, x - hs  + 1, y - vs + 1), m_br_grid.Get());
		}
	}


	//도형을 그리는 중에는 스크롤 보정된 실제 좌표이므로 그릴때는 역보정하여 화면에 그려줘야 한다.
	if (m_lbutton_down && !m_spacebar_down && m_pt_lbutton_down.x >= 0 && m_pt_lbutton_down.y >= 0)
	{
		CPoint pt_lbutton_down = m_pt_lbutton_down;
		adjust_scroll_offset(pt_lbutton_down, true, true);

		CPoint pt_cur = m_pt_cur;
		adjust_scroll_offset(pt_cur, true, true);

		if (pt_lbutton_down != pt_cur)
			d2dc->DrawRectangle(D2D1::RectF(pt_lbutton_down.x, pt_lbutton_down.y, pt_cur.x, pt_cur.y), m_br_draw.Get());
	}

	//역순으로 그려줘야 z-order가 유효하다.
	for (int i = pDoc->m_data.size() - 1; i >= 0; i--)
	{
		CSCUIElement* el = pDoc->m_data[i];
		Gdiplus::RectF r = el->m_r;

		r.Offset(-hs, -vs);

		D2D1_RECT_F rf = { r.X, r.Y, r.GetRight(), r.GetBottom() };

		draw_rect(d2dc, rf, el->m_cr_stroke, el->m_cr_fill, el->m_stroke_thickness, el->m_round[0]);

		//hover된 항목 highlight
		if (el == m_item_hover && !el->m_selected)
			draw_rect(d2dc, rf, Gdiplus::Color::RoyalBlue, Gdiplus::Color::Transparent, 2.0f, el->m_round[0]);

		m_br_label->SetColor(get_d2color(pDoc->m_data[i]->m_cr_text));

		m_WriteFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_WriteFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		d2dc->DrawText(pDoc->m_data[i]->m_text, pDoc->m_data[i]->m_text.GetLength(), m_WriteFormat, rf, m_br_label.Get());

		//좌표 확인용 코드
		if (true)
		{
			CString text;
			text.Format(_T("%d. %s"), i, get_rect_info_str(gpRectF_to_CRect(el->m_r), 0));

			rf.top -= 20;
			rf.bottom = rf.bottom + 20;

			if (rf.left < 0)
				rf.left = 0;
			if (rf.top < 0)
				rf.top = 0;

			rf.bottom = rf.top + 20;

			d2dc->DrawText(text, text.GetLength(), m_WriteFormat, rf, m_br_grid.Get());
		}
	}

	if (m_item_selected)
	{
		Gdiplus::RectF r_selected = m_item_selected->m_r;
		r_selected.Offset(-hs, -vs);
		draw_rect(d2dc, r_selected, Gdiplus::Color::RoyalBlue, Gdiplus::Color::Transparent, 1.0f, m_item_selected->m_round[0]);
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
/*
CPoint CUXStudioView::get_near_grid(CPoint pt)
{
	CPoint res = pt;
	res.x = (res.x / m_sz_grid.cx) * m_sz_grid.cx;
	res.y = (res.y / m_sz_grid.cy) * m_sz_grid.cy;

	return res;
}
*/
void CUXStudioView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_in_editing)
		edit_end();

	SetCapture();

	//hover인 항목을 클릭하면 편집모드로 전환된다.
	CPoint pt = point;
	adjust_scroll_offset(pt, true);

	//선택된 항목들을 이동, 크기를 변경 모드 시작
	//if (!m_r_selected.IsEmptyArea() && m_handle_index >= corner_inside)
	//trace(m_item_selected);
	//trace(m_handle_index);
	if (m_item_selected && m_handle_index >= corner_inside)
	{
		m_is_resizing = true;
		trace(m_is_resizing);
		m_pt_lbutton_down = get_near_grid(pt);
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

			//선택되지 않았던 항목을 클릭하면 바로 이동 모드로 전환되도록 아래 코드 추가.
			m_is_resizing = true;
			m_handle_index = corner_inside;
			m_pt_lbutton_down = get_near_grid(pt);
		}
		else
		{
			m_item_hover->m_selected = false;
			m_item_selected = NULL;
			m_r_selected.Width = 0;
		}

		Invalidate();

		((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_propertyDlg.set_property(m_item_selected);

		return;
	}
	else
	{
		select_all(false);
		m_item_selected = NULL;
		m_r_selected.Width = 0;
		((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_propertyDlg.set_property(m_item_selected);
	}

	m_lbutton_down = true;
	m_pt_cur = m_pt_lbutton_down = pt;// get_near_grid(pt);

	CFormView::OnLButtonDown(nFlags, point);
}

void CUXStudioView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPoint pt = point;
	adjust_scroll_offset(pt, true);
	//pt = get_near_grid(pt);
	((CMainFrame*)(AfxGetApp()->m_pMainWnd))->set_cursor_info(pt);

	//trace(m_lbutton_down);
	trace(m_is_resizing);

	if (m_is_resizing)
	{
		trace(m_handle_index);
		move_resize_item(pt);
		((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_propertyDlg.set_property(m_item_selected);
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
			m_pt_cur = pt;
		}
		Invalidate();
	}
	else
	{
		m_item_hover = get_hover_item(pt);
		TRACE(_T("hover = %p\n"), m_item_hover);

		CRect rc;
		GetClientRect(rc);
		m_d2dc.on_size_changed(rc.Width(), rc.Height());
		Invalidate();
	}

	CFormView::OnMouseMove(nFlags, point);
}

void CUXStudioView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	ReleaseCapture();

	CPoint pt = point;
	adjust_scroll_offset(pt, true);
	//pt = get_near_grid(pt);

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
			m_pt_cur = pt;
			//adjust_scroll_offset(m_pt_cur);
			//m_pt_cur = get_near_grid(m_pt_cur);
			Gdiplus::RectF r(m_pt_lbutton_down.x, m_pt_lbutton_down.y, m_pt_cur.x - m_pt_lbutton_down.x, m_pt_cur.y - m_pt_lbutton_down.y);
			normalize_rect(r);

			m_pt_lbutton_down = CPoint(-1, -1);

			if (r.Width < 20 && r.Height < 20)
				return;

			//가장 최종으로 그려진 항목이 가장 첫번째가 되어야 한다.
			pDoc->m_data.push_front(new CSCUIElement(r));
		}
		Invalidate();
	}

	CFormView::OnLButtonUp(nFlags, point);
}

void CUXStudioView::move_resize_item(CPoint pt)
{
	if (!m_item_selected)
		return;

	CSCUIElement* item = m_item_selected;
	/*
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
	*/

	//pt = get_near_grid(pt);
	//adjust_sc

	switch (m_handle_index)
	{
		case corner_inside:
			item->m_r.X += pt.x - m_pt_lbutton_down.x;
			item->m_r.Y += pt.y - m_pt_lbutton_down.y;
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

	item->m_r = get_near_grid(item->m_r);
	Invalidate();
}

//방향키를 이용한 이동, 크기조정
void CUXStudioView::move_resize_item(int key)
{
	if (m_in_editing)
		return;

	//방향키만 누르면 grid 만큼 씩 이동,
	//shift를 누르면 1씩 이동,
	//ctrl을 누르면 grid * 2씩 이동,
	//shift+ctrl이면 grid * 4씩 이동한다.
	switch (key)
	{
		case VK_LEFT:
			m_item_selected->m_r.X -= pDoc->m_sz_grid.cx;
			break;
		case VK_RIGHT:
			m_item_selected->m_r.X += pDoc->m_sz_grid.cx;
			break;
		case VK_UP:
			m_item_selected->m_r.Y -= pDoc->m_sz_grid.cy;
			break;
		case VK_DOWN:
			m_item_selected->m_r.Y += pDoc->m_sz_grid.cy;
			break;
	}

	m_item_selected->m_r = get_near_grid(m_item_selected->m_r);
	Invalidate();
	push_undo();
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
			case VK_RETURN:
				//if (GetFocus() == &m_edit)
				//{
				//	edit_end();
				//	return TRUE;
				//}
				break;
			case VK_ESCAPE:
				if (m_in_editing)
				{
					edit_end(false);
					return TRUE;
				}
				//개발단계에서 esc키를 누르면 프로그램을 바로 종료하기 위해 return false;로 하여
				//CMainFrame에서 이 키를 처리하도록 전달함.
				//종료를 막으르면 이를 break;로 변경할 것.
				return false;
			case VK_SPACE:
				m_spacebar_down = true;
				break;
			case VK_DELETE:
				OnMenuViewDelete();
				return true;
			case VK_F2:
				OnMenuViewLabelEdit();
				return true;
			case 'C':
				if (IsCtrlPressed())
				{
					OnMenuViewCopy();
					return true;
				}
				break;
			case 'V':
				if (IsCtrlPressed())
				{
					OnMenuViewPaste();
					return true;
				}
				break;
			case VK_LEFT:
			case VK_RIGHT:
			case VK_UP:
			case VK_DOWN:
				move_resize_item(pMsg->wParam);
				break;
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

	m_item_selected = NULL;
	Invalidate();
}

BOOL CUXStudioView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//팝업메뉴가 표시중이거나 편집중이거나 선택항목이 없다면 커서의 변경은 없다.
	if (m_is_context_menu_displaying || !m_item_selected || m_in_editing)
		return CFormView::OnSetCursor(pWnd, nHitTest, message);

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

void CUXStudioView::OnMenuViewSendToBottom()
{
	std::deque<CSCUIElement*> selected = get_selected_items();
	if (selected.size() != 1)
		return;

	//선택된 항목을 맨 뒤에 넣어주고 0 ~ n-1 중에 있던 해당 아이템을 deque에서 제거한다.
	pDoc->m_data.push_back(selected[0]);
	m_item_selected = pDoc->m_data.back();
	auto res = std::find(pDoc->m_data.begin(), pDoc->m_data.end() - 1, selected[0]);

	CSCUIElement* data = *res;
	pDoc->m_data.erase(res);

	Invalidate();
}

void CUXStudioView::OnMenuViewSendToBack()
{
	std::deque<CSCUIElement*> selected = get_selected_items();
	if (selected.size() != 1)
		return;

	//바로 뒤에 있는 항목과 swap 시킨다. 현재 항목이 맨 끝이라면 아무 동작도 하지 않는다.
	auto res = std::find(pDoc->m_data.begin(), pDoc->m_data.end(), selected[0]);
	if (res == pDoc->m_data.end() - 1)
		return;

	std::iter_swap(res, res + 1);

	Invalidate();
}

void CUXStudioView::OnMenuViewSendToFore()
{
	std::deque<CSCUIElement*> selected = get_selected_items();
	if (selected.size() != 1)
		return;

	//바로 앞에 있는 항목과 swap 시킨다. 현재 항목이 맨 앞이라면 아무 동작도 하지 않는다.
	auto res = std::find(pDoc->m_data.begin(), pDoc->m_data.end(), selected[0]);
	if (res == pDoc->m_data.begin())
		return;

	std::iter_swap(res - 1, res);

	Invalidate();
}

void CUXStudioView::OnMenuViewSendToTop()
{
	std::deque<CSCUIElement*> selected = get_selected_items();
	if (selected.size() != 1)
		return;

	//선택된 항목을 맨 앞에 넣어주고 1 ~ n 중에 있던 해당 아이템을 deque에서 제거한다.
	pDoc->m_data.push_front(selected[0]);
	m_item_selected = *(pDoc->m_data.begin());
	auto res = std::find(pDoc->m_data.begin() + 1, pDoc->m_data.end(), selected[0]);

	CSCUIElement* data = *res;
	pDoc->m_data.erase(res);
	
	Invalidate();
}

std::deque<CSCUIElement*> CUXStudioView::get_selected_items()
{
	std::deque<CSCUIElement*> items;

	for (auto el : pDoc->m_data)
	{
		if (el->m_selected)
			items.push_back(el);
	}

	return items;
}

void CUXStudioView::OnMenuViewCopy()
{
	m_item_copy_src = m_item_selected;
}

void CUXStudioView::OnMenuViewPaste()
{
	CSCUIElement* new_item = new CSCUIElement();
	m_item_copy_src->copy(new_item);

	new_item->m_r.Offset(20, 20);
	adjust_scroll_offset(new_item->m_r, true);

	//맨 끝에 넣지 않고 현재 선택된 항목의 바로 다음 차례로 넣어준다.
	auto it = std::find(pDoc->m_data.begin(), pDoc->m_data.end(), m_item_selected);
	if (it == pDoc->m_data.end())
		pDoc->m_data.push_front(new_item);
	else
		pDoc->m_data.insert(it, new_item);

	m_item_selected = new_item;

	Invalidate();
}

void CUXStudioView::OnMenuViewLabelEdit()
{
	m_in_editing = false;

	if (!m_item_selected)
		return;

	if (m_edit.m_hWnd == NULL)
	{
		DWORD dwStyle = ES_CENTER | /*WS_BORDER |*/ WS_CHILD | /*| ES_AUTOHSCROLL |*/  ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN;
		m_edit.create(dwStyle, CRect(0, 0, 1, 1), this, 0);
	}

	CRect r = gpRectF_to_CRect(m_item_selected->m_r);
	CPoint cp = r.CenterPoint();
	//r = make_center_rect(cp.x, cp.y, r.Width(), 20);
	r.DeflateRect(RECT_RESIZE_HANDLE_SIZE, RECT_RESIZE_HANDLE_SIZE);
	m_edit.MoveWindow(r);

	m_edit.set_text_color(m_item_selected->m_cr_text);
	m_edit.set_back_color(m_item_selected->m_cr_fill);
	m_edit.set_font_name(m_item_selected->m_font_name);
	m_edit.set_font_size(m_item_selected->m_font_size);
	m_edit.set_font_weight(m_item_selected->m_font_bold ? FW_BOLD : FW_NORMAL);

	m_edit.set_text(m_item_selected->m_text);
	m_edit.ShowWindow(SW_SHOW);
	m_edit.SetSel(0, -1);
	m_edit.SetFocus();

	m_in_editing = true;
}

LRESULT CUXStudioView::on_message_CSCEdit(WPARAM wParam, LPARAM lParam)
{
	CSCEdit* pEdit = (CSCEdit*)wParam;
	m_edit.ShowWindow(SW_HIDE);
	m_in_editing = false;

	return 0;
}

void CUXStudioView::edit_end(bool valid)
{
	m_in_editing = false;
	m_edit.ShowWindow(SW_HIDE);

	if (!valid)
		return;

	m_item_selected->m_text = m_edit.get_text();
}

void CUXStudioView::apply_changed_property(CSCUIElement* item)
{
	push_undo();
	Invalidate();
}

void CUXStudioView::apply_canvas_property_changed(int canvas_cx, int canvas_cy, int grid_cx, int grid_cy)
{
	SetScrollSizes(MM_TEXT, CSize(canvas_cx, canvas_cy));
	pDoc->m_sz_grid = CSize(grid_cx, grid_cy);
	Invalidate();
}

void CUXStudioView::OnMenuViewDelete()
{
	delete_selected_items();
	((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_propertyDlg.set_property(m_item_selected);
}

//m_data의 사본을 만들고 m_undo에 push한다.
//현재 m_data에 저장된 각 element들의 복사본을 생성하여 data에 넣어줘야 한다.
//그냥 주소값만 복사하면 안된다.
void CUXStudioView::push_undo()
{
	auto data = (std::deque<CSCUIElement*>*)new std::deque<CSCUIElement*>;

	for (int i = 0; i < pDoc->m_data.size(); i++)
	{
		CSCUIElement* el = new CSCUIElement;
		pDoc->m_data[i]->copy(el);
		data->push_back(el);
	}

	m_undo.push_back(data);
	m_undo_iter = m_undo.end();
}

//m_undo의 마지막 항목을 m_data의 원소들로 새로 추가해준다.
//이 때 m_data의 주소는 유지되어야 한다.
//그래야 undo / redo를 해도 그 주소는 유지된다.
void CUXStudioView::undo()
{
	if (m_undo.size() == 0)
		return;

	if (m_undo_iter < m_undo.begin())
		return;

	pDoc->release_data();
	
	//undo를 누른다는 것은 맨 마지막에 저장된 데이터로 돌리는 것이 아닌
	//끝에서 두번째에 존재하는 데이터로 돌리는 것이므로 현재 end라면 -2를 해줘야 한다.
	if (m_undo_iter > m_undo.begin())
		m_undo_iter -= (m_undo_iter == m_undo.end() ? 2 : 1);

	std::deque<CSCUIElement*>* data = *m_undo_iter;

	for (int i = 0; i < data->size(); i++)
	{
		CSCUIElement* el = new CSCUIElement;
		data->at(i)->copy(el);
		pDoc->m_data.push_back(el);
	}

	m_item_hover = m_item_selected = NULL;

	//if (m_undo_iter > m_undo.begin())
	//	m_undo_iter--;

	Invalidate();
}

void CUXStudioView::OnEditUndo()
{
	undo();
}

void CUXStudioView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (!pDoc)
		return;

	push_undo();
}
