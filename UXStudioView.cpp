
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

	for (int i = 0; i < m_data.size(); i++)
	{
		CSCUIElement* el = m_data[i];
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
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), m_br_element.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::RoyalBlue), m_br_hover.GetAddressOf());
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::RoyalBlue), m_br_selected.GetAddressOf());

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


	if (m_lbutton_down)
		d2dc->DrawRectangle(D2D1::RectF(m_pt_lbutton_down.x, m_pt_lbutton_down.y, m_pt_cur.x, m_pt_cur.y), m_br_draw.Get());

	for (int i = 0; i < m_data.size(); i++)
	{
		CSCUIElement* el = m_data[i];
		Gdiplus::RectF r = el->m_r;

		r.Offset(-hs, -vs);

		D2D1_RECT_F rf = { r.X, r.Y, r.GetRight(), r.GetBottom() };

		if (el->m_cr_fill.GetValue() != Gdiplus::Color::Transparent)
		{
			m_br_element->SetColor(get_d2color(el->m_cr_fill));
			d2dc->FillRectangle(rf, m_br_element.Get());
		}

		if (el == m_el_selected)
		{
			d2dc->DrawRectangle(rf, m_br_selected.Get(), (el == m_el_hover ? 2.0f : 1.0f));
			get_resizable_handle(r, m_resize_handle, 3);
			draw_resize_handle(d2dc);
		}
		else if (el == m_el_hover)
		{
			d2dc->DrawRectangle(rf, m_br_hover.Get(), 2.0f);
		}
		else if (el->m_cr_stroke.GetValue() != Gdiplus::Color::Transparent)
		{
			m_br_element->SetColor(get_d2color(el->m_cr_stroke));
			d2dc->DrawRectangle(rf, m_br_element.Get());
		}
	}

	HRESULT hr = d2dc->EndDraw();

	if (SUCCEEDED(hr))
		hr = m_d2dc.get_swapchain()->Present(0, 0);

	//if (m_lbutton_down)
	//	draw_line(pDC, m_pt_lbutton_down, m_pt_cur, red, 1, PS_SOLID, R2_XORPEN);
	//else
	//	draw_line(pDC, m_pt_lbutton_down, m_pt_cur, red, 1, PS_SOLID);

	//d2gdi->ReleaseDC(rc);
}

void CUXStudioView::draw_resize_handle(ID2D1DeviceContext* d2dc)
{
	//0번은 inside이므로 생략한다.
	for (int i = 1; i < RECT_RESIZE_HANDLE_COUNT; i++)
	{
		CRect r = m_resize_handle[i];
		draw_rect(d2dc, CRect2GpRectF(r), Gdiplus::Color::RoyalBlue, Gdiplus::Color::White);
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
	//CRect rc;
	//GetClientRect(rc);
	//m_d2dc.on_size_changed(rc.Width(), rc.Height());
	Invalidate();
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CUXStudioView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//CRect rc;
	//GetClientRect(rc);
	//m_d2dc.on_size_changed(rc.Width(), rc.Height());
	Invalidate();
	CFormView::OnVScroll(nSBCode, nPos, pScrollBar);
}

CPoint CUXStudioView::get_near_grid(CPoint pt)
{
	CPoint res = pt;
	res.Offset(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
	res.x = (res.x / m_sz_grid.cx) * m_sz_grid.cx;
	res.y = (res.y / m_sz_grid.cy) * m_sz_grid.cy;

	return res;
}

void CUXStudioView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//hover인 항목을 클릭하면 편집모드로 전환된다.
	CPoint pt = point;
	pt.Offset(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));

	if (m_el_hover && m_el_hover->pt_in_rect(pt.x, pt.y))
	{
		m_el_selected = m_el_hover;
		get_resizable_handle(m_el_selected->m_r, m_resize_handle, 3);
		Invalidate();
		return;
	}

	m_lbutton_down = true;
	m_pt_lbutton_down = get_near_grid(point);
	SetCapture();

	CFormView::OnLButtonDown(nFlags, point);
}

void CUXStudioView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_lbutton_down)
	{
		m_lbutton_down = false;
		ReleaseCapture();

		point = get_near_grid(point);
		Gdiplus::RectF r(m_pt_lbutton_down.x, m_pt_lbutton_down.y, point.x - m_pt_lbutton_down.x, point.y - m_pt_lbutton_down.y);
		normalize_rect(r);

		if (r.Width < 20 && r.Height < 20)
			return;

		r.Offset(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
		m_data.push_back(new CSCUIElement(r));
		Invalidate();
	}

	CFormView::OnLButtonUp(nFlags, point);
}

void CUXStudioView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPoint pt = point;
	pt.Offset(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
	((CMainFrame*)(AfxGetApp()->m_pMainWnd))->set_cursor_info(pt);

	if (m_lbutton_down)
	{
		m_pt_cur = get_near_grid(point);
		Invalidate();
	}
	else
	{
		m_el_hover = get_hover_item(pt);
		TRACE(_T("hover = %p\n"), m_el_hover);
		Invalidate();
	}

	CFormView::OnMouseMove(nFlags, point);
}

CSCUIElement* CUXStudioView::get_hover_item(CPoint pt)
{
	m_el_hover = NULL;
	
	auto res = std::find_if(m_data.begin(), m_data.end(),
		[&](const auto& el)
		{
			if (pt_in_rect(el->m_r, pt))
			{
				return el;
			}
		});

	if (res != m_data.end())
		m_el_hover = *res;

	return m_el_hover;
}
