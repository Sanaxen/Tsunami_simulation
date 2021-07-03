#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>



int main(int argc, char** argv)
{
	FILE* fp = fopen( argv[1], "r");

	if ( fp == NULL ) return -1;

	char buf[1024];

	fgets(buf, 1024, fp);

	int n = 0;
	std::string output = argv[1];
	output += ".out";
	FILE* fp2 = fopen(output.c_str(), "w");
	while( fgets(buf, 1024, fp) != NULL )
	{
		double length;
		double width;
		double depth;
		double strike;
		double dip;
		double rake;
		double slip;
		double lat;
		double lon;

		sscanf(buf, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", 
			&length, &width, &depth, &strike, &dip, &rake, &slip, &lat, &lon);

		fprintf(fp2, "#%03d\n", ++n);
		fprintf(fp2, "FAULT_PARAMETERS\n");
		fprintf(fp2, "%f\n", lon);
		fprintf(fp2, "%f\n", lat);
		fprintf(fp2, "%f\n", length*1000.0);
		fprintf(fp2, "%f\n", strike);
		fprintf(fp2, "%f\n", width*1000.0);
		fprintf(fp2, "%f\n", depth*1000.0);
		fprintf(fp2, "%f\n", dip);
		fprintf(fp2, "%f\n", rake);
		fprintf(fp2, "%f\n\n", slip);
	}
	fclose(fp);
	fclose(fp2);
	return 0;
}