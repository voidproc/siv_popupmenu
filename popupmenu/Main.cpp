#include "stdafx.h"
#include "resource.h"
#include "PopupMenu.h"

void Main()
{
	Scene::SetBackground(ColorF{ 0 });

	PopupMenu popupMenu{ IDR_MENU1 };

	while (System::Update())
	{
		if (MouseR.up())
		{
			popupMenu.show();
		}

		if (const auto selected = popupMenu.selectedItem(); selected)
		{
			Print << U"メニュー (ID={}) がクリックされた."_fmt(*selected);

			switch (*selected)
			{
			case ID_MENUITEM_APPEXIT:
				System::Exit();
				break;
			}
		}
	}
}
