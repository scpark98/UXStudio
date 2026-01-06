
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
#include <format>

#include "Common/office/xlnt/include/xlnt/xlnt.hpp"

#include "Common/Functions.h"
#include "Common/Json/rapid_json/json.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CUXStudioDoc

IMPLEMENT_DYNCREATE(CUXStudioDoc, CDocument)

BEGIN_MESSAGE_MAP(CUXStudioDoc, CDocument)
	ON_COMMAND(ID_FILE_SAVE, &CUXStudioDoc::OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, &CUXStudioDoc::OnFileSaveAs)
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
		//if (m_data[i]->m_image)
		//{
		//	delete m_data[i]->m_image;
		//}
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
	m_sz_canvas.cy = json.get("canvas_cy", 1080);
	m_cr_canvas = json.get("cr_canvas", (UINT)(Gdiplus::Color::White));

	m_sz_grid.cx = json.get("grid_cx", 8);
	m_sz_grid.cy = json.get("grid_cy", 8);
	m_cr_grid = json.get("cr_grid", (UINT)(Gdiplus::Color::LightGray));

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

		normalize_rect(el->m_r);

		rapidjson::Value& round = items[i]["round"];
		el->m_round[0] = round[0].GetFloat();
		el->m_round[1] = round[1].GetFloat();
		el->m_round[2] = round[2].GetFloat();
		el->m_round[3] = round[3].GetFloat();

		el->m_text = json.get_array_member("items", i, "label", CString());
		el->m_image_path = json.get_array_member("items", i, "image_path", CString());
		el->m_text_align = json.get_array_member("items", i, "label_align", (int)DWRITE_TEXT_ALIGNMENT_CENTER);
		el->m_text_valign = json.get_array_member("items", i, "label_valign", (int)DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		el->m_text_visible = json.get_array_member("items", i, "label_visible", true);
		el->m_cr_text = Gdiplus::Color(json.get_array_member("items", i, "cr_text", (UINT)(Gdiplus::Color::Black)));
		el->m_cr_back = Gdiplus::Color(json.get_array_member("items", i, "cr_back", (UINT)(Gdiplus::Color::Transparent)));

		el->m_font_name = _T("나눔스퀘어 Bold");// json.get_array_member("items", i, "font_name", "Arial");
		el->m_font_size = json.get_array_member("items", i, "font_size", 10);
		int font_weight = json.get_array_member("items", i, "font_weight", (int)DWRITE_FONT_WEIGHT_BOLD);
		Validate(font_weight, (int)DWRITE_FONT_WEIGHT_THIN, (int)DWRITE_FONT_WEIGHT_ULTRA_BLACK, (int)DWRITE_FONT_WEIGHT_NORMAL);
		el->m_font_weight = font_weight;
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
	if (get_part(lpszPathName, fn_ext).MakeLower() == _T("json"))
		save_as_json(lpszPathName);
	else if (get_part(lpszPathName, fn_ext).MakeLower() == _T("txt"))
		save_as_txt(lpszPathName);
	else if (get_part(lpszPathName, fn_ext).MakeLower() == _T("xlsx"))
		save_as_xlsx(lpszPathName);
	else
		AfxMessageBox(_T("not yet supported format. only json or txt available."));

	return TRUE;
}

//txt 파일로 저장하되 구분자는 '|' 기호를 사용한다.
void CUXStudioDoc::save_as_txt(CString filepath)
{
	FILE* fp = NULL;
	_tfopen_s(&fp, filepath, _T("wt")CHARSET);

	_ftprintf(fp, _T("canvas size : %d x %d\n"), m_sz_canvas.cx, m_sz_canvas.cy);
	_ftprintf(fp, _T("cr_canvas : %u (%s)\n"), (UINT)m_cr_canvas.GetValue(), get_color_str(m_cr_canvas));

	_ftprintf(fp, _T("grid size : %d x %d\n"), m_sz_grid.cx, m_sz_grid.cy);
	_ftprintf(fp, _T("cr_grid : %u (%s)\n\n"), (UINT)m_cr_grid.GetValue(), get_color_str(m_cr_grid));

	_ftprintf(fp, _T("index|type|selected|x1|y1|x2|y2|width|height|gapx_prec|gapy_prec|round0|round1|round2|round3|")
				  _T("label|image_path|label_align|label_valign|label_visible|cr_text|cr_text_argb|cr_back|cr_back_argb|")
				  _T("font_name|font_size|font_weight|font_italic|stroke_thickness|cr_stroke|cr_stroke_argb|cr_fill|cr_fill_argb\n"));

	for (int i = 0; i < m_data.size(); i++)
	{
		_ftprintf(fp, _T("%d|%d|%d|%.0f|%0.f|%.0f|%.0f|%.0f|%.0f|%.0f|%.0f|%.0f|%.0f|%.0f|%.0f|"),
			i, m_data[i]->m_type, m_data[i]->m_selected, m_data[i]->m_r.X, m_data[i]->m_r.Y, m_data[i]->m_r.GetRight(), m_data[i]->m_r.GetBottom(),
			m_data[i]->m_r.Width, m_data[i]->m_r.Height,
			(i == 0 ? 0.0f : m_data[i]->m_r.X - m_data[i - 1]->m_r.GetRight()),		//이전 X2와의 거리
			(i == 0 ? 0.0f : m_data[i]->m_r.Y - m_data[i - 1]->m_r.GetBottom()),	//이전 Y2와의 거리
			m_data[i]->m_round[0], m_data[i]->m_round[1], m_data[i]->m_round[2], m_data[i]->m_round[3]);
		_ftprintf(fp, _T("%s|%s|%d|%d|%d|%u|%s|%u|%s|"),
			m_data[i]->m_text, m_data[i]->m_image_path, m_data[i]->m_text_align, m_data[i]->m_text_valign, m_data[i]->m_text_visible,
			m_data[i]->m_cr_text, get_color_str(m_data[i]->m_cr_text), m_data[i]->m_cr_back, get_color_str(m_data[i]->m_cr_back));
		_ftprintf(fp, _T("%s|%d|%d|%d|%.1f|%u|%s|%u|%s\n"),
			m_data[i]->m_font_name, m_data[i]->m_font_size, m_data[i]->m_font_weight, m_data[i]->m_font_italic, m_data[i]->m_stroke_thickness,
			m_data[i]->m_cr_stroke, get_color_str(m_data[i]->m_cr_stroke), m_data[i]->m_cr_fill, get_color_str(m_data[i]->m_cr_fill));
	}

	fclose(fp);
}

//저장에 문제는 없으나 xlnt가 최신 버전이 아니라서인지 memory leak이 발생한다.
//https://luckygg.tistory.com/235의 의견으로는 False memory일 수 있다고 한다.
//다만 셀에 한글을 입력하면 정상적으로 저장되지 않는 치명적인 문제가 있다.
void CUXStudioDoc::save_as_xlsx(CString filepath)
{
	//해당 파일이 열려있을 경우 에러가 발생하므로 미리 확인.
	FILE* fp = NULL;
	_tfopen_s(&fp, filepath, _T("wb"));
	if (fp == NULL)
	{
		AfxMessageBox(filepath + _T("\n\n다른 프로세스에 의해 열려있으므로 파일을 저장할 수 없습니다.\n파일을 닫은 후 다시 시도해주세요."));
		return;
	}
	fclose(fp);

	xlnt::workbook wb;
	xlnt::worksheet ws = wb.active_sheet();

	ws.cell("A1").value("sz_canvas");
	ws.cell("B1").value(string_format("%d x %d", m_sz_canvas.cx, m_sz_canvas.cy));
	ws.cell("A2").value("cr_canvas");
	ws.cell("B2").value(string_format("%u", m_cr_canvas.GetValue()));
	ws.cell("C2").value(string_format("%s", get_color_str(m_cr_canvas)));

	ws.cell("A3").value("sz_grid");
	ws.cell("B3").value(string_format("%d x %d", m_sz_grid.cx, m_sz_grid.cy));
	ws.cell("A4").value("cr_grid");
	ws.cell("B4").value(string_format("%u", m_cr_grid.GetValue()));
	ws.cell("C4").value(string_format("%s", get_color_str(m_cr_grid)));

	//33 columns
	CString title = _T("index|type|selected|x1|y1|x2|y2|width|height|gapx_prec|gapy_prec|round0|round1|round2|round3|")
					_T("label|image_path|label_align|label_valign|label_visible|cr_text|cr_text_argb|cr_back|cr_back_argb|")
					_T("font_name|font_size|font_weight|font_italic|stroke_thickness|cr_stroke|cr_stroke_argb|cr_fill|cr_fill_argb");

	int i;
	std::deque<CString> token;
	get_token_str(title, token, _T("|"), false);
	std::string cell_pos;
	std::string sstr;

	//6th 라인에 컬럼 타이틀 표시
	for (i = 0; i < token.size(); i++)
	{
		//std::string cell_pos = std::format("{:s}6", get_excel_column(i).c_str());
		cell_pos = string_format("%s6", get_excel_column(i).c_str());
		ws.cell(cell_pos).value(CString2string(token[i]));
	}

	//틀고정
	ws.freeze_panes("A7");

	//7th 라인부터 실제 데이터 출력
	int data_line_start = 7;
	for (i = 0; i < m_data.size(); i++)
	{
		cell_pos = string_format("A%d", data_line_start);
		ws.cell(cell_pos).value(i);

		cell_pos = string_format("B%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_type);

		cell_pos = string_format("C%d", data_line_start);
		ws.cell(cell_pos).value((int)(m_data[i]->m_selected));

		cell_pos = string_format("D%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_r.X);

		cell_pos = string_format("E%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_r.Y);

		cell_pos = string_format("F%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_r.GetRight());

		cell_pos = string_format("G%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_r.GetBottom());

		cell_pos = string_format("H%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_r.Width);

		cell_pos = string_format("I%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_r.Height);

		//현재 x와 바로 이전 항목의 right 간격 크기
		cell_pos = string_format("J%d", data_line_start);
		ws.cell(cell_pos).value(i == 0 ? 0.0f : (m_data[i]->m_r.X - m_data[i - 1]->m_r.GetRight()));

		//현재 y와 바로 이전 항목의 bottom 간격 크기
		cell_pos = string_format("K%d", data_line_start);
		ws.cell(cell_pos).value(i == 0 ? 0.0f : (m_data[i]->m_r.Y - m_data[i - 1]->m_r.GetBottom()));

		cell_pos = string_format("L%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_round[0]);

		cell_pos = string_format("M%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_round[1]);

		cell_pos = string_format("N%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_round[2]);

		cell_pos = string_format("O%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_round[3]);

		if (false)//m_data[i]->m_text.IsEmpty() == false)
		{
			sstr = CString2string(m_data[i]->m_text);
			cell_pos = string_format("P%d", data_line_start);
			ws.cell(cell_pos).value(sstr);
		}

		sstr = CString2string(m_data[i]->m_image_path);
		cell_pos = string_format("Q%d", data_line_start);
		ws.cell(cell_pos).value(sstr.c_str());

		cell_pos = string_format("R%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_text_align);

		cell_pos = string_format("S%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_text_valign);

		cell_pos = string_format("T%d", data_line_start);
		ws.cell(cell_pos).value((int)(m_data[i]->m_text_visible));

		cell_pos = string_format("U%d", data_line_start);
		ws.cell(cell_pos).value((UINT)(m_data[i]->m_cr_text.GetValue()));

		cell_pos = string_format("V%d", data_line_start);
		ws.cell(cell_pos).value(CString2string(get_color_str(m_data[i]->m_cr_text)).c_str());

		cell_pos = string_format("W%d", data_line_start);
		ws.cell(cell_pos).value((UINT)(m_data[i]->m_cr_back.GetValue()));

		cell_pos = string_format("X%d", data_line_start);
		ws.cell(cell_pos).value(CString2string(get_color_str(m_data[i]->m_cr_back)).c_str());

		cell_pos = string_format("Y%d", data_line_start);
		//ws.cell(cell_pos).value(CString2string(m_data[i]->m_font_name).c_str());

		cell_pos = string_format("Z%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_font_size);

		cell_pos = string_format("AA%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_font_weight);

		cell_pos = string_format("AB%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_font_italic);

		cell_pos = string_format("AC%d", data_line_start);
		ws.cell(cell_pos).value(m_data[i]->m_stroke_thickness);

		cell_pos = string_format("AD%d", data_line_start);
		ws.cell(cell_pos).value((UINT)(m_data[i]->m_cr_stroke.GetValue()));

		cell_pos = string_format("AE%d", data_line_start);
		ws.cell(cell_pos).value(CString2string(get_color_str(m_data[i]->m_cr_stroke)).c_str());

		cell_pos = string_format("AF%d", data_line_start);
		ws.cell(cell_pos).value((UINT)(m_data[i]->m_cr_fill.GetValue()));

		cell_pos = string_format("AG%d", data_line_start);
		ws.cell(cell_pos).value(CString2string(get_color_str(m_data[i]->m_cr_fill)).c_str());

		data_line_start++;
	}

	wb.save(CString2string(filepath));
	//wb.save("d:\\result.xlsx");
	SHELL_OPEN(filepath);
}

void CUXStudioDoc::save_as_json(CString filepath)
{
	Json json;
	rapidjson::Document::AllocatorType& allocator = json.doc.GetAllocator();

	json.doc.AddMember("canvas_cx", m_sz_canvas.cx, allocator);
	json.doc.AddMember("canvas_cy", m_sz_canvas.cy, allocator);
	json.doc.AddMember("cr_canvas", (UINT)m_cr_canvas.GetValue(), allocator);

	json.doc.AddMember("grid_cx", m_sz_grid.cx, allocator);
	json.doc.AddMember("grid_cy", m_sz_grid.cy, allocator);
	json.doc.AddMember("cr_grid", (UINT)m_cr_grid.GetValue(), allocator);

	rapidjson::Value items(rapidjson::kArrayType);

	m_filepath = filepath;

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
		std::string sstr = CString2string(m_data[i]->m_text);
		item.AddMember(rapidjson::Value("label", allocator).Move(), sstr, allocator);
		sstr = CT2CA(m_data[i]->m_image_path);
		item.AddMember(rapidjson::Value("image_path", allocator).Move(), sstr, allocator);

		item.AddMember(rapidjson::Value("label_align", allocator).Move(), m_data[i]->m_text_align, allocator);
		item.AddMember(rapidjson::Value("label_valign", allocator).Move(), m_data[i]->m_text_valign, allocator);
		item.AddMember(rapidjson::Value("label_visible", allocator).Move(), m_data[i]->m_text_visible, allocator);
		item.AddMember(rapidjson::Value("cr_text", allocator).Move(), (UINT)m_data[i]->m_cr_text.GetValue(), allocator);
		item.AddMember(rapidjson::Value("cr_back", allocator).Move(), (UINT)m_data[i]->m_cr_back.GetValue(), allocator);

		sstr = CT2CA(m_data[i]->m_font_name);
		item.AddMember(rapidjson::Value("font_name", allocator).Move(), sstr, allocator);
		item.AddMember(rapidjson::Value("font_size", allocator).Move(), m_data[i]->m_font_size, allocator);
		item.AddMember(rapidjson::Value("font_weight", allocator).Move(), m_data[i]->m_font_weight, allocator);
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

	return;// TRUE;
	//return CDocument::OnSaveDocument(lpszPathName);
}

void CUXStudioDoc::OnFileSave()
{
	if (m_filepath.IsEmpty())
	{
		m_filepath.Format(_T("%s\\UXStudio.json"), get_exe_directory());
	}

	OnSaveDocument(m_filepath);
}

void CUXStudioDoc::OnFileSaveAs()
{
	if (m_filepath.IsEmpty())
	{
		m_filepath.Format(_T("%s\\UXStudio.json"), get_exe_directory());
	}

	CString filter = _T("Json Files (*.json)|*.json|Text Files (*.txt)|*.txt|Excel 파일(*.xlsx)|*.xlsx|All Files (*.*)|*.*||");
	CFileDialog dlg(FALSE, _T("json"), m_filepath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDCANCEL)
		return;

	CString filepath = dlg.GetPathName();
	OnSaveDocument(filepath);

	//json으로 저장할 경우에만 현재 문서 경로를 변경해준다.
	if (get_part(filepath, fn_ext).MakeLower() == _T("json"))
		SetPathName(filepath);
}
