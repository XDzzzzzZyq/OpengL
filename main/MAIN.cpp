
#include "Application.h"

int main()
{
	Application* app = Application::Get();

	app->Init();
	app->Run();
	app->Terminate();

	return 0;
}
