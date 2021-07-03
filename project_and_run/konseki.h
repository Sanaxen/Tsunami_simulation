//#pragma warning ( disable : 4996 )の代わり
#define _CRT_SECURE_NO_DEPRECATE 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <algorithm>

#include <string>
#include <vector>
using namespace std;

#define LINEMAX (4096*10)
#define LINEMAX2 (4096*100)

#define MERGE_TOL	(1.25)
#define ERROR_TOL	(1.15)

class konsekiValue
{
public:
	int id;				//津波ID
	int kid;			//痕跡ID
	char rank;			//痕跡信頼度
	char confidence;	//文献信頼度
	char h_confidence;	//文献記載_高さ定義_信頼度
	int type;			//文献記載_痕跡パターン
	std::string name;
	double lat;
	double lon;
	double hight;
	bool AlreadyOutput;
	konsekiValue()
	{
		AlreadyOutput = false;
		min = 99999999.0;
		max = -99999999.0;
		diff = min;
	}
	double min;
	double max;
	double diff;		//格子点との差
};

class ValueCell
{
public:
	double max;	//最大
	double min;	//最小
	double sum;	//総和
	int dup;	//重複数
	int rank;
	int id;
	std::string name;
	double lat;
	double lon;
	int kid;
	int type;			//文献記載_痕跡パターン



	std::vector<konsekiValue> konseki_list;
	ValueCell()
	{
		max = -9999999999.0;
		min = 9999999999.0;
		dup = 0;
		sum = 0;
		rank = 0;
		id = -1;
		kid = -1;
		konseki_list.clear();
	}
	bool operator <(const ValueCell& f) { // 大小比較用 less 演算子
        return max < f.max;
    }

};
inline int compare_ValueCell(const void *a, const void *b)
{
	return ((ValueCell*)a)->max - ((ValueCell*)b)->max;
}


class rgbrow
{
public:
	int rgb[3];
};

class colorGrid
{
public:
	int rgb[3];	//色
	ValueCell* id;		//対応させた「ValueCell」

	colorGrid()
	{
		rgb[0] = rgb[1] = rgb[2] = 0;
		id = 0;
	}
};


class csv_getline
{
	int eof;
public:
	int size;
	char* buf;

	csv_getline()
	{
		eof = 0;
		size = 4096;
		buf = new char [size];
	}
	~csv_getline()
	{
		delete [] buf;
	}

	char* buffer()
	{
		if ( eof ) return NULL;
		return buf;
	}

	char* get(FILE* fp)
	{
		buf[0] = '\0';
		if ( eof ) return NULL;
		int n = 0;
		int c;
		while( (c = fgetc(fp)) != EOF )
		{
			if ( n+1 >= size )
			{
				size += 1000;
				char* tmp = new char[size];
				memcpy(tmp, buf, n);
				delete [] buf;
				buf = tmp;
			}
			buf[n] = c;
			if ( n >= 1 && buf[n-1]=='\"' && buf[n] == '\n' )
			{
				break;
			}
			n++;
		}
		if ( c == EOF ) eof = 1;
		buf[n+1] = '\0';
		if ( eof ) return NULL;
		return buf;
	}
};







