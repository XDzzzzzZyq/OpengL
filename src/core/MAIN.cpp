
#include "Application.h"

int main()
{
	auto& app = Application::Get();

	app.Init();
	app.Run();
	app.Terminate();

	return 0;
}
