#pragma once

// Siv3D (v0.6.x) アプリで Windows のポップアップメニュー（コンテキストメニュー）を表示する
class PopupMenu
{
public:
	PopupMenu(WORD menuResourceId);

	~PopupMenu();

	// メニューを表示する
	void show();

	// クリックされたメニュー項目があれば、そのリソースIDを返す
	Optional<WORD> selectedItem() const;

private:
	WORD menuResourceId_;
};
