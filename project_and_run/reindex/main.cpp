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

	int no_file = 0;

	printf( "argc %d\n", argc);
	if ( argc > 1) printf( "argv[1] %s\n", argv[1]);

	if ( argc > 1 && strcmp(argv[1], "image3D") == 0 ) goto IMAGE3D;

	printf("image\n");
	// image
	while ( no_file < 3000 )
	{
		sprintf(fname, "image\\output_W%06d.bmp", i);
		FILE* fp = fopen(fname, "r");

		if ( fp )
		{
			printf("            \rimage %06d", number);
			fclose(fp);
			sprintf(fname2, "image\\output_W%06d.bmp_", number);
			MoveFileA(fname, fname2);
			number++;
			no_file = 0;
		}else
		{
			no_file++;
		}
		i++;
	}

	for ( int k = 0; k < number; k++ )
	{
		sprintf(fname, "image\\output_W%06d.bmp_", k);
		sprintf(fname2, "image\\output_W%06d.bmp", k);
		MoveFileA(fname, fname2);
	}

	printf("\n");
	number = 0;
	i = 0;
	no_file = 0;
	while ( no_file < 3000 )
	{
		sprintf(fname, "image\\output_W%06d.png", i);
		FILE* fp = fopen(fname, "r");

		if ( fp )
		{
			printf("            \rimage(png) %06d", number);
			fclose(fp);
			sprintf(fname2, "image\\output_W%06d.png_", number);
			MoveFileA(fname, fname2);
			number++;
			no_file = 0;
		}else
		{
			no_file++;
		}
		i++;
	}

	for ( int k = 0; k < number; k++ )
	{
		sprintf(fname, "image\\output_W%06d.png_", k);
		sprintf(fname2, "image\\output_W%06d.png", k);
		MoveFileA(fname, fname2);
	}
	if ( argc > 1 && strcmp(argv[1], "image") == 0 ) return 0;
	//////////////////////////////////////////////////////////////////////

IMAGE3D:	;
	printf("image3D\n");
	// image3D
	number = 0;
	i = 0;
	no_file = 0;
	while ( no_file < 3000 )
	{
		sprintf(fname, "image3D\\output_%06d.bmp", i);
		FILE* fp = fopen(fname, "r");

		if ( fp )
		{
			printf("            \rimage3D %06d", number);
			fclose(fp);
			sprintf(fname2, "image3D\\output_%06d.bmp_", number);
			MoveFileA(fname, fname2);
			number++;
			no_file = 0;
		}else
		{
			no_file++;
		}
		i++;
	}

	for ( int k = 0; k < number; k++ )
	{
		sprintf(fname, "image3D\\output_%06d.bmp_", k);
		sprintf(fname2, "image3D\\output_%06d.bmp", k);
		MoveFileA(fname, fname2);
	}
	printf("\n");
	if ( argc > 1 && strcmp(argv[1], "image3D") == 0 ) return 0;
	//////////////////////////////////////////////////////////////////////


	// obj
	printf("obj\n");
	number = 0;
	i = 0;
	no_file = 0;
	while ( no_file < 3000 )
	{
		sprintf(fname, "obj\\output_%06d.obj", i);
		FILE* fp = fopen(fname, "r");

		if ( fp )
		{
			printf("            \robj %06d", number);
			fclose(fp);
			sprintf(fname2, "obj\\output_%06d.obj_", number);
			MoveFileA(fname, fname2);

			sprintf(fname, "obj\\output_%06d.mtl", i);
			FILE* fp2 = fopen(fname, "r");
			if ( fp2 )
			{
				fclose(fp2);
				sprintf(fname2, "obj\\output_%06d.mtl_", number);
				MoveFileA(fname, fname2);
			}

			sprintf(fname, "obj\\output_%06d.obj.vector", i);
			fp2 = fopen(fname, "r");
			if ( fp2 )
			{
				fclose(fp2);
				sprintf(fname2, "obj\\output_%06d.ob,vectorj_", number);
				MoveFileA(fname, fname2);
			}
			number++;
			no_file = 0;
		}else
		{
			no_file++;
		}
		i++;
	}

	for ( int k = 0; k < number; k++ )
	{
		sprintf(fname, "obj\\output_%06d.obj_", k);
		sprintf(fname2, "obj\\output_%06d.obj", k);
		MoveFileA(fname, fname2);

		sprintf(fname, "obj\\output_%06d.mtl_", k);
		sprintf(fname2, "obj\\output_%06d.mtl", k);
		MoveFileA(fname, fname2);
		
		sprintf(fname, "obj\\output_%06d.vectorj_", k);
		sprintf(fname2, "obj\\output_%06d.vectorj", k);
		MoveFileA(fname, fname2);
	}

	for ( int k = 0; k < number; k++ )
	{
		sprintf(fname2, "obj\\output_%06d.mtl", k);
		FILE* fp = fopen(fname2, "r");
		if ( fp )
		{
			printf("            \robj %06d", number);
			fclose(fp);
			sprintf(fname, "obj\\output_%06d.obj", k);
			fp = fopen( fname, "r");

			sprintf(fname, "obj\\output_%06d.obj__", k);
			FILE* fp2 = fopen(fname, "w");

			char buf[512];
			while( fgets(buf, 512, fp) != NULL )
			{
				if ( strstr(buf, "mtllib") )
				{
					fprintf(fp2, "mtllib ./output_%06d.mtl\n", k);
					continue;
				}
				fprintf(fp2, "%s", buf);
			}
			fclose(fp2);
			fclose(fp);
			sprintf(fname, "obj\\output_%06d.obj", k);
			MoveFileA(fname, fname2);
		}
	}
	printf("\n");
	//////////////////////////////////////////////////////////////////////

	//output
	printf("output\n");
	number = 0;
	i = 0;
	no_file = 0;
	while ( no_file < 3000 )
	{
		sprintf(fname, "output\\output_W%06d.csv", i);
		FILE* fp = fopen(fname, "r");

		if ( fp )
		{
			printf("            \routput %06d", number);
			fclose(fp);
			sprintf(fname2, "output\\output_W%06d.csv_", number);
			MoveFileA(fname, fname2);

			sprintf(fname, "output\\time%06d.txt", i);
			sprintf(fname2, "output\\time%06d.txt_", number);
			MoveFileA(fname, fname2);
			number++;
			no_file = 0;
		}else
		{
			no_file++;
		}
		i++;
	}

	for ( int k = 0; k < number; k++ )
	{
		sprintf(fname, "output\\output_W%06d.csv_", k);
		sprintf(fname2, "output\\output_W%06d.csv", k);
		MoveFileA(fname, fname2);

		sprintf(fname, "output\\time%06d.txt_", k);
		sprintf(fname2, "output\\time%06d.txt", k);
		MoveFileA(fname, fname2);
	}
	printf("\n");

	return 0;
}
