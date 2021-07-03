#include "tu_solver.h"
#include <exception>

int main( int argc, char** argv)
{
	// ƒƒ‚ƒŠƒŠ[ƒNŒŸo
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	if ( argc < 2 )
	{
		return 1;
	}

	int stat = 0;
	try
	{
		stat = tu_solver(argv[1]);
	}catch( const char* msg)
	{
		printf("exception:%s\n", msg);
		stat = -1;
	}catch(exception& ex)
	{
		printf("exception:%s\n", ex.what());
		stat = -1;
	}catch(...)
	{
		printf("exception\n");
		stat = -1;
	}
	return 0;
}


