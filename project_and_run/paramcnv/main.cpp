#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>

class fault
{
public:
	double param[11];
};

int conv(char *path, std::vector<fault>& flist, int caseNo, int sec)
{
	char fname[256];

	sprintf(fname, "%s\\断層パラメータ_ケース%02d_%03ds_JGD2000.txt", path, caseNo, sec);

	FILE* fp = fopen(fname, "r");
	if (fp == NULL) return -1;

	;
	char buf[256];
	while (fgets(buf, 256, fp) != NULL)
	{
		fault f;
		sscanf(buf, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
			f.param, f.param + 1, f.param + 2, f.param + 3, f.param + 4, f.param + 5,
			f.param + 6, f.param + 7, f.param + 8, f.param + 9, f.param + 10);

		if (f.param[8] < 0.000001)
		{
			printf("slip:%f -> skipp!!\n", f.param[8]);
			continue;
		}
		flist.push_back(f);
	}
	fclose(fp);

	return 0;
}
bool comp(const fault& lh, fault& rh) 
{

	return lh.param[10] < rh.param[10];
}

int main(int argc, char** argv)
{
	if (argc < 5)
	{
		printf("paramcnv.exe path case_number startTime endTime\n");
		return -1;
	}
	
	char* path = argv[1];
	int caseNum = atoi(argv[2]);
	int startTime = atoi(argv[3]);
	int endTime = atoi(argv[4]);

	printf("path=[%s]\n", path);
	printf("case=%d\n", caseNum);
	printf("startTime=%d\n", startTime);
	printf("endTime=%d\n", endTime);

	std::vector<fault> flist;

	for (int i = startTime; i <= endTime; i += 10)
	{
		conv(path, flist, caseNum, i);
	}
	std::sort(flist.begin(), flist.end(), comp);

	FILE* fp = fopen("fault_parameters.txt", "w");
	for (int i = 0; i < flist.size(); i += 1)
	{
		const fault& f = flist[i];

		if (i == 0)
		{
			fprintf(fp, "#FAULT_PARAMETERS T%f\n", f.param[10]);
			fprintf(fp, "FAULT_PARAMETERS\n");
		}
		else
		{
			fprintf(fp, "FAULT_PARAMETERS T%f\n", f.param[10]);
		}
		fprintf(fp, "%f\n", f.param[1]);
		fprintf(fp, "%f\n", f.param[0]);
		fprintf(fp, "%f\n", f.param[6]);
		fprintf(fp, "%f\n", f.param[3]);
		fprintf(fp, "%f\n", f.param[7]);
		fprintf(fp, "%f\n", f.param[2]);
		fprintf(fp, "%f\n", f.param[4]);
		fprintf(fp, "%f\n", f.param[5]);
		fprintf(fp, "%f\n", f.param[8]);
		fprintf(fp, "\n");
	}
	fclose(fp);

	return 0;
}