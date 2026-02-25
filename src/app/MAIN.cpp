
#include "Application.h"

#include <stdexcept>
#include <iostream>

int main()
{
	try
	{
		Application app;
		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}
