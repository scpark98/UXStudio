#pragma once
#include "afxdialogex.h"

#include "Common/CEdit/SCEdit/SCEdit.h"

// CMoveIndexDlg 대화 상자

class CMoveIndexDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMoveIndexDlg)

public:
	CMoveIndexDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMoveIndexDlg();

	int			m_cur_index = -1;
	int			m_new_index = -1;
	int			m_end_index = -1;
	int			m_method = -1;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOVE_INDEX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_static_description;
	CSCEdit m_edit_new_index;
	CSCEdit m_edit_end_index;
	afx_msg void OnBnClickedRadioOnlyCurrent();
	afx_msg void OnBnClickedRadioIncludeFollows();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeEditNewIndex();
};
