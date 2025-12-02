
// UXStudioDoc.cpp: CUXStudioDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "UXStudio.h"
#endif

#include "UXStudioDoc.h"

#include <propkey.h>

#include "Common/Functions.h"
#include "Common/Json/rapid_json/json.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CUXStudioDoc

IMPLEMENT_DYNCREATE(CUXStudioDoc, CDocument)

BEGIN_MESSAGE_MAP(CUXStudioDoc, CDocument)
	ON_COMMAND(ID_FILE_SAVE, &CUXStudioDoc::OnFileSave)
END_MESSAGE_MAP()


// CUXStudioDoc 생성/소멸

CUXStudioDoc::CUXStudioDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CUXStudioDoc::~CUXStudioDoc()
{
	release_data();
}

BOOL CUXStudioDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CUXStudioDoc serialization

void CUXStudioDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CUXStudioDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CUXStudioDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CUXStudioDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CUXStudioDoc 진단

#ifdef _DEBUG
void CUXStudioDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CUXStudioDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CUXStudioDoc 명령
void CUXStudioDoc::release_data()
{
	for (int i = 0; i < m_data.size(); i++)
	{
		delete m_data[i];
	}

	m_data.clear();
}

BOOL CUXStudioDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	//if (!CDocument::OnOpenDocument(lpszPathName))
	//	return FALSE;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_filepath = lpszPathName;

	release_data();

	Json json;
	json.load(m_filepath);
	json.get_all_data();

	SetTitle(m_filepath);

	AfxGetApp()->WriteProfileString(_T("setting"), _T("recent file"), m_filepath);

	m_sz_canvas.cx = json.get("canvas_cx", 1920);
	m_sz_canvas.cy = json.get("canvas_cy", 1920);

	m_sz_grid.cx = json.get("grid_cx", 8);
	m_sz_grid.cy = json.get("grid_cy", 8);

	rapidjson::Value& items = json.doc["items"];

	for (int i = 0; i < items.Size(); i++)
	{
		CSCUIElement* el = new CSCUIElement;

		el->m_type = json.get_array_member("items", i, "type", 0);
		el->m_selected = json.get_array_member("items", i, "selected", false);


		rapidjson::Value& r = items[i]["r"];
		el->m_r.X = r[0].GetFloat();
		el->m_r.Y = r[1].GetFloat();
		el->m_r.Width = r[2].GetFloat();
		el->m_r.Height = r[3].GetFloat();

		rapidjson::Value& round = items[i]["round"];
		el->m_round[0] = round[0].GetFloat();
		el->m_round[1] = round[1].GetFloat();
		el->m_round[2] = round[2].GetFloat();
		el->m_round[3] = round[3].GetFloat();

		el->m_text = json.get_array_member("items", i, "label", CString());
		//TRACE(_T("%s\n"), CString(json.get_array_member("items", i, "label", std::string("")).c_str()));
		//TRACE(_T("%S\n"), json.get_array_member("items", i, "label", ""));
		el->m_text_align = json.get_array_member("items", i, "label_align", (int)DWRITE_TEXT_ALIGNMENT_CENTER);
		el->m_text_valign = json.get_array_member("items", i, "label_valign", (int)DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		el->m_text_visible = json.get_array_member("items", i, "label_visible", true);
		el->m_cr_text = Gdiplus::Color(json.get_array_member("items", i, "cr_text", (UINT)(Gdiplus::Color::Black)));
		el->m_cr_back = Gdiplus::Color(json.get_array_member("items", i, "cr_back", (UINT)(Gdiplus::Color::Transparent)));

		el->m_font_name = json.get_array_member("items", i, "font_name", "Arial");
		el->m_font_size = json.get_array_member("items", i, "font_size", 10);
		el->m_font_bold = json.get_array_member("items", i, "font_bold", false);
		el->m_font_italic = json.get_array_member("items", i, "font_italic", false);

		el->m_stroke_thickness = json.get_array_member("items", i, "stroke_thickness", 1.0f);
		el->m_cr_stroke = Gdiplus::Color(json.get_array_member("items", i, "cr_stroke", (UINT)(Gdiplus::Color::RoyalBlue)));
		el->m_cr_fill = Gdiplus::Color(json.get_array_member("items", i, "cr_fill", (UINT)(Gdiplus::Color::Transparent)));

		m_data.push_back(el);
	}

	UpdateAllViews(NULL);

	return TRUE;
}

BOOL CUXStudioDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	Json json;
	rapidjson::Document::AllocatorType& allocator = json.doc.GetAllocator();

	json.doc.AddMember("canvas_cx", m_sz_canvas.cx, allocator);
	json.doc.AddMember("canvas_cy", m_sz_canvas.cy, allocator);

	json.doc.AddMember("grid_cx", m_sz_grid.cx, allocator);
	json.doc.AddMember("grid_cy", m_sz_grid.cy, allocator);

	rapidjson::Value items(rapidjson::kArrayType);

	m_filepath = lpszPathName;

	for (int i = 0; i < m_data.size(); i++)
	{
		rapidjson::Value item;
		item.SetObject();

		item.AddMember("type", m_data[i]->m_type, allocator);
		item.AddMember(rapidjson::Value("selected", allocator).Move(), m_data[i]->m_selected, allocator);

		rapidjson::Value r(rapidjson::kArrayType);
		r.PushBack(m_data[i]->m_r.X, allocator);
		r.PushBack(m_data[i]->m_r.Y, allocator);
		r.PushBack(m_data[i]->m_r.Width, allocator);
		r.PushBack(m_data[i]->m_r.Height, allocator);
		item.AddMember("r", r, allocator);

		//item.AddMember(rapidjson::Value("round", allocator).Move(), m_data[i]->m_round, allocator);
		rapidjson::Value round(rapidjson::kArrayType);
		round.PushBack(m_data[i]->m_round[0], allocator);
		round.PushBack(m_data[i]->m_round[1], allocator);
		round.PushBack(m_data[i]->m_round[2], allocator);
		round.PushBack(m_data[i]->m_round[3], allocator);
		item.AddMember("round", round, allocator);


		//CT2CA(m_data[i]->m_text)을 직접 파라미터로 넘기면 컴파일 에러가 발생한다.
		std::string sstr = CT2CA(m_data[i]->m_text);
		item.AddMember(rapidjson::Value("label", allocator).Move(), sstr, allocator);
		item.AddMember(rapidjson::Value("label_align", allocator).Move(), m_data[i]->m_text_align, allocator);
		item.AddMember(rapidjson::Value("label_valign", allocator).Move(), m_data[i]->m_text_valign, allocator);
		item.AddMember(rapidjson::Value("label_visible", allocator).Move(), m_data[i]->m_text_visible, allocator);
		item.AddMember(rapidjson::Value("cr_text", allocator).Move(), (UINT)m_data[i]->m_cr_text.GetValue(), allocator);
		item.AddMember(rapidjson::Value("cr_back", allocator).Move(), (UINT)m_data[i]->m_cr_back.GetValue(), allocator);

		sstr = CT2CA(m_data[i]->m_font_name);
		item.AddMember(rapidjson::Value("font_name", allocator).Move(), sstr, allocator);
		item.AddMember(rapidjson::Value("font_size", allocator).Move(), m_data[i]->m_font_size, allocator);
		item.AddMember(rapidjson::Value("font_bold", allocator).Move(), m_data[i]->m_font_bold, allocator);
		item.AddMember(rapidjson::Value("font_italic", allocator).Move(), m_data[i]->m_font_italic, allocator);

		item.AddMember(rapidjson::Value("stroke_thickness", allocator).Move(), m_data[i]->m_stroke_thickness, allocator);
		item.AddMember(rapidjson::Value("cr_stroke", allocator).Move(), (UINT)m_data[i]->m_cr_stroke.GetValue(), allocator);
		item.AddMember(rapidjson::Value("cr_fill", allocator).Move(), (UINT)m_data[i]->m_cr_fill.GetValue(), allocator);

		//하나의 item에 모든 항목값이 채워지면 전체 array인 items에 넣는다.
		items.PushBack(item, allocator);
	}

	json.doc.AddMember("items", items, allocator);
	json.get_all_data();

	json.save(m_filepath);
	AfxGetApp()->WriteProfileString(_T("setting"), _T("recent file"), m_filepath);

	return TRUE;
	return CDocument::OnSaveDocument(lpszPathName);
}

void CUXStudioDoc::OnFileSave()
{
	if (m_filepath.IsEmpty())
	{
		m_filepath.Format(_T("%s\\UXStudio.json"), get_exe_directory());
	}

	OnSaveDocument(m_filepath);
}
