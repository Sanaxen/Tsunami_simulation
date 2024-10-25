#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <vector>


int main( int argc, char** argv)
{
	if ( argc < 3 )
	{
		return -1;
	}

	char path[256];
	char baseName[256];
	strcpy(path, argv[1]);
	strcpy(baseName, argv[2]);

	int startnum = 0;
	int num = 0;
	int step = 1;

	char filename[256];
	sprintf(filename, "%s\\elevation_data.obj", path);
	FILE* fp = fopen(filename, "r");
	if ( fp == NULL )
	{
		return -1;
	}
	printf("obj2pov.exe \"%s\" %s.tmp %s\n", filename, "elevation_data", "elevation_data");
	fclose(fp);
	


	sprintf(filename, "%s\\water_depth_data.obj", path);
	fp = fopen(filename, "r");
	if ( fp == NULL )
	{
		return -1;
	}
	printf("obj2pov.exe \"%s\" %s.tmp %s\n", filename, "water_depth_data", "water_depth_data");
	fclose(fp);

	int step2 = 1;
	int step_stat = -1;
	while(1)
	{
		char filename[256];
		sprintf(filename, "%s\\%s_%06d.obj", path, baseName, num);
		FILE* fp = fopen(filename, "r");
		if ( fp == NULL )
		{
			if ( step_stat == 0 ) break;
			step2++;
			num++;
			continue;
		}
		if ( num > 0 )
		{
			step = step2;
			step_stat = 0;
		}


		printf("obj2pov.exe \"%s\" 0_tmp %06d\n", filename, num);
		fclose(fp);

		printf("type %s.tmp > tmp\n", "elevation_data");
		printf("type %s.tmp >> tmp\n", "water_depth_data");
		printf("type 0_tmp >> tmp\n");
		printf("del 0_tmp\n");

		printf("type mesh_model__begin.txt > pov\\%06d_main.pov\n", num);
		printf("type tmp >> pov\\%06d_main.pov\n", num);
		printf("type mesh_model__end.txt >> pov\\%06d_main.pov\n", num);
		printf("del tmp\n");
		num += step;

		//if ( num > 10 ) break;
	}

	printf("echo call povray2.bat %06d_main.pov > レンダリング実行.bat\n", startnum);
	for ( int i = startnum+step; i < num; i += step )
	{
		printf("echo call povray2.bat %06d_main.pov >> レンダリング実行.bat\n", i);
	}

}
