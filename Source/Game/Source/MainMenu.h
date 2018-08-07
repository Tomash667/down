#pragma once

class MainMenu
{
public:
	enum Action
	{
		Action_Continue,
		Action_LoadGame,
		Action_NewGame,
		Action_Options,
		Action_Website,
		Action_Quit
	};

private:
	void OnEvent(int id);
};
