#pragma once
#include "afxdialogex.h"

#include "Common/ResizeCtrl.h"
#include "Common/CStatic/SCStatic/SCStatic.h"
#include "Common/data_types/CSCUIElement/SCUIElement.h"
// CPropertyDlg 대화 상자

class CPropertyDlg : public CPaneDialog
{
	DECLARE_DYNAMIC(CPropertyDlg)

public:
	CPropertyDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CPropertyDlg();

	void				init_controls();
	CResizeCtrl			m_resize;
	CSCColorTheme		m_theme = CSCColorTheme(this);

	CSCUIElement*		m_item_cur = NULL;
	void				set_property(CSCUIElement* item);

	LRESULT				on_message_CSCStatic(WPARAM wParam, LPARAM lParam);

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
	CSCStatic m_static_x;
	CSCStatic m_static_y;
	CSCStatic m_static_w;
	CSCStatic m_static_h;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
