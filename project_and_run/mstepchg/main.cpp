#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>


int main( int argc, char** argv)
{

	int number = 0;
	int i = 0;
	char fname[256];
	char fname2[256];
	char IDname[128];

	const char prjname[2][32] = {"Orthogonal", "Perspective"};

	if ( argc < 2 )
	{
		printf("mstepchg.exe Orthogonal:0/Perspective:1 [IDname]\n");
		return -1;
	}

	int s = atoi(argv[1]);
	if ( s < 0 || s > 1 ) return -1;

	if ( argc == 3 )
	{
		strcpy(IDname, argv[2]);
	}else
	{
		FILE* fp = fopen("CalculationParameters.txt", "r");
		if ( fp == NULL )
		{
			printf("mstepchg.exe Orthogonal:0/Perspective:1 [IDname]\n");
			return -1;
		}

		IDname[0] = '\0';
		char buf[256];
		while( fgets(buf, 256, fp) != NULL )
		{
			if ( strncmp(buf, "ID\n", 3) == 0 )
			{
				fgets(buf, 255, fp);
				strcpy(IDname, buf);
				char* p = strchr(IDname, '\n');
				if ( p ) *p = '\0';
			}
		}
	}
	if ( IDname[0] == '\0' )
	{
		printf("mstepchg.exe Orthogonal:0/Perspective:1 [IDname]\n");
		return -1;
	}

	i = 0;
	while ( 1 )
	{
		sprintf(fname, "Scenario\\TSUNAMI_Viewr_%s_%s_%03d.sav", IDname, prjname[s], i);
		FILE* fp = fopen(fname, "r");
		if ( fp )
		{
			fclose(fp);
			if ( i % 2 == 0 )
			{
				printf("[%d]%s\n", i, fname);
				fclose(fp);
				sprintf(fname2, "Scenario\\TSUNAMI_Viewr_%s_%s_%03d.sav_", IDname, prjname[s], number);
				MoveFileA(fname, fname2);
				number++;
			}else
			{
				_unlink(fname);
			}
		}else
		{
			break;
		}
		i++;
	}

	for ( int k = 0; k < number; k++ )
	{
		sprintf(fname, "Scenario\\TSUNAMI_Viewr_%s_%s_%03d.sav_", IDname, prjname[s], k);
		sprintf(fname2, "Scenario\\TSUNAMI_Viewr_%s_%s_%03d.sav", IDname, prjname[s], k);
		MoveFileA(fname, fname2);
	}


	return 0;
}
