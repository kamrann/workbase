#include <Polycode.h>
#include "manual_thrusters_test.h"
#include "PolycodeView.h"
#include "windows.h"

using namespace Polycode;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	PolycodeView *view = new PolycodeView(hInstance, nCmdShow, L"Bot NN Tester");
	manual_thrusters_test_app *app = new manual_thrusters_test_app(view);

	MSG Msg;
	do {
		if(PeekMessage(&Msg, NULL, 0,0,PM_REMOVE)) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	} while(app->Update());
	return Msg.wParam;
}