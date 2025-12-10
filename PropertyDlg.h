#pragma once
#include "afxdialogex.h"

#include "Common/ResizeCtrl.h"
#include "Common/CStatic/SCStatic/SCStatic.h"
#include "Common/CButton/GdiButton/GdiButton.h"
#include "Common/CComboBox/SCComboBox/SCComboBox.h"
#include "Common/data_types/CSCUIElement/SCUIElement.h"
// CPropertyDlg 대화 상자

class CPropertyDlg : public CPaneDialog
{
	DECLARE_DYNAMIC(CPropertyDlg)

public:
	CPropertyDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CPropertyDlg();

	//item을 선택해야 enable되지만 canvas_size, grid_size는 항상 enable이어야한다.
	void				enable_window(bool enable);

	void				init_controls();
	CResizeCtrl			m_resize;
	CSCColorTheme		m_theme = CSCColorTheme(this);

	void				set_canvas_property(int canvas_cx, int canvas_cy, Gdiplus::Color cr_canvas, int grid_cx, int grid_cy, Gdiplus::Color cr_grid);

	std::deque<CSCUIElement*>* m_cur_items = NULL;
	void				set_property(std::deque<CSCUIElement*>* items);

	LRESULT				on_message_CSCStatic(WPARAM wParam, LPARAM lParam);

	//items의 특정 멤버를 value값으로 변경해야 하는데 c++에서는 정식 방법을 지원하지 않는다.
	//할 수 없이 아래와 같이 변수명을 문자열로 주고 해당 변수명에 따라 처리한다.
	template <class T> void	update_all_values(std::deque<CSCUIElement*>* items, CString member, T value)
	{
		for (auto it = items->begin(); it != items->end(); ++it)
		{
			if (member == _T("m_text"))
			{
				CString* str = reinterpret_cast<CString*>(&value);
				(*it)->m_text = *str;
			}
			else if (member == _T("m_r.X"))
			{
				float* f = reinterpret_cast<float*>(&value);
				(*it)->m_r.X = *f;
				(*it)->m_r.Width = (*it)->m_r.GetRight() - (*it)->m_r.X;
			}
			else if (member == _T("m_r.Y"))
			{
				float* f = reinterpret_cast<float*>(&value);
				(*it)->m_r.Y = *f;
				(*it)->m_r.Height = (*it)->m_r.GetBottom() - (*it)->m_r.Y;
			}
			else if (member == _T("m_r.X2"))
			{
				float* f = reinterpret_cast<float*>(&value);
				(*it)->m_r.Width = *f - (*it)->m_r.X;
			}
			else if (member == _T("m_r.Y2"))
			{
				float* f = reinterpret_cast<float*>(&value);
				(*it)->m_r.Height = *f - (*it)->m_r.Y;
			}
			else if (member == _T("m_r.Width"))
			{
				//float* f = reinterpret_cast<float*>(&value);
				(*it)->m_r.Width = *(reinterpret_cast<float*>(&value));
			}
			else if (member == _T("m_r.Height"))
			{
				float* f = reinterpret_cast<float*>(&value);
				(*it)->m_r.Height = *(reinterpret_cast<float*>(&value));
			}
			else if (member == _T("m_round[0]"))
			{
				//float* f = reinterpret_cast<float*>(&value);
				(*it)->m_round[0] = *(reinterpret_cast<float*>(&value));
			}
			else if (member == _T("m_round[1]"))
			{
				//float* f = reinterpret_cast<float*>(&value);
				(*it)->m_round[1] = *(reinterpret_cast<float*>(&value));
			}
			else if (member == _T("m_round[2]"))
			{
				//float* f = reinterpret_cast<float*>(&value);
				(*it)->m_round[2] = *(reinterpret_cast<float*>(&value));
			}
			else if (member == _T("m_round[3]"))
			{
				//float* f = reinterpret_cast<float*>(&value);
				(*it)->m_round[3] = *(reinterpret_cast<float*>(&value));
			}
			else if (member == _T("fill_color"))
			{
				CString* str = reinterpret_cast<CString*>(&value);
				(*it)->m_cr_fill = get_color_from_token_str(*str, _T(", "));
			}
			else if (member == _T("fill_opacity"))
			{
				int* alpha = reinterpret_cast<int*>(&value);
				set_color((*it)->m_cr_fill, 0, *alpha);
			}
			else if (member == _T("stroke_color"))
			{
				CString* str = reinterpret_cast<CString*>(&value);
				(*it)->m_cr_stroke = get_color_from_token_str(*str, _T(", "));
			}
			else if (member == _T("stroke_opacity"))
			{
				int* alpha = reinterpret_cast<int*>(&value);
				set_color((*it)->m_cr_stroke, 0, *alpha);
			}
			else if (member == _T("stroke_thickness"))
			{
				(*it)->m_stroke_thickness = *(reinterpret_cast<float*>(&value));
			}
			else if (member == _T("text_color"))
			{
				CString* str = reinterpret_cast<CString*>(&value);
				(*it)->m_cr_text = get_color_from_token_str(*str, _T(", "));
			}
			else if (member == _T("text_opacity"))
			{
				int* alpha = reinterpret_cast<int*>(&value);
				set_color((*it)->m_cr_text, 0, *alpha);
			}
			else if (member == _T("font_size"))
			{
				(*it)->m_font_size = *(reinterpret_cast<int*>(&value));
			}
			else if (member == _T("font_weight"))
			{
				(*it)->m_font_weight = *(reinterpret_cast<int*>(&value));
			}
			else if (member == _T("font_italic"))
			{
				(*it)->m_font_italic = *(reinterpret_cast<int*>(&value));
			}
			else if (member == _T("font_name"))
			{
				(*it)->m_font_name = *(reinterpret_cast<CString*>(&value));
			}
			else if (member == _T("text_align"))
			{
				(*it)->m_text_align = *(reinterpret_cast<int*>(&value));
			}
			else if (member == _T("text_valign"))
			{
				(*it)->m_text_valign = *(reinterpret_cast<int*>(&value));
			}
		}
	}


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPERTY };
#endif

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
	{
		CPaneDialog::OnUpdateCmdUI(pTarget, FALSE);
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	//afx_msg LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CSCStatic m_static_label;
	CSCStatic m_static_x1;
	CSCStatic m_static_y1;
	CSCStatic m_static_x2;
	CSCStatic m_static_y2;
	CSCStatic m_static_w;
	CSCStatic m_static_h;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CSCStatic m_static_fill;
	CSCStatic m_static_fill_color;
	CSCStatic m_static_fill_opacity;
	CSCStatic m_static_stroke;
	CSCStatic m_static_stroke_color;
	CSCStatic m_static_stroke_opacity;
	CSCStatic m_static_stroke_thickness;
	CSCStatic m_static_round;
	CSCStatic m_static_round0;
	CSCStatic m_static_round1;
	CSCStatic m_static_round2;
	CSCStatic m_static_round3;
	CSCStatic m_static_canvas_size;
	CSCStatic m_static_canvas_size_cx;
	CSCStatic m_static_canvas_size_cy;
	CSCStatic m_static_grid_size;
	CSCStatic m_static_grid_size_cx;
	CSCStatic m_static_grid_size_cy;
	CSCComboBox m_combo_font;
	CSCStatic m_static_font;
	CSCStatic m_static_font_size;
	CSCStatic m_static_font_weight;
	CGdiButton m_check_font_italic;
	CSCStatic m_static_text_color;
	afx_msg void OnBnClickedCheckFontItalic();
	CSCStatic m_static_text_opacity;
	afx_msg void OnCbnSelchangeComboFont();
	afx_msg void OnBnClickedRadioAlignLeft();
	afx_msg void OnBnClickedRadioAlignCenter();
	afx_msg void OnBnClickedRadioAlignRight();
	afx_msg void OnBnClickedRadioVAlignTop();
	afx_msg void OnBnClickedRadioVAlignCenter();
	afx_msg void OnBnClickedRadioVAlignBottom();
	CSCStatic m_static_text_align;
	CGdiButton m_radio_align_left;
	CGdiButton m_radio_align_center;
	CGdiButton m_radio_align_right;
	CGdiButton m_radio_valign_top;
	CGdiButton m_radio_valign_center;
	CGdiButton m_radio_valign_bottom;
	CSCStatic m_static_canvas_color;
	CSCStatic m_static_grid_color;
};
