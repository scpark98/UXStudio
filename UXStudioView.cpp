
// UXStudioView.cpp: CUXStudioView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "UXStudio.h"
#endif

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
END_MESSAGE_MAP()

// CUXStudioView 생성/소멸

CUXStudioView::CUXStudioView() noexcept
	: CFormView(IDD_UXSTUDIO_FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CUXStudioView::~CUXStudioView()
{
	SAFE_RELEASE(m_brush);
	SAFE_RELEASE(m_br_grid);
	SAFE_RELEASE(m_WriteFactory);
	SAFE_RELEASE(m_WriteFormat);
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

	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_brush);
	m_d2dc.get_d2dc()->CreateSolidColorBrush(D2D1::ColorF(.5f, .5f, .5f, 0.6f), &m_br_grid);
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

	//CMemoryDC dc(pDC, &rc);

	ID2D1DeviceContext* d2dc = m_d2dc.get_d2dc();

	D2D1_SIZE_F sz_dc = m_d2dc.get_size();

	d2dc->BeginDraw();
	d2dc->SetTransform(D2D1::Matrix3x2F::Identity());

	int ix = 10;
	int iy = 10;
	int x, y;

	for (x = ix; x < sz_dc.width; x += ix)
	{
		for (y = iy; y < sz_dc.height; y += iy)
		{
			d2dc->FillRectangle(D2D1::RectF(x - 1, y - 1, x + 1, y + 1), m_br_grid);
		}
	}

	HRESULT hr = d2dc->EndDraw();

	if (SUCCEEDED(hr))
		hr = m_d2dc.get_swapchain()->Present(0, 0);


	pDC->MoveTo(rc.TopLeft());
	pDC->LineTo(rc.BottomRight());

	draw_line(pDC, 0, 0, rc.right, rc.bottom, red);
}

void CUXStudioView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_d2dc.get_d2dc())
		return;

	m_d2dc.get_d2dc()->SetTarget(nullptr);

	m_d2dc.get_swapchain()->ResizeBuffers(
			0,
			cx,
			cy,
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0
		);

	Invalidate();
}

BOOL CUXStudioView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return TRUE;
	return CFormView::OnEraseBkgnd(pDC);
}
