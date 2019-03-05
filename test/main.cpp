#include <iostream>
#include "GLoader.hpp"

using fct = int	(*)(int i);

int	main()
{
	GLoader	gl;

	#ifdef _WIN32
	std::cout << gl.load<fct>("./libtest.dll", "test")(10) << std::endl;
	#else
	std::cout << gl.load<fct>("./libtest.so", "test")(10) << std::endl;
	#endif
	return 0;
}