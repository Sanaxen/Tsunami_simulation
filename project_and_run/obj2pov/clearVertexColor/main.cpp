#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#include <vector>

int main( int argc, char** argv)
{
	if ( argc < 2 ) return -1;

	FILE* fp = fopen(argv[1], "r");
	if ( fp == NULL ) return -2;

	char buf[256];

	while( fgets(buf, 256, fp) != NULL )
	{
		if ( strstr(buf, "texture_list") )
		{
			int c = EOF;
			int par = 1;
			while( (c = fgetc(fp)) != EOF )
			{
				if ( c == '{' ) par++;
				if ( c == '}' ) par--;
				if ( par == 0 ) break;
			}
			continue;
		}

		if ( strstr(buf, "face_indices") )
		{
			printf("%s", buf);
			fgets(buf, 256, fp);
			printf("%s", buf);

			while( 1 )
			{
				fgets(buf, 256, fp);
				char* p = strchr(buf, '>');
				*p = '\0';

				p++;
				int n = 0;
				while( n < 3 )
				{
					if ( *p == ',' ) n++;
					p++;
					continue;
				}
				while(1)
				{
					if ( isdigit(*p)) p++;
					else break;
				}
				printf("%s", buf);
				if ( *p == ',' )
				{
					printf(">,\n");
				}else
				{
					printf(">\n");
					break;
				}
			}
			fgets(buf, 256, fp);
			printf("%s", buf);
			continue;
		}
		printf("%s", buf);
	}
	fclose(fp);

	return 0;
}




