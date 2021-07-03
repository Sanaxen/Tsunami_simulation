#ifndef _TU_SOLVER_H
#define _TU_SOLVER_H

#include "tsu.h"
#include <iostream>
#include <time.h>

using namespace std;

int tu_solver(char* parameterFile);

void CreateImage(Solver& solv, double* solv_w, int i, double min, double max, double max2, char* drive, char* dir, char* drive2, char* dir2, bool smooth_flg, int smooth_num, double smooth[3][3], BitMap& ColorMap, bool colormap, double timesum, double* lon, double* lat);
void RenderCode(int loopnum, int render_step, char* drive, char* dir);
void ElvContourBitmpa(Solver& solv, double zmin, double zmax, char* drive, char* dir, char* exe_drive, char* exe_dir);
void ElvContourBitmpa1(Solver& solv, double zmin, double zmax, char* drive, char* dir, char* exe_drive, char* exe_dir);
void ElvContourBitmpa2(Solver& solv, double zmin, double zmax, char* drive, char* dir, char* exe_drive, char* exe_dir);
void ElvContourBitmpa3(Solver& solv, double zmin, double zmax, char* drive, char* dir, char* exe_drive, char* exe_dir);
void ElvContourRunUpBitmpa(Solver& solv, double zmax, char* drive, char* dir, char* exe_drive, char* exe_dir);

void ElvContourBitmpaTmp(Solver& solv, int kk, int* flag, char* drive, char* dir, char* exe_drive, char* exe_dir);

void outputCrustalMovement(Solver& solv, AbeNoguera& grid, double* www, int timesum, double scale, double depth_scale, int& crustalMovement_count, char* exe_drive, char* exe_dir, char* drive, char* dir, int latitude_flag, double* latitude, int longitude_flag, double* longitude);

void MNVector(Solver* solver, BitMap& bmp, double* latitude, double* longitude, int pitch);

inline int printDiskFreeSpace( Solver& solv, char* drive )
{
    char           szRoot[ 16 ];
    ULARGE_INTEGER  i64Used;
    ULARGE_INTEGER  i64Free;
    ULARGE_INTEGER  i64Avail;
    ULARGE_INTEGER  i64Total;
    
    // ドライブの設定
    strcpy( szRoot, drive );
	if ( strlen(szRoot) > 2 && szRoot[strlen(szRoot)-2] == '\\' ) szRoot[strlen(szRoot)-2] = '\0';
	if ( szRoot[0] == '\0' ) strcpy(szRoot, "C:");

    // ディスク情報の取得
    GetDiskFreeSpaceExA( szRoot, &i64Free, &i64Total, &i64Avail );

	solv.log_printf( "ドライブ名：%s\n", szRoot );

	char    szTotal[128];
	__int64 n64Total;
	double     nTotal;


	sprintf( szTotal, "%I64u", i64Avail.QuadPart );
	n64Total = _atoi64(szTotal);
	nTotal   = ((double)n64Total  / (1024.0*1024.0*1024.0));
	solv.log_printf( "ディスクの空き容量：%I64u  %.1f[G]\n", i64Avail.QuadPart, (double)nTotal);

	sprintf( szTotal, "%I64u", i64Total.QuadPart );
	n64Total = _atoi64(szTotal);
	nTotal   = ((double)n64Total  / (1024.0*1024.0*1024.0));
	solv.log_printf( "ディスクの全体容量：%I64u  %.1f[G]\n", i64Total.QuadPart, (double)nTotal);
    return 0;
}

inline int isDiskFreeSpaceOK( char* drive, int gbytes )
{
    char           szRoot[ 16 ];
    ULARGE_INTEGER  i64Used;
    ULARGE_INTEGER  i64Free;
    ULARGE_INTEGER  i64Avail;
    ULARGE_INTEGER  i64Total;
    
    // ドライブの設定
    strcpy( szRoot, drive );
	if ( strlen(szRoot) > 2 && szRoot[strlen(szRoot)-2] == '\\' ) szRoot[strlen(szRoot)-2] = '\0';
 	if ( szRoot[0] == '\0' ) strcpy(szRoot, "C:");
   // ディスク情報の取得
    GetDiskFreeSpaceExA( szRoot, &i64Free, &i64Total, &i64Avail );

	char    szTotal[128];
	__int64 n64Total;
	double     nTotal;

	sprintf( szTotal, "%I64u", i64Avail.QuadPart );
	n64Total = _atoi64(szTotal);
	nTotal   = ((double)n64Total  / (1024.0*1024.0*1024.0));

	double x = gbytes;
	double y = nTotal;

	if ( x > y ) return 0;
    return 1;
}

#define MIN_DISK_FREE_SPACE	5	//Gbyte



//波高
#if OUTPUT_VALUE==0
#define TSUNAMI_WAVE_MIN_LEVEL	(-2.5)
#define TSUNAMI_WAVE_MAX_LEVEL	(2.5)
#endif

//流量フラックス
#if OUTPUT_VALUE==1 || OUTPUT_VALUE==2
#define TSUNAMI_WAVE_MIN_LEVEL	(-305.5)
#define TSUNAMI_WAVE_MAX_LEVEL	(305.5)
#endif

#if OUTPUT_VALUE==3
#define TSUNAMI_WAVE_MIN_LEVEL	(0)
#define TSUNAMI_WAVE_MAX_LEVEL	(300.0)
#endif

#define TAKASIO_WAVE_MIN_LEVEL	(-2.5)
#define TAKASIO_WAVE_MAX_LEVEL	(2.5)

#endif
