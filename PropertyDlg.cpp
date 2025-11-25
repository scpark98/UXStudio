// PropertyDlg.cpp: 구현 파일
//

#include "pch.h"
#include "UXStudio.h"
#include "MainFrm.h"
#include "afxdialogex.h"
#include "PropertyDlg.h"

#include "Common/Functions.h"
#include "Common/MemoryDC.h"

// CPropertyDlg 대화 상자

IMPLEMENT_DYNAMIC(CPropertyDlg, CPaneDialog)

CPropertyDlg::CPropertyDlg(CWnd* pParent /*=nullptr*/)
	//: CPaneDialog()
{

}

CPropertyDlg::~CPropertyDlg()
{
}

void CPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CPaneDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_LABEL, m_static_label);
	DDX_Control(pDX, IDC_STATIC_X, m_static_x);
	DDX_Control(pDX, IDC_STATIC_Y, m_static_y);
	DDX_Control(pDX, IDC_STATIC_W, m_static_w);
	DDX_Control(pDX, IDC_STATIC_H, m_static_h);
	DDX_Control(pDX, IDC_STATIC_FILL, m_static_fill);
	DDX_Control(pDX, IDC_STATIC_FILL_COLOR, m_static_fill_color);
	DDX_Control(pDX, IDC_STATIC_FILL_OPACITY, m_static_fill_opacity);
}


BEGIN_MESSAGE_MAP(CPropertyDlg, CPaneDialog)
	//ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
	ON_BN_CLICKED(IDOK, &CPropertyDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPropertyDlg::OnBnClickedCancel)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(Message_CSCStatic, &CPropertyDlg::on_message_CSCStatic)
END_MESSAGE_MAP()


// CPropertyDlg 메시지 처리기
//CPaneDialog에는 기본 OnInitDialog() 핸들러가 없어서 수동으로 추가했으나
//이럴 경우 모든 컨트롤들의 핸들이 NULL인 상태가 된다.
//CPropertyDlg::OnCreate() 함수에서도 아직 NULL인 상태이므로 불가하고
//MainFrame에서 Create()후에 CPropertyDlg::init_controls()를 호출하여 초기화를 수행시켜야 한다.
/*
LRESULT CPropertyDlg::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_resize.Create(this);
	m_resize.Add(IDC_EDIT_LABEL, 0, 0, 100, 0);

	m_theme.set_color_theme(CSCColorTheme::color_theme_dark_gray);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
*/

void CPropertyDlg::init_controls()
{
	m_resize.Create(this);
	m_resize.Add(IDC_STATIC_LABEL, 0, 0, 100, 0);
	m_resize.Add(IDC_STATIC_X, 0, 0, 25, 0);
	m_resize.Add(IDC_STATIC_Y, 25, 0, 25, 0);
	m_resize.Add(IDC_STATIC_W, 50, 0, 25, 0);
	m_resize.Add(IDC_STATIC_H, 75, 0, 25, 0);
	m_resize.Add(IDC_STATIC_FILL_COLOR, 0, 0, 50, 0);
	m_resize.Add(IDC_STATIC_FILL_OPACITY, 50, 0, 50, 0);

	m_theme.set_color_theme(CSCColorTheme::color_theme_dark_gray);
	m_theme.cr_back = gGRAY(44);

	m_static_label.set_text_color(m_theme.cr_text);
	m_static_label.set_back_color(gGRAY(96));
	m_static_label.set_prefix_space(2);
	m_static_label.set_round(4, m_theme.cr_back, m_theme.cr_back);
	m_static_label.set_use_edit();
	m_static_label.set_edit_text_color(Gdiplus::Color::White);

	m_static_label.copy_properties(m_static_x);
	m_static_label.copy_properties(m_static_y);
	m_static_label.copy_properties(m_static_w);
	m_static_label.copy_properties(m_static_h);


	m_static_fill.set_text_color(m_theme.cr_text);
	m_static_fill.set_back_color(m_theme.cr_back);
	m_static_label.copy_properties(m_static_fill_color);
	m_static_label.copy_properties(m_static_fill_opacity);
}

void CPropertyDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CPropertyDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

BOOL CPropertyDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return FALSE;
	return CPaneDialog::OnEraseBkgnd(pDC);
}

void CPropertyDlg::OnPaint()
{
	CPaintDC dc1(this); // device context for painting
	CRect rc;

	GetClientRect(rc);
	CMemoryDC dc(&dc1, &rc);

	dc.FillSolidRect(rc, m_theme.cr_back.ToCOLORREF());
}

BOOL CPropertyDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	return CPaneDialog::PreTranslateMessage(pMsg);
}

int CPropertyDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPaneDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

void CPropertyDlg::OnSize(UINT nType, int cx, int cy)
{
	CPaneDialog::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

LRESULT CPropertyDlg::on_message_CSCStatic(WPARAM wParam, LPARAM lParam)
{
	//if (!m_item_cur)
	//	return 0;

	CSCStaticMsg* msg = (CSCStaticMsg*)wParam;

	if (msg->msg == CSCStaticMsg::msg_text_value_changed)
	{
		if (msg->pThis == &m_static_label)
			m_item_cur->m_label = msg->value;
		else if (msg->pThis == &m_static_x)
			m_item_cur->m_r.X = _ttof(msg->value);
		else if (msg->pThis == &m_static_y)
			m_item_cur->m_r.Y = _ttof(msg->value);
		else if (msg->pThis == &m_static_w)
			m_item_cur->m_r.Width = _ttof(msg->value);
		else if (msg->pThis == &m_static_h)
			m_item_cur->m_r.Height = _ttof(msg->value);
	}

	((CUXStudioApp*)(AfxGetApp()))->apply_changed_property(m_item_cur);

	return 0;
}

void CPropertyDlg::set_property(CSCUIElement* item)
{
	m_item_cur = item;

	if (item)
	{
		m_static_label.set_text_value(item->m_label);
		m_static_x.set_text_value(d2S(item->m_r.X, false, 1));
		m_static_y.set_text_value(d2S(item->m_r.Y, false, 1));
		m_static_w.set_text_value(d2S(item->m_r.Width, false, 1));
		m_static_h.set_text_value(d2S(item->m_r.Height, false, 1));
		m_static_fill_color.set_text_color(item->m_cr_fill);

		CString str;
		str.Format(_T("%d, %d, %d"), item->m_cr_fill.GetR(), item->m_cr_fill.GetG(), item->m_cr_fill.GetB());
		m_static_fill_color.set_text_value(str);
		m_static_fill_opacity.set_text_value(i2S(item->m_cr_fill.GetA()));

		EnableWindow(TRUE);
	}
	else
	{
		m_static_label.set_text_value(_T(""));
		m_static_x.set_text_value(_T(""));
		m_static_y.set_text_value(_T(""));
		m_static_w.set_text_value(_T(""));
		m_static_h.set_text_value(_T(""));
		m_static_fill_color.set_text_color(Gdiplus::Color::Transparent);
		m_static_fill_color.set_text_value(_T(""));
		m_static_fill_opacity.set_text_value(_T(""));

		EnableWindow(FALSE);
	}
}
