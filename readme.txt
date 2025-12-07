[수정될 내용]
- 순서 변경
- 방향키로 위치, 크기 변경
- canvas color or image
- 순서 정렬 기능 : left -> top 순서 정렬기능이 있어야 이웃한 항목 순서로 출력할 수 있다.

[수정된 내용]

[정렬]
- 어떤 항목이 다른 항목의 안에 존재하면 그 항목을 앞으로 보낸다.
  포함관계가 없다면 left 순서로, left < m 이하라면 y <= n 이면 순서대로

- 포함 관계가 없다면 왼쪽에 있는 항목이 우선한다.
  단, y <= n인 경우에만 적용되며 그 이상인 경우는 같은 그룹 항목이 아니다.

[멀티선택 구현방식]
1. 각 el의 is_selected flag를 저장하는 방법 (figma 방식)
- 선택항목은 선택되었음을 그려준다.
- 선택정보 또한 저장할 수 있다. vector 방식도 선택여부를 저장할 수 있다.(save시에 flag 세팅)
- m_r_selected를 구하고 이는 최대 사각형을 의미한다.
- move, resize시에 모든 루프를 돌면서 선택된 항목에 대해서만 처리하므로 매번 모든 루프를 돌아야 한다.

2. std::vector<CSCUIElement*> m_item_selected;
- 선택된 항목들을 vector로 저장한다. (ppt 방식)
- 선택 항목은 선택되었음을 그려준다.
- m_r_selected? 를 구하고 이는 최대 사각형을 의미한다.

- OnMouseMove()에서는 m_r_selected안에 있으면 반응하지 않는다.

[자석기능]
- move/resize시에 다른 항목과의 자동 정렬을 지원해야 한다.
- l, t, r, b, cp, canvas_cp 등 어디에 일치하는지를 기억해야 한다.
- a라는 항목의 l, t에 일치한다면 이를 기억해서 OnDraw()에서 그려줘야 한다.

[undo/redo]
- m_data의 복사본을 vector에 저장해서 불러오는 방식으로 구현

[속성창]
- label
- x y w h
- round (indivisual 클릭 시 각각 조절 가능)
- stroke color, alpha, width, style
- fill color, alpha, pattern

[스크롤 보정]
- 도형을 그릴때도 스크롤 보정을 하고 그려야 한다. 그래야만 grid를 따라 그릴 수 있다.
- 그려진 후에도 스크롤 보정된 좌표를 vector에 추가한다.
- 

[향후 수정될 내용]
- 편집 내용이 바로 서버에 저장되고 공유한 사용자 화면에 바로 적용해야 한다. (broadcast)
- 버전 관리 (repository, branch)