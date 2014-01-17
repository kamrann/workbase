#include <Polycode.h>
#include "nn_testing.h"
#include "PolycodeView.h"
#include "windows.h"

using namespace Polycode;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	PolycodeView *view = new PolycodeView(hInstance, nCmdShow, L"Bot NN Tester");
//	PolycodeView *graph_view = new PolycodeView(hInstance, nCmdShow, L"Bot NN Tester Graph");
	nn_testing_app *app = new nn_testing_app(view, nullptr);//graph_view);

	MSG Msg;
	do {
		if(PeekMessage(&Msg, NULL, 0,0,PM_REMOVE)) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	} while(app->Update());
	return Msg.wParam;
}