#include "../fault_deformation.h"

#include "../utf8_printf.hpp"
int main( int argc, char** argv)
{
 // ƒƒ‚ƒŠƒŠ[ƒNŒŸo
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	if ( argc < 2 )
	{
		return 1;
	}

#if 0
	{
		std::vector<accelerator> accs = accelerator::get_all();
		for (int i = 0; i < accs.size(); i++)
		{
			printf("[%d]description:%s\n", i, WStringToString(accs[i].description).c_str());
			printf("[%d]device_path:%s\n", i, WStringToString(accs[i].device_path).c_str());
			printf("[%d]dedicated_memory:%d\n", i, accs[i].dedicated_memory);
			printf("[%d]supports_double_precision:%s\n\n",
				i, accs[i].supports_double_precision ? "true" : "false");
		}

		accelerator default_acc;
		printf("default_acc.description:%s\n", WStringToString(default_acc.description).c_str());
		printf("default_acc.device_path:%s\n", WStringToString(default_acc.device_path).c_str());
		printf("default_acc.dedicated_memory:%d\n", default_acc.dedicated_memory);
		printf("default_acc.supports_double_precision:%s\n\n",
			default_acc.supports_double_precision ? "true" : "false");
	}
#endif

	return fault_deformation(argv[1]);
}
