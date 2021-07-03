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

	fprintf(stderr, "reindex2 step filename\n filename(sample) => image\\output_W%%06d.bmp\n");

	printf( "argc %d\n", argc);
	if (argc < 3) return -1;


	int step = atoi(argv[1]);
	char arg[256];
	strcpy(arg, argv[2]);

	int k = 1;
	while ( no_file < 3000 )
	{
		sprintf(fname, argv[2], i);
		FILE* fp = fopen(fname, "r");

		if (fp) number++;
		if ( fp && i % step == 0)
		{
			//printf("            \r %06d", i);
			fclose(fp);
			sprintf(fname2, arg, k);
			remove(fname2);
			CopyFileA(fname, fname2, FALSE);
			printf("%s -> %s\n", fname, fname2);
			k++;
			no_file = 0;
		}else
		{
			no_file++;
		}
		if (fp) fclose(fp);
		i++;
	}
	printf("%d -> %d Files\n", number, k);
	for (int ii = k; ii <= number; ii++)
	{
		sprintf(fname2, argv[2], ii);
		printf("delete %s\n", fname2);
		_unlink(fname2);
	}


	return 0;
}
