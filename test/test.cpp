
#include <iostream>

extern "C" {
	#ifdef _WIN32
	__declspec( dllexport ) int	test(int i)
	#else
	int	test(int i)
	#endif
	{
		std::cout << "Hello, your number is " << i << std::endl;
		return 1;
	}
}