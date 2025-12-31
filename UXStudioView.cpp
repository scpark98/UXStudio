
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
ON_COMMAND(ID_MENU_VIEW_SHOW_COORD, &CUXStudioView::OnMenuViewShowCoord)
ON_WM_TIMER()
ON_COMMAND(ID_MENU_VIEW_SORT, &CUXStudioView::OnMenuViewSort)
ON_WM_DROPFILES()
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
		{
			//if (data->at(j)->m_image)
			//{
			//	delete data->at(j)->m_image;
			//}

			delete data->at(j);
		}

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
	DragAcceptFiles();
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
	//우클릭으로 팝업메뉴를 표시할 때 선택된 항목이 아니면 선택 상태로 만들어준 후 팝업메뉴를 표시한다.
	//이미 선택된 항목이라면 그냥 팝업메뉴를 표시하면 된다.
	if (m_item_hover && !is_selected(m_item_hover))
	{
		select_all(false);
		m_selected_items.push_back(m_item_hover);
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

	pMenu->CheckMenuItem(ID_MENU_VIEW_SHOW_COORD, m_show_element_coord ? MF_CHECKED : MF_UNCHECKED);

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

	//배경색으로 칠한 후
	d2dc->Clear(get_d2color(pDoc->m_cr_canvas));

	int ix = pDoc->m_sz_grid.cx;
	int iy = pDoc->m_sz_grid.cy;
	int x, y;

	for (x = ix; x < pDoc->m_sz_canvas.cx; x += ix)
	{
		for (y = iy; y < pDoc->m_sz_canvas.cy; y += iy)
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

	m_resize_handle.clear();

	//역순으로 그려줘야 z-order가 유효하다.
	for (int i = pDoc->m_data.size() - 1; i >= 0; i--)
	{
		CSCUIElement* el = pDoc->m_data[i];
		Gdiplus::RectF r = el->m_r;

		r.Offset(-hs, -vs);

		D2D1_RECT_F rf = { r.X, r.Y, r.GetRight(), r.GetBottom() };

		//도형을 그려주고
		ID2D1PathGeometry* path = draw_rect(d2dc, rf, el->m_cr_stroke, el->m_cr_fill, el->m_stroke_thickness, el->m_round[0], el->m_round[1], el->m_round[2], el->m_round[3]);

		//image path가 있다면 도형 모양으로 이미지를 그려준다.
		if (el->m_image)
		{
			d2dc->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), path), nullptr);

			d2dc->DrawBitmap(el->m_image->get(), rf, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
			d2dc->PopLayer();
		}


		//hover된 항목 highlight
		if (el == m_item_hover && !el->m_selected)
			draw_rect(d2dc, rf, Gdiplus::Color::RoyalBlue, Gdiplus::Color::Transparent, 2.0f, el->m_round[0], el->m_round[1], el->m_round[2], el->m_round[3]);

		//글자색 설정 후
		m_br_label->SetColor(get_d2color(pDoc->m_data[i]->m_cr_text));

		//텍스트를 출력시킨다.
		IDWriteTextFormat* wf = NULL;
		HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&wf));
		m_WriteFactory->CreateTextFormat(pDoc->m_data[i]->m_font_name, nullptr,
			(DWRITE_FONT_WEIGHT)pDoc->m_data[i]->m_font_weight,
			pDoc->m_data[i]->m_font_italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			pDoc->m_data[i]->m_font_size, _T("ko-kr"), &wf);
		wf->SetTextAlignment((DWRITE_TEXT_ALIGNMENT)pDoc->m_data[i]->m_text_align);
		wf->SetParagraphAlignment((DWRITE_PARAGRAPH_ALIGNMENT)pDoc->m_data[i]->m_text_valign);
		d2dc->DrawText(pDoc->m_data[i]->m_text, pDoc->m_data[i]->m_text.GetLength(), wf, rf, m_br_label.Get());

		if (is_selected(el))
		{
			//Gdiplus::RectF r_selected = el->m_r;
			//r_selected.Offset(-hs, -vs);
			draw_rect(d2dc, r, Gdiplus::Color::RoyalBlue, Gdiplus::Color::Transparent, 1.0f, el->m_round[0], el->m_round[1], el->m_round[2], el->m_round[3]);

			std::vector<CRect> resize_handle;
			get_resizable_handle(r, &resize_handle);
			m_resize_handle.push_back(resize_handle);
			draw_resize_handle(d2dc, &resize_handle);
		}
	}

	//모든 도형을 그려준 후 인덱스를 표시한다.
	if (m_show_element_coord)
	{
		for (int i = pDoc->m_data.size() - 1; i >= 0; i--)
		{
			CSCUIElement* el = pDoc->m_data[i];
			Gdiplus::RectF r = el->m_r;

			r.Offset(-hs, -vs);
			D2D1_RECT_F rf = { r.X, r.Y, r.GetRight(), r.GetBottom() };

			CString text;
			//text.Format(_T("%d. %s"), i, get_rect_info_str(gpRectF_to_CRect(el->m_r), 0));
			text.Format(_T("%d"), i);

			D2D1_RECT_F rtext = make_center_d2rect(r.X, r.Y, pDC->GetTextExtent(text).cx, 8);
			//D2D1_ELLIPSE ellipse = { D2D1::Point2F(center(r).X, center(r).Y), 8, 8 };
			//d2dc->FillEllipse(&ellipse, m_br_grid.Get());
			draw_rect(d2dc, rtext, Gdiplus::Color::Transparent, Gdiplus::Color(128, 255, 0, 0), 1.0f, 2);

			m_WriteFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			m_WriteFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			d2dc->DrawText(text, text.GetLength(), m_WriteFormat, rtext, m_br_index.Get());
		}
	}

	//move, resize시에 다른 항목과 일치했던 기록이 있다면 라인을 그려서 표시한다.
	for (int i = 0; i < m_pt_align_fit.size(); i += 2)
	{
		D2D1_POINT_2F pt[2];
		pt[0] = m_pt_align_fit[i];
		pt[1] = m_pt_align_fit[i + 1];
		adjust_scroll_offset(pt[0], false, true);
		adjust_scroll_offset(pt[1], false, true);

		d2dc->DrawLine(pt[0], pt[1], m_br_align_fit.Get(), 1.0f, m_stroke_style.Get());
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

void CUXStudioView::draw_resize_handle(ID2D1DeviceContext* d2dc, CRect* resize_handle)
{
	//0번은 inside이므로 별도로 그려주는 것이 없으므로 1번부터 그린다.
	//요즘은 left, top, right, bottom에는 handle을 그리지 않고 resize커서로만 표시하는게 깔끔하다.
	for (int i = 1; i < RECT_RESIZE_HANDLE_COUNT; i++)
	{
		CRect r = resize_handle[i];
		draw_rect(d2dc, CRect_to_gpRectF(r), Gdiplus::Color::RoyalBlue, Gdiplus::Color::White);
	}
}

void CUXStudioView::draw_resize_handle(ID2D1DeviceContext* d2dc, std::vector<CRect>* resize_handle)
{
	for (int i = 1; i < RECT_RESIZE_HANDLE_COUNT; i++)
	{
		CRect r = resize_handle->at(i);;
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

	if (m_spacebar_down)
	{
		m_lbutton_down = true;
		adjust_scroll_offset(point, true);
		m_pt_lbutton_down = point;
		return;
	}

	//hover인 항목을 클릭하면 편집모드로 전환된다.
	CPoint pt = point;
	//adjust_scroll_offset(pt, true);

	//선택된 항목들을 이동, 크기를 변경 모드 시작
	//if (!m_r_selected.IsEmptyArea() && m_handle_index >= corner_inside)
	//trace(m_selected_items);
	trace(m_handle_index);
	if (m_selected_items.size() && m_handle_index >= corner_inside)
	{
		//Ctrl키를 누른채로 드래깅하면 복사모드로 동작한다.
		//원래 선택된 항목들을 해제하고 복사된 항목들이 선택된 상태로 만들어줘야 한다.
		//복사된 항목들은 선택항목들 중 맨 마지막 항목의 뒤에 추가된다.
		if (IsCtrlPressed() && (m_handle_index == corner_inside))
		{
			std::deque<CSCUIElement*> new_items;
			CSCUIElement* last_of_selected_items = get_last_selected_item();

			for (int i = 0; i < m_selected_items.size(); i++)
			{
				CSCUIElement* new_item = new CSCUIElement();
				m_selected_items[i]->copy(new_item);
				new_items.push_back(new_item);
			}

			insert(last_of_selected_items, &new_items);

			m_selected_items.clear();
			m_selected_items.assign(new_items.begin(), new_items.end());
		}

		m_pt_lbutton_down = adjust_scroll_offset(pt, false);
		m_is_resizing = true;
		trace(m_is_resizing);
		return;
	}
	else
	{
		m_is_resizing = false;
		trace(m_is_resizing);
	}

	if (m_item_hover)
	{
		//hover item이 클릭되었는지 비교할때는 scroll_offset은 보정하되 near_grid는 해서는 안된다.
		adjust_scroll_offset(pt, false);

		if (m_item_hover->pt_in_rect(pt.x, pt.y))
		{
			//shift나 ctrl을 누르지 않은채로 클릭하면 단일 선택된다.
			if (!IsShiftPressed() && !IsCtrlPressed())
			{
				select_all(false);
				m_selected_items.push_back(m_item_hover);
			}
			//shift나 ctrl을 누른 채 hover 항목을 클릭하면 멀티선택된다.
			else if (IsShiftPressed() || IsCtrlPressed())
			{
				//현재 hover 항목이 선택되지 않은 상태였다면 선택 목록에 추가하고
				if (!is_selected(m_item_hover))
				{
					m_selected_items.push_back(m_item_hover);
				}
				//선택된 항목이었다면 그 항목만 선택 해제한다.
				else
				{
					auto it = get_iterator(m_item_hover, &m_selected_items);
					if (it != m_selected_items.end())
						m_selected_items.erase(it);
				}
			}

			//m_item_hover->m_selected = true;
			////get_bound_selected_rect();
			////get_resizable_handle(m_r_selected, m_resize_handle);
			//m_selected_items = m_item_hover;
			////get_resizable_handle(m_selected_items->m_r, m_resize_handle);

			//선택되지 않았던 항목을 클릭하면 바로 이동 모드로 전환되도록 아래 코드 추가.
			//=> 클릭만 하려 했으나 간혹 move되는 경우가 발생되어 우선 주석처리 함.
			//m_is_resizing = true;
			//m_handle_index = corner_inside;
			//m_pt_lbutton_down = get_near_grid(pt);
		}
		else
		{
			//hover item이 있는데 클릭된 위치가 hover의 내부가 아닐 경우가 발생하나??
			select_all(false);
			//m_item_hover->m_selected = false;
			//m_selected_items = NULL;
			//m_r_selected.Width = 0;
		}

		Invalidate();

		update_property();
		return;
	}
	else
	{
		select_all(false);
		//m_selected_items = NULL;
		//m_r_selected.Width = 0;
		update_property();
	}

	//새로운 항목을 그리기 시작...
	m_lbutton_down = true;
	adjust_scroll_offset(pt, true);
	m_pt_cur = m_pt_lbutton_down = pt;

	CFormView::OnLButtonDown(nFlags, point);
}

void CUXStudioView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPoint pt = point;
	adjust_scroll_offset(pt, false);
	((CMainFrame*)(AfxGetApp()->m_pMainWnd))->set_cursor_info(pt);

	//trace(m_handle_index);

	//현재 move or resizing 중일때
	if (m_is_resizing)
	{
		move_or_resize_item(pt);
		//m_pt_lbutton_down = pt;
		update_property();
	}
	else if (m_lbutton_down)
	{
		//spacebar + lbutton으로 스크롤 중
		if (m_spacebar_down)
		{
			SetScrollPos(SB_HORZ, m_pt_lbutton_down.x - point.x);
			SetScrollPos(SB_VERT, m_pt_lbutton_down.y - point.y);
			Invalidate();
			return;
		}
		else
		{
			m_pt_cur = get_near_grid(pt);
		}
		Invalidate();
	}
	//lbutton down도 아니고 spacebar down도 아니라면 hover item...
	else if (!m_spacebar_down)
	{
		CSCUIElement* hover = get_hover_item(pt);

		if (hover != m_item_hover)
		{
			m_item_hover = get_hover_item(pt);
			//TRACE(_T("hover = %p\n"), m_item_hover);

			//CRect rc;
			//GetClientRect(rc);
			//m_d2dc.on_size_changed(rc.Width(), rc.Height());
			Invalidate();
		}
	}

	CFormView::OnMouseMove(nFlags, point);
}

void CUXStudioView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	ReleaseCapture();

	CPoint pt = point;
	adjust_scroll_offset(pt, true);

	m_pt_align_fit.clear();

	if (m_is_resizing)
	{
		m_is_resizing = false;
		m_pt_lbutton_down = CPoint(-1, -1);

		for (int i = 0; i < m_selected_items.size(); i++)
			normalize_rect(m_selected_items[i]->m_r);

		update_property();
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

			//그려진 크기가 일정 크기 미만이라면 무시한다.
			if (r.Width < 20 && r.Height < 20)
			{
				Invalidate();
				return;
			}

			//가장 최종으로 그려진 항목이 가장 첫번째가 되어야 한다.
			pDoc->m_data.push_front(new CSCUIElement(r));
		}
	}

	Invalidate();

	push_undo();

	CFormView::OnLButtonUp(nFlags, point);
}

void CUXStudioView::move_or_resize_item(CPoint pt)
{
	if (m_selected_items.size() == 0 || m_handle_index < corner_inside)
		return;

	CPoint pt_down = m_pt_lbutton_down;

	//선택된 항목들에 대해 모두 처리해줘야 한다.
	for (int i = 0; i < m_selected_items.size(); i++)
	{
		CSCUIElement* item = m_selected_items[i];

		switch (m_handle_index)
		{
			case corner_inside:
				{
					item->m_r.X += pt.x - pt_down.x;
					item->m_r.Y += pt.y - pt_down.y;
					get_fit_others_of_inside(item);

					//if (fit[0] < 0.0f)// && fit[2] < 0.0f)
					//	item->m_r.X = pt.x - pt_offset.x;
					//else
					//	item->m_r.X = fit[0];
					//if (fit[1] < 0.0f && fit[3] < 0.0f)
					//	item->m_r.Y = pt.y - pt_offset.y;
				}
				break;
			case corner_left:
				set_left(item->m_r, pt.x);
				get_fit_others(m_handle_index, item);
				break;
			case corner_right:
				item->m_r.Width = pt.x - item->m_r.X;
				get_fit_others(m_handle_index, item);
				break;
			case corner_top:
				set_top(item->m_r, pt.y);
				get_fit_others(m_handle_index, item);
				break;
			case corner_bottom:
				item->m_r.Height = pt.y - item->m_r.Y;
				get_fit_others(m_handle_index, item);
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

		//canvas를 벗어나지 않도록 보정, 단 멀티 선택일때는 이를 적용해서는 안된다.
		//멀티 선택 후 move하면 
		adjust_rect_range(item->m_r, Gdiplus::RectF(0, 0, pDoc->m_sz_canvas.cx, pDoc->m_sz_canvas.cy));
	}

	if (m_handle_index == corner_inside)
	{
		m_pt_lbutton_down = pt;
		//if (m_pt_align_fit.size() == 0)
		//{
		//	m_selected_items[0]->m_r = get_near_grid(m_selected_items[0]->m_r);
		//	//m_selected_items[0]->m_r.X = get_near_grid(m_selected_items[0]->m_r.X, true);
		//	//m_selected_items[0]->m_r.Y = get_near_grid(m_selected_items[0]->m_r.Y, false);
		//}
	}

	Invalidate();
}

//move시에 다른 항목과의 일치되는 위치로 자동 보정. resize시의 코드내에서 분기처리 하려 했으나 예상보다 복잡하여 분리함.
void CUXStudioView::get_fit_others_of_inside(CSCUIElement* el)
{
	//left, top 등과는 다르게 inside는 옮길 때 마우스를 미세하게 움직이면 이동이 거의 되지 않는다.
	//이는 gravity = 4 였을때 마우스 변동이 4보다 크지 않으면 계속 r.X에 잡혀있기 때문이다.
	//여러 방법들로 시도해봤으나 현재로서는 gravity = 1 ~ 2로 세팅하여 사용한다.
	int gravity = 1;
	int extend = 16;		//fit line을 좀 더 늘려서 그려준다.

	m_pt_align_fit.clear();

	//예를 들어 어떤 항목과 left fit이 발생했다면 다른 항목들과는 left fit인지 검사할 필요가 없다.
	float fit[4] = { -1.0f, -1.0f, -1.0f, -1.0f };

	for (int i = 0; i < pDoc->m_data.size(); i++)
	{
		//자기자신일 경우와 selected인 항목일 경우는 비교를 스킵한다.
		if (el == pDoc->m_data[i] || is_selected(pDoc->m_data[i]))
			continue;

		Gdiplus::RectF r = pDoc->m_data[i]->m_r;

		//left가 다른 도형에 left fit인지
		if (fit[0] < 0.0f && is_in_range(el->m_r.X, r.X - gravity, r.X + gravity))
		{
			el->m_r.X = fit[0] = r.X;
			m_pt_align_fit.push_back(D2D1::Point2F(r.X, MIN(r.Y, el->m_r.Y) - extend));
			m_pt_align_fit.push_back(D2D1::Point2F(r.X, MAX(r.GetBottom(), el->m_r.GetBottom()) + extend));
		}
		//left가 다른 도형에 right fit인지
		else if (fit[0] < 0.0f && is_in_range(el->m_r.X, r.GetRight() - gravity, r.GetRight() + gravity))
		{
			el->m_r.X = fit[0] = r.GetRight();
			m_pt_align_fit.push_back(D2D1::Point2F(r.GetRight(), MIN(r.Y, el->m_r.Y) - extend));
			m_pt_align_fit.push_back(D2D1::Point2F(r.GetRight(), MAX(r.GetBottom(), el->m_r.GetBottom()) + extend));
		}
		else
		{
			//el->m_r.X = get_near_grid(el->m_r.X);
		}

		//right가 다른 도형에 left fit인지
		if (fit[2] < 0.0f && is_in_range(el->m_r.GetRight(), r.X - gravity, r.X + gravity))
		{
			el->m_r.X = fit[2] = r.X - el->m_r.Width;
			m_pt_align_fit.push_back(D2D1::Point2F(r.X, MIN(r.Y, el->m_r.Y) - extend));
			m_pt_align_fit.push_back(D2D1::Point2F(r.X, MAX(r.GetBottom(), el->m_r.GetBottom()) + extend));
		}
		//right가 다른 도형에 right fit인지
		else if (fit[2] < 0.0f && is_in_range(el->m_r.GetRight(), r.GetRight() - gravity, r.GetRight() + gravity))
		{
			el->m_r.X = fit[2] = r.GetRight() - el->m_r.Width;
			m_pt_align_fit.push_back(D2D1::Point2F(r.GetRight(), MIN(r.Y, el->m_r.Y) - extend));
			m_pt_align_fit.push_back(D2D1::Point2F(r.GetRight(), MAX(r.GetBottom(), el->m_r.GetBottom() + extend)));
		}

		//top이 다른 도형에 top fit인지
		if (fit[1] < 0.0f && is_in_range(el->m_r.Y, r.Y - gravity, r.Y + gravity))
		{
			el->m_r.Y = fit[1] = r.Y;
			m_pt_align_fit.push_back(D2D1::Point2F(MIN(r.X, el->m_r.X) - extend, el->m_r.Y));
			m_pt_align_fit.push_back(D2D1::Point2F(MAX(r.GetRight(), el->m_r.GetRight()) + extend, el->m_r.Y));
		}
		//top이 다른 도형에 bottom fit인지
		else if (fit[1] < 0.0f && is_in_range(el->m_r.Y, r.GetBottom() - gravity, r.GetBottom() + gravity))
		{
			el->m_r.Y = fit[1] = r.GetBottom();
			m_pt_align_fit.push_back(D2D1::Point2F(MIN(r.X, el->m_r.X) - extend, r.GetBottom()));
			m_pt_align_fit.push_back(D2D1::Point2F(MAX(r.GetRight(), el->m_r.GetRight()) + extend, r.GetBottom()));
		}

		//bottom이 다른 도형에 top fit인지
		if (fit[3] < 0.0f && is_in_range(el->m_r.GetBottom(), r.Y - gravity, r.Y + gravity))
		{
			el->m_r.Y = fit[3] = r.Y - el->m_r.Height;
			m_pt_align_fit.push_back(D2D1::Point2F(MIN(r.X, el->m_r.X) - extend, r.Y));
			m_pt_align_fit.push_back(D2D1::Point2F(MAX(r.GetRight(), el->m_r.GetRight()) + extend, r.Y));
		}
		//bottom이 다른 도형에 bottom fit인지
		else if (fit[3] < 0.0f && is_in_range(el->m_r.GetBottom(), r.GetBottom() - gravity, r.GetBottom() + gravity))
		{
			el->m_r.Y = fit[3] = r.GetBottom() - el->m_r.Height;
			m_pt_align_fit.push_back(D2D1::Point2F(MIN(r.X, el->m_r.X) - extend, r.GetBottom()));
			m_pt_align_fit.push_back(D2D1::Point2F(MAX(r.GetRight(), el->m_r.GetRight() + extend), r.GetBottom()));
		}
	}

	//if (fit[0] < 0.0f && fit[2] < 0.0f)
	//	el->m_r.X = get_near_grid(el->m_r.X);

	//if (fit[1] < 0.0f && fit[3] < 0.0f)
	//	el->m_r.Y = get_near_grid(el->m_r.Y);
}

//resize시에 다른 항목과의 일치되는 위치로 자동 보정
void CUXStudioView::get_fit_others(int index, CSCUIElement* el, CPoint pt, CPoint pt_down)
{
	int gravity = 4;
	int extend = 16;		//fit line을 좀 더 늘려서 그려준다.

	m_pt_align_fit.clear();

	for (int i = 0; i < pDoc->m_data.size(); i++)
	{
		//자기자신일 경우와 selected인 항목일 경우는 비교를 스킵한다.
		if (el == pDoc->m_data[i] || is_selected(pDoc->m_data[i]))
			continue;

		Gdiplus::RectF r = pDoc->m_data[i]->m_r;

		//range에 맞는 항목을 찾았다면 left를 맞춰주고 바로 리턴해야 한다.
		//그렇지 않으면 다른 항목들과도 또 left가 맞는지 확인하고 그렇지 않으면 grid로 움직이게 된다.
		//=>바로 리턴하면 left와 top모두 일치하는 경우를 처리하지 못한다.
		//left fit flag를 true로 하고 계속 검사해야 한다. true인 코너는 더 이상 검사하지 않는다.
		if (index == corner_left)
		{
			//다른 도형에 left fit인지
			if (is_in_range(el->m_r.X, r.X - gravity, r.X + gravity))
			{
				set_left(el->m_r, r.X);
				m_pt_align_fit.push_back(D2D1::Point2F(el->m_r.X, MIN(r.Y, el->m_r.Y) - extend));
				m_pt_align_fit.push_back(D2D1::Point2F(el->m_r.X, MAX(r.GetBottom(), el->m_r.GetBottom()) + extend));
				return;
			}
			//다른 도형에 right fit인지
			else if (is_in_range(el->m_r.X, r.GetRight() - gravity, r.GetRight() + gravity))
			{
				set_left(el->m_r, r.GetRight());

				m_pt_align_fit.push_back(D2D1::Point2F(r.GetRight(), MIN(r.Y, el->m_r.Y) - extend));
				m_pt_align_fit.push_back(D2D1::Point2F(r.GetRight(), MAX(r.GetBottom(), el->m_r.GetBottom()) + extend));
				return;
			}
			else
			{
				set_left(el->m_r, get_near_grid(el->m_r.X));
			}
		}
		else if (index == corner_right)
		{
			if (is_in_range(el->m_r.GetRight(), r.GetRight() - gravity, r.GetRight() + gravity))
			{
				el->m_r.Width = r.GetRight() - el->m_r.X;
				TRACE(_T("i = %d, w = %.1f\n"), i, el->m_r.Width);

				m_pt_align_fit.push_back(D2D1::Point2F(r.GetRight(), MIN(r.Y, el->m_r.Y) - extend));
				m_pt_align_fit.push_back(D2D1::Point2F(r.GetRight(), MAX(r.GetBottom(), el->m_r.GetBottom() + extend)));
				return;
			}
			//다른 도형에 left fit인지
			else if (is_in_range(el->m_r.GetRight(), r.X - gravity, r.X + gravity))
			{
				el->m_r.Width = r.X - el->m_r.X;

				m_pt_align_fit.push_back(D2D1::Point2F(r.X, MIN(r.Y, el->m_r.Y) - extend));
				m_pt_align_fit.push_back(D2D1::Point2F(r.X, MAX(r.GetBottom(), el->m_r.GetBottom()) + extend));
				return;
			}
			else
			{
				float right = get_near_grid(el->m_r.GetRight());
				el->m_r.Width = right - el->m_r.X;
			}
			//TRACE(_T("new : %.1f\n"), el->m_r.Width);
		}
		else if (index == corner_top)
		{
			if (is_in_range(el->m_r.Y, r.Y - gravity, r.Y + gravity))
			{
				set_top(el->m_r, r.Y);
				m_pt_align_fit.push_back(D2D1::Point2F(MIN(r.X, el->m_r.X) - extend, el->m_r.Y));
				m_pt_align_fit.push_back(D2D1::Point2F(MAX(r.GetRight(), el->m_r.GetRight()) + extend, el->m_r.Y));
				return;
			}
			else if (is_in_range(el->m_r.Y, r.GetBottom() - gravity, r.GetBottom() + gravity))
			{
				set_top(el->m_r, r.GetBottom());
				m_pt_align_fit.push_back(D2D1::Point2F(MIN(r.X, el->m_r.X) - extend, r.GetBottom()));
				m_pt_align_fit.push_back(D2D1::Point2F(MAX(r.GetRight(), el->m_r.GetRight()) + extend, r.GetBottom()));
				return;
			}
			else
			{
				set_top(el->m_r, get_near_grid(el->m_r.Y, false));
			}
		}
		else if (index == corner_bottom)
		{
			if (is_in_range(el->m_r.GetBottom(), r.GetBottom() - gravity, r.GetBottom() + gravity))
			{
				el->m_r.Height = r.GetBottom() - el->m_r.Y;
				m_pt_align_fit.push_back(D2D1::Point2F(MIN(r.X, el->m_r.X) - extend, r.GetBottom()));
				m_pt_align_fit.push_back(D2D1::Point2F(MAX(r.GetRight(), el->m_r.GetRight() + extend), r.GetBottom()));
				return;
			}
			else if (is_in_range(el->m_r.GetBottom(), r.Y - gravity, r.Y + gravity))
			{
				el->m_r.Height = r.Y - el->m_r.Y;

				m_pt_align_fit.push_back(D2D1::Point2F(MIN(r.X, el->m_r.X) - extend, r.Y));
				m_pt_align_fit.push_back(D2D1::Point2F(MAX(r.GetRight(), el->m_r.GetRight()) + extend, r.Y));
				return;
			}
			else
			{
				float bottom = get_near_grid(el->m_r.GetBottom());
				el->m_r.Height = bottom - el->m_r.Y;
			}
		}
	}
}

//방향키를 이용한 이동, 크기조정
void CUXStudioView::move_or_resize_item(int key)
{
	if (m_in_editing || m_selected_items.size() == 0)
		return;

	//방향키만 누르면 grid 만큼 씩 이동,
	//shift를 누르면 1씩 이동,
	//ctrl을 누르면 grid * 2씩 이동,
	//shift+ctrl이면 grid * 4씩 이동한다.
	CSize sz_interval = pDoc->m_sz_grid;

	if (IsShiftPressed() && IsCtrlPressed())
	{
		sz_interval.cx *= 4;
		sz_interval.cy *= 4;
	}
	else if (IsCtrlPressed())
	{
		sz_interval.cx *= 2;
		sz_interval.cy *= 2;
	}
	else if (IsShiftPressed())
	{
		sz_interval.cx = 1;
		sz_interval.cy = 1;
	}

	for (int i = 0; i < m_selected_items.size(); i++)
	{
		CSCUIElement* item = m_selected_items[i];

		switch (key)
		{
			case VK_LEFT:
				item->m_r.X -= sz_interval.cx;
				break;
			case VK_RIGHT:
				item->m_r.X += sz_interval.cx;
				break;
			case VK_UP:
				item->m_r.Y -= sz_interval.cy;
				break;
			case VK_DOWN:
				item->m_r.Y += sz_interval.cy;
				break;
		}
	}

	Invalidate();

	update_property();
}

CSCUIElement* CUXStudioView::get_hover_item(CPoint pt)
{
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
		return *res;

	return NULL;
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
				move_or_resize_item(pMsg->wParam);
				break;
			case VK_OEM_4://'[' <= 문자로 써주면 인식안됨
				OnMenuViewSendToBack();
				break;
			case VK_OEM_6://']'
				OnMenuViewSendToFore();
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

/*
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
*/

//모든 항목을 선택 또는 해제한다.
void CUXStudioView::select_all(bool select)
{
	m_selected_items.clear();

	if (select)
	{
		m_selected_items.assign(pDoc->m_data.begin(), pDoc->m_data.end());
	}

	Invalidate();
}

void CUXStudioView::delete_selected_items()
{
	for (int i = m_selected_items.size() - 1; i >= 0; i--)
	{
		auto found = std::find(pDoc->m_data.begin(), pDoc->m_data.end(), m_selected_items[i]);

		if (found != pDoc->m_data.end())
		{
			delete* found;
			pDoc->m_data.erase(found);

			m_selected_items.erase(m_selected_items.begin() + i);
		}
	}

	update_property();

	Invalidate();
}

BOOL CUXStudioView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//팝업메뉴가 표시중이거나 편집중이거나 선택항목이 없다면 커서의 변경은 없다.
	if (m_is_context_menu_displaying || m_selected_items.size() == 0 || m_in_editing || m_spacebar_down)
		return CFormView::OnSetCursor(pWnd, nHitTest, message);

	CPoint pt;
	CRect rc;

	GetCursorPos(&pt);
	ScreenToClient(&pt);

	m_handle_index = -1;

	for (int i = 0; i < m_resize_handle.size(); i++)
	{
		for (int j = 0; j < RECT_RESIZE_HANDLE_COUNT; j++)
		{
			if (m_resize_handle[i][j].PtInRect(pt))
			{
				m_handle_index = j;
				break;
			}
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
	if (m_selected_items.size() != 1)
		return;

	//선택된 항목을 맨 뒤에 넣어주고 0 ~ n-1 중에 있던 해당 아이템을 deque에서 제거한다.
	pDoc->m_data.push_back(m_selected_items[0]);
	m_selected_items[0] = pDoc->m_data.back();
	auto res = std::find(pDoc->m_data.begin(), pDoc->m_data.end() - 1, m_selected_items[0]);

	CSCUIElement* data = *res;
	pDoc->m_data.erase(res);

	Invalidate();
}

void CUXStudioView::OnMenuViewSendToBack()
{
	if (m_selected_items.size() != 1)
		return;

	//바로 뒤에 있는 항목과 swap 시킨다. 현재 항목이 맨 끝이라면 아무 동작도 하지 않는다.
	auto res = std::find(pDoc->m_data.begin(), pDoc->m_data.end(), m_selected_items[0]);
	if (res == pDoc->m_data.end() - 1)
		return;

	std::iter_swap(res, res + 1);

	Invalidate();
}

void CUXStudioView::OnMenuViewSendToFore()
{
	if (m_selected_items.size() != 1)
		return;

	//바로 앞에 있는 항목과 swap 시킨다. 현재 항목이 맨 앞이라면 아무 동작도 하지 않는다.
	auto res = std::find(pDoc->m_data.begin(), pDoc->m_data.end(), m_selected_items[0]);
	if (res == pDoc->m_data.begin())
		return;

	std::iter_swap(res - 1, res);

	Invalidate();
}

void CUXStudioView::OnMenuViewSendToTop()
{
	if (m_selected_items.size() != 1)
		return;

	//선택된 항목을 맨 앞에 넣어주고 1 ~ n 중에 있던 해당 아이템을 deque에서 제거한다.
	pDoc->m_data.push_front(m_selected_items[0]);
	m_selected_items[0] = *(pDoc->m_data.begin());
	auto res = std::find(pDoc->m_data.begin() + 1, pDoc->m_data.end(), m_selected_items[0]);

	CSCUIElement* data = *res;
	pDoc->m_data.erase(res);
	
	Invalidate();
}

/*
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
*/
void CUXStudioView::OnMenuViewCopy()
{
	m_d2dc.save(_T("d:\\saved.png"));

	m_item_copy_src.clear();
	m_item_copy_src.assign(m_selected_items.begin(), m_selected_items.end());
}

void CUXStudioView::OnMenuViewPaste()
{
	std::deque<CSCUIElement*> new_selected_items;

	for (int i = 0; i < m_item_copy_src.size(); i++)
	{
		CSCUIElement* new_item = new CSCUIElement();
		m_item_copy_src[i]->copy(new_item);

		new_item->m_r.Offset(20, 20);
		//adjust_scroll_offset(new_item->m_r, false);

		//전체 element의 맨 처음에 넣지 않고 맨 마지막에 선택된 항목의 바로 다음 차례로 넣어준다.
		auto it = std::find(pDoc->m_data.begin(), pDoc->m_data.end(), m_selected_items.back());
		if (it == pDoc->m_data.end())
			pDoc->m_data.push_front(new_item);
		else
			pDoc->m_data.insert(it, new_item);
		//pDoc->m_data.insert(m_selected_items.back(), new_item);

		new_selected_items.push_back(new_item);
	}

	m_selected_items.clear();
	m_selected_items.assign(new_selected_items.begin(), new_selected_items.end());

	Invalidate();
}

void CUXStudioView::OnMenuViewLabelEdit()
{
	m_in_editing = false;

	if (m_selected_items.size() == 0)
		return;

	if (m_edit.m_hWnd == NULL)
	{
		DWORD dwStyle = ES_CENTER | /*WS_BORDER |*/ WS_CHILD | /*| ES_AUTOHSCROLL |*/  ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN;
		m_edit.create(dwStyle, CRect(0, 0, 1, 1), this, 0);
	}

	//편집 시 선택 항목 중 맨 마지막 항목에만 CEdit을 표시하고 편집후에는
	//선택된 모든 항목에 적용시킨다.
	CSCUIElement* el = m_selected_items.back();

	CRect r = gpRectF_to_CRect(el->m_r);
	CPoint cp = r.CenterPoint();
	r.DeflateRect(RECT_RESIZE_HANDLE_SIZE * 2, RECT_RESIZE_HANDLE_SIZE * 2);
	m_edit.MoveWindow(r);

	m_edit.set_text_color(el->m_cr_text);
	m_edit.set_transparent();
	//m_edit.set_back_color(el->m_cr_fill);
	m_edit.set_font_name(el->m_font_name);
	m_edit.set_font_size(el->m_font_size);
	m_edit.set_font_weight(el->m_font_weight);

	m_edit.set_text(el->m_text);
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

	CString new_label = m_edit.get_text();
	for (int i = 0; i < m_selected_items.size(); i++)
		m_selected_items[i]->m_text = new_label;
}

//속성창에서 값 변경 시 view에 적용시킨다.
void CUXStudioView::apply_changed_property(std::deque<CSCUIElement*>* items)
{
	Invalidate();
}

void CUXStudioView::apply_canvas_property_changed(int canvas_cx, int canvas_cy, Gdiplus::Color cr_canvas, int grid_cx, int grid_cy, Gdiplus::Color cr_grid)
{
	SetScrollSizes(MM_TEXT, CSize(canvas_cx, canvas_cy));
	m_br_canvas->SetColor(get_d2color(cr_canvas));
	pDoc->m_cr_canvas = cr_canvas;

	pDoc->m_sz_grid = CSize(grid_cx, grid_cy);
	m_br_grid->SetColor(get_d2color(cr_grid));
	pDoc->m_cr_grid = cr_grid;

	Invalidate();
}

void CUXStudioView::OnMenuViewDelete()
{
	delete_selected_items();
	update_property();
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
	m_undo_iter = m_undo.end() - 1;
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
		m_undo_iter -= 1;// (m_undo_iter == m_undo.end() ? 2 : 1);

	std::deque<CSCUIElement*>* data = *m_undo_iter;

	for (int i = 0; i < data->size(); i++)
	{
		CSCUIElement* el = new CSCUIElement;
		data->at(i)->copy(el);
		pDoc->m_data.push_back(el);
	}

	m_item_hover = NULL;
	m_selected_items.clear();

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

	SetScrollSizes(MM_TEXT, pDoc->m_sz_canvas);

	m_d2dc.init(m_hWnd, pDoc->m_sz_canvas.cx, pDoc->m_sz_canvas.cy);

	HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_WriteFactory));
	if (SUCCEEDED(hr))
	{
		m_WriteFactory->CreateTextFormat(_T("맑은 고딕"), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 8.0f, _T("ko-kr"), &m_WriteFormat);
		m_WriteFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		m_WriteFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}

	m_d2dc.get_d2dc()->CreateSolidColorBrush(get_d2color(pDoc->m_cr_canvas), m_br_canvas.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(get_d2color(pDoc->m_cr_grid), m_br_grid.GetAddressOf());

	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::RoyalBlue), m_br_draw.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), m_br_item.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), m_br_align_fit.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::RoyalBlue), m_br_hover.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::RoyalBlue), m_br_selected.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue), m_br_multi_selected.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), m_br_label.GetAddressOf());

	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), m_br_index.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), m_br_index_back.GetAddressOf());

	D2D1_STROKE_STYLE_PROPERTIES strokeStyle =
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_FLAT,  // startCap
			D2D1_CAP_STYLE_FLAT,  // endCap
			D2D1_CAP_STYLE_ROUND, // dashCap
			D2D1_LINE_JOIN_ROUND, // lineJoin
			1.0f,                // miterLimit
			D2D1_DASH_STYLE_DASH_DOT, // dashStyle 
			0.f);

	m_d2dc.get_factory()->CreateStrokeStyle(strokeStyle, NULL, 0, m_stroke_style.GetAddressOf());

	((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_propertyDlg.set_canvas_property(1920, 1080, pDoc->m_cr_canvas, pDoc->m_sz_grid.cx, pDoc->m_sz_grid.cy, pDoc->m_cr_grid);

	m_show_element_coord = AfxGetApp()->GetProfileInt(_T("setting"), _T("m_show_element_coord"), false);

	//맨 처음 data를 로딩한 후 image path가 유효한 element들에 대해 이미지를 로딩한다.
	for (int i = 0; i < pDoc->m_data.size(); i++)
	{
		pDoc->m_data[i]->load_image(m_d2dc.get_WICFactory(), m_d2dc.get_d2dc());
		//if ((pDoc->m_data[i]->m_image_path.IsEmpty() == false) && PathFileExists(pDoc->m_data[i]->m_image_path))
		//{

		//	pDoc->m_data[i]->m_image = new CSCD2Image;
		//	pDoc->m_data[i]->m_image->load(m_d2dc.get_WICFactory(), m_d2dc.get_d2dc(), pDoc->m_data[i]->m_image_path);
		//}
	}

	push_undo();
	update_property(false);
}

void CUXStudioView::OnMenuViewShowCoord()
{
	m_show_element_coord = !m_show_element_coord;
	AfxGetApp()->WriteProfileInt(_T("setting"), _T("m_show_element_coord"), m_show_element_coord);
	Invalidate();
}

//after = true이면 현재 선택된 항목의 뒤에 추가한다.
void CUXStudioView::insert(CSCUIElement* pos, std::deque<CSCUIElement*>* new_items)
{
	auto it = get_iterator(pos);
	pDoc->m_data.insert(it + 1, new_items->begin(), new_items->end());
	/*
	if (after && it == pDoc->m_data.end())
		pDoc->m_data.push_back(new_item);
	else if (!after && it == pDoc->m_data.begin())
		pDoc->m_data.push_front(new_item);
	else
		pDoc->m_data.insert(it + 1, new_item);	
	*/
}

//선택된 항목들 중 맨 마지막 인덱스의 항목을 리턴한다.
CSCUIElement* CUXStudioView::get_last_selected_item()
{
	int distance = 0;
	int last_index = 0;

	for (int i = 0; i < m_selected_items.size(); i++)
	{
		int index = get_index(m_selected_items[i], &pDoc->m_data);
		if (index > last_index)
			last_index = index;
	}

	return pDoc->m_data[last_index];
}

//항목이 변경되면 undo에 기록하고 속성창에도 이를 반영한다.
//단 mouse 액션일 경우는 너무 빈번하게 발생되므로 timer를 적용하여 반영한다.
void CUXStudioView::update_property(bool doc_modified)
{
	//개발단계에서는 임시 주석처리.
	//if (doc_modified)
	//	pDoc->SetModifiedFlag(true);

	SetTimer(timer_doc_modified, 10, NULL);
}

void CUXStudioView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == timer_doc_modified)
	{
		KillTimer(timer_doc_modified);
		
		int index = -1;
		
		if (m_selected_items.size())
			index = get_index(m_selected_items.back());

		((CMainFrame*)(AfxGetApp()->m_pMainWnd))->update_property(&m_selected_items, index);
	}

	CFormView::OnTimer(nIDEvent);
}

//item이 선택된 항목인지 판별한다.
bool CUXStudioView::is_selected(CSCUIElement* item)
{
	auto it = std::find(m_selected_items.begin(), m_selected_items.end(), item);
	return (it != m_selected_items.end());
}

//item의 iterator를 리턴한다. dq = NULL이면 pDoc->m_data를 대상으로 한다.
std::deque<CSCUIElement*>::iterator CUXStudioView::get_iterator(CSCUIElement* item, std::deque<CSCUIElement*> *dq)
{
	if (dq == NULL)
		dq = &pDoc->m_data;

	auto it = std::find(dq->begin(), dq->end(), item);
	return it;
}

//item의 index를 리턴한다. dq = NULL이면 pDoc->m_data를 대상으로 한다.
int CUXStudioView::get_index(CSCUIElement* item, std::deque<CSCUIElement*>* dq)
{
	if (dq == NULL)
		dq = &pDoc->m_data;

	auto it = std::find(dq->begin(), dq->end(), item);
	return std::distance(dq->begin(), it);
}

void CUXStudioView::OnMenuViewSort()
{
	//y기준으로 정렬하고 y가 동일할 경우는 x기준으로 정렬한다.
	//선택된 항목들이 있으면 선택된 항목들만, 없다면 전체 항목을 대상으로 한다.
	std::deque<CSCUIElement*>* dq = (m_selected_items.size() > 0 ? &m_selected_items : &pDoc->m_data);


	const int rowTolerance = 24;

	std::sort(dq->begin(), dq->end(),
		[&](const CSCUIElement* a, const CSCUIElement* b)
		{
			int dy = abs(a->m_r.Y - b->m_r.Y);

			// 같은 row 로 간주 → left 기준 정렬
			if (dy <= rowTolerance)
				return a->m_r.X < b->m_r.X;

			// 다른 row → top 기준 정렬
			return a->m_r.Y < b->m_r.Y;
		});

	Invalidate();
	push_undo();
}

void CUXStudioView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//이미지를 element에 drop하면 이미지 경로를 저장시킨다.
	TCHAR sfile[MAX_PATH] = { 0, };
	CPoint pt;

	DragQueryFile(hDropInfo, 0, sfile, MAX_PATH);
	DragQueryPoint(hDropInfo, &pt);

	if (get_filetype_from_filename(sfile) != FILE_TYPE_IMAGE)
		return;
	
	//drop된 element를 찾는다.
	adjust_scroll_offset(pt, false);

	auto res = std::find_if(pDoc->m_data.begin(), pDoc->m_data.end(),
		[&](const auto& el)
		{
			Gdiplus::RectF r = el->m_r;
			if (pt_in_rect(r, pt))
			{
				return el;
			}
		});

	if (res == pDoc->m_data.end())
		return;

	(*res)->m_image_path = sfile;
	(*res)->load_image(m_d2dc.get_WICFactory(), m_d2dc.get_d2dc());

	m_selected_items.clear();
	m_selected_items.push_back(*res);
	update_property();

	CFormView::OnDropFiles(hDropInfo);
}
