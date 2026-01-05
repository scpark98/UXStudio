// MoveIndexDlg.cpp: 구현 파일
//

#include "pch.h"
#include "UXStudio.h"
#include "afxdialogex.h"
#include "MoveIndexDlg.h"


// CMoveIndexDlg 대화 상자

IMPLEMENT_DYNAMIC(CMoveIndexDlg, CDialogEx)

CMoveIndexDlg::CMoveIndexDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MOVE_INDEX, pParent)
{

}

CMoveIndexDlg::~CMoveIndexDlg()
{
}

void CMoveIndexDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NEW_INDEX, m_edit_new_index);
	DDX_Control(pDX, IDC_STATIC_DESCRIPTION, m_static_description);
	DDX_Control(pDX, IDC_EDIT_END_INDEX, m_edit_end_index);
}


BEGIN_MESSAGE_MAP(CMoveIndexDlg, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_ONLY_CURRENT, &CMoveIndexDlg::OnBnClickedRadioOnlyCurrent)
	ON_BN_CLICKED(IDC_RADIO_INCLUDE_FOLLOWS, &CMoveIndexDlg::OnBnClickedRadioIncludeFollows)
	ON_BN_CLICKED(IDOK, &CMoveIndexDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMoveIndexDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CMoveIndexDlg 메시지 처리기

BOOL CMoveIndexDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CString str;

	str.Format(_T("현재 선택된 항목의 인덱스(%d)를 새 인덱스로 변경합니다.\n이후 항목들의 인덱스도 일괄적으로 변경할 수 있습니다."),
			m_cur_index);
	m_static_description.SetWindowTextW(str);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMoveIndexDlg::OnBnClickedRadioOnlyCurrent()
{
	m_method = 0;
}

void CMoveIndexDlg::OnBnClickedRadioIncludeFollows()
{
	m_method = 1;
}

void CMoveIndexDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString text = m_edit_new_index.get_text();

	text.Trim();
	if (text.IsEmpty())
		return;

	m_new_index = _ttoi(text);


	text = m_edit_end_index.get_text();
	text.Trim();
	if (!text.IsEmpty())
		m_end_index = _ttoi(text);

	CDialogEx::OnOK();
}

void CMoveIndexDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}
