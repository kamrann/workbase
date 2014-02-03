#include <Polycode.h>
#include "draw_graph_app.h"
#include "PolycodeView.h"

#include <windows.h>
#include <tchar.h>

using namespace Polycode;


int _tmain(int argc, _TCHAR* argv[])
{
	return _tWinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOW);
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	PolycodeView *view = new PolycodeView(hInstance, nCmdShow, L"Draw Graph");
	draw_graph_app *app = new draw_graph_app(view);

	MSG Msg;
	do
	{
		if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
	while(app->Update());
	
	return Msg.wParam;
}