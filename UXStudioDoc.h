
// UXStudioDoc.h: CUXStudioDoc 클래스의 인터페이스
//


#pragma once

#include <vector>
#include <deque>
#include "Common/data_types/CSCUIElement/SCUIElement.h"


class CUXStudioDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CUXStudioDoc() noexcept;
	DECLARE_DYNCREATE(CUXStudioDoc)

// 특성입니다.
public:
	CString							m_filepath;

	CSize							m_sz_canvas = CSize(1920, 1080);
	CSize							m_sz_grid = CSize(8, 8);
	Gdiplus::Color					m_cr_canvas = Gdiplus::Color::White;
	Gdiplus::Color					m_cr_grid = Gdiplus::Color::LightGray;

	std::deque<CSCUIElement*>		m_data;

// 작업입니다.
public:
	void							release_data();
	void							save_as_txt(CString filepath);
	void							save_as_json(CString filepath);
	void							save_as_xlsx(CString filepath);

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CUXStudioDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
};
