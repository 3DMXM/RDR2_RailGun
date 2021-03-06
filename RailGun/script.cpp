#include "script.h"

#include "scriptmenu.h"
#include "keyboard.h"

#include "RailGun.h"




void main()
{
	auto menuController = new MenuController();
	auto mainMenu = CreateMainMenu(menuController);

	while (true)
	{
		if (!menuController->HasActiveMenu() && MenuInput::MenuSwitchPressed())
		{
			MenuInput::MenuInputBeep();
			menuController->PushMenu(mainMenu);
		}
		menuController->Update();
		WAIT(0);
	}
}

void ScriptMain()
{
	srand(GetTickCount());
	main();
}