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
	DDX_Control(pDX, IDC_STATIC_X1, m_static_x1);
	DDX_Control(pDX, IDC_STATIC_Y1, m_static_y1);
	DDX_Control(pDX, IDC_STATIC_X2, m_static_x2);
	DDX_Control(pDX, IDC_STATIC_Y2, m_static_y2);
	DDX_Control(pDX, IDC_STATIC_W, m_static_w);
	DDX_Control(pDX, IDC_STATIC_H, m_static_h);
	DDX_Control(pDX, IDC_STATIC_FILL, m_static_fill);
	DDX_Control(pDX, IDC_STATIC_FILL_COLOR, m_static_fill_color);
	DDX_Control(pDX, IDC_STATIC_FILL_OPACITY, m_static_fill_opacity);
	DDX_Control(pDX, IDC_STATIC_FILL, m_static_fill);
	DDX_Control(pDX, IDC_STATIC_STROKE, m_static_stroke);
	DDX_Control(pDX, IDC_STATIC_STROKE_COLOR, m_static_stroke_color);
	DDX_Control(pDX, IDC_STATIC_STROKE_OPACITY, m_static_stroke_opacity);
	DDX_Control(pDX, IDC_STATIC_STROKE_THICKNESS, m_static_stroke_thickness);
	DDX_Control(pDX, IDC_STATIC_ROUND0, m_static_round0);
	DDX_Control(pDX, IDC_STATIC_ROUND1, m_static_round1);
	DDX_Control(pDX, IDC_STATIC_ROUND2, m_static_round2);
	DDX_Control(pDX, IDC_STATIC_ROUND3, m_static_round3);
	DDX_Control(pDX, IDC_STATIC_ROUND, m_static_round);
	DDX_Control(pDX, IDC_STATIC_CANVAS_SIZE, m_static_canvas_size);
	DDX_Control(pDX, IDC_STATIC_CANVAS_SIZE_CX, m_static_canvas_size_cx);
	DDX_Control(pDX, IDC_STATIC_CANVAS_SIZE_CY, m_static_canvas_size_cy);
	DDX_Control(pDX, IDC_STATIC_GRID_SIZE, m_static_grid_size);
	DDX_Control(pDX, IDC_STATIC_GRID_SIZE_CX, m_static_grid_size_cx);
	DDX_Control(pDX, IDC_STATIC_GRID_SIZE_CY, m_static_grid_size_cy);
	DDX_Control(pDX, IDC_COMBO_FONT, m_combo_font);
	DDX_Control(pDX, IDC_STATIC_FONT, m_static_font);
	DDX_Control(pDX, IDC_STATIC_FONT_SIZE, m_static_font_size);
	DDX_Control(pDX, IDC_STATIC_FONT_WEIGHT, m_static_font_weight);
	DDX_Control(pDX, IDC_CHECK_FONT_ITALIC, m_check_font_italic);
	DDX_Control(pDX, IDC_STATIC_TEXT_COLOR, m_static_text_color);
	DDX_Control(pDX, IDC_STATIC_TEXT_OPACITY, m_static_text_opacity);
	DDX_Control(pDX, IDC_RADIO_ALIGN_LEFT, m_radio_align_left);
	DDX_Control(pDX, IDC_RADIO_ALIGN_CENTER, m_radio_align_center);
	DDX_Control(pDX, IDC_RADIO_ALIGN_RIGHT, m_radio_align_right);
	DDX_Control(pDX, IDC_RADIO_VALIGN_TOP, m_radio_valign_top);
	DDX_Control(pDX, IDC_RADIO_VALIGN_CENTER, m_radio_valign_center);
	DDX_Control(pDX, IDC_RADIO_VALIGN_BOTTOM, m_radio_valign_bottom);
	DDX_Control(pDX, IDC_STATIC_TEXT_ALIGN, m_static_text_align);
	DDX_Control(pDX, IDC_STATIC_CANVAS_COLOR, m_static_canvas_color);
	DDX_Control(pDX, IDC_STATIC_GRID_COLOR, m_static_grid_color);
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
	ON_BN_CLICKED(IDC_CHECK_FONT_ITALIC, &CPropertyDlg::OnBnClickedCheckFontItalic)
	ON_CBN_SELCHANGE(IDC_COMBO_FONT, &CPropertyDlg::OnCbnSelchangeComboFont)
	ON_BN_CLICKED(IDC_RADIO_ALIGN_LEFT, &CPropertyDlg::OnBnClickedRadioAlignLeft)
	ON_BN_CLICKED(IDC_RADIO_ALIGN_CENTER, &CPropertyDlg::OnBnClickedRadioAlignCenter)
	ON_BN_CLICKED(IDC_RADIO_ALIGN_RIGHT, &CPropertyDlg::OnBnClickedRadioAlignRight)
	ON_BN_CLICKED(IDC_RADIO_VALIGN_TOP, &CPropertyDlg::OnBnClickedRadioVAlignTop)
	ON_BN_CLICKED(IDC_RADIO_VALIGN_CENTER, &CPropertyDlg::OnBnClickedRadioVAlignCenter)
	ON_BN_CLICKED(IDC_RADIO_VALIGN_BOTTOM, &CPropertyDlg::OnBnClickedRadioVAlignBottom)
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
	m_resize.Add(IDC_EDIT_text, 0, 0, 100, 0);

	m_theme.set_color_theme(CSCColorTheme::color_theme_dark_gray);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
*/

void CPropertyDlg::enable_window(bool enable)
{
	//EnableWindow(enable);

	/*
	if (!enable)
	{
		m_static_canvas_size.EnableWindow(TRUE);
		m_static_canvas_size_cx.EnableWindow(TRUE);
		m_static_canvas_size_cy.EnableWindow(TRUE);

		m_static_grid_size.EnableWindow(TRUE);
		m_static_grid_size_cx.EnableWindow(TRUE);
		m_static_grid_size_cy.EnableWindow(TRUE);
	}
	*/
	//CWnd* pWnd = &m_static_label;

	//while (pWnd)
	//{
	//	pWnd->EnableWindow(enable);
	//	pWnd = GetNextDlgTabItem(pWnd);
	//	if (pWnd == &m_static_canvas_size)
	//		break;
	//}
}

void CPropertyDlg::init_controls()
{
	m_resize.Create(this);

	m_resize.Add(IDC_STATIC_CANVAS_SIZE_CX, 0, 0, 25, 0);
	m_resize.Add(IDC_STATIC_CANVAS_SIZE_CY, 25, 0, 50, 0);
	m_resize.Add(IDC_STATIC_CANVAS_COLOR, 50, 0, 50, 0);

	m_resize.Add(IDC_STATIC_GRID_SIZE_CX, 0, 0, 25, 0);
	m_resize.Add(IDC_STATIC_GRID_SIZE_CY, 25, 0, 50, 0);
	m_resize.Add(IDC_STATIC_GRID_COLOR, 50, 0, 50, 0);

	m_resize.Add(IDC_STATIC_LABEL, 0, 0, 100, 0);
	m_resize.Add(IDC_STATIC_X1, 0, 0, 25, 0);
	m_resize.Add(IDC_STATIC_Y1, 25, 0, 25, 0);
	m_resize.Add(IDC_STATIC_X2, 50, 0, 25, 0);
	m_resize.Add(IDC_STATIC_Y2, 75, 0, 25, 0);
	m_resize.Add(IDC_STATIC_W, 0, 0, 50, 0);
	m_resize.Add(IDC_STATIC_H, 50, 0, 50, 0);

	m_resize.Add(IDC_STATIC_ROUND0, 0, 0, 25, 0);
	m_resize.Add(IDC_STATIC_ROUND1, 25, 0, 25, 0);
	m_resize.Add(IDC_STATIC_ROUND2, 50, 0, 25, 0);
	m_resize.Add(IDC_STATIC_ROUND3, 75, 0, 25, 0);

	m_resize.Add(IDC_COMBO_FONT, 0, 0, 100, 0);
	m_resize.Add(IDC_STATIC_FONT_SIZE, 0, 0, 50, 0);
	m_resize.Add(IDC_STATIC_FONT_WEIGHT, 50, 0, 50, 0);
	m_resize.Add(IDC_CHECK_FONT_ITALIC, 100, 0, 0, 0);

	m_resize.Add(IDC_STATIC_FILL_COLOR, 0, 0, 50, 0);
	m_resize.Add(IDC_STATIC_FILL_OPACITY, 50, 0, 50, 0);

	m_resize.Add(IDC_STATIC_STROKE_COLOR, 0, 0, 50, 0);
	m_resize.Add(IDC_STATIC_STROKE_OPACITY, 50, 0, 50, 0);
	m_resize.Add(IDC_STATIC_STROKE_THICKNESS, 50, 0, 50, 0);

	m_theme.set_color_theme(CSCColorTheme::color_theme_dark_gray);
	m_theme.cr_text = gGRAY(160);
	m_theme.cr_back = gGRAY(44);

	m_static_canvas_size.set_text_color(m_theme.cr_text);
	m_static_canvas_size.set_back_color(m_theme.cr_back);
	m_static_canvas_size.copy_properties(m_static_grid_size);
	m_static_canvas_size.copy_properties(m_static_text_align);
	m_static_canvas_size.copy_properties(m_static_font);
	m_static_canvas_size.copy_properties(m_static_fill);
	m_static_canvas_size.copy_properties(m_static_stroke);
	m_static_canvas_size.copy_properties(m_static_round);

	m_static_canvas_size_cx.set_text_color(m_theme.cr_text);
	m_static_canvas_size_cx.set_back_color(gGRAY(64));
	m_static_canvas_size_cx.set_prefix_space(2);
	m_static_canvas_size_cx.set_round(4, m_theme.cr_back, m_theme.cr_back);
	m_static_canvas_size_cx.set_use_edit();
	m_static_canvas_size_cx.set_edit_text_color(Gdiplus::Color::White);
	m_static_canvas_size_cx.copy_properties(m_static_canvas_size_cy);
	m_static_canvas_size_cx.copy_properties(m_static_canvas_color);
	m_static_canvas_size_cx.copy_properties(m_static_grid_color);

	m_static_canvas_size_cx.copy_properties(m_static_grid_size_cx);
	m_static_canvas_size_cx.copy_properties(m_static_grid_size_cy);

	m_static_canvas_size_cx.copy_properties(m_static_label);

	m_static_canvas_size_cx.copy_properties(m_static_x1);
	m_static_canvas_size_cx.copy_properties(m_static_y1);
	m_static_canvas_size_cx.copy_properties(m_static_x2);
	m_static_canvas_size_cx.copy_properties(m_static_y2);
	m_static_canvas_size_cx.copy_properties(m_static_w);
	m_static_canvas_size_cx.copy_properties(m_static_h);

	m_static_canvas_size_cx.copy_properties(m_static_fill_color);
	m_static_canvas_size_cx.copy_properties(m_static_fill_opacity);

	m_static_canvas_size_cx.copy_properties(m_static_stroke_color);
	m_static_canvas_size_cx.copy_properties(m_static_stroke_opacity);
	m_static_canvas_size_cx.copy_properties(m_static_stroke_thickness);

	m_static_canvas_size_cx.copy_properties(m_static_round0);
	m_static_canvas_size_cx.copy_properties(m_static_round1);
	m_static_canvas_size_cx.copy_properties(m_static_round2);
	m_static_canvas_size_cx.copy_properties(m_static_round3);

	m_combo_font.set_color_theme(m_theme.get_color_theme());
	m_combo_font.set_as_font_combo();
	m_combo_font.set_line_height(14);

	m_static_grid_size_cx.copy_properties(m_static_font_size);
	m_static_grid_size_cx.copy_properties(m_static_font_weight);
	m_check_font_italic.set_color(m_theme.cr_text, m_theme.cr_back, false);
	m_static_canvas_size_cx.copy_properties(m_static_text_color);
	m_static_canvas_size_cx.copy_properties(m_static_text_opacity);

	m_radio_align_left.add_image(IDB_TEXT_ALIGN_LEFT);
	m_radio_align_center.add_image(IDB_TEXT_ALIGN_CENTER);
	m_radio_align_right.add_image(IDB_TEXT_ALIGN_RIGHT);
	m_radio_valign_top.add_image(IDB_TEXT_ALIGN_TOP);
	m_radio_valign_center.add_image(IDB_TEXT_ALIGN_MIDDLE);
	m_radio_valign_bottom.add_image(IDB_TEXT_ALIGN_BOTTOM);

	enable_window(false);
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

void CPropertyDlg::set_canvas_property(int canvas_cx, int canvas_cy, Gdiplus::Color cr_canvas, int grid_cx, int grid_cy, Gdiplus::Color cr_grid)
{
	m_static_canvas_size_cx.set_text_value(i2S(canvas_cx));
	m_static_canvas_size_cy.set_text_value(i2S(canvas_cy));
	m_static_canvas_color.set_text_value(get_RGB_str(cr_canvas));

	m_static_grid_size_cx.set_text_value(i2S(grid_cx));
	m_static_grid_size_cy.set_text_value(i2S(grid_cy));
	m_static_grid_color.set_text_value(get_RGB_str(cr_grid));
}

LRESULT CPropertyDlg::on_message_CSCStatic(WPARAM wParam, LPARAM lParam)
{
	CSCStaticMsg* msg = (CSCStaticMsg*)wParam;

	if (msg->msg == CSCStaticMsg::msg_text_value_changed)
	{
		if (msg->pThis == &m_static_canvas_size_cx ||
			msg->pThis == &m_static_canvas_size_cy ||
			msg->pThis == &m_static_canvas_color ||
			msg->pThis == &m_static_grid_size_cx ||
			msg->pThis == &m_static_grid_size_cy ||
			msg->pThis == &m_static_grid_color)
		{
			int canvas_cx = _ttoi(m_static_canvas_size_cx.get_text_value());
			int canvas_cy = _ttoi(m_static_canvas_size_cy.get_text_value());
			Gdiplus::Color cr_canvas = get_color_from_token_str(m_static_canvas_color.get_text_value(), _T(", "));

			int grid_cx = _ttoi(m_static_grid_size_cx.get_text_value());
			int grid_cy = _ttoi(m_static_grid_size_cy.get_text_value());
			Gdiplus::Color cr_grid = get_color_from_token_str(m_static_grid_color.get_text_value(), _T(", "));

			((CUXStudioApp*)(AfxGetApp()))->apply_canvas_property_changed(canvas_cx, canvas_cy, cr_canvas, grid_cx, grid_cy, cr_grid);
			return 0;
		}

		if (!m_cur_items || m_cur_items->size() == 0)
			return 0;

		if (msg->sValue.IsEmpty())
			return 0;

		if (msg->pThis == &m_static_label)
		{
			update_all_values<CString>(m_cur_items, VAR_TO_CSTRING(m_text), msg->sValue);
		}
		//좌표값 또는 w, h를 변경하면 서로 영향을 주는데 이 때 기준은 좌표값을 유지시키도록 한다.
		//즉, x2를 변경하면 x1을 변경시키는 것이 아니라 width를 변경시켜준다.
		else if (msg->pThis == &m_static_x1)
		{
			//x1를 변경하면 x2을 변경하는 것이 아니라 width를 조정해준다.
			update_all_values<float>(m_cur_items, VAR_TO_CSTRING(m_r.X), _ttof(msg->sValue));
			m_static_w.set_text_value(_T("%.1f"), m_cur_items->at(0)->m_r.Width);
		}
		else if (msg->pThis == &m_static_y1)
		{
			//y1를 변경하면 y2을 변경하는 것이 아니라 height를 조정해준다.
			update_all_values<float>(m_cur_items, VAR_TO_CSTRING(m_r.Y), _ttof(msg->sValue));
			m_static_h.set_text_value(_T("%.1f"), m_cur_items->at(0)->m_r.Height);
		}
		else if (msg->pThis == &m_static_x2)
		{
			//x2를 변경하면 x1을 변경하는 것이 아니라 width를 조정해준다.
			//m_cur_items->m_r.Width = _ttof(msg->sValue) - m_cur_items->m_r.X;
			update_all_values<float>(m_cur_items, VAR_TO_CSTRING(m_r.X2), _ttof(msg->sValue));
			m_static_w.set_text_value(_T("%.1f"), m_cur_items->at(0)->m_r.Width);
		}
		else if (msg->pThis == &m_static_y2)
		{
			//y2를 변경하면 y1을 변경하는 것이 아니라 height를 조정해준다.
			//m_cur_items->m_r.Height = _ttof(msg->sValue) - m_cur_items->m_r.Y;
			update_all_values<float>(m_cur_items, VAR_TO_CSTRING(m_r.Y2), _ttof(msg->sValue));
			m_static_h.set_text_value(_T("%.1f"), m_cur_items->at(0)->m_r.Height);
		}
		else if (msg->pThis == &m_static_w)
		{
			//width를 변경하면 x2를 조정해준다.
			//m_cur_items->m_r.Width = _ttof(msg->sValue);
			update_all_values<float>(m_cur_items, VAR_TO_CSTRING(m_r.Width), _ttof(msg->sValue));
			m_static_x2.set_text_value(d2S(m_cur_items->at(0)->m_r.GetRight(), false, 1));
		}
		else if (msg->pThis == &m_static_h)
		{
			//height를 변경하면 y2를 조정해준다.
			//m_cur_items->m_r.Height = _ttof(msg->sValue);
			update_all_values<float>(m_cur_items, VAR_TO_CSTRING(m_r.Height), _ttof(msg->sValue));
			m_static_y2.set_text_value(d2S(m_cur_items->at(0)->m_r.GetBottom(), false, 1));
		}
		/*
		else if (msg->pThis == &m_static_round0)
			m_cur_items->m_round[0] = _ttof(msg->sValue);
		else if (msg->pThis == &m_static_round1)
			m_cur_items->m_round[1] = _ttof(msg->sValue);
		else if (msg->pThis == &m_static_round2)
			m_cur_items->m_round[2] = _ttof(msg->sValue);
		else if (msg->pThis == &m_static_round3)
			m_cur_items->m_round[3] = _ttof(msg->sValue);

		else if (msg->pThis == &m_static_fill_color)
		{
			m_cur_items->m_cr_fill = get_color_from_token_str(msg->sValue, _T(", "));
		}
		else if (msg->pThis == &m_static_fill_opacity)
		{
			set_color(m_cur_items->m_cr_fill, 0, _ttoi(msg->sValue));
		}

		else if (msg->pThis == &m_static_stroke_color)
		{
			m_cur_items->m_cr_stroke = get_color_from_token_str(msg->sValue, _T(", "));
		}
		else if (msg->pThis == &m_static_stroke_opacity)
		{
			set_color(m_cur_items->m_cr_stroke, 0, _ttoi(msg->sValue));
		}
		else if (msg->pThis == &m_static_stroke_thickness)
		{
			m_cur_items->m_stroke_thickness = _ttof(msg->sValue);
		}
		else if (msg->pThis == &m_static_text_color)
		{
			m_cur_items->m_cr_text = get_color_from_token_str(msg->sValue, _T(", "));
		}
		else if (msg->pThis == &m_static_text_opacity)
		{
			set_color(m_cur_items->m_cr_text, 0, _ttoi(msg->sValue));
		}
		else if (msg->pThis == &m_static_font_size)
		{
			m_cur_items->m_font_size = _ttoi(msg->sValue);
		}
		else if (msg->pThis == &m_static_font_weight)
		{
			int font_weight = _ttoi(msg->sValue);
			Validate(font_weight, (int)DWRITE_FONT_WEIGHT_THIN, (int)DWRITE_FONT_WEIGHT_ULTRA_BLACK, (int)DWRITE_FONT_WEIGHT_NORMAL);
			m_static_font_weight.set_text_value(i2S(font_weight));
			m_cur_items->m_font_weight = font_weight;
		}
		*/

		((CUXStudioApp*)(AfxGetApp()))->apply_changed_property(m_cur_items);
	}


	return 0;
}

void CPropertyDlg::set_property(std::deque<CSCUIElement*>* items)
{
	m_cur_items = items;

	m_radio_align_left.SetCheck(BST_UNCHECKED);
	m_radio_align_center.SetCheck(BST_UNCHECKED);
	m_radio_align_right.SetCheck(BST_UNCHECKED);
	m_radio_valign_top.SetCheck(BST_UNCHECKED);
	m_radio_valign_center.SetCheck(BST_UNCHECKED);
	m_radio_valign_bottom.SetCheck(BST_UNCHECKED);

	CSCUIElement el;
	CSCUIElement copied;

	Gdiplus::Color cr_unused(123, 234, 123, 234);

	if (items->size() > 0)
	{
		items->at(0)->copy(&el);
		items->at(0)->copy(&copied);
	}

	//2개 이상 멀티 선택되었을 때는 0번 항목의 각 변수값이 다른 값들과 하나라도 다르다면 해당 항목은 ""로 표시한다.
	for (int i = 1; i < items->size(); i++)
	{
		if (!el.m_text.IsEmpty() && el.m_text != items->at(i)->m_text)
			el.m_text = _T("");

		//다른 항목들과는 달리 m_r의 X, Y, Width, Height는 서로 영향이 있기 때문에 el의 값을 직접 변경하지 않고 copied를 이용해야 한다.
		//ex. Width를 비교할 때 X가 이미 FLT_MAX값으로 변경됐다면 올바른 Width값이 아니게 된다.
		if (copied.m_r.X != FLT_MAX && copied.m_r.X != items->at(i)->m_r.X)
			el.m_r.X = FLT_MAX;

		if (copied.m_r.Y != FLT_MAX && copied.m_r.Y != items->at(i)->m_r.Y)
			el.m_r.Y = FLT_MAX;

		if (copied.m_r.Width != FLT_MAX && copied.m_r.Width != items->at(i)->m_r.Width)
			el.m_r.Width = FLT_MAX;

		if (copied.m_r.Height != FLT_MAX && copied.m_r.Height != items->at(i)->m_r.Height)
			el.m_r.Height = FLT_MAX;

		for (int j = 0; j < 4; j++)
		{
			if (el.m_round[j] != FLT_MAX && el.m_round[j] != items->at(i)->m_round[j])
				el.m_round[j] = FLT_MAX;
		}

		if (!el.m_font_name.IsEmpty() && el.m_font_name != items->at(i)->m_font_name)
			el.m_font_name = _T("");

		if (el.m_font_weight != INT_MAX && el.m_font_weight != items->at(i)->m_font_weight)
			el.m_font_weight = -1;


		if (el.m_text_align != INT_MAX && el.m_text_align != items->at(i)->m_text_align)
			el.m_text_align = INT_MAX;

		if (el.m_text_valign != INT_MAX && el.m_text_valign != items->at(i)->m_text_valign)
			el.m_text_valign = INT_MAX;

		if (el.m_cr_fill.GetValue() != cr_unused.GetValue() && el.m_cr_fill.GetValue() != items->at(i)->m_cr_fill.GetValue())
			el.m_cr_fill = cr_unused;

		if (el.m_cr_stroke.GetValue() != cr_unused.GetValue() && el.m_cr_stroke.GetValue() != items->at(i)->m_cr_stroke.GetValue())
			el.m_cr_stroke = cr_unused;

		if (el.m_cr_text.GetValue() != cr_unused.GetValue() && el.m_cr_text.GetValue() != items->at(i)->m_cr_text.GetValue())
			el.m_cr_text = cr_unused;
	}

	if (items->size() > 0)
	{
		CString str;

		//items deque 목록에 있는 각 m_text를 검사하여 모두 동일한 값이면 해당값을 사용하고 동일하지 않으면 해당 항목을 "" 처리한다.
		m_static_label.set_text_value(el.m_text);

		m_static_x1.set_text_value(el.m_r.X == FLT_MAX ? _T("") : d2S(el.m_r.X, false, 1));
		m_static_y1.set_text_value(el.m_r.Y == FLT_MAX ? _T("") : d2S(el.m_r.Y, false, 1));
		m_static_x2.set_text_value((el.m_r.X == FLT_MAX || el.m_r.Width == FLT_MAX) ? _T("") : d2S(el.m_r.X + el.m_r.Width, false, 1));
		m_static_y2.set_text_value((el.m_r.Y == FLT_MAX || el.m_r.Height == FLT_MAX) ? _T("") : d2S(el.m_r.Y + el.m_r.Height, false, 1));
		m_static_w.set_text_value(el.m_r.Width == FLT_MAX ? _T("") : d2S(el.m_r.Width, false, 1));
		m_static_h.set_text_value(el.m_r.Height == FLT_MAX ? _T("") : d2S(el.m_r.Height, false, 1));

		m_static_round0.set_text_value(el.m_round[0] == FLT_MAX ? _T("") : d2S(el.m_round[0], false, 1));
		m_static_round1.set_text_value(el.m_round[1] == FLT_MAX ? _T("") : d2S(el.m_round[1], false, 1));
		m_static_round2.set_text_value(el.m_round[2] == FLT_MAX ? _T("") : d2S(el.m_round[2], false, 1));
		m_static_round3.set_text_value(el.m_round[3] == FLT_MAX ? _T("") : d2S(el.m_round[3], false, 1));

		switch (el.m_text_align)
		{
		case DWRITE_TEXT_ALIGNMENT_LEADING:
			m_radio_align_left.SetCheck(BST_CHECKED);
			break;
		case DWRITE_TEXT_ALIGNMENT_CENTER:
			m_radio_align_center.SetCheck(BST_CHECKED);
			break;
		case DWRITE_TEXT_ALIGNMENT_TRAILING:
			m_radio_align_right.SetCheck(BST_CHECKED);
			break;
		}

		switch (el.m_text_valign)
		{
		case DWRITE_PARAGRAPH_ALIGNMENT_NEAR:
			m_radio_valign_top.SetCheck(BST_CHECKED);
			break;
		case DWRITE_PARAGRAPH_ALIGNMENT_CENTER:
			m_radio_valign_center.SetCheck(BST_CHECKED);
			break;
		case DWRITE_PARAGRAPH_ALIGNMENT_FAR:
			m_radio_valign_bottom.SetCheck(BST_CHECKED);
			break;
		}

		//"color picker"일 때 현재 색상값을 text_color로 설정하면 그 색으로 사각형을 그린다.
		//그런데 만약 red 컬러인데 alpha값이 0일 경우, 투명하게 그려지게 되므로 문제가 된다.
		//a를 255로 강제 세팅해서 그려줘야 한다.
		Gdiplus::Color cr;


		cr = (el.m_cr_fill.GetValue() == cr_unused.GetValue() ? Gdiplus::Color::DimGray : el.m_cr_fill);
		set_color(cr, 0, 255);
		m_static_fill_color.set_text_color(cr);
		str = get_RGB_str(cr);
		m_static_fill_color.set_text_value(str);
		m_static_fill_opacity.set_text_value(i2S(el.m_cr_fill.GetA()));

		//stroke color 적용
		cr = (el.m_cr_stroke.GetValue() == cr_unused.GetValue() ? Gdiplus::Color::DimGray : el.m_cr_stroke);
		set_color(cr, 0, 255);
		m_static_stroke_color.set_text_color(cr);
		str = get_RGB_str(cr);
		m_static_stroke_color.set_text_value(str);
		m_static_stroke_opacity.set_text_value(i2S(el.m_cr_stroke.GetA()));
		m_static_stroke_thickness.set_text_value(i2S(el.m_stroke_thickness));

		//글자색 적용
		cr = (el.m_cr_text.GetValue() == cr_unused.GetValue() ? Gdiplus::Color::DimGray : el.m_cr_text);
		set_color(cr, 0, 255);
		m_static_text_color.set_text_color(cr);
		str = get_RGB_str(cr);
		m_static_text_color.set_text_value(str);
		m_static_text_opacity.set_text_value(i2S(el.m_cr_text.GetA()));

		//font
		m_static_font_size.set_text_value(i2S(el.m_font_size));
		m_static_font_weight.set_text_value(el.m_font_weight < DWRITE_FONT_WEIGHT_THIN ? _T("") : i2S(el.m_font_weight));
		m_check_font_italic.SetCheck(el.m_font_italic ? BST_CHECKED : BST_UNCHECKED);

		enable_window(true);
	}
	else
	{
		m_static_label.set_text_value();

		m_static_x1.set_text_value();
		m_static_y1.set_text_value();
		m_static_x2.set_text_value();
		m_static_y2.set_text_value();
		m_static_w.set_text_value();
		m_static_h.set_text_value();

		m_static_round0.set_text_value();
		m_static_round1.set_text_value();
		m_static_round2.set_text_value();
		m_static_round3.set_text_value();

		m_static_fill_color.set_text_color(Gdiplus::Color::Gray);
		m_static_fill_color.set_text_value();
		m_static_fill_opacity.set_text_value();

		m_static_stroke_color.set_text_color(Gdiplus::Color::Gray);
		m_static_stroke_color.set_text_value();
		m_static_stroke_opacity.set_text_value();
		m_static_stroke_thickness.set_text_value();

		m_static_text_color.set_text_color(Gdiplus::Color::Gray);
		m_static_text_color.set_text_value();
		m_static_text_opacity.set_text_value();

		enable_window(false);
	}
}

void CPropertyDlg::OnBnClickedCheckFontItalic()
{
	//m_cur_items->m_font_italic = (m_check_font_italic.GetCheck() == BST_CHECKED);
	//((CUXStudioApp*)(AfxGetApp()))->apply_changed_property(m_cur_items);
}

void CPropertyDlg::OnCbnSelchangeComboFont()
{
	//m_cur_items->m_font_name = m_combo_font.get_text();
	//((CUXStudioApp*)(AfxGetApp()))->apply_changed_property(m_cur_items);
}

void CPropertyDlg::OnBnClickedRadioAlignLeft()
{
	//m_cur_items->m_text_align = DWRITE_TEXT_ALIGNMENT_LEADING;
	//((CUXStudioApp*)(AfxGetApp()))->apply_changed_property(m_cur_items);
}

void CPropertyDlg::OnBnClickedRadioAlignCenter()
{
	//m_cur_items->m_text_align = DWRITE_TEXT_ALIGNMENT_CENTER;
	//((CUXStudioApp*)(AfxGetApp()))->apply_changed_property(m_cur_items);
}

void CPropertyDlg::OnBnClickedRadioAlignRight()
{
	//m_cur_items->m_text_align = DWRITE_TEXT_ALIGNMENT_TRAILING;
	//((CUXStudioApp*)(AfxGetApp()))->apply_changed_property(m_cur_items);
}

void CPropertyDlg::OnBnClickedRadioVAlignTop()
{
	//m_cur_items->m_text_valign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
	//((CUXStudioApp*)(AfxGetApp()))->apply_changed_property(m_cur_items);
}

void CPropertyDlg::OnBnClickedRadioVAlignCenter()
{
	//m_cur_items->m_text_valign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
	//((CUXStudioApp*)(AfxGetApp()))->apply_changed_property(m_cur_items);
}

void CPropertyDlg::OnBnClickedRadioVAlignBottom()
{
	//m_cur_items->m_text_valign = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
	//((CUXStudioApp*)(AfxGetApp()))->apply_changed_property(m_cur_items);
}
