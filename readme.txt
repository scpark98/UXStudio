[수정될 내용]
- 순서 변경

[멀티선택 구현방식]
1. 각 el의 is_selected flag를 저장하는 방법
- 선택항목은 선택되었음을 그려준다.
- m_r_selected를 구하고 이는 최대 사각형을 의미한다.

2. std::vector<CSCUIElement*> m_item_selected;
- 선택된 항목들을 vector로 저장한다.
- 선택항목은 선택되었음을 그려준다.
- m_r_selected를 구하고 이는 최대 사각형을 의미한다.

- OnMouseMove()에서는 m_r_selected안에 있으면 반응하지 않는다.

[향후 수정될 내용]
- 편집 내용이 바로 서버에 저장되고 공유한 사용자 화면에 바로 적용해야 한다. (broadcast)
- 버전 관리 (repository, branch)