#include "tu_solver.h"

#include "DirectryTool.h"

#include "cmdProcess.h"
#include "Storm.h"

#define USE_STB_IMAGE

#ifdef USE_STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#endif

#define ZSCALE_ELV	(6.0)
#define ZSCALE_DPT	(6.0)
#define ZSCALE_WAV	(650.0)

Storm_Line Storm;

static BitMap topography_data_residential_area;
static BitMap topography_data_nothing_area;
static BitMap topography_data_edit;

static BitMap topography_data_mask_bmp;
static BitMap topography_data_mask2_bmp;
static int OutputLastCounter=0;
static BitMap runup_color_level0;
static BitMap runup_color_level;
static BitMap wave_colormap;
static int OutputRunUpImage;
static int runup_elv_bitmap_make;
char IDname[256];
static double* CrustalMovementData = 0;
static double* CrustalMovementDiff = 0;
static BitMap colormap_default;

static int vtk_export = 0;
static int kml_export = 1;
static double kml_start_time = 0.0;
static int checker_bord_error_debug = 0;
static int damage_estimation = 1;
static double colorbar_max = TSUNAMI_WAVE_MAX_LEVEL;
static double colorbar_min = TSUNAMI_WAVE_MIN_LEVEL;

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#include <amp.h>

float *debug_marker=0;

inline int isRender(int icount, double render_stp, double time_diff)
{
	if ( icount == 0 ) return 1;
	if ( time_diff >= render_stp ) return 1;
	return 0;
}

int tu_solver( char* parameterFile)
{
	setlocale( LC_ALL, "Japanese" );

	//{
	//	double y = 559776.000;
	//	double x =  442882;
	//	double ph0 = DEG(140,5, 16), ph;
	//	double la0 = DEG(36,6,13), la;
	//	coordTransform( ph0, la0, x, y, ph,  la);
	//	printf("%f %f\n", la, ph);

	//	y = -54063.707;
	//	x = 158760.557;
	//	la0 = DEG(135,0, 0);
	//	ph0 = DEG(36,0,0);
	//	coordTransform( ph0, la0, x, y, ph,  la);
	//	printf("%f %f\n", la, ph);
	//	exit(0);
	//}

	runup_elv_bitmap_make = 0;
	OutputLastCounter = 0;
	OutputRunUpImage = 0;
	CrustalMovementData = 0;
	CrustalMovementDiff = 0;

	int batheymetry_smoothing = 0;
	double Z_SCALE_WAV = ZSCALE_WAV;
	double Z_SCALE_DPT = ZSCALE_DPT;
	double Z_SCALE_ELV = ZSCALE_ELV;
	double h_min = 0.0;
	double h_max = 1.0;
	double e_min = 0.0;
	double e_max = 1.0;
	double w_scale[2] = {1.0, 1.0};
	double time_step = 0.5;
	double dx = 100.0;
	double dy = 100.0;
	double render_z_scale = 1.0;
	int loopnum = 1;
	int render_only = 0;
	int objsmooth = 0;

	int absorbingZone = 20;
	double attenuation = 0.015;

	double MinimumWaterDepth = MINIMUM_DEPTH2;	//最低水深
	double MaximumWaterDepth = 0;	//最大水深
	double Manning = 1.0;
	double Manning_waterArea = 1.0;

	unsigned char maskcolor[3]={37,116,154};
	int colormap = 0;
	double colorbar_max_prm = 0;
	double colorbar_min_prm = 0;

	double L = 0.0;		//断層の長さ
	double D = 0.0;		//断層までの最短距離

	double rise_time = 10.0;	//ライズタイム
	int CrustalMovement = 0;	//地殻変動の地形リアルタイム出力

	double typhoon_radius_coef = 1.0;

	double sim_time = 0.0;
	double render_step = 1;
	int render_step_float = 0;
	kml_start_time = 0.0;

	double latitude[4];
	bool latitude_flag = false;
	double longitude[4];
	bool longitude_flag = false;

	int Coriolis_force = 0;
	int Soliton = 0;
	int RunUp = 0;
	int solver = 0;

	bool Dmin_flag = false;
	double Dmin = 0.01;

	bool smooth_flg = false;
	double smooth[3][3]={{0,0,0},{0,1,0},{0,0,0}};
	int smooth_num = 1;

	int sampling = 0;
	int point_sampling = 0;
	int realtimeImage = 1;

	int ogjfile = 1;
	int csv2obj = 0;

	double stopHeight = -10000000.0;

	int filterCycle1 = FILTER_CYCLE3;
	int filterCycle2 = FILTER_CYCLE4;
	int filterCycle3 = FILTER_CYCLE5;
	int filterCycle4 = FILTER_CYCLE6;
	int filterCycle5 = FILTER_CYCLE7;
	int filterOrder = FILTER_ORDER;
	int filtermthod = FILTER_METHOD;

	double flow_res = 1.0;
	double flow_res2 = FLOW_DEPTH_LIMIT_COND_COEF3;

	double tide_level_offset = 0.0;

	std::vector<HTable> htable;
	std::vector<HTable> etable;
	std::vector<HTable> wuptable;
	std::vector<HTable> wdntable;
	std::vector<ChkPointMarker> chkpointList;

	double average_population_density=343;		//１区画内の平均人口密度
	double house_number_rate=3.5;
	double Revetment_h=REVETMENT;			//護岸・堤防判定

	double startUptime = 0.0;				//台風生成時間
	int storm_surge_only = -1;				//高潮計算のみ:1 津波計算のみ:-1 同時計算:0
	double tsunami_start_time=0;			//台風発生後(台風生成時間を基準とする)の津波発生までの時間
	double cd_coef=-1.0;						//海面抵抗係数に対する補正倍率(2*rm以内のみ）
	double cd_coef2=-1.0;					//海面抵抗係数に対する補正倍率(全域)

	double global_scale = 1.0;
	int obj_standerd = 0;
	int obj_normalize = 0;
	int obj_Material[3] = {1,1,1};

	int impact = 0;							//隕石衝突

	double nu = 0.0;

	double dispersion_correction_coefficient1=1.0;
	double dispersion_correction_coefficient2=1.0;

	int use_upwindow_method=-1;

	double gap_max_coef = GAP_MAX;
	double gap_depth_coef = GAP_DEPTH;

	double steep_slope1[2] ={STEEP_SLOPE1_1,STEEP_SLOPE1_2};
	double steep_slope2[2] ={STEEP_SLOPE2_1,STEEP_SLOPE2_2};

	double exclusionRange[2] = {-1.0, -1.0};

	int useJST = 0;

	int water_depth_smoothing_num = 2;
	int terrain_displacement = 1;
	double checker_bord_theshold = CHECKER_BORD_THESHOLD1;

	RainfallIntensityConst rainfallIntensityConst;
	std::vector<River> river;
	std::vector<Inflow> inflow;
	std::vector<Barir> barir;
	double river_running_time = 600;

	char drive[_MAX_DRIVE];	// ドライブ名
    char dir[_MAX_DIR];		// ディレクトリ名
    char fname[_MAX_FNAME];	// ファイル名
    char ext[_MAX_EXT];		// 拡張子

    _splitpath( parameterFile, drive, dir, fname, ext );

    cout << "Drive=" << drive << endl;
    cout << "Dir  =" << dir   << endl;
    cout << "Fname=" << fname << endl;
    cout << "Ext  =" << ext   << endl;

	char thisModuleFileName[_MAX_PATH];
	//実行中のプロセスのフルパス名を取得する
	GetModuleFileNameA(NULL, thisModuleFileName, sizeof(thisModuleFileName));

	char exe_drive[_MAX_DRIVE];	// ドライブ名
    char exe_dir[_MAX_DIR];		// ディレクトリ名
    char exe_fname[_MAX_FNAME];	// ファイル名
    char exe_ext[_MAX_EXT];		// 拡張子

    _splitpath( thisModuleFileName, exe_drive, exe_dir, exe_fname, exe_ext );

    cout << "Drive=" << exe_drive << endl;
    cout << "Dir  =" << exe_dir   << endl;
    cout << "Fname=" << exe_fname << endl;
    cout << "Ext  =" << exe_ext   << endl;

	FILE* fp = fopen( parameterFile, "r");
	if ( fp == NULL )
	{
		printf("file open error.\n");
		return -1;
	}

	{
		char dirname[512];
		DirectryTool createDir;
		
		sprintf(dirname, "%s%srunup_image\\", drive, dir);
		createDir.MakeDir(dirname);

		sprintf(dirname, "%s%simage\\", drive, dir);
		createDir.MakeDir(dirname);

		sprintf(dirname, "%s%simage3D\\", drive, dir);
		createDir.MakeDir(dirname);

		sprintf(dirname, "%s%soutput\\", drive, dir);
		createDir.MakeDir(dirname);

		sprintf(dirname, "%s%spov\\", drive, dir);
		createDir.MakeDir(dirname);

		sprintf(dirname, "%s%srender\\", drive, dir);
		createDir.MakeDir(dirname);

		sprintf(dirname, "%s%ssampling\\", drive, dir);
		createDir.MakeDir(dirname);

		sprintf(dirname, "%s%sobj\\", drive, dir);
		createDir.MakeDir(dirname);

		sprintf(dirname, "%s%sobj\\vtk\\", drive, dir);
		createDir.MakeDir(dirname);

		sprintf(dirname, "%s%swrk\\", drive, dir);
		createDir.MakeDir(dirname);

		sprintf(dirname, "%s%schk\\", drive, dir);
		createDir.MakeDir(dirname);
		
		sprintf(dirname, "%s%sTerrain\\", drive, dir);
		createDir.MakeDir(dirname);

		sprintf(dirname, "%s%sgoogle_earth\\", drive, dir);
		createDir.MakeDir(dirname);

		sprintf(dirname, "%s%sScenario\\", drive, dir);
		createDir.MakeDir(dirname);
	}

	Solver solv;


	strcpy(IDname, "untitled");
	char buf[256];
	while( fgets(buf, 256, fp ) != NULL )
	{
		if ( strcmp(buf, "ID\n") == 0 )
		{
			fgets(IDname, 256, fp);
			IDname[strlen(IDname)-1] = '\0';
			{
				FILE* fp = fopen(IDname, "w");
				if ( fp == NULL )
				{
					printf("ERROR:IDで利用できない文字が使われています");
					strcpy(IDname, "untitled");
					printf("=>%sを利用します\n", IDname);
				}else
				{
					fclose(fp);
					remove(IDname);
				}
			}
			continue;
		}
		if ( strcmp(buf, "H_MAX\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &h_max);
			continue;
		}
		if ( strcmp(buf, "H_MIN\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &h_min);
			continue;
		}
		if ( strcmp(buf, "H_TABLE\n") == 0 )
		{
			do{
				fgets(buf, 256, fp);
				if ( !isdigit(buf[0]) ) break;
				HTable h;
				sscanf(buf, "%d,%lf", &h.level, &h.depth);
				printf("水深テーブル[%d] %f\n", h.level, h.depth);
				htable.push_back(h);
			}while(1);
			continue;
		}

		if ( strcmp(buf, "E_MAX\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &e_max);
			continue;
		}
		if ( strcmp(buf, "E_MIN\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &e_min);
			continue;
		}
		if ( strcmp(buf, "E_TABLE\n") == 0 )
		{
			do{
				fgets(buf, 256, fp);
				if ( !isdigit(buf[0]) ) break;
				HTable e;
				sscanf(buf, "%d,%lf", &e.level, &e.depth);
				printf("標高テーブル[%d] %f\n", e.level, e.depth);
				etable.push_back(e);
			}while(1);
			continue;
		}

		if ( strcmp(buf, "DMIN\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &Dmin);
			Dmin_flag = true;
			continue;
		}
		if ( strcmp(buf, "SIM_TIME\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &sim_time);
			continue;
		}

		if ( strcmp(buf, "W_SCALE\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", w_scale);
			continue;
		}
		if ( strcmp(buf, "W_SCALE_UP\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", w_scale);
			continue;
		}
		if ( strcmp(buf, "W_UP_TABLE\n") == 0 )
		{
			do{
				fgets(buf, 256, fp);
				if ( !isdigit(buf[0]) ) break;
				HTable wup;
				sscanf(buf, "%d,%lf", &wup.level, &wup.depth);
				printf("海面隆起テーブル[%d] %f\n", wup.level, wup.depth);
				wuptable.push_back(wup);
			}while(1);
			continue;
		}

		if ( strcmp(buf, "W_SCALE_DOWN\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", w_scale+1);
			continue;
		}
		if ( strcmp(buf, "W_DOWN_TABLE\n") == 0 )
		{
			do{
				fgets(buf, 256, fp);
				if ( !isdigit(buf[0]) ) break;
				HTable wdn;
				sscanf(buf, "%d,%lf", &wdn.level, &wdn.depth);
				printf("海面沈降テーブル[%d] %f\n", wdn.level, wdn.depth);
				wdntable.push_back(wdn);
			}while(1);
			continue;
		}

		if ( strcmp(buf, "L\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &L);
			continue;
		}
		if ( strcmp(buf, "D\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &D);
			continue;
		}

		if ( strcmp(buf, "SMOOTH\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf %lf %lf", &(smooth[0][0]), &(smooth[0][1]), &(smooth[0][2]));
			fgets(buf, 256, fp);
			sscanf(buf, "%lf %lf %lf", &(smooth[1][0]), &(smooth[1][1]), &(smooth[1][2]));
			fgets(buf, 256, fp);
			sscanf(buf, "%lf %lf %lf", &(smooth[2][0]), &(smooth[2][1]), &(smooth[2][2]));
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &smooth_num);
			smooth_flg = true;
			continue;
		}

		if ( strcmp(buf, "TIME_STEP\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &time_step);
			continue;
		}
		if ( strcmp(buf, "SAMPLING\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &sampling);
			printf("sampling %d\n", sampling);
			continue;
		}
		if (strcmp(buf, "POINT_SAMPLING\n") == 0)
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &point_sampling);
			printf("point_sampling %d\n", point_sampling);
			continue;
		}
		if (strcmp(buf, "DX\n") == 0)
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &dx);
			continue;
		}
		if ( strcmp(buf, "DY\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &dy);
			continue;
		}
		if ( strcmp(buf, "N\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &loopnum);
			continue;
		}
		if ( strcmp(buf, "ABSOR\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d,%lf", &absorbingZone, &attenuation);
			continue;
		}
		if ( strcmp(buf, "MANNING\n") == 0 || strcmp(buf, "MANNINIG\n") == 0)
		{
			if ( strcmp(buf, "MANNINIG\n") == 0 )
			{
				printf("keyword error!! \'MANNINIG\'->\'MANNING\'\n");
			}
			fgets(buf, 256, fp);
			if (2 == sscanf(buf, "%lf %lf", &Manning, &Manning_waterArea))
			{
				/* empty*/
			}
			else
			{
				sscanf(buf, "%lf", &Manning);
			}
			continue;
		}

		if ( strcmp(buf, "LAT\n") == 0 )
		{
			latitude_flag = true;
			printf("緯度\n");
			double h,m,s;
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); latitude[0] = (h+m/60.0 + s/3600.0)*3.1415926535897/180.0;
			printf("北東:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, latitude[0]);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); latitude[1] = (h+m/60.0 + s/3600.0)*3.1415926535897/180.0;
			printf("北西:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, latitude[1]);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); latitude[2] = (h+m/60.0 + s/3600.0)*3.1415926535897/180.0;
			printf("南東:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, latitude[2]);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); latitude[3] = (h+m/60.0 + s/3600.0)*3.1415926535897/180.0;
			printf("南西:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, latitude[3]);
			continue;
		}
		if ( strcmp(buf, "LON\n") == 0 )
		{
			longitude_flag = true;
			printf("経度\n");
			double h,m,s;
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); longitude[0] = (h+m/60.0 + s/3600.0)*3.1415926535897/180.0;;
			printf("北東:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, longitude[0]);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); longitude[1] = (h+m/60.0 + s/3600.0)*3.1415926535897/180.0;;
			printf("北西:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, longitude[1]);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); longitude[2] = (h+m/60.0 + s/3600.0)*3.1415926535897/180.0;;
			printf("南東:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, longitude[2]);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); longitude[3] = (h+m/60.0 + s/3600.0)*3.1415926535897/180.0;;
			printf("南西:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, longitude[3]);
			continue;
		}

		if ( strcmp(buf, "CORIOLIS\n") == 0 )
		{
			fgets(buf, 256, fp);
			int d = 0;
			sscanf(buf, "%d", &Coriolis_force);
			printf("コリオリ因子の考慮:%d\n", Coriolis_force);
			continue;
		}
		if ( strcmp(buf, "SOLITON\n") == 0 )
		{
			fgets(buf, 256, fp);
			int d = 0;
			sscanf(buf, "%d", &Soliton);
			printf("ソリトン分裂の考慮:%d\n", Soliton);
			continue;
		}
		if ( strcmp(buf, "SOLVER\n") == 0 )
		{
			fgets(buf, 256, fp);
			int d = 0;
			sscanf(buf, "%d", &solver);
			printf("ソルバ:%d\n", solver);
			continue;
		}
		if ( strcmp(buf, "RUNUP\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &RunUp);
			printf("遡上効果の考慮:%d\n", RunUp);
			continue;
		}
		if ( strcmp(buf, "MIN_DEPTH\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &MinimumWaterDepth);
			printf("最低水深の考慮:%f\n", MinimumWaterDepth);
			continue;
		}
		if (strcmp(buf, "MAX_DEPTH\n") == 0)
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &MaximumWaterDepth);
			printf("最大水深の考慮:%f\n", MaximumWaterDepth);
			continue;
		}

		if ( strcmp(buf, "RENDER_Z_SCALE\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &render_z_scale);
			continue;
		}
		if ( strcmp(buf, "RENDER_ONLY\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &render_only);
			continue;
		}
		if ( strcmp(buf, "RENDER_STEP\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &render_step);
			if ( strchr(buf, '.' ) ) render_step_float = 1;
			continue;
		}
		if ( strcmp(buf, "REALTIMEIMAGE\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &realtimeImage);
			continue;
		}

		if ( strcmp(buf, "EXPORT_OBJ\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &ogjfile);
			continue;
		}
		if ( strcmp(buf, "EXPORT_VTK\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &vtk_export);
			continue;
		}
		if ( strcmp(buf, "CSV2OBJ\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &csv2obj);
			continue;
		}
		if ( strcmp(buf, "OBJSMOOTH\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &objsmooth);
			continue;
		}
		if ( strcmp(buf, "STOP\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &stopHeight);
			continue;
		}
		if ( strcmp(buf, "FILTER_ORDER\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &filterOrder);
			if ( filterOrder != 6 && filterOrder != 8 && filterOrder != 10 ) filterOrder = FILTER_ORDER;
			continue;
		}
		if ( strcmp(buf, "FILTER_CYCLE\n") == 0 )
		{
			int a, b;
			fgets(buf, 256, fp);
			
			int n = sscanf(buf, "%d", &filterCycle1);
			if ( n == 1 )
			{
				if ( filterCycle1 <= 0 ) filterCycle1 = FILTER_CYCLE3;
			}
			n = sscanf(buf, "%d %d", &filterCycle1, &filterCycle2);
			if ( n == 2 )
			{
				if ( filterCycle1 <= 0 ) filterCycle1 = FILTER_CYCLE3;
				if ( filterCycle2 <= 0 ) filterCycle2 = FILTER_CYCLE4;
			}
			n = sscanf(buf, "%d %d %d", &filterCycle1, &filterCycle2, &filterCycle4);
			if ( n == 3 )
			{
				if ( filterCycle1 <= 0 ) filterCycle1 = FILTER_CYCLE3;
				if ( filterCycle2 <= 0 ) filterCycle2 = FILTER_CYCLE4;
				if ( filterCycle4 <= 0 ) filterCycle4 = FILTER_CYCLE6;
			}
			//n = sscanf(buf, "%d %d %d %d", &filterCycle1, &filterCycle2, &filterCycle3, &filterCycle4);
			//if ( n == 4 )
			//{
			//	if ( filterCycle1 <= 0 ) filterCycle1 = FILTER_CYCLE3;
			//	if ( filterCycle2 <= 0 ) filterCycle2 = FILTER_CYCLE4;
			//	if ( filterCycle3 <= 0 ) filterCycle3 = FILTER_CYCLE5;
			//	if ( filterCycle4 <= 0 ) filterCycle4 = FILTER_CYCLE6;
			//}
			//n = sscanf(buf, "%d %d %d %d %d", &filterCycle1, &filterCycle2, &filterCycle3, &filterCycle4, &filterCycle5);
			//if ( n == 4 )
			//{
			//	if ( filterCycle1 <= 0 ) filterCycle1 = FILTER_CYCLE3;
			//	if ( filterCycle2 <= 0 ) filterCycle2 = FILTER_CYCLE4;
			//	if ( filterCycle3 <= 0 ) filterCycle3 = FILTER_CYCLE5;
			//	if ( filterCycle4 <= 0 ) filterCycle4 = FILTER_CYCLE6;
			//	if ( filterCycle5 <= 0 ) filterCycle5 = FILTER_CYCLE7;
			//}
			continue;
		}
		
		if ( strcmp(buf, "COLORMAP\n") == 0 )
		{
			int rgb[3];
			colormap = 1;
			fgets(buf, 256, fp);
			sscanf(buf, "%d,%d,%d", rgb, rgb+1, rgb+2);
			maskcolor[0] = rgb[0];
			maskcolor[1] = rgb[1];
			maskcolor[2] = rgb[2];
			continue;
		}
		if ( strcmp(buf, "TIDE_LEVEL\n") == 0 )
		{
			fgets(buf, 256, fp);
			tide_level_offset = atof(buf);
			continue;
		}
		if ( strcmp(buf, "CHKPOINT_BEGIN\n") == 0 )
		{
			while(1)
			{
				ChkPointMarker cp;
				fgets(buf, 256, fp);
				if ( buf[0] == '@' )
				{
					break;
				}
				if ( strcmp(buf, "CHKPOINT_END\n") == 0 ) break;
				buf[strlen(buf)-1] = '\0';
				if ( buf[0] == '*' )
				{
					cp.ref = true;
				}else
				{
					cp.ref = false;
				}
				cp.name = buf;
				fgets(buf, 256, fp);
				sscanf(buf, "%d %d %d", &(cp.r), &(cp.g), &(cp.b));
				fgets(buf, 256, fp);
				cp.trace = atof(buf+1);
				if ( isalpha(buf[0])) cp.rank = buf[0] - 'A';
				else cp.rank = 999;
				cp.type = 1;
				fgets(buf, 256, fp);
				if ( strncmp(buf, "LON:",4) == 0 )
				{
					sscanf(buf, "LON:%lf LAT:%lf\n", &cp.lon, &cp.lat);
					fgets(buf, 256, fp);
				}
				if ( strncmp(buf, "PTN:",4) == 0 )
				{
					sscanf(buf, "PTN:%d\n", &cp.type);
					fgets(buf, 256, fp);
				}
				chkpointList.push_back(cp);
			}
			if ( buf[0] == '@' )
			{
				char fname[512];
				strcpy(fname, buf+1);
				char* p = strchr(fname, '\n');
				*p = '\0';
				FILE* fp = fopen(fname, "r");
				fgets(buf, 256, fp);
				if ( strcmp(buf, "CHKPOINT_BEGIN\n") == 0 )
				{
					while(1)
					{
						ChkPointMarker cp;
						fgets(buf, 256, fp);
						if ( strcmp(buf, "CHKPOINT_END\n") == 0 )
						{
							fclose(fp);
							break;
						}
						buf[strlen(buf)-1] = '\0';
						if ( buf[0] == '*' )
						{
							cp.ref = true;
						}else
						{
							cp.ref = false;
						}
						cp.name = buf;

						int xx, yy;
						char* p = &buf[strlen(buf) - 1];
						while (*p != '[') p--;
						sscanf(p, "[(%d %d)]", &yy, &xx);
						cp.xxx = xx;
						cp.yyy = yy;

						fgets(buf, 256, fp);
						sscanf(buf, "%d %d %d", &(cp.r), &(cp.g), &(cp.b));
						fgets(buf, 256, fp);
						cp.trace = atof(buf+1);
						if ( isalpha(buf[0])) cp.rank = buf[0] - 'A';
						else cp.rank = 999;
						if ( buf[0] == '@') cp.rank = '@';
						cp.type = 1;
						fgets(buf, 256, fp);
						if ( strncmp(buf, "LON:",4) == 0 )
						{
							sscanf(buf, "LON:%lf LAT:%lf\n", &cp.lon, &cp.lat);
							fgets(buf, 256, fp);
						}
						if ( strncmp(buf, "PTN:",4) == 0 )
						{
							sscanf(buf, "PTN:%d\n", &cp.type);
							fgets(buf, 256, fp);
						}
						chkpointList.push_back(cp);
					}
				}
			}
			continue;
		}
		if ( strcmp(buf, "AVERAGE_DENSITY\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &average_population_density);
			continue;
		}
		if ( strcmp(buf, "HOUSE_NUMBER_RATE\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &house_number_rate);
			continue;
		}

		if ( strcmp(buf, "REVETMENT\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &Revetment_h);
			continue;
		}
		if ( strcmp(buf, "RISETIME\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &rise_time);
			continue;
		}
		if ( strcmp(buf, "FLOWRES\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &flow_res);
			continue;
		}		
		if ( strcmp(buf, "FLOWRES2\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &flow_res2);
			if ( flow_res2 <= 0.0 ) flow_res2 = FLOW_DEPTH_LIMIT_COND_COEF3;
			continue;
		}		
		if ( strcmp(buf, "STARTUP_TIME\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &startUptime);
			continue;
		}	
		if ( strcmp(buf, "STORM_SURGE_ONLY\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &storm_surge_only);
			continue;
		}	
		if ( strcmp(buf, "TSUNAMI_START_TIME\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &tsunami_start_time);
			continue;
		}
		if ( strcmp(buf, "CD_COEF\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &cd_coef);
			continue;
		}
		if ( strcmp(buf, "CD_COEF2\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &cd_coef2);
			continue;
		}
		if ( strcmp(buf, "COLOR_BAR_MIN_MAX\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf %lf", &colorbar_min_prm, &colorbar_max_prm);
			continue;
		}
		if ( strcmp(buf, "IMPACT\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &impact);
			continue;
		}
		//Horizontal Eddy Viscosity
		if ( strcmp(buf, "HORIZONATAL_EDDY_VISCOSITY\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &nu);
			continue;
		}
		//dispersion
		if ( strcmp(buf, "DISPERSION_COEF1\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &dispersion_correction_coefficient1);
			continue;
		}
		if ( strcmp(buf, "DISPERSION_COEF2\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &dispersion_correction_coefficient2);
			continue;
		}
		
		if ( strcmp(buf, "UPWINDOW\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &use_upwindow_method);
			continue;
		}
		if ( strcmp(buf, "GAP\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &gap_max_coef);
			continue;
		}
		if ( strcmp(buf, "GAP_DEPTH\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &gap_depth_coef);
			continue;
		}
		if ( strcmp(buf, "JST\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &useJST);
			continue;
		}
		if ( strcmp(buf, "WATER_DEPTH_SMOOTHING\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &water_depth_smoothing_num);
			continue;
		}
		if (strcmp(buf, "TERRAIN_DISPLACEMENT\n") == 0)
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &terrain_displacement);
			continue;
		}
		if (strcmp(buf, "CHECKERBOARD_THESHOLD\n") == 0)
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &checker_bord_theshold);
			continue;
		}
		if (strcmp(buf, "TYPHOON_RADIUS_COEF\n") == 0)
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &typhoon_radius_coef);
			continue;
		}
		if (strcmp(buf, "STEEP_SLOPE1\n") == 0)
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf %lf", steep_slope1, steep_slope1+1);
			continue;
		}
		if (strcmp(buf, "STEEP_SLOPE2\n") == 0)
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf %lf", steep_slope2, steep_slope2+1);
			continue;
		}

		if (strcmp(buf, "RIVER_RUNNING_TIME\n") == 0)
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &river_running_time);
			continue;
		}

		if (strcmp(buf, "RAINFALL_INTENSITY\n") == 0)
		{
			double x, y, z, u = 0.0;
			fgets(buf, 256, fp);
			sscanf(buf, "%lf %lf %lf", &rainfallIntensityConst.rainfall, &rainfallIntensityConst.start, &rainfallIntensityConst.end);
			continue;
		}

		if (strcmp(buf, "Barir\n") == 0)
		{
			double x, y, z, u = 0.0;
			Barir rr;
			fgets(buf, 256, fp);
			sscanf(buf, "%lf %lf", &x, &rr.zup);

			rr.color.r = 255;
			rr.color.g = x;
			rr.color.b = 255;
			barir.push_back(rr);
			continue;
		}
		if (strcmp(buf, "River\n") == 0)
		{
			double x, y, z, u=0.0;
			River rr;
			fgets(buf, 256, fp);
			sscanf(buf, "%lf %lf %lf %lf", &x, &y, &z, &u);

			rr.velocity = 0.0;
			fgets(buf, 256, fp);
			if (buf[0] == '\0' && buf[0] == '\n')
			{
				/* empty */
			}
			else
			{
				sscanf(buf, "%lf", &rr.velocity);
			}
			rr.color.r = 113;
			rr.color.g = x;
			rr.color.b = 255;
			rr.depth = y;
			rr.initflow = z;
			rr.Barrier_height = u;
			river.push_back(rr);
			continue;
		}
		if (strcmp(buf, "Inflow\n") == 0)
		{
			double x, y, z;
			Inflow rr;
			fgets(buf, 256, fp);
			sscanf(buf, "%lf %lf %lf", &x, &y, &z);

			fgets(buf, 256, fp);
			if (buf[0] == '\0' && buf[0] == '\n')
			{
				/* empty */
			}
			else
			{
				sscanf(buf, "%lf %lf", &rr.dir, &rr.velocity);
			}

			rr.color.r = 255;
			rr.color.g = 255;
			rr.color.b = x;
			rr.depth = y;
			rr.flow = z;
			inflow.push_back(rr);
			continue;
		}
	}
	fclose(fp);

	if ( cd_coef > 0.0 ) Storm.cd_coef = cd_coef;
	if ( cd_coef2 > 0.0 ) Storm.cd_coef2 = cd_coef2;
	Storm.typhoon_radius_coef = typhoon_radius_coef;
	Storm.JST = useJST;
	Storm.Load(parameterFile);

	if ( getenv("RUNUP") )
	{
		RunUp = atoi(getenv("RUNUP"));
		printf("環境変数により遡上計算が%sなりました\n", RunUp?"有効に":"無効に");
	}


	if ( getenv("ELV_SCALE") )
	{
		double s = atof(getenv("ELV_SCALE"));
		if ( s <= 0.0 ) s = ZSCALE_ELV;
		printf("標高を%f倍にスケーリングしました\n", s);
		Z_SCALE_ELV = s;
	}
	if ( getenv("DEPTH_SCALE") )
	{
		double s = atof(getenv("DEPTH_SCALE"));
		if ( s <= 0.0 ) s = ZSCALE_DPT;
		printf("水深を%f倍にスケーリングしました\n", s);
		Z_SCALE_DPT = s;
	}
	if ( getenv("WAVE_SCALE") )
	{
		double s = atof(getenv("WAVE_SCALE"));
		if ( s <= 0.0 ) s = ZSCALE_DPT;
		printf("波高を%f倍にスケーリングしました\n", s);
		Z_SCALE_WAV = s;
	}
	if ( getenv("WAVE_SCALE_COEF") )
	{
		double s = atof(getenv("WAVE_SCALE_COEF"));
		printf("波高を%f*%f=%f倍にスケーリングしました\n", s, Z_SCALE_WAV, s*Z_SCALE_WAV);
		Z_SCALE_WAV *= s;
	}
	if ( getenv("MIN_DEPTH") )
	{
		MinimumWaterDepth = atof(getenv("MIN_DEPTH"));
		printf("最低水深:%f\n", MinimumWaterDepth);
	}
	if (getenv("MAX_DEPTH"))
	{
		MaximumWaterDepth = atof(getenv("MAX_DEPTH"));
		printf("最大水深:%f\n", MaximumWaterDepth);
	}
	if ( getenv("CRUSTAL") )
	{
		CrustalMovement = atoi(getenv("CRUSTAL"));
		printf("地殻変動の変移を出力する\n");
	}
	if ( getenv("KML_EXPORT") )
	{
		kml_export = atoi(getenv("KML_EXPORT"));
		printf("kmlファイルを作成する\n");
	}
	if (getenv("VTK_EXPORT"))
	{
		vtk_export = atoi(getenv("VTK_EXPORT"));
		printf("vtkファイルを作成する\n");
	}
	if ( getenv("BATHEYMETRY_SMOOTH") )
	{
		batheymetry_smoothing = atoi(getenv("BATHEYMETRY_SMOOTH"));
		printf("海底地形スムージング\n");
	}
	if ( getenv("CHACKER_BORD_DEBUG") )
	{
		checker_bord_error_debug = atoi(getenv("CHACKER_BORD_DEBUG"));
		printf("チェッカーボード振動デバック\n");
	}
	if ( getenv("DAMAGE_ESTIMATION") )
	{
		damage_estimation = atoi(getenv("DAMAGE_ESTIMATION"));
		printf("被害想定評価:%s\n", damage_estimation ? "する":"しない");
	}
	if ( getenv("GLOBAL_SCALE") )
	{
		global_scale = atof(getenv("GLOBAL_SCALE"));
	}
	printf("3Dデータのスケール因子:%f\n", global_scale);

	if ( getenv("OBJ_STANDERD") )
	{
		obj_standerd = atoi(getenv("OBJ_STANDERD"));
	}
	printf("標準OBJ形式に:%s\n", obj_standerd ? "する":"しない");

	if ( getenv("OBJ_NORMALIZE") )
	{
		obj_normalize = atoi(getenv("OBJ_NORMALIZE"));
	}
	printf("3Dデータの正規化:%s\n", obj_normalize ? "する":"しない");

	if ( getenv("OBJ_MATERIAL_E") )
	{
		obj_Material[0] = atoi(getenv("OBJ_MATERIAL_E"));
	}
	if ( getenv("OBJ_MATERIAL_D") )
	{
		obj_Material[1] = atoi(getenv("OBJ_MATERIAL_D"));
	}
	if ( getenv("OBJ_MATERIAL_W") )
	{
		obj_Material[2] = atoi(getenv("OBJ_MATERIAL_W"));
	}
	printf("陸地3Dデータの色を設定:%s\n", obj_Material[0] ? "する":"しない");
	printf("海底3Dデータの色を設定:%s\n", obj_Material[1] ? "する":"しない");
	printf("流体3Dデータの色を設定:%s\n", obj_Material[2] ? "する":"しない");

	if ( getenv("EXCLUSION_RANGE_MIN") )
	{
		exclusionRange[0]  = atof(getenv("EXCLUSION_RANGE_MIN"));
	}
	if ( getenv("EXCLUSION_RANGE_MAX") )
	{
		exclusionRange[1]  = atof(getenv("EXCLUSION_RANGE_MAX"));
	}
	if ( getenv("WATER_DEPTH_SMOOTHING") )
	{
		water_depth_smoothing_num  = atof(getenv("WATER_DEPTH_SMOOTHING"));
	}
	if ( getenv("MANNING_WATAREA") )
	{
		Manning_waterArea  = atof(getenv("MANNING_WATAREA"));
	}

#ifdef CHACKER_BORD_DEBUG
	checker_bord_error_debug = 1;
#endif
#ifdef CHACKER_BORD_DEBUG2
	checker_bord_error_debug = 1;
#endif

	char* topography_data = new char[512];
	char* Initial_wave_data[3];
	Initial_wave_data[0] =  new char[512];
	Initial_wave_data[1] =  new char[512];
	Initial_wave_data[2] =  new char[512];

	char* water_depth_data = new char[512];
	char* topography_data_mask = new char[512];
	char* latitude_data = new char[512];
	char* manning_data = new char[512];
	char* elevation_data = new char[512];
	char* samplingline_data = new char[512];
	char* samplingpoint_data = new char[512];
	char* logfilename = new char[512];


	sprintf(logfilename, "%s%s%s.log", drive, dir, IDname);
	sprintf(topography_data, "%s%stopography_data.csv", drive, dir);
	sprintf(Initial_wave_data[0], "%s%sInitial_wave_dataUp.csv", drive, dir);
	sprintf(Initial_wave_data[1], "%s%sInitial_wave_dataDown.csv", drive, dir);
	sprintf(Initial_wave_data[2], "%s%sInitial_wave_data2.csv", drive, dir);
	sprintf(water_depth_data, "%s%swater_depth_data.csv", drive, dir);
	sprintf(latitude_data, "%s%slatitude_data.csv", drive, dir);
	sprintf(manning_data, "%s%smanninig_data.csv", drive, dir);
	sprintf(elevation_data, "%s%selevation_data.csv", drive, dir);
	sprintf(samplingline_data, "%s%ssamplingline_data.dat", drive, dir);
	sprintf(samplingpoint_data, "%s%ssamplingpoint_data.dat", drive, dir);

	solv.logOpen(logfilename);

#if 10
	BitMap bmp;

	sprintf(fname, "%s%stopography_data.bmp", drive, dir);
	bmp.Read(fname);

	if ( bmp.GetImage() )
	{
		bmp.WriteCsv(topography_data, 0);
	}
	bmp.Clear();

	sprintf(fname, "%s%sInitial_wave_data.bmp", drive, dir);
	bmp.Read(fname);
	if ( bmp.GetImage() )
	{
		bmp.WriteCsv(Initial_wave_data[0], 0);
		bmp.WriteCsv(Initial_wave_data[1], 2);
	}
	bmp.Clear();

	sprintf(fname, "%s%swater_depth_data.bmp", drive, dir);
	bmp.Read(fname);
	if ( bmp.GetImage() )
	{
		bmp.WriteCsv(water_depth_data, 0);
	}
	bmp.Clear();

	sprintf(fname, "%s%selevation_data.bmp", drive, dir);
	bmp.Read(fname);
	if ( bmp.GetImage() )
	{
		bmp.WriteCsv(elevation_data, 0);
	}
	bmp.Clear();

	sprintf(fname, "%s%smanning_data.bmp", drive, dir);
	bmp.Read(fname);
	if ( bmp.GetImage() )
	{
		printf("Manning data read.\n");
		bmp.WriteCsv255to1(manning_data, 0);
	}
	bmp.Clear();

	BitMap samplingline;
	BitMap samplingpoint;
	if (sampling)
	{
		sprintf(fname, "%s%ssamplingline_data.bmp", drive, dir);
		samplingline.Read(fname);
	}
	if (point_sampling)
	{
		sprintf(fname, "%s%ssamplingpoint_data.bmp", drive, dir);
		samplingpoint.Read(fname);
	}
#endif

	AbeNoguera grid(topography_data, water_depth_data, elevation_data, Initial_wave_data[0], Initial_wave_data[1], Initial_wave_data[2], impact);

	if (chkpointList.size() && longitude_flag && latitude_flag )
	{
		BitMap chkpointMap;
		sprintf(fname, "%s%schkpoint_data.bmp", drive, dir);
		chkpointMap.Read(fname);

		for (int i = 0; i < chkpointList.size(); i++)
		{
			int xx = (int)((grid.iX - 1)*(chkpointList[i].lon*M_PI / 180.0 - longitude[0]) / (longitude[1] - longitude[0]) + 0.5);
			int yy = (int)((grid.jY - 1)*(chkpointList[i].lat*M_PI / 180.0 - latitude[0]) / (latitude[2] - latitude[0]) + 0.5);

			if (xx < 0 || xx >= grid.iX || yy < 0 || yy >= grid.jY)
			{
				continue;
			}
			chkpointList[i].xx = xx;
			chkpointList[i].yy = grid.jY - yy;

			//printf("%d -- %d    %d -- %d %d\n", chkpointList[i].xxx, chkpointList[i].xx, chkpointList[i].yyy, yy, chkpointMap.cell(chkpointList[i].yy, chkpointList[i].xx).r);
		}
	}

	grid.Manning(Manning, Manning_waterArea, manning_data, solv.getLogFp());
	if ( grid.manning == NULL )
	{
		grid.MakeManninig(0.025, 1.0, manning_data);
		grid.Manning(Manning, Manning_waterArea, manning_data, solv.getLogFp());
	}

	if ( latitude_flag )
	{
		grid.MakeLatitude(latitude, latitude_data);
	}
	grid.Latitude(latitude_data);

	grid.h_min = h_min;
	grid.h_max = h_max;
	grid.w_scale[0] = w_scale[0];
	grid.w_scale[1] = w_scale[1];
	grid.e_min = e_min;
	grid.e_max = e_max;

	grid.htable = htable;
	grid.wuptable = wuptable;
	grid.wdntable = wdntable;
	grid.etable = etable;

	solv.SetData(grid);

	solv.drive = drive;
	solv.dir = dir;

	solv.absorbingZone = absorbingZone;
	solv.attenuation = attenuation;

	solv.Manning = Manning;
	solv.Coriolis_force = Coriolis_force;
	solv.Soliton = Soliton;
	solv.RunUp = RunUp;
	solv.tide_level_offset = tide_level_offset;
	solv.Revetment_h = Revetment_h;
	solv.flow_res = flow_res;
	solv.flow_res2 = flow_res2;

	solv.use_upwindow = use_upwindow_method;

	if ( solver == 0 ) solver = 3;
	solv.solver = solver;

	solv.filterCycle1 = filterCycle1;
	solv.filterCycle2 = filterCycle2;
	solv.filterCycle3 = filterCycle3;
	solv.filterCycle4 = filterCycle4;
	solv.filterOrder = filterOrder;

	solv.riverInfo.Init(grid.iX, grid.jY);
	solv.riverInfo.riverList = river;
	solv.riverInfo.inflowList = inflow;
	solv.riverInfo.barirList = barir;
	solv.river_running_time = river_running_time;
	solv.riverInfo.rainfallIntensityConst = rainfallIntensityConst;

	solv.impact = impact;
	if ( nu <= -1.0 )
	{
		//http://www.dpri.kyoto-u.ac.jp/nenpo/no38/38b2/a38b2p32.pdf
		nu = 0.01*pow(sqrt( dx*dy), 4.0/3.0);
	}
	solv.nu = -nu;
	solv.dispersion_correction_coefficient1 = dispersion_correction_coefficient1;
	solv.dispersion_correction_coefficient2 = dispersion_correction_coefficient2;
	solv.gap_max_coef = gap_max_coef;
	solv.gap_depth_coef = gap_depth_coef;

	solv.terrain_displacement = terrain_displacement;
	solv.checker_bord_theshold = checker_bord_theshold;

	solv.steep_slope1[0] = steep_slope1[0];
	solv.steep_slope1[1] = steep_slope1[1];
	solv.steep_slope2[0] = steep_slope2[0];
	solv.steep_slope2[1] = steep_slope2[1];


	switch(solver)
	{
	case 1:
		solv.log_printf("-------------線形長波理論-------------\n");
		break;
	case 2:
		solv.log_printf("-------------非線形長波理論-------------\n");
		if ( use_upwindow_method < 0 )
		{
			use_upwindow_method = 1;
			solv.use_upwindow = 1;
		}
		break;
	case 3:
	case 4:
		solv.log_printf("-------------非線形分散波理論[%s]-------------\n", (solver==3)?"Madsen and Sorensen/Boussinesq":"Peregrine");
		solv.log_printf("DISPERSION_COEF %.3f %.3f\n", dispersion_correction_coefficient1, dispersion_correction_coefficient2);
		if ( use_upwindow_method < 0 )
		{
			use_upwindow_method = 3;
			solv.use_upwindow = 3;
		}
		break;
	}
	if (use_upwindow_method == 1) solv.log_printf("移流項：1次精度風上差分法\n");
	else if (use_upwindow_method == 3) solv.log_printf("移流項：3次精度風上差分法\n");
	else solv.log_printf("移流項：2次精度中心差分法\n");



	for (int i = 0; i < 8; i++ )
	{
		unsigned char x = BIT(i);
		show_unsigned_char(solv, i, x);
	}

	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	solv.log_printf("CPUのコア数:%u\n", SysInfo.dwNumberOfProcessors);
#ifdef _OPENMP
	solv.log_printf("OpenMP[スレッド数定義:%d]\n", omp_get_max_threads());
#else
	solv.log_printf("シングルスレッドで処理\n");
#endif
	solv.log_printf("サイクル(流量フラックス):%d\n", solv.filterCycle1);
	//solv.log_printf("サイクル(陸地境界):%d\n", solv.filterCycle2);
	solv.log_printf("サイクル(波形):%d\n", solv.filterCycle2);
	solv.log_printf("サイクル(圧力振動のみ検知):%d\n", solv.filterCycle4);
	//solv.log_printf("サイクル(格子Z値ギャップ位置の計算値):%d\n", solv.filterCycle5);
	solv.log_printf("数値振動認識幅:%f 数値振動偏差:%f\n", CHECKER_BORD_THESHOLD2, solv.checker_bord_theshold);
	solv.log_printf("absorbingZone  Crejan(1985):%d alpha:%f\n", solv.absorbingZone, solv.attenuation);

	solv.log_printf("流量フラックス制限係数:%f %f\n", flow_res, flow_res2);
	solv.log_printf("水平渦動粘性係数:%f[m^2/s]\n", solv.nu);
	printDiskFreeSpace(solv, drive);

	if ( Revetment_h < 0.3 ) solv.log_printf("護岸・堤防考慮無し\n");
	else solv.log_printf("護岸・堤防条件:%f\n", Revetment_h);

	{
		TempBuffer fname(512);
		sprintf(fname.p, "%s%sdx_distance.txt", drive, dir);
		FILE* fp = fopen(fname.p, "r");
		if ( fp )
		{
			grid.r_dx = new double[grid.jY];

			TempBuffer buf(128);
			for ( int i = 0; i < grid.jY; i++ )
			{
				fgets(buf.p, 128, fp);

				grid.r_dx[i] = 1.0/atof(buf.p);
			}
			fclose(fp);
			solv.log_printf("東西方向の緯度による格子長さの補正\n");
		}else
		{
			if ( longitude_flag && latitude_flag )
			{
				double*x = longitude;
				double*y = latitude;
				int h = grid.jY;
				int w = grid.iX;

				grid.r_dx = new double[grid.jY];

				int k = 0;
				Storm_Line tmp;
				double dt = 1.0/(double)(h-1);
				for ( int i = h-1; i >= 0; i-- )
				{
					double t = (double)i*dt;
					double yy = y[0]*(1.0 - t) + y[2]*t;
					double d = tmp.distance_lambert(yy, x[0], yy, x[1]);
					grid.r_dx[k] = 1.0/(d/w);
					k++;
				}
			}
		}
	}

	if ( smooth_flg ) memcpy(solv.smooth, smooth, 9*sizeof(double));

	if ( Storm.stormList.size() >= 2 && storm_surge_only != -1)
	{
		solv.Storm_p = &Storm;
		solv.Storm_p->SetSolver( (void*)(&solv));
		solv.Storm_p->StartUpTime(20);
		solv.Storm_p->Interpolation(60);
		solv.Storm_p->ChkArea();
	}
	if ( storm_surge_only < 0 ) solv.log_printf("津波計算単独モード\n");
	if ( storm_surge_only == 0 && Storm.stormList.size() < 2)
	{
		solv.log_printf("津波・台風高潮同時計算モード=>台風定義無し=>津波計算単独モード\n");
	}
	if ( storm_surge_only == 0 && Storm.stormList.size() >= 2)
	{
		solv.log_printf("津波・台風高潮同時計算モード\n");
	}
	if ( storm_surge_only == 1 && Storm.stormList.size() < 2)
	{
		solv.log_printf("台風高潮計算単独モード=>台風定義無し\n");
		return 0;
	}
	if ( Storm.stormList.size() == 1 )
	{
		solv.log_printf("台風移動定義が不足しています\n");
	}
	if ( storm_surge_only == 1 && Storm.stormList.size() >= 2)
	{
		solv.log_printf("台風高潮単独計算モード\n");
		colorbar_max = TAKASIO_WAVE_MAX_LEVEL;
		colorbar_min = TAKASIO_WAVE_MIN_LEVEL;
	}

	if ( colorbar_min_prm != colorbar_max_prm )
	{
		colorbar_min = colorbar_min_prm;
		colorbar_max = colorbar_max_prm;
		solv.log_printf("カラー割り当てのMIN/MAXを変更しました min:%f max:%f\n", colorbar_min, colorbar_max);
	}

	if ( exclusionRange[0] >= 0.0 && exclusionRange[1] >= 0 )
	{
		solv.log_printf("相田指標：除外痕跡評価下限 %f\n", exclusionRange[0]);
		solv.log_printf("相田指標：除外痕跡評価上限 %f\n", exclusionRange[1]);
	}

#if 0
	{
		std::vector<accelerator> accs = accelerator::get_all();
		for (int i = 0; i < accs.size(); i++) 
		{		
			solv.log_printf("[%d]description:%s\n", i, WStringToString(accs[i].description).c_str());
			solv.log_printf("[%d]device_path:%s\n", i, WStringToString(accs[i].device_path).c_str());
			solv.log_printf("[%d]dedicated_memory:%d\n", i, accs[i].dedicated_memory);
			solv.log_printf("[%d]supports_double_precision:%s\n\n",
				i, accs[i].supports_double_precision ? "true" : "false");
		}

		accelerator default_acc;
		solv.log_printf("default_acc.description:%s\n", WStringToString(default_acc.description).c_str());
		solv.log_printf("default_acc.device_path:%s\n", WStringToString(default_acc.device_path).c_str());
		solv.log_printf("default_acc.dedicated_memory:%d\n", default_acc.dedicated_memory);
		solv.log_printf("default_acc.supports_double_precision:%s\n\n",
			default_acc.supports_double_precision ? "true" : "false");
#ifdef STORM_USE_GPU
		solv.log_printf("高潮計算:GPU使用\n");
#else
		solv.log_printf("高潮計算:GPU未使用\n");
#endif
#ifdef BMP_USE_GPU
		solv.log_printf("画像生成:GPU使用\n");
#else
		solv.log_printf("画像生成:GPU未使用\n");
#endif
	}
#endif
	{
		char fname[256];
		sprintf(fname, "%s%s%s_map.html", drive, dir, IDname);
		FILE* fp = fopen(fname, "w");

		if ( fp )
		fprintf(fp,
			"<script type=\"text/javascript\" src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.6.1/jquery.min.js\"></script>\n"
			"<script type=\"text/javascript\" src=\"http://maps.google.com/maps/api/js?sensor=false\"></script>\n"
			"<script type=\"text/javascript\">\n"
			"\n"
			"function conv(h, m, s)\n"
			"{\n"
			"	return h + (m / 60) + (s/3600);\n"
			"}\n"
			"\n"
			"$(document).ready(function(){\n"
			"	points = [\n"
			"		{ \n"
			"			\"lat\": %f,\n"
			"			\"lon\": %f\n"
			"		},\n"
			"		{ \n"
			"			\"lat\": %f,\n"
			"			\"lon\": %f\n"
			"		},\n"
			"		{ \n"
			"			\"lat\": %f,\n"
			"			\"lon\": %f\n"
			"		},\n"
			"		{ \n"
			"			\"lat\": %f, \n"
			"			\"lon\": %f\n"
			"		},\n"
			"		{ \n"
			"			\"lat\": %f,\n"
			"			\"lon\": %f\n"
			"		}\n"
			"	];\n"
			"	showMap(points);\n"
			"});\n"
			"\n"
			"\n"
			"\n"
			"\n"
			"function showMap(points){\n"
			"	\n"
			"	var i, map, mapOptions, lines, bounds, ll;\n"
			"	var latlngs = [];\n"
			"	\n"
			"	// 地図の作成\n"
			"	mapOptions = {\n"
			"		zoom: 3,\n"
			"		mapTypeId: google.maps.MapTypeId.ROADMAP,\n"
			"		mapTypeControlOptions: { \n"
			"			style: google.maps.MapTypeControlStyle.DROPDOWN_MENU\n"
			"		}\n"
			"	};\n"
			"\n"
			"	map = new google.maps.Map(\n"
			"		document.getElementById('map_canvas'), \n"
			"		mapOptions);\n"
			"\n"
			"	// 境界の設定\n"
			"	bounds = new google.maps.LatLngBounds();\n"
			"	\n"
			"	for(i=0;i<points.length;i++){\n"
			"		ll = new google.maps.LatLng(points[i].lat, points[i].lon);\n"
			"		latlngs.push(ll);\n"
			"		bounds.extend(ll);\n"
			"	}\n"
			"	\n"
			"	map.fitBounds(bounds);\n"
			"	\n"
			"	/*\n"
			"	// マーカーの作成	\n"
			"	for(i=0;i<latlngs.length;i++){\n"
			"		new google.maps.Marker({\n"
			"			position: latlngs[i],\n"
			"			map: map,\n"
			"			draggable: false\n"
			"		});\n"
			"	}\n"
			"	*/\n"
			"	\n"
			"	// 線を引く\n"
			"	lines = new google.maps.Polyline({\n"
			"		path: latlngs,\n"
			"		strokeColor: \"#FF0000\",\n"
			"		strokeOpacity: 1,\n"
			"		strokeWeight: 0.5\n"
			"	});\n"
			"\n"
			"	lines.setMap(map);	\n"
			"}\n"
			"</script>\n"
			"<p>地図上の矩形</p>\n"
			"<div id=\"map_canvas\" style='width:1500px;height:4000px;'></div>\n",
			latitude[0] * 180.0 / M_PI, longitude[0] * 180.0 / M_PI,
			latitude[1] * 180.0 / M_PI, longitude[1] * 180.0 / M_PI,
			latitude[3] * 180.0 / M_PI, longitude[3] * 180.0 / M_PI,
			latitude[2] * 180.0 / M_PI, longitude[2] * 180.0 / M_PI,
			latitude[0] * 180.0 / M_PI, longitude[0] * 180.0 / M_PI
			);
		if (fp) fclose(fp);
	}
	{
		//グローバル座標でのX軸とY軸を求める
		if ( longitude && latitude )
		{
			double t = 1.0/(double)(grid.jY-1);
			double s = 1.0/(double)(grid.iX-1);
			double z2 = 0.0;
			double v1[3];
			double v2[3];
			double xx[3], yy[3], zz[3];

			int i = 0;
			int j = 0;
			double lat = latitude[2]*(1.0 - i*t) + latitude[0]*i*t;
			double lon = longitude[0]*(1.0 - j*s) + longitude[1]*j*s;

			const double f84 = 1.0/298.257223563;
			const double a = 6378137.000;
			const double e2 = f84*(2.0 - f84);
					
			double ee = 1.0 - e2*sin(lat)*sin(lat);
			if ( ee < 0.0 ) ee = 0.0;
			double N = a/sqrt(ee);

			v1[0] = (N+z2)*cos(lat)*cos(lon);
			v1[1] = (N+z2)*cos(lat)*sin(lon);
			v1[2] = (N*(1.0 -e2)+z2)*sin(lat);

			i = 0;
			j = grid.iX-1;
			lat = latitude[2]*(1.0 - i*t) + latitude[0]*i*t;
			lon = longitude[0]*(1.0 - j*s) + longitude[1]*j*s;

			ee = 1.0 - e2*sin(lat)*sin(lat);
			if ( ee < 0.0 ) ee = 0.0;
			N = a/sqrt(ee);

			v2[0] = (N+z2)*cos(lat)*cos(lon);
			v2[1] = (N+z2)*cos(lat)*sin(lon);
			v2[2] = (N*(1.0 -e2)+z2)*sin(lat);

			double ln = (v1[0] - v2[0])*(v1[0] - v2[0])+(v1[1] - v2[1])*(v1[1] - v2[1])+(v1[2] - v2[2])*(v1[2] - v2[2]);
			if ( ln > 0.0001 )
			{
				ln = sqrt(ln);
				double x = (v2[0] - v1[0])/ln;
				double y = (v2[1] - v1[1])/ln;
				double z = (v2[2] - v1[2])/ln;
				solv.log_printf("X %f %f %f\n", x, y, z);
				xx[0] = x;
				xx[1] = y;
				xx[2] = z;
			}



			i = grid.jY-1;
			j = 0;
			lat = latitude[2]*(1.0 - i*t) + latitude[0]*i*t;
			lon = longitude[0]*(1.0 - j*s) + longitude[1]*j*s;

			ee = 1.0 - e2*sin(lat)*sin(lat);
			if ( ee < 0.0 ) ee = 0.0;
			N = a/sqrt(ee);

			v2[0] = (N+z2)*cos(lat)*cos(lon);
			v2[1] = (N+z2)*cos(lat)*sin(lon);
			v2[2] = (N*(1.0 -e2)+z2)*sin(lat);

			ln = (v1[0] - v2[0])*(v1[0] - v2[0])+(v1[1] - v2[1])*(v1[1] - v2[1])+(v1[2] - v2[2])*(v1[2] - v2[2]);
			if ( ln > 0.0001 )
			{
				ln = sqrt(ln);
				double x = (v2[0] - v1[0])/ln;
				double y = (v2[1] - v1[1])/ln;
				double z = (v2[2] - v1[2])/ln;
				solv.log_printf("Y %f %f %f\n", x, y, z);
				yy[0] = x;
				yy[1] = y;
				yy[2] = z;

				x = xx[1]*yy[2] - xx[2]*yy[1];
				y = xx[2]*yy[0] - xx[0]*yy[2];
				z = xx[0]*yy[1] - xx[1]*yy[0];
				ln = sqrt(x*x + y*y + z*z);
				if ( ln > 0.0001 )
				{
					solv.log_printf("Z %f %f %f\n", x/ln, y/ln, z/ln);
				}
			}
		}
	}

	bool water_depth_and_elevation = false;
	//精度の高い水深・標高データを使う。
	{
		char fname[256];

		//水深データ
		sprintf(fname, "%s%swater_depth_data3.csv", drive, dir);
		FILE* fp = fopen(fname, "r");
		if ( !fp )
		{
			sprintf(fname, "%s%swater_depth_data2.csv", drive, dir);
			fp = fopen(fname, "r");
		}else
		{
			water_depth_and_elevation = true;
		}
		if ( water_depth_and_elevation ) printf("[水深+標高]一括データを使用\n");

		double* water_depth_and_elevation_ptr = 0;

		if ( fp )
		{
			fclose(fp);
			if ( water_depth_and_elevation )
			{
				sprintf(water_depth_data, "%s%swater_depth_data3.csv", drive, dir);
			}else
			{
				sprintf(water_depth_data, "%s%swater_depth_data2.csv", drive, dir);
			}

			if (grid.h) delete [] grid.h;
			
			grid.h = ReadCsv(water_depth_data,grid.iX, grid.jY);

			//穴埋め補間
			if (water_depth_and_elevation)
			{
				water_depth_and_elevation_ptr = new double[grid.iX*grid.jY];
				memcpy(water_depth_and_elevation_ptr, grid.h, sizeof(double)*grid.iX*grid.jY);

				sprintf(fname, "%s%stopography_data_fill.bmp", drive, dir);

				BitMap fill;
				fill.Read(fname);

				if (fill.GetImage())
				{
					solv.log_printf("穴埋め補間 START\n");
					const int absorbingZone_ = solv.absorbingZone;
					const int IX = grid.iX;
					const int JY = grid.jY;

					bool fillrun = true;
					double* elv = new double[grid.iX*grid.jY];

					while (fillrun)
					{
						memcpy(elv, water_depth_and_elevation_ptr, sizeof(double)*grid.iX*grid.jY);
						for (int i = 0; i < JY; i++)
						{
							for (int j = 0; j < IX; j++)
							{
								//穴埋め位置
								if (IS_TOPO_FILL(fill.cell(i, j)))
								{
									const int wd = 2;
									double zz_e = 0.0;
									double num = 0;
									for (int ki = -wd; ki <= wd; ki++)
									{
										for (int kj = -wd; kj <= wd; kj++)
										{
											if (i + ki < 0 || i + ki >= grid.jY) continue;
											if (j + kj < 0 || j + kj >= grid.iX) continue;
											if (ki == 0 && kj == 0) continue;

											if (fabs(water_depth_and_elevation_ptr[(i + ki)*IX + (j + kj)]) < 0.00001)
											{
												continue;
											}
											if (TOPO_FILL_FLAG(fill.cell(i + ki, j + kj)) == 0)
											{
												zz_e += water_depth_and_elevation_ptr[(i + ki)*IX + (j + kj)];
												num++;
											}
										}
									}
									if (num >= 2*wd)
									{
										elv[i*IX + j] = zz_e / num;
										//printf("%f %f => %f\n", zz_e, num, elv[i*IX + j]);
										TOPO_FILL_FLAG_SET(fill.cell(i, j),1);
									}
								}
							}
						}
						for (int i = 0; i < JY; i++)
						{
							for (int j = 0; j < IX; j++)
							{
								if (TOPO_FILL_FLAG(fill.cell(i, j))==1)
								{
									TOPO_FILL_FLAG_SET(fill.cell(i, j),0);
								}
							}
						}
						memcpy(water_depth_and_elevation_ptr, elv, sizeof(double)*grid.iX*grid.jY);

						fillrun = false;
						for (int i = 0; i < JY; i++)
						{
							for (int j = 0; j < IX; j++)
							{
								//穴埋め位置
								if (IS_TOPO_FILL(fill.cell(i, j)))
								{
									fillrun = true;
									break;
								}
							}
							if (fillrun) break;
						}
					}
					delete[] elv;

					sprintf(fname, "%s%stopography_data_fill_out.bmp", drive, dir);
					fill.Write(fname);

					//穴埋めした場所の陸地/水域フラグ設定
					if (fill.GetImage())
					{
						int fillNum = -1;
						while (fillNum)
						{
							fillNum = 0;
							for (int i = 0; i < JY; i++)
							{
								for (int j = 0; j < IX; j++)
								{
									if (color_eq(fill.cell(i, j), 0, 102, 0)|| 
										color_eq(fill.cell(i, j), 0, 103, 0)|| 
										color_eq(fill.cell(i, j), 0, 104, 0))
									{
										fillNum++;
										int water = 0;
										int elv = 0;
										const double wd = 10;
										for (int ki = -wd; ki <= wd; ki++)
										{
											for (int kj = -wd; kj <= wd; kj++)
											{
												if (i + ki < 0 || i + ki >= grid.jY) continue;
												if (j + kj < 0 || j + kj >= grid.iX) continue;
												if (IS_TOPO_FILL(fill.cell(i + ki, j + kj)))
												{
													continue;
												}
												if (color_eq(fill.cell(i + ki, j + kj), 0, 102, 0) || 
													color_eq(fill.cell(i + ki, j + kj), 0, 103, 0) || 
													color_eq(fill.cell(i + ki, j + kj), 0, 104, 0))
												{
													continue;
												}
												if (grid.T(i + ki, j + kj) == 0) elv++;
												if (grid.T(i + ki, j + kj) != 0) water++;
											}
										}
										if (elv > 3 || water > 3)
										{
											if (color_eq(fill.cell(i, j), 0, 102, 0))
											{
												if (elv > water) grid.T(i, j) = 0;
												else grid.T(i, j) = 255;
											}
											if (color_eq(fill.cell(i, j), 0, 103, 0))
											{
												grid.T(i, j) = 0;
											}
											if (color_eq(fill.cell(i, j), 0, 104, 0))
											{
												grid.T(i, j) = 255;
											}
											fill.cell(i, j).r = 1;
										}
									}
								}
							}
						}
					}

					{
						BitMap top;
						top.Copy(fill);


						for (int i = 0; i < JY; i++)
						{
							for (int j = 0; j < IX; j++)
							{
								if (grid.T(i, j) == 0)
								{
									top.cell(i, j).r = 0;
									top.cell(i, j).g = 0;
									top.cell(i, j).b = 0;
								}
								if (grid.T(i, j) != 0)
								{
									top.cell(i, j).r = 255;
									top.cell(i, j).g = 255;
									top.cell(i, j).b = 255;
								}
							}
						}
						sprintf(fname, "%s%stopography_data_fill_out_top.bmp", drive, dir);
						top.Write(fname);
					}

					//穴埋め箇所の平滑化
					double* wrk = new double[IX*JY];
					for ( int kk = 0; kk < 5; kk++ )
					{
						memcpy(wrk, water_depth_and_elevation_ptr, sizeof(double)*IX*JY);
						for (int i = 1; i < JY-1; i++)
						{
							for (int j = 1; j < IX-1; j++)
							{
								if (color_eq(fill.cell(i, j), 1, 102, 0)|| 
									color_eq(fill.cell(i, j), 1, 103, 0)|| 
									color_eq(fill.cell(i, j), 1, 104, 0)|| 
									(fabs(water_depth_and_elevation_ptr[i*IX+j]) < 1.0e-16)
									)
								{
									wrk[i*IX+j] = 0.25*(
										water_depth_and_elevation_ptr[(i-1)*IX+j]+
										water_depth_and_elevation_ptr[(i+1)*IX+j]+
										water_depth_and_elevation_ptr[i*IX+(j-1)]+
										water_depth_and_elevation_ptr[i*IX+(j+1)]);
								}
							}
						}
						memcpy(water_depth_and_elevation_ptr, wrk, sizeof(double)*IX*JY);
					}
					delete [] wrk;
					solv.log_printf("穴埋め補間 END\n");
				}
			}

			if (water_depth_and_elevation_ptr)
			{
				memcpy(grid.h, water_depth_and_elevation_ptr, sizeof(double)*grid.iX*grid.jY);
			}

			//水深の符号を変更
			double min = 9999999999.0;
			double max = -9999999999.0;
			for ( int i = 0; i < grid.jY; i++ )
			{
				for ( int j = 0; j < grid.iX; j++ )
				{
					grid.h[i*grid.iX+j] = -grid.h[i*grid.iX+j];
					if ( grid.topog[i*grid.iX+j] != 0 )
					{
						if ( min > grid.h[i*grid.iX+j] ) min = grid.h[i*grid.iX+j];
						if ( max < grid.h[i*grid.iX+j] ) max = grid.h[i*grid.iX+j];
					}
				}
			}
			printf("depth min %f max %f\n", min, max);
		}
		

		if ( water_depth_and_elevation )
		{
			sprintf(fname, "%s%swater_depth_data3.csv", drive, dir);
		}else
		{
			sprintf(fname, "%s%selevation_data2.csv", drive, dir);
		}
		fp = fopen(fname, "r");
		if ( fp )
		{
			fclose(fp);
			if ( water_depth_and_elevation )
			{
				sprintf(elevation_data, "%s%swater_depth_data3.csv", drive, dir);
			}else
			{
				sprintf(elevation_data, "%s%selevation_data2.csv", drive, dir);
			}
			if(grid.elevation)delete [] grid.elevation;
			
			grid.elevation = ReadCsv(elevation_data,grid.iX, grid.jY);
			if (water_depth_and_elevation_ptr)
			{
				memcpy(grid.elevation, water_depth_and_elevation_ptr, sizeof(double)*grid.iX*grid.jY);
			}
			
			if ( water_depth_and_elevation_ptr )
			{
				delete[] water_depth_and_elevation_ptr;
			}
			water_depth_and_elevation_ptr = NULL;


			double min = 9999999999.0;
			double max = -9999999999.0;
			for ( int i = 0; i < grid.jY; i++ )
			{
				for ( int j = 0; j < grid.iX; j++ )
				{
					//if ( grid.topog[i*grid.iX+j] == 0 && grid.elevation[i*grid.iX+j] < 0.0 )
					//{
					//	grid.elevation[i*grid.iX+j] = 0.0;
					//}
					if ( grid.topog[i*grid.iX+j] == 0 )
					{
						if ( min > grid.elevation[i*grid.iX+j] ) min = grid.elevation[i*grid.iX+j];
						if ( max < grid.elevation[i*grid.iX+j] ) max = grid.elevation[i*grid.iX+j];
					}
				}
			}
			printf("elevation min %f max %f\n", min, max);
			solv.LinearInterpolation = false;
		}
	}
	solv.UpdateGridInfo();


	if ( solv.LinearInterpolation ) solv.log_printf("標高と水深データを線形補間する\n");
	else solv.log_printf("標高と水深データを線形補間しない\n");
	if ( solv.Data->wave_LinearInterpolation ) solv.log_printf("初期波源を線形補間する\n");
	else solv.log_printf("初期波源を線形補間しない\n");



	//time_step *= 0.25;
	//time_step *= 0.9;
	solv.Setup0(time_step, dx, dy);

	if ( Dmin_flag ) solv.Dmin = Dmin;
	else
	{
		double d1 = solv.dx*VIRTUAL_DEPTH_SIGMA;
		double d2 = solv.dy*VIRTUAL_DEPTH_SIGMA;
		double d = d1;
		if ( d > d2 ) d = d2;
		solv.Dmin = d;

		printf("仮想水深:%f\n", d);
		if ( d < D_UNDER_LIMIT ) solv.Dmin = D_UNDER_LIMIT;
		else solv.Dmin = d;
	}
	solv.log_printf("==>Dmin:%f\n", solv.Dmin);

	solv.gap_max = solv.gap_max_coef*ADVECTION_TERM_DEPTH;
	solv.gap_depth = solv.gap_depth_coef*ADVECTION_TERM_DEPTH;
	solv.log_printf("今は未使用（計算に寄与しない）/GAPMAX(段差)：%f ->%f[m] (DEPTH(認識する最大水深):%f)\n", solv.gap_max_coef, solv.gap_max, solv.gap_depth);

	solv.log_printf("急勾配1: 最低水深:%f 近傍との差:%f\n", solv.steep_slope1[0], solv.steep_slope1[1]);
	solv.log_printf("急勾配2: 最低水深:%f 近傍との差:%f\n", solv.steep_slope2[0], solv.steep_slope2[1]);

	if (batheymetry_smoothing)
	{
		//地形と海底を接続した形状を出力
		{
			sprintf(fname, "%s%s%s", drive, dir, "chk\\smooth_pre.obj");

			double* ww = new double[grid.iX*grid.jY];
#pragma omp parallel for OMP_SCHEDULE
			for ( int i = 0; i < grid.jY; i++ )
			{
				for ( int j = 0; j < grid.iX; j++ )
				{
					ww[i*grid.iX+j] = -grid.h[i*grid.iX+j];
					if ( grid.topog[i*grid.iX+j] == 0 )
					{
						ww[i*grid.iX+j] = grid.elevation[i*grid.iX+j];
					}
				}
			}
			CsvToObj obj3(ww, grid.iX, grid.jY, 0, 0);
			obj3.r_dx = solv.Data->r_dx;
			obj3.dx = solv.dx;
			obj3.dy = solv.dy;
			obj3.zscale = Z_SCALE_DPT;
			obj3.vertex_color = 0;
			obj3.Color(109,50,50);
			//丸みを付ける
			if ( latitude_flag ) obj3.latitude = latitude;
			if ( longitude_flag ) obj3.longitude = longitude;
			obj3.Output(fname, 0);
			delete [] ww;
		}

		solv.DeleteBoundaryBandMap();
		solv.CreateBoundaryBandMap(6);

		//平滑化
		for ( int kk = 0; kk < 2; kk++ )
		{
			double* ee = new double[grid.iX*grid.jY];
			memcpy(ee, grid.elevation, sizeof(double)*grid.iX*grid.jY);

			double* hh = new double[grid.iX*grid.jY];
			memcpy(hh, grid.h, sizeof(double)*grid.iX*grid.jY);

			//Z値が標高・水深なので処理では丸みを付けてはいけない。
			CsvToObj obj(0, grid.iX, grid.jY, 0, 0);

			obj.dx = solv.dx;
			obj.dy = solv.dy;
			obj.laplacianSmoothing(grid.h, grid.elevation, grid.topog, 0.3, 9999999.0);

#pragma omp parallel for OMP_SCHEDULE
			for ( int i = 1; i < grid.jY-1; i++ )
			{
				for ( int j = 1; j < grid.iX-1; j++ )
				{
					if ( solv.BoundaryBand(i,j, 6) )
					{
						if( grid.T(i,j) != 0 )
						{
							hh[i*grid.iX+j] = grid.H(i,j);
						}
						else
						{
							//if ( kk == 0 && solv.BoundaryBand(i,j, 1) )
							//{
							//	ee[i*grid.iX+j] = grid.ELV(i,j);
							//	if ( ee[i*grid.iX+j] < -0.1 )
							//	{
							//		printf("ELV:%f\n", ee[i*grid.iX+j]);
							//		grid.topog[i*grid.iX+j] = 255;
							//	}
							//}
						}
					}
				}
			}
			delete [] grid.elevation;
			delete [] grid.h;
			grid.h = hh;
			grid.elevation = ee;
		}

		//地形と海底を接続した形状を出力
		{
			sprintf(fname, "%s%s%s", drive, dir, "chk\\smooth_post.obj");

			double* ww = new double[grid.iX*grid.jY];
#pragma omp parallel for OMP_SCHEDULE
			for ( int i = 0; i < grid.jY; i++ )
			{
				for ( int j = 0; j < grid.iX; j++ )
				{
					ww[i*grid.iX+j] = -grid.h[i*grid.iX+j];
					if ( grid.topog[i*grid.iX+j] == 0 )
					{
						ww[i*grid.iX+j] = grid.elevation[i*grid.iX+j];
					}
				}
			}
			CsvToObj obj3(ww, grid.iX, grid.jY, 0, 0);
			obj3.dx = solv.dx;
			obj3.dy = solv.dy;
			obj3.r_dx = solv.Data->r_dx;
			//丸みを付ける
			if ( latitude_flag ) obj3.latitude = latitude;
			if ( longitude_flag ) obj3.longitude = longitude;
			obj3.zscale = Z_SCALE_DPT;
			obj3.Color(50,50,109);
			obj3.vertex_color = 0;
			obj3.Output(fname, 0);
			delete [] ww;
		}
	}


	//陸地境界を滑らかにして異常な地形を修正する
	double* elv = new double[grid.iX*grid.jY];
	for ( int i = 0; i < grid.jY; i++ )
	{
		for ( int j = 0; j < grid.iX; j++ )
		{
			if ( grid.topog[i*grid.iX+j] != 0)
			{
				grid.elevation[i*grid.iX+j] = -grid.H(i,j);
			}
		}
	}
	memcpy(elv, grid.elevation, sizeof(double)*grid.iX*grid.jY);

	solv.DeleteBoundaryBandMap();
	solv.CreateBoundaryBandMap(10);

#if 0
	for ( int i = 1; i < grid.jY-1; i++ )
	{
		for ( int j = 1; j < grid.iX-1; j++ )
		{
			//陸地境界
			if ( grid.topog[i*grid.iX+j] == 0 && solv.BoundaryBand(i, j, 10))
			{
				double z1 = grid.ELV(i-1,j);
				double z2 = grid.ELV(i+1,j);
				double z3 = grid.ELV(i,j-1);
				double z4 = grid.ELV(i,j+1);

				double av = 0.25*(z1+z2+z3+z4);
				double z = av + 0.85*(grid.ELV(i,j) - av);
				//if ( z <= EDIT_ELV_HEIGHT_LIM ) z = EDIT_ELV_HEIGHT_LIM;
				if ( z < grid.ELV(i,j) ) elv[i*grid.iX+j] = z;
			}
			if ( grid.topog[i*grid.iX+j] != 0 && solv.BoundaryBand(i, j, 10) )
			{
				double z1 = grid.ELV(i-1,j); if ( grid.T(i-1,j) == 0 && z1 < 0.0 ) z1 = 0.0;
				double z2 = grid.ELV(i+1,j); if ( grid.T(i+1,j) == 0 && z2 < 0.0 ) z2 = 0.0;
				double z3 = grid.ELV(i,j-1); if ( grid.T(i,j-1) == 0 && z3 < 0.0 ) z3 = 0.0;
				double z4 = grid.ELV(i,j+1); if ( grid.T(i,j+1) == 0 && z4 < 0.0 ) z4 = 0.0;

				double av = 0.25*(z1+z2+z3+z4);
				elv[i*grid.iX+j] = av;
			}
		}
	}
	memcpy(grid.elevation, elv, sizeof(double)*grid.iX*grid.jY);
	delete [] elv;
	


	elv = new double[grid.iX*grid.jY];
	for ( int k = 0; k < 2; k++ )
	{
		memcpy(elv, grid.elevation, sizeof(double)*grid.iX*grid.jY);

		solv.CreateGapFlg();

		for ( int i = 1; i < grid.jY-1; i++ )
		{
			for ( int j = 1; j < grid.iX-1; j++ )
			{
				if ( grid.topog[i*grid.iX+j] != 0 )
				{
					if ( grid.T(i,j) != 0 && solv.GAP(i,j) & GAP_1)
					{
						double z1 = grid.ELV(i-1,j); if ( grid.T(i-1,j) == 0 && z1 < 0.0 ) z1 = 0.0;
						double z2 = grid.ELV(i+1,j); if ( grid.T(i+1,j) == 0 && z2 < 0.0 ) z2 = 0.0;
						double z3 = grid.ELV(i,j-1); if ( grid.T(i,j-1) == 0 && z3 < 0.0 ) z3 = 0.0;
						double z4 = grid.ELV(i,j+1); if ( grid.T(i,j+1) == 0 && z4 < 0.0 ) z4 = 0.0;

						double av = 0.25*(z1+z2+z3+z4);
						elv[i*grid.iX+j] = av;
					}
				}
			}
		}
		memcpy(grid.elevation, elv, sizeof(double)*grid.iX*grid.jY);
		solv.DeleteGapFlg();
	}
	delete [] elv;
#else
	for ( int k = 0; k < water_depth_smoothing_num; k++ )
	{
		memcpy(elv, grid.elevation, sizeof(double)*grid.iX*grid.jY);

		for ( int i = 1; i < grid.jY-1; i++ )
		{
			for ( int j = 1; j < grid.iX-1; j++ )
			{
				if ( grid.topog[i*grid.iX+j] != 0 || solv.BoundaryBand(i, j, 10))
				{
					double z1 = grid.ELV(i - 1, j);
					double z2 = grid.ELV(i + 1, j);
					double z3 = grid.ELV(i, j - 1);
					double z4 = grid.ELV(i, j + 1);

					if (grid.T(i - 1, j) == 0 && z1 < 0.0) z1 = 0.0;
					if (grid.T(i + 1, j) == 0 && z2 < 0.0) z2 = 0.0;
					if (grid.T(i, j - 1) == 0 && z3 < 0.0) z3 = 0.0;
					if (grid.T(i, j + 1) == 0 && z4 < 0.0) z4 = 0.0;

					if (grid.T(i - 1, j) != 0 && z1 > -MINIMUM_DEPTH2) z1 = -MINIMUM_DEPTH2;
					if (grid.T(i + 1, j) != 0 && z2 > -MINIMUM_DEPTH2) z2 = -MINIMUM_DEPTH2;
					if (grid.T(i, j - 1) != 0 && z3 > -MINIMUM_DEPTH2) z3 = -MINIMUM_DEPTH2;
					if (grid.T(i, j + 1) != 0 && z4 > -MINIMUM_DEPTH2) z4 = -MINIMUM_DEPTH2;

					double av = 0.25*(z1 + z2 + z3 + z4);
					elv[i*grid.iX+j] = av;
				}
			}
		}
		memcpy(grid.elevation, elv, sizeof(double)*grid.iX*grid.jY);
	}
	delete[] elv;
#endif
	solv.DeleteBoundaryBandMap();


	solv.CreateBoundaryBandMap(4);
	elv = new double[grid.iX*grid.jY];
	for ( int k = 0; k < water_depth_smoothing_num*3; k++ )
	{
		memcpy(elv, grid.elevation, sizeof(double)*grid.iX*grid.jY);

		for ( int i = 1; i < grid.jY-1; i++ )
		{
			for ( int j = 1; j < grid.iX-1; j++ )
			{
				if ( /*grid.topog[i*grid.iX+j] != 0 ||*/ solv.BoundaryBand(i, j, 4))
				{
					double z1 = grid.ELV(i - 1, j);
					double z2 = grid.ELV(i + 1, j);
					double z3 = grid.ELV(i, j - 1);
					double z4 = grid.ELV(i, j + 1);

					if (grid.T(i - 1, j) == 0 && z1 < 0.0) z1 = 0.0;
					if (grid.T(i + 1, j) == 0 && z2 < 0.0) z2 = 0.0;
					if (grid.T(i, j - 1) == 0 && z3 < 0.0) z3 = 0.0;
					if (grid.T(i, j + 1) == 0 && z4 < 0.0) z4 = 0.0;

					if (grid.T(i - 1, j) != 0 && z1 > -MINIMUM_DEPTH2) z1 = -MINIMUM_DEPTH2;
					if (grid.T(i + 1, j) != 0 && z2 > -MINIMUM_DEPTH2) z2 = -MINIMUM_DEPTH2;
					if (grid.T(i, j - 1) != 0 && z3 > -MINIMUM_DEPTH2) z3 = -MINIMUM_DEPTH2;
					if (grid.T(i, j + 1) != 0 && z4 > -MINIMUM_DEPTH2) z4 = -MINIMUM_DEPTH2;

					double av = 0.25*(z1 + z2 + z3 + z4);
					elv[i*grid.iX+j] = av;
				}
			}
		}
		memcpy(grid.elevation, elv, sizeof(double)*grid.iX*grid.jY);
	}
	delete[] elv;
	solv.DeleteBoundaryBandMap();


	for ( int i = 0; i < grid.jY; i++ )
	{
		for ( int j = 0; j < grid.iX; j++ )
		{
			if ( grid.T(i,j) == 0 && grid.ELV(i,j) < EDIT_ELV_HEIGHT_LIM)
			{
				grid.topog[i*grid.iX+j] = 255;
			}
			if ( grid.T(i,j) != 0 && grid.ELV(i,j) > EDIT_WTR_DEPTH)
			{
				grid.topog[i*grid.iX+j] = 0;
			}

			if ( grid.T(i,j) != 0 )
			{
				grid.h[i*grid.iX+j]  = -grid.ELV(i,j);
			}
		}
	}


	{
		TempBuffer fname;
		sprintf(fname.p, "%s%stopography_data_mask.bmp", drive, dir);
		topography_data_mask_bmp.Read(fname.p);
		
		//google map等
		sprintf(fname.p, "%s%stopography_data_mask2.bmp", drive, dir);
		topography_data_mask2_bmp.Read(fname.p);
	}


	//河川等の地形修正
	{
		sprintf(fname, "%s%stopography_data_edit.bmp", drive, dir);
		topography_data_edit.Read(fname);

		if (topography_data_edit.GetImage())
		{
			const int absorbingZone_ = solv.absorbingZone;
			const int IX = grid.iX;
			const int JY = grid.jY;

			for (int i = 1; i < JY - 1; i++)
			{
				for (int j = 1; j < IX - 1; j++)
				{
					//侵入不可領域
					if (IS_INVIOLABILITY(topography_data_edit.cell(i, j)))
					{
						solv.SetInviolability(i, j) = 1;
					}

					//堤防
					if (IS_BARIR(topography_data_edit.cell(i, j)))
					{
						for (int k = 0; k < solv.riverInfo.barirList.size(); k++)
						{
							if (color_eq(topography_data_edit.cell(i, j), solv.riverInfo.barirList[k].color))
							{
								solv.riverInfo.setBarir(i, j, k);
							}
						}
					}
					//河川経路
					if (IS_RIVER_PATH(topography_data_edit.cell(i, j)))
					{
						for (int k = 0; k < solv.riverInfo.riverList.size(); k++)
						{
							if (color_eq(topography_data_edit.cell(i, j), solv.riverInfo.riverList[k].color))
							{
								solv.riverInfo.setRiver(i, j, k);
							}
						}
					}

					//川上の流入位置
					if (IS_INFLOW(topography_data_edit.cell(i, j)))
					{
						for (int k = 0; k < solv.riverInfo.inflowList.size(); k++)
						{
							if (color_eq(topography_data_edit.cell(i, j), solv.riverInfo.inflowList[k].color))
							{
								solv.riverInfo.setInflow(i, j, k);
							}
						}
					}
				}
			}


			//河川底の平滑化
			elv = new double[grid.iX*grid.jY];
			for (int kk = 0; kk < water_depth_smoothing_num * 15; kk++)
			{
				memcpy(elv, grid.elevation, sizeof(double)*grid.iX*grid.jY);
				for (int i = 1; i < JY - 1; i++)
				{
					for (int j = 1; j < IX - 1; j++)
					{
						if (!solv.riverInfo.isRiver_or_Inflow(i, j))
						{
							continue;
						}

						{
							double zz = 0;
							double num = 0;

							//自分自身を含めて周囲９箇所の平均を取る
							for (int ki = -1; ki <= 1; ki++)
							{
								for (int kj = -1; kj <= 1; kj++)
								{
									if (i + ki < 0 || i + ki >= JY) continue;
									if (j + kj < 0 || j + kj >= IX) continue;

									if (!solv.riverInfo.isRiver_or_Inflow(i + ki, j + kj))
									{
										//河川底以外
										continue;
									}
									//if ( grid.T(i+ki, j+kj) == 0 )
									{
										zz += grid.ELV(i + ki, j + kj);
										num++;
									}
								}
							}
							if (num > 0)
							{
								elv[i*IX + j] = zz / num;
							}
						}
					}
				}
				memcpy(grid.elevation, elv, sizeof(double)*grid.iX*grid.jY);
			}

			//河川の縁
			for (int kk = 0; kk < 2; kk++)
			{
				memcpy(elv, grid.elevation, sizeof(double)*grid.iX*grid.jY);
				for (int i = 1; i < JY - 1; i++)
				{
					for (int j = 1; j < IX - 1; j++)
					{
						if (solv.riverInfo.isRiver_or_Inflow(i, j))
						{
							continue;
						}

						//河川底以外
						bool smooth_on = false;

						const int wd = 1;
						for (int ki = -wd; ki <= wd; ki++)
						{
							for (int kj = -wd; kj <= wd; kj++)
							{
								if (i + ki < 0 || i + ki >= JY) continue;
								if (j + kj < 0 || j + kj >= IX) continue;
								//河川経路
								if (solv.riverInfo.isRiver(i + ki, j + kj))
								{
									smooth_on = true;
									break;
								}

								//川上の流入位置
								if (solv.riverInfo.isInflow(i + ki, j + kj))
								{
									smooth_on = true;
									break;
								}
							}
							if (smooth_on) break;
						}

						if (smooth_on)
						{
							double zz = 0;
							double num = 0;

							//自分自身を含めて周囲９箇所の平均を取る
							for (int ki = -1; ki <= 1; ki++)
							{
								for (int kj = -1; kj <= 1; kj++)
								{
									if (i + ki < 0 || i + ki >= JY) continue;
									if (j + kj < 0 || j + kj >= IX) continue;
									//if ( grid.T(i+ki, j+kj) == 0 )
									{
										zz += grid.ELV(i + ki, j + kj);
										num++;
									}
								}
							}
							if (num > 0)
							{
								elv[i*IX + j] = zz / num;
							}
						}
					}
				}
				memcpy(grid.elevation, elv, sizeof(double)*grid.iX*grid.jY);
			}

			delete[] elv;


			///
			for (int i = ZERO_AREA_WD; i < JY - ZERO_AREA_WD; i++)
			{
				for (int j = ZERO_AREA_WD; j < IX - ZERO_AREA_WD; j++)
				{
					//陸地
					if (grid.topog[i*grid.iX + j] == 0)
					{
						int id = -1;
						//堤防（障害壁）
						if ((id = solv.riverInfo.isBarir(i, j) - 1) >= 0)
						{
							double h = solv.riverInfo.barirList[id].zup;
							grid.elevation[i*grid.iX + j] += h;
						}

						//河川経路の標高値を下げる
						if ((id = solv.riverInfo.isRiver(i, j) - 1) >= 0)
						{
							if (grid.topog[i*grid.iX + j] != 0)
							{
								//陸地の扱いに変更する
								grid.topog[i*grid.iX + j] = 0;
							}

							grid.elevation[i*grid.iX + j] -= solv.riverInfo.riverList[id].depth;

							//自動堤防生成
							for (int k = 0; k < solv.riverInfo.riverList.size(); k++)
							{
								if (solv.riverInfo.riverList[k].Barrier_height)
								{
									for (int ki = -1; ki <= 1; ki++)
									{
										for (int kj = -1; kj <= 1; kj++)
										{
											//if (IsAbsorbingZoneBounray(i+ki, j+kj))
											//{
											//	continue;
											//}

											//堤防生成しない印
											if (NO_CONFLUENCE(topography_data_edit.cell(i + ki, j + kj)))
											{
												continue;
											}
											//川上の流入位置なので堤防生成しない
											if (solv.riverInfo.isInflow(i + ki, j + kj))
											{
												continue;
											}

											if (solv.riverInfo.isBarir(i + ki, j + kj))
											{
												continue;
											}
											if (!(solv.riverInfo.isRiver(i + ki, j + kj)))
											{
												grid.elevation[(i + ki)*grid.iX + j + kj] += solv.riverInfo.riverList[k].Barrier_height;
											}
										}
									}
								}
							}
						}

						//川上の流入位置
						if ((id = solv.riverInfo.isInflow(i, j) - 1) >= 0)
						{
							if (grid.topog[i*grid.iX + j] != 0)
							{
								//陸地の扱いに変更する
								grid.topog[i*grid.iX + j] = 0;
							}

							grid.elevation[i*grid.iX + j] -= solv.riverInfo.inflowList[id].depth;
							//ここだけは少しだけ高くして川下に流れるようにする
							grid.elevation[i*grid.iX + j] += 0.01;
						}

						//海域への変更
						//陸地
						if (grid.topog[i*grid.iX + j] == 0)
						{
							if (TO_WATER(topography_data_edit.cell(i, j)))
							{
								double e = grid.elevation[(i)*grid.iX + j];

								e = e - 3.0;
								if (e > EDIT_WTR_DEPTH)
								{
									e = EDIT_WTR_DEPTH;
								}
								if (grid.elevation[i*grid.iX + j] > e) grid.elevation[i*grid.iX + j] = e;
								grid.topog[i*grid.iX + j] = 255;
							}
						}
					}
					else
					{
						//島の場合
						if (TO_LAND(topography_data_edit.cell(i, j)))
						{
							double e = 0.25*(grid.h[(i - 1)*grid.iX + j] + grid.h[(i + 1)*grid.iX + j] + grid.h[i*grid.iX + (j - 1)] + grid.h[i*grid.iX + (j - 1)]);

							e = e + 2.5;
							if (e < EDIT_WTR_DEPTH)
							{
								e = EDIT_WTR_DEPTH;
							}
							if (e > grid.elevation[i*grid.iX + j])
							{
								grid.elevation[i*grid.iX + j] = e;
							}
							grid.topog[i*grid.iX + j] = 0;
						}
					}
				}
			}

			{
				double* tmp = new double[IX*JY];
				for ( int k = 0; k < 20; k++ )
				{
					memcpy(tmp, grid.elevation, sizeof(double)*IX*JY);
					for (int i = ZERO_AREA_WD; i < JY - ZERO_AREA_WD; i++)
					{
						for (int j = ZERO_AREA_WD; j < IX - ZERO_AREA_WD; j++)
						{
							//陸地
							if (grid.topog[i*grid.iX + j] == 0)
							{
								int id = -1;


								for (int ki = -3; ki <= 3; ki++)
								{
									for (int kj = -3; kj <= 3; kj++)
									{
										//堤防（障害壁）
										if ((id = solv.riverInfo.isBarir(i+ki, j+kj) - 1) >= 0)
										{
											if (solv.riverInfo.barirList[id].zup < 0)
											{
												grid.elevation[i*grid.iX + j] = 0.25*(tmp[(i - 1)*IX + j] + tmp[(i + 1)*IX + j] + tmp[i*IX + j - 1] + tmp[i*IX + j + 1]);
											}
										}
									}
								}
							}
						}
					}
				}
				delete [] tmp;
			}

			{
				elv = new double[grid.iX*grid.jY];
				for (int kk = 0; kk < water_depth_smoothing_num * 3; kk++)
				{
					memcpy(elv, grid.elevation, sizeof(double)*grid.iX*grid.jY);
					for (int i = ZERO_AREA_WD; i < JY - ZERO_AREA_WD; i++)
					{
						for (int j = ZERO_AREA_WD; j < IX - ZERO_AREA_WD; j++)
						{
							//陸地
							if (grid.topog[i*grid.iX + j] == 0 && !solv.riverInfo.isBarir(i, j))
							{
								int id = -1;
								//堤防（障害壁）
								for (int ki = -2; ki <= 2; ki++)
								{
									for (int kj = -2; kj <= 2; kj++)
									{
										//堤防（障害壁）
										if ((id = solv.riverInfo.isBarir(i + ki, j + kj) - 1) >= 0)
										{
											if (solv.riverInfo.barirList[id].zup > 0)
											{
												elv[i*grid.iX + j] = 0.25*(grid.elevation[(i - 1)*IX + j] + grid.elevation[(i + 1)*IX + j] + grid.elevation[i*IX + j - 1] + grid.elevation[i*IX + j + 1]);
											}
										}
									}
								}
							}
						}
					}
					memcpy(grid.elevation, elv, sizeof(double)*grid.iX*grid.jY);
				}
				delete[] elv;
			}

			//自動的に河川の水深を計算する
			if ( 1 )
			{
				solv.riverInfo.AutoWaterDepth();
				for (int k = 0; k < solv.riverInfo.riverList.size(); k++)
				{
					for (int i = ZERO_AREA_WD; i < JY - ZERO_AREA_WD; i++)
					{
						for (int j = ZERO_AREA_WD; j < IX - ZERO_AREA_WD; j++)
						{
							if (!(solv.riverInfo.isRiver_or_Inflow(i, j)))
							{
								continue;
							}

							//河川内の地盤高さ
							double d = grid.elevation[(i)*grid.iX + j];

							//河川内から周囲を探索して最も近い河川外を見つける
							bool lookup = false;

							int wd = 1;
							while (!lookup/* && wd < grid.iX*0.25*/)
							{
								double h = 0.0;
								double num = 0.0;
								for (int ki = -wd; ki <= wd; ki++)
								{
									for (int kj = -wd; kj <= wd; kj++)
									{
										if (i + ki < 0 || i + ki >= grid.jY) continue;
										if (j + kj < 0 || j + kj >= grid.iX) continue;

										if (solv.riverInfo.isBarir(i + ki, j + kj))
										{
											continue;
										}
										if (!(solv.riverInfo.isRiver_or_Inflow(i + ki, j + kj)))
										{
											double dd = grid.elevation[(i + ki)*grid.iX + j + kj];
											if (dd > d)
											{
												h += grid.elevation[(i + ki)*grid.iX + j + kj];
												num++;
												lookup = true;
											}
										}
									}
								}
								if (lookup)
								{
									solv.riverInfo.waterDepth[i*grid.iX + j] = h / num - d;
									if (solv.riverInfo.waterDepth[i*grid.iX + j] < 0.0)
									{
										solv.riverInfo.waterDepth[i*grid.iX + j] = 0.0;
										lookup = false;
									}
									//if (lookup)printf("%f\n", solv.riverInfo.waterDepth[i*grid.iX + j]);
								}
								wd += 1;
							}
						}
					}
				}

				double *wrk = new double[grid.iX*grid.jY];

				for (int k = 0; k < 5; k++)
				{
					memcpy(wrk, solv.riverInfo.waterDepth, sizeof(double)*grid.iX*grid.jY);
					for (int i = 1; i < JY - 1; i++)
					{
						for (int j = 1; j < IX -1; j++)
						{
							wrk[i*IX + j] = 0.25*(solv.riverInfo.WaterDepth(i-1,j) + solv.riverInfo.WaterDepth(i+1,j) + solv.riverInfo.WaterDepth(i,j-1) + solv.riverInfo.WaterDepth(i,j+1));
						}
					}
					memcpy(solv.riverInfo.waterDepth, wrk, sizeof(double)*grid.iX*grid.jY);
				}
				delete[] wrk;
			}


#if 10
			{
				char* p = getenv("DEBUG_RIVER_INFO");
				if (p && atoi(p) > 0)
				{
					//画像に反映
					for (int i = ZERO_AREA_WD; i < JY - ZERO_AREA_WD; i++)
					{
						for (int j = ZERO_AREA_WD; j < IX - ZERO_AREA_WD; j++)
						{
							Rgb color(255, 255, 255);
							double alp = 0.5;

							if (solv.riverInfo.isBarir(i, j))
							{
								if (topography_data_mask_bmp.GetImage() != NULL)
								{
									double r = (double)topography_data_mask_bmp.cell(i, j).r*alp + (double)color.r*(1.0 - alp);
									double g = (double)topography_data_mask_bmp.cell(i, j).g*alp + (double)color.g*(1.0 - alp);
									double b = (double)topography_data_mask_bmp.cell(i, j).b*alp + (double)color.b*(1.0 - alp);

									topography_data_mask_bmp.cell(i, j) = Rgb(r, g, b);
								}
								if (topography_data_mask2_bmp.GetImage() != NULL)
								{
									double r = (double)topography_data_mask2_bmp.cell(i, j).r*alp + (double)color.r*(1.0 - alp);
									double g = (double)topography_data_mask2_bmp.cell(i, j).g*alp + (double)color.g*(1.0 - alp);
									double b = (double)topography_data_mask2_bmp.cell(i, j).b*alp + (double)color.b*(1.0 - alp);

									topography_data_mask2_bmp.cell(i, j) = Rgb(r, g, b);
								}
							}
							if (solv.riverInfo.isRiver(i, j))
							{
								Rgb color(71, 147, 239);
								double alp = 0.5;

								if (topography_data_mask_bmp.GetImage() != NULL)
								{
									double r = (double)topography_data_mask_bmp.cell(i, j).r*alp + (double)color.r*(1.0 - alp);
									double g = (double)topography_data_mask_bmp.cell(i, j).g*alp + (double)color.g*(1.0 - alp);
									double b = (double)topography_data_mask_bmp.cell(i, j).b*alp + (double)color.b*(1.0 - alp);

									topography_data_mask_bmp.cell(i, j) = Rgb(r, g, b);
								}
								if (topography_data_mask2_bmp.GetImage() != NULL)
								{
									double r = (double)topography_data_mask2_bmp.cell(i, j).r*alp + (double)color.r*(1.0 - alp);
									double g = (double)topography_data_mask2_bmp.cell(i, j).g*alp + (double)color.g*(1.0 - alp);
									double b = (double)topography_data_mask2_bmp.cell(i, j).b*alp + (double)color.b*(1.0 - alp);

									topography_data_mask2_bmp.cell(i, j) = Rgb(r, g, b);
								}
							}
						}
					}
				}
			}
#endif
		}
	}

	//陸地の水深ゼロにする
	for ( int i = 0; i < grid.jY; i++ )
	{
		for ( int j = 0; j < grid.iX; j++ )
		{
			if ( grid.h[i*grid.iX+j]  < MinimumWaterDepth ) grid.h[i*grid.iX+j] = MinimumWaterDepth;
			if (grid.h[i*grid.iX + j]  > MaximumWaterDepth && MaximumWaterDepth > 0.0) grid.h[i*grid.iX + j] = MaximumWaterDepth;
			if ( grid.topog[i*grid.iX+j] == 0 ) grid.h[i*grid.iX+j] = 0.0;
		}
	}



	for ( int i = 0; i < grid.jY; i++ )
	{
		for ( int j = 0; j < grid.iX; j++ )
		{
			if ( grid.h[i*grid.iX+j]  < 0.0 ) printf("=======> %f\n", grid.h[i*grid.iX+j]);
		}
	}

	debug_marker = new float[grid.jY*grid.iX];
	memset(debug_marker, '\0', sizeof(float)*grid.iX*grid.jY);

	solv.CreateBoundaryBandMap(10);

	//初期波源の修正
	#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 0; i < grid.jY; i++ )
	{
		for ( int j = 0; j < grid.iX; j++ )
		{
			//（陸地に波は立たない)
			if ( grid.topog[i*grid.iX+j] == 0 || solv.isAbsorbingZoneBounray(i,j) || fabs(grid.w[i*grid.iX+j]) < 0.00001 ) grid.w[i*grid.iX+j] = 0.0;
		}
	}

	sprintf(fname, "%s%stopography_data_nothing_area.bmp", drive, dir);
	topography_data_nothing_area.Read(fname);

	sprintf(fname, "%s%stopography_data_residential_area.bmp", drive, dir);
	topography_data_residential_area.Read(fname);

	//被害算出
	Victim* victim = new Victim[grid.iX*grid.jY];
	#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 0; i < grid.jY; i++ )
	{
		for ( int j = 0; j < grid.iX; j++ )
		{
			if ( grid.topog[i*grid.iX+j] == 0 )
			{
				victim[i*grid.iX + j].average_population_density_adp = average_population_density;
				victim[i*grid.iX+j].average_population_density_n_adp = average_population_density;
				victim[i*grid.iX+j].HouseDensity(house_number_rate);
				victim[i*grid.iX+j].area = dx*dy;

				if (topography_data_nothing_area.GetImage())
				{
					if (color_eq(topography_data_nothing_area.cell(i,j), 255, 0, 0))
					{
						victim[i*grid.iX + j].average_population_density_adp = 0;
						victim[i*grid.iX + j].average_population_density_n_adp = 0;
						victim[i*grid.iX + j].house_density = 0;
						victim[i*grid.iX + j].house_number_rate = 0;						
					}
				}
				if (topography_data_residential_area.GetImage())
				{
					if (!color_eq(topography_data_residential_area.cell(i, j), 255, 0, 0))
					{
						victim[i*grid.iX + j].average_population_density_adp = 0;
						victim[i*grid.iX + j].average_population_density_n_adp = 0;
						victim[i*grid.iX + j].house_density = 0;
						victim[i*grid.iX + j].house_number_rate = 0;
					}
				}
			}
		}
	}

	grid.h_org = new double[grid.iX*grid.jY];
	memcpy(grid.h_org, grid.h, sizeof(double)*(grid.iX*grid.jY));

	int crustalMovement_count = 0;
	if ( CrustalMovement )
	{
		outputCrustalMovement(solv, grid, 0, 0, Z_SCALE_WAV, Z_SCALE_DPT, crustalMovement_count, exe_drive, exe_dir, drive, dir, latitude_flag, latitude, longitude_flag, longitude);
	}

	//地殻変動設定
	if ( solv.Setup(time_step, dx, dy) != 0)
	{
		printf("水域が存在していません\n");
		//throw;
	}


	//変移結果を保存
	double* w_last = new double[grid.iX*grid.jY];
	memcpy(w_last, grid.w, sizeof(double)*(grid.iX*grid.jY));

	//変移開始状態へのリセット
	solv.w_init = 1;
	memset(grid.w, '\0', sizeof(double)*(grid.iX*grid.jY));	//本当の初期状態は波高は全域で0


	sprintf(fname, "%s%sdebug_water_depth_data.csv", drive, dir);
	WriteCsv(fname, grid.iX, grid.jY, grid.h, 1.0);
	sprintf(fname, "%s%sdebug_Initial_wave_data.csv", drive, dir);
	WriteCsv(fname, grid.iX, grid.jY, grid.w, 1.0);
	//exit(0);

	ElvContourBitmpa(solv, EDIT_ELV_HEIGHT_LIM, EDIT_ELV_HEIGHT_LIM/2, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa(solv, EDIT_ELV_HEIGHT_LIM/2, EDIT_ELV_HEIGHT_LIM/3, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa(solv, EDIT_ELV_HEIGHT_LIM/3, 0.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa(solv, 0.0, 1.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa(solv, 0.0, 3.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa(solv, 0.0, 5.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa(solv, 0.0, 15.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa(solv, 0.0, 25.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa(solv, 0.0, 35.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa(solv, 0.0, solv.emax, drive, dir, exe_drive, exe_dir);

	ElvContourBitmpa1(solv, EDIT_WTR_DEPTH, 0.1, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, 0.1, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, 1.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, 5.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, 10.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, 15.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, 20.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, 35.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, 50.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, 100.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, 150.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, 300.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, 400.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, 500.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, 600.0, drive, dir, exe_drive, exe_dir);
	ElvContourBitmpa1(solv, 0.0, solv.hmax, drive, dir, exe_drive, exe_dir);

	ElvContourBitmpa2(solv, solv.hmax, solv.emax, drive, dir, exe_drive, exe_dir);
	
	{
		char fname[256];

		BitMap depthColor;
		sprintf(fname, "%s%s..\\colormap\\%s", exe_drive, exe_dir, "depth_colormap .bmp");
		depthColor.Read(fname);

		sprintf(fname, "%s%s%s", drive, dir, "obj\\water_depth_data.obj");
		CsvToObj obj1(grid.h_org, grid.iX, grid.jY, 0, 0);

		obj1.dx = solv.dx;
		obj1.dy = solv.dy;
		obj1.r_dx = solv.Data->r_dx;
		//丸みを付ける
		if ( latitude_flag ) obj1.latitude = latitude;
		if ( longitude_flag ) obj1.longitude = longitude;
		obj1.sign = -1;
		obj1.zscale = Z_SCALE_DPT;
		if ( obj_Material[1] ) obj1.Color(3,24,112);
		//obj1.smoothL();
		obj1.offset = 0.0;
		//obj1.Output(fname, 1);
		
		VertexColor* vertexcolor = new VertexColor[grid.iX*grid.jY];	
		obj1.vertex_color = vertexcolor;
		obj1.colormap = &depthColor;

#pragma omp parallel for OMP_SCHEDULE
		for ( int i = 0; i < grid.jY; i++ )
		{
			for ( int j = 0; j < grid.iX; j++ )
			{
				double h = (obj1.Vertex(i,j) - solv.hmin)/(solv.hmax - solv.hmin);
				if ( h < 0.0 ) h = 0.0;
				if ( h > 1.0 ) h = 1.0;

				int color_index = (int)(h*(double)depthColor.W()-1);
				if ( color_index < 0 ) color_index = 0;
				if ( color_index >= depthColor.W() )
				{
					obj1.vertexColor(i,j).r = 255;
					obj1.vertexColor(i,j).g = 255;
					obj1.vertexColor(i,j).b = 255;
				}else
				{
					obj1.vertexColor(i,j).r = depthColor.cell(depthColor.H()/2, color_index).r;
					obj1.vertexColor(i,j).g = depthColor.cell(depthColor.H()/2, color_index).g;
					obj1.vertexColor(i,j).b = depthColor.cell(depthColor.H()/2, color_index).b;
				}
			}
		}
		if ( !obj_Material[1] ) obj1.vertex_color = 0;

		obj1.globalScale = global_scale;
		if ( obj_normalize )
		{
			obj1.scan = true;
			obj1.normalization = true;
		}

		obj1.Output(fname, 0);
		if (vtk_export)
		{
#pragma omp parallel for OMP_SCHEDULE
			for (int i = 0; i < grid.jY; i++)
			{
				for (int j = 0; j < grid.iX; j++)
				{
					double h = (obj1.Vertex(i, j) - solv.hmin) / (solv.hmax - solv.hmin);
					if (h < 0.0) h = 0.0;
					if (h > 1.0) h = 1.0;

					int color_index = (int)(h*(double)depthColor.W() - 1);
					if (color_index < 0) color_index = 0;
					obj1.vertexColor(i, j).id = color_index;
				}
			}
			sprintf(fname, "%s%s%s", drive, dir, "obj\\vtk\\water_depth_data.vtk");
			obj1.vtk_export = 1;
			obj1.Output(fname, 0);
		}

		delete[] vertexcolor;
		if ( obj_standerd )
		{
			TempBuffer cmd(512);
			sprintf(cmd.p, "%s%s%s %s %s", exe_drive, exe_dir, "OBJConvFormat.exe -fcol ", fname, fname);
			if ( obj_Material[1] ) cmdProcess(cmd.p);
		}


		//標高データ
		BitMap elevationColor;
		sprintf(fname, "%s%s..\\colormap\\%s", exe_drive, exe_dir, "elevation_colormap.bmp");
		elevationColor.Read(fname);

		sprintf(fname, "%s%s%s", drive, dir, "obj\\elevation_data.obj");
		CsvToObj obj2(grid.elevation, grid.iX, grid.jY, 0, 0);
		obj2.dx = solv.dx;
		obj2.dy = solv.dy;
		obj2.r_dx = solv.Data->r_dx;
		//丸みを付ける
		if ( latitude_flag ) obj2.latitude = latitude;
		if ( longitude_flag ) obj2.longitude = longitude;
		obj2.zscale = Z_SCALE_ELV;
		if ( obj_Material[0] ) obj2.Color(109,87,56);
		
		vertexcolor = new VertexColor[grid.iX*grid.jY];	
		obj2.vertex_color = vertexcolor;
		obj2.colormap = &elevationColor;

		if ( topography_data_mask2_bmp.GetImage() == NULL )
		{
			char fname[256];
			sprintf(fname, "%s%stopography_data_mask2.bmp", drive, dir);
			topography_data_mask2_bmp.Read(fname);
		}

#pragma omp parallel for OMP_SCHEDULE
		for ( int i = 0; i < grid.jY; i++ )
		{
			for ( int j = 0; j < grid.iX; j++ )
			{
				double h = (obj2.Vertex(i,j) - solv.emin)/(solv.emax - solv.emin);
				if ( h < 0.0 ) h = 0.0;
				if ( h > 1.0 ) h = 1.0;

				int color_index = (int)(h*(double)elevationColor.W()-1);
				if ( color_index < 0 ) color_index = 0;
				if ( color_index >= elevationColor.W() )
				{
					obj2.vertexColor(i,j).r = 255;
					obj2.vertexColor(i,j).g = 255;
					obj2.vertexColor(i,j).b = 255;
				}else
				{
					obj2.vertexColor(i,j).r = elevationColor.cell(elevationColor.H()/2, color_index).r;
					obj2.vertexColor(i,j).g = elevationColor.cell(elevationColor.H()/2, color_index).g;
					obj2.vertexColor(i,j).b = elevationColor.cell(elevationColor.H()/2, color_index).b;

					if ( topography_data_mask2_bmp.GetImage() )
					{
						//if ( topography_data_mask2_bmp.cell(i,j).r >90 && topography_data_mask2_bmp.cell(i,j).g < 80 && topography_data_mask2_bmp.cell(i,j).b < 80)
						//{
						//	/* empty */
						//}else
						{
							obj2.vertexColor(i,j).r = topography_data_mask2_bmp.cell(i,j).r;
							obj2.vertexColor(i,j).g = topography_data_mask2_bmp.cell(i,j).g;
							obj2.vertexColor(i,j).b = topography_data_mask2_bmp.cell(i,j).b;
						}
					}
				}
				if ( grid.T(i,j) != 0 )
				{
					obj2.vertexColor(i,j).r = 19;
					obj2.vertexColor(i,j).g = 22;
					obj2.vertexColor(i,j).b = 43;
				}
			}
		}
		if ( !obj_Material[0] ) obj2.vertex_color = 0;

		obj2.globalScale = global_scale;
		obj2.minz = -solv.hmax*Z_SCALE_DPT*1.5;
		if ( obj_normalize )
		{
			obj2.normalization = true;
		}

		obj2.Output(fname, 0, grid.topog);
		if (vtk_export)
		{
#pragma omp parallel for OMP_SCHEDULE
			for (int i = 0; i < grid.jY; i++)
			{
				for (int j = 0; j < grid.iX; j++)
				{
					double h = (obj2.Vertex(i, j) - solv.emin) / (solv.emax - solv.emin);
					if (h < 0.0) h = 0.0;
					if (h > 1.0) h = 1.0;

					int color_index = (int)(h*(double)elevationColor.W() - 1);
					if (color_index < 0) color_index = 0;
					obj2.vertexColor(i, j).id = color_index;
					if (grid.T(i, j) != 0)
					{
						obj2.vertexColor(i, j).id = elevationColor.W();
					}
				}
			}
			sprintf(fname, "%s%s%s", drive, dir, "obj\\vtk\\elevation_data.vtk");
			obj2.vtk_export = 2;
			obj2.Output(fname, 0, grid.topog);
		}

		delete [] vertexcolor;
		if ( obj_standerd )
		{
			TempBuffer cmd(512);
			sprintf(cmd.p, "%s%s%s %s %s", exe_drive, exe_dir, "OBJConvFormat.exe -fcol ", fname, fname);
			if ( obj_Material[0] )cmdProcess(cmd.p);
		}
	}
	//exit(0);


	//地形と海底を接続した形状を出力
	{
		BitMap depthColor;
		sprintf(fname, "%s%s..\\colormap\\%s", exe_drive, exe_dir, "depth_colormap .bmp");
		depthColor.Read(fname);

		BitMap elevationColor;
		sprintf(fname, "%s%s..\\colormap\\%s", exe_drive, exe_dir, "elevation_colormap.bmp");
		elevationColor.Read(fname);

		sprintf(fname, "%s%schk\\%s%s", drive, dir, IDname, "_fullMap.obj");

		double* ww = new double[grid.iX*grid.jY];
#pragma omp parallel for OMP_SCHEDULE
		for ( int i = 0; i < grid.jY; i++ )
		{
			for ( int j = 0; j < grid.iX; j++ )
			{
				ww[i*grid.iX+j] = -grid.h_org[i*grid.iX+j];
				if ( grid.topog[i*grid.iX+j] == 0 )
				{
					ww[i*grid.iX+j] = grid.elevation[i*grid.iX+j];
				}
			}
		}
		CsvToObj obj3(ww, grid.iX, grid.jY, 0, 0);
		obj3.dx = solv.dx;
		obj3.dy = solv.dy;
		obj3.r_dx = solv.Data->r_dx;
		//丸みを付ける
		if ( latitude_flag ) obj3.latitude = latitude;
		if ( longitude_flag ) obj3.longitude = longitude;
		obj3.zscale = Z_SCALE_DPT;
		obj3.Color(109,87,56);
		
		VertexColor* vertexcolor = new VertexColor[grid.iX*grid.jY];	
		obj3.vertex_color = vertexcolor;

#pragma omp parallel for OMP_SCHEDULE
		for ( int i = 0; i < grid.jY; i++ )
		{
			for ( int j = 0; j < grid.iX; j++ )
			{
				if ( grid.topog[i*grid.iX+j] != 0 )
				{
					double h = (-obj3.Vertex(i,j) - solv.hmin)/(solv.hmax - solv.hmin);
					if ( h < 0.0 ) h = 0.0;
					if ( h > 1.0 ) h = 1.0;

					int color_index = (int)(h*(double)depthColor.W()-1);
					if ( color_index < 0 ) color_index = 0;
					if ( color_index >= depthColor.W() )
					{
						obj3.vertexColor(i,j).r = 255;
						obj3.vertexColor(i,j).g = 255;
						obj3.vertexColor(i,j).b = 255;
					}else
					{
						obj3.vertexColor(i,j).r = depthColor.cell(depthColor.H()/2, color_index).r;
						obj3.vertexColor(i,j).g = depthColor.cell(depthColor.H()/2, color_index).g;
						obj3.vertexColor(i,j).b = depthColor.cell(depthColor.H()/2, color_index).b;
					}
				}else
				{
					double h = (obj3.Vertex(i,j) - solv.emin)/(solv.emax - solv.emin);
					if ( h < 0.0 ) h = 0.0;
					if ( h > 1.0 ) h = 1.0;

					int color_index = (int)(h*(double)elevationColor.W()-1);
					if ( color_index < 0 ) color_index = 0;
					if ( color_index >= elevationColor.W() )
					{
						obj3.vertexColor(i,j).r = 255;
						obj3.vertexColor(i,j).g = 255;
						obj3.vertexColor(i,j).b = 255;
					}else
					{
						obj3.vertexColor(i,j).r = elevationColor.cell(elevationColor.H()/2, color_index).r;
						obj3.vertexColor(i,j).g = elevationColor.cell(elevationColor.H()/2, color_index).g;
						obj3.vertexColor(i,j).b = elevationColor.cell(elevationColor.H()/2, color_index).b;
					}
				}
			}
		}
		obj3.globalScale = global_scale;
		if ( obj_normalize )
		{
			obj3.normalization = true;
		}
		obj3.Output(fname, 0);
		delete [] vertexcolor;
		delete [] ww;
		if ( obj_standerd )
		{
			TempBuffer cmd(512);
			sprintf(cmd.p, "%s%s%s %s %s", exe_drive, exe_dir, "OBJConvFormat.exe -fcol ", fname, fname);
			cmdProcess(cmd.p);
		}
	}
	//exit(0);
	{
		CsvToObj obj;
		//丸みを付ける
		if ( latitude_flag ) obj.latitude = latitude;
		if ( longitude_flag ) obj.longitude = longitude;
		
		sprintf(fname, "%s%s%s", drive, dir, "obj\\Earth.obj");
		obj.globalScale = global_scale;
		if ( obj_normalize )
		{
			obj.normalization = true;
		}
		obj.OutputEarth(fname);
		if ( obj_standerd )
		{
			TempBuffer cmd(512);
			sprintf(cmd.p, "%s%s%s %s %s", exe_drive, exe_dir, "OBJConvFormat.exe -fcol ", fname, fname);
			cmdProcess(cmd.p);
		}

		sprintf(fname, "%s%s%s", drive, dir, "obj\\Earth2.obj");
		obj.globalScale = global_scale;
		if ( obj_normalize )
		{
			obj.normalization = true;
		}
		obj.OutputEarth2(fname);
	}

	{
		BitMap depthColor;
		sprintf(fname, "%s%s..\\colormap\\%s", exe_drive, exe_dir, "depth_colormap .bmp");
		depthColor.Read(fname);

		BitMap elevationColor;
		sprintf(fname, "%s%s..\\colormap\\%s", exe_drive, exe_dir, "elevation_colormap.bmp");
		elevationColor.Read(fname);

		sprintf(fname, "%s%schk\\%s%s", drive, dir, IDname, "_fullMap.obj");

		CsvToObj obj;

		double* depth = 0;
		double* elv = 0;
		int* tflg = 0;
		double zmin;
		obj.Read(fname, Z_SCALE_ELV, grid.iX, grid.jY, &depth, &elv, &tflg, zmin);

		sprintf(fname, "%s%s%s", drive, dir, "chk\\elevation_data__2.obj");
		CsvToObj obj2(elv, grid.iX, grid.jY, 0, 0);
		obj2.dx = solv.dx;
		obj2.dy = solv.dy;
		//obj2.r_dx = solv.Data->r_dx;
		//if ( latitude_flag ) obj2.latitude = latitude;
		//if ( longitude_flag ) obj2.longitude = longitude;
		obj2.zscale = 1.0;
		obj2.minz = zmin*2.0;
		obj2.Color(109,87,56);
		VertexColor* vertexcolor = new VertexColor[grid.iX*grid.jY];	
		obj2.vertex_color = vertexcolor;

#pragma omp parallel for OMP_SCHEDULE
		for ( int i = 0; i < grid.jY; i++ )
		{
			for ( int j = 0; j < grid.iX; j++ )
			{
				double h = (obj2.Vertex(i,j) - solv.emin)/(solv.emax - solv.emin);
				if ( h < 0.0 ) h = 0.0;
				if ( h > 1.0 ) h = 1.0;

				int color_index = (int)(h*(double)elevationColor.W()-1);
				if ( color_index < 0 ) color_index = 0;
				if ( color_index >= elevationColor.W() )
				{
					obj2.vertexColor(i,j).r = 255;
					obj2.vertexColor(i,j).g = 255;
					obj2.vertexColor(i,j).b = 255;
				}else
				{
					obj2.vertexColor(i,j).r = elevationColor.cell(elevationColor.H()/2, color_index).r;
					obj2.vertexColor(i,j).g = elevationColor.cell(elevationColor.H()/2, color_index).g;
					obj2.vertexColor(i,j).b = elevationColor.cell(elevationColor.H()/2, color_index).b;
				}
			}
		}

		obj2.Output(fname, 0, tflg);
		delete [] vertexcolor;

#if 0
		sprintf(fname, "%s%s%s", drive, dir, "chk\\water_depth_data__2.obj");
		CsvToObj obj1(depth, grid.iX, grid.jY, 0, 0);

		obj1.dx = solv.dx;
		obj1.dy = solv.dy;
		//obj1.r_dx = solv.Data->r_dx;
		//if ( latitude_flag ) obj1.latitude = latitude;
		//if ( longitude_flag ) obj1.longitude = longitude;
		obj1.sign = -1;
		obj1.zscale = 1.0;
		obj1.Color(93,94,192);
		
		vertexcolor = new VertexColor[grid.iX*grid.jY];	
		obj1.vertex_color = vertexcolor;

#pragma omp parallel for OMP_SCHEDULE
		for ( int i = 0; i < grid.jY; i++ )
		{
			for ( int j = 0; j < grid.iX; j++ )
			{
				double h = (obj1.Vertex(i,j) - solv.hmin)/(solv.hmax - solv.hmin);
				if ( h < 0.0 ) h = 0.0;
				if ( h > 1.0 ) h = 1.0;

				int color_index = (int)(h*(double)depthColor.W()-1);
				if ( color_index < 0 ) color_index = 0;
				if ( color_index >= depthColor.W() )
				{
					obj1.vertexColor(i,j).r = 255;
					obj1.vertexColor(i,j).g = 255;
					obj1.vertexColor(i,j).b = 255;
				}else
				{
					obj1.vertexColor(i,j).r = depthColor.cell(depthColor.H()/2, color_index).r;
					obj1.vertexColor(i,j).g = depthColor.cell(depthColor.H()/2, color_index).g;
					obj1.vertexColor(i,j).b = depthColor.cell(depthColor.H()/2, color_index).b;
				}
			}
		}

		obj1.Output(fname, 0);
		delete [] vertexcolor;


		BitMap bmp;

		sprintf(fname, "%s%s%s", drive, dir, "chk\\elevation_data__2.csv");
		WriteCsv(fname, grid.iX, grid.jY, elv,  1.0);
		bmp.ReadCsv(fname, e_min, e_max);
		sprintf(fname, "%s%s%s", drive, dir, "chk\\elevation_data__2.bmp");
		bmp.Write(fname);
		bmp.Clear();

		sprintf(fname, "%s%s%s", drive, dir, "chk\\water_depth_data__2.csv");
		WriteCsv(fname, grid.iX, grid.jY, depth,  1.0);
		bmp.ReadCsv(fname, h_min, h_max);
		sprintf(fname, "%s%s%s", drive, dir, "chk\\water_depth_data__2.bmp");
		bmp.Write(fname);
		bmp.Clear();

		sprintf(fname, "%s%s%s", drive, dir, "chk\\topography_data__2.csv");
		WriteCsv(fname, grid.iX, grid.jY, tflg,  1.0);
		bmp.ReadCsv(fname, 0.0, 1.0);
		sprintf(fname, "%s%s%s", drive, dir, "chk\\topography_data__2.bmp");
		bmp.Write(fname);
		bmp.Clear();
#endif
		delete [] depth;
		delete [] elv;
		delete [] tflg;
	}
	delete [] grid.h_org;
	grid.h_org = 0;


	solv.RunupConditionFlag = new int[solv.jY*solv.iX];
	memset(solv.RunupConditionFlag, '\0', sizeof(int)*solv.jY*solv.iX);
#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < solv.iX*solv.jY; ii__++)
	//for (int i = ZERO_AREA_WD; i < solv.jY; i++)
	{
		int i = ii__ / solv.iX;
		int j = ii__ % solv.iX;
		if (i < ZERO_AREA_WD || i >= solv.jY-ZERO_AREA_WD || j < ZERO_AREA_WD || j >= solv.iX-ZERO_AREA_WD)
		{
			continue;
		}
		//for (int j = ZERO_AREA_WD; j < solv.iX; j++)
		{
			if (solv.Data->T(i, j) == 0) solv.RunupConditionFlag[i*solv.iX + j] = IS_DRY;
			else solv.RunupConditionFlag[i*solv.iX + j] = IS_WATER;
		}
	}

	solv.d2vale = new double[solv.jY*solv.iX];
	memset(solv.d2vale, '\0', sizeof(double)*solv.jY*solv.iX);

	sprintf(fname, "%s%s..\\colormap\\wave_colormap.bmp", exe_drive, exe_dir);
	wave_colormap.Read(fname);

	sprintf(fname, "%s%s..\\colormap\\colormap_default.bmp", exe_drive, exe_dir);
	colormap_default.Read(fname);

	
	if ( solv.RunUp )
	{
		sprintf(fname, "%s%s..\\colormap\\run_up_colormap0.bmp", exe_drive, exe_dir);
		runup_color_level0.Read(fname);

		sprintf(fname, "%s%s..\\colormap\\run_up_colormap.bmp", exe_drive, exe_dir);
		runup_color_level.Read(fname);

		if ( solv.RunUpMarker == NULL )
		{
			solv.RunUpMarker = new int[solv.jY*solv.iX];
			for ( int i = 0; i < solv.jY*solv.iX; i++ ) solv.RunUpMarker[i] = -1;
		}
	}

	if (chkpointList.size())
	{
		const int sz = chkpointList.size();

		int *num = new int[sz];
		memset(num, '\0', sizeof(int)*sz);

		double *elvsum = new double[sz];
		memset(elvsum, '\0', sizeof(double)*sz);

		double *elvminmax = new double[2*sz];
		memset(elvminmax, '\0', sizeof(double)*2*sz);
		for ( int k = 0; k < sz; k++ )
		{
			elvminmax[2*k+0] = 9999999999.0;
			elvminmax[2*k+1] = -9999999999.0;
		}

		for ( int ii = 0; ii < grid.jY; ii++ )
		{
			for ( int jj = 0; jj < grid.iX; jj++ )
			{
				for ( int k = 0; k < sz; k++ )
				{
					if ( chkpointList[k].type <= 3 )
					{
						continue;
					}
					if ( chkpointList[k].xx == jj && chkpointList[k].yy == ii )
					{
						if ( solv.Data->T(ii,jj) == 0 )
						{
							num[k]++;
							elvsum[k] += solv.Data->ELV(ii,jj);
							if ( elvminmax[2*k+0] > solv.Data->ELV(ii,jj)) elvminmax[2*k+0] = solv.Data->ELV(ii,jj);
							if ( elvminmax[2*k+1] < solv.Data->ELV(ii,jj)) elvminmax[2*k+1] = solv.Data->ELV(ii,jj);
						}
					}
				}
			}
		}

		for ( int k = 0; k < sz; k++ )
		{
			if ( chkpointList[k].type < 3 )
			{
				continue;
			}
			if ( num[k] == 0 )
			{
				chkpointList[k].ref = true;
				continue;
			}

			double av =  elvsum[k]/(double)num[k];

			if ( fabs(elvminmax[2*k+1] - elvminmax[2*k+0]) > 2.0 )
			{
				printf("浸水深:%f=>痕跡高　区画高低差:%fのため不採用\n", chkpointList[k].trace, fabs(elvminmax[2*k+1] - elvminmax[2*k+0]));
				chkpointList[k].ref = true;
				continue;
			}
			printf("浸水深:%f=>痕跡高:%f\n", chkpointList[k].trace, chkpointList[k].trace+av);
			chkpointList[k].trace += av;
		}
		delete [] num;
		delete [] elvsum;
		delete [] elvminmax;
	}

	BitMap ColorMap;


	solv.CreateGapFlg();
	ElvContourBitmpa3(solv, 0.0, solv.gap_max, drive, dir, exe_drive, exe_dir);

	if ( render_step_float == 0 )
	{
		render_step = render_step*solv.dt;
	}

	//loopnum = 2;
	solv.log_printf("=========== シミュレーション START =================\n");
	int stat = -1;
	
	if ( sim_time != 0.0 )
	{
		int num = (int)(sim_time/solv.dt)+1;
		loopnum = num;
	}else
	{
		sim_time = loopnum*solv.dt;
	}

	int ren_n = (int)((double)loopnum/(double)render_step)+1;	//キャプチャ数
	double capture_step = sim_time/(double)ren_n;				//キャプチャ間隔
	solv.log_printf("image step %d/%.3f[sec] image num %d\n", render_step, capture_step, ren_n);

	if ( render_only == 1 )
	{
		TempBuffer fname(512);
		sprintf(fname.p, "%s%soutput_outline.txt", drive, dir);
		FILE* fp = fopen(fname.p, "r");
		if ( fp == NULL )
		{
			return -1;
		}
		TempBuffer buf(256);
		fgets(buf.p, 256, fp);
		sscanf(buf.p, "%lf", &solv.wmin);
		sscanf(buf.p, "%lf", &solv.wmax);
		sscanf(buf.p, "%lf", &solv.w_max);
		fclose(fp);

		realtimeImage = 0;
		solv.wmin = -6.0;
		solv.wmax = solv.w_max = 6.0;
		goto create_image_onry;
	}
	if ( render_only == 2 )
	{
		goto render_code_only;
	}


	//地殻変動時間
	double tau = rise_time;
	solv.log_printf("Rise Time:%f[sec]\n", tau);

	int tau_capt_num = tau/capture_step;		//地殻変動間でキャプチャされる枚数
	solv.log_printf("初期変動でキャプチャされる枚数 %d\n", tau_capt_num);

	double tau_dt = solv.dt;
	if ( tau_capt_num < 1 )
	{
		while( tau_capt_num < 1 )
		{
			tau_dt = tau_dt*0.85;	//地殻変動時間ステップ

			int num = (int)(sim_time/tau_dt)+1;
			int ren_n = (int)((double)num/(double)render_step)+1;	//キャプチャ数
			double capture_step = sim_time/(double)ren_n;				//キャプチャ間隔
			tau_capt_num = tau/capture_step;
		}
		solv.log_printf("=>地殻変動時間ステップ:%.3f\n", tau_dt);
		solv.log_printf("=>初期変動でキャプチャされる枚数 %d\n", tau_capt_num);
	}

	//地殻変動時間ステップの補正
	tau_dt = (tau_dt > solv.dt ) ? solv.dt  : tau_dt;
	solv.log_printf("■■■%s時間ステップの補正:%.2f■■■\n", (!impact) ? "地殻変動":"隕石衝突", tau_dt);

	//処理回数への加算
	int tau_n = (int)(tau/tau_dt +0.5);
	loopnum += tau_n;

	double dt_org = solv.dt;
	solv.dt = tau_dt;


	int half_loopnum = loopnum/2;

	int putimageCount = 0;
	clock_t calc_time = 0;

	//津波初期波源は無視（高潮だけ計算)
	if (storm_surge_only == 1)
	{
		memset(w_last, '\0', sizeof(double)*grid.iX*grid.jY);
	}

	int w_list = 0;
	{
		TempBuffer fname;
		sprintf(fname.p, "%s%sInitial_wave_data3_0.csv", drive, dir);
		FILE* fp = fopen(fname.p, "r");
		if ( fp )
		{
			fclose(fp);
			w_list = 1;
		}

		sprintf(fname.p, "%s%sInitial_wave_data3_0.000.csv", drive, dir);
		fp = fopen(fname.p, "r");
		if (fp)
		{
			fclose(fp);
			w_list = 1;
		}

		sprintf(fname.p, "%s%sInitial_wave_data3_0.000_0.000.csv", drive, dir);
		fp = fopen(fname.p, "r");
		if (fp)
		{
			fclose(fp);
			w_list = 1;
		}
	}

	{
		TempBuffer fname;
		for (int i = 0; i < MAX_FAULT_TIMES; i++)
		{
			double t;
			sprintf(fname.p, "%s%sInitial_wave_data3_%d.csv", drive, dir, i);
			FILE* fp = fopen(fname.p, "r");
			if (fp)
			{
				fclose(fp);
				initial_wave s;
				s.fname = std::string(fname.p);
				s.start_time = i;
				s.displacement_time = tau;
				s.displacement_num = tau_n;
				int w, h;
				s.w_last = NULL;// ReadCsv(fname.p, w, h);
				solv.initial_wave_list.push_back(s);
				w_list = 1;
			}
		}
		if (solv.initial_wave_list.size() == 0 )
		{
			HANDLE hFind;
			WIN32_FIND_DATAA win32fd;

			sprintf(fname.p, "%s%sInitial_wave_data3_*.csv", drive, dir);
			hFind = FindFirstFileA(fname.p, &win32fd);

			if (hFind != INVALID_HANDLE_VALUE)
			{
				do {
					if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						//printf("%s (DIR)\n", win32fd.cFileName);
					}
					else {
						printf("%s\n", win32fd.cFileName);

						char* p = win32fd.cFileName;
						p = strstr(p, "Initial_wave_data3_");
						p = p + strlen("Initial_wave_data3_");

						double t = 0.0, ss = tau;;
						int is = sscanf(p, "%lf_%lf.csv", &t, &ss);
						if (is != 2)
						{
							int is = sscanf(p, "%lf.csv", &t);
						}
						FILE* fp = fopen(win32fd.cFileName, "r");
						if (fp)
						{
							w_list = 1;
							fclose(fp);
							initial_wave s;
							s.fname = win32fd.cFileName;
							s.start_time = t;
							s.displacement_time = ss;
							s.displacement_num = (int)(s.displacement_time / tau_dt + 0.5);;
							int w, h;
							s.w_last = NULL;// ReadCsv((char*)s.fname.c_str(), w, h);
							solv.initial_wave_list.push_back(s);
						}
					}
				} while (FindNextFileA(hFind, &win32fd));
			}
			FindClose(hFind);
		}
	}

	if (CrustalMovement)
	{
		//陸地も含めて変位量をw_start_orgに保存しておく
		memcpy(grid.w_start_org, w_last, sizeof(double)*grid.iX*grid.jY);
	}
	if (w_list == 0)
	{
		initial_wave s;
		s.start_time = 0;
		s.displacement_time = tau;
		s.displacement_num = tau_n;
		s.w_last = w_last;
		solv.initial_wave_list.push_back(s);
		w_last = 0;
	}

	double save_dt = solv.dt;	//元のdt
	bool dt_restore = false;	//後でdtは戻す
	bool dt_modfy_log = false;
	solv.log_printf("津波開始時の時間ステップ:%.2f\n", solv.dt);
	if ( solv.Storm_p )
	{
		int num = startUptime/dt_org;
		solv.Storm_p->StartUpTime(num);
		solv.log_printf("台風立ち上がりステップ数:%d\n", num);

		solv.Storm_p->SetZeroArea(ZERO_AREA_WD);
		solv.Storm_p->put("TyphoonParameter.txt");
		solv.Storm_p->putLog();

		solv.dt = dt_org;
		solv.log_printf("■■■これより変更時間ステップ:%.2f■■■\n", solv.dt);
	}

	int impact_dummy_mode = impact;

	double timesum = 0.0;	//津波時間
	double timesum2 = 0.0;	//台風時間

	double timesum_diff = 0.0;
	int render_count = 0;

	for ( int i = 0; i < loopnum; i++ )
	{
		//立ち上げが終わったらdtを元に戻す
		if ( solv.Storm_p )
		{
			if ( !dt_restore && !solv.Storm_p->isSetup())
			{
				if ( storm_surge_only == 1 )
				{
					solv.log_printf("<<<<< 立ち上げ終了　>>>>>>\n");
					dt_restore = true;	//戻し処理は１回だけ
					solv.dt = dt_org;	//後はこのdtで計算を続ける
					solv.log_printf("これより時間ステップ:%f\n", solv.dt);
				}else
				{
					//まだ津波（断層変移）は無い
					if ( timesum2 < tsunami_start_time )
					{
						//のでdtは通常の値に戻す
						solv.dt = dt_org;
						if ( !dt_modfy_log )
						{
							solv.log_printf("これより時間ステップ:%f\n", solv.dt);
							dt_modfy_log = true;
						}
					}else
					{
						solv.log_printf("■■■■これより津波（%s）の開始■■■■\n", impact?"断層変移":"隕石衝突");
						//津波（断層変移）の開始
						dt_restore = true;	//戻し処理は１回だけ
						solv.dt = save_dt;	//後は津波の計算依存となる
						solv.log_printf("これより時間ステップ:%f\n", solv.dt);
					}
				}
			}
		}
		timesum += solv.dt;
		timesum_diff += solv.dt;

		if ( solv.Storm_p )
		{
			//セットアップ中は台風が形成されるまで時間は進めない
			if ( solv.Storm_p->isSetup() )
			{
				timesum -= solv.dt;
				timesum_diff -= solv.dt;
				solv.Storm_p->SetTime( 0.0 );
			}else
			{
				if ( storm_surge_only == 0 && timesum2 < tsunami_start_time )
				{
					//まだ津波は発生しない
					timesum -= solv.dt;
					timesum_diff -= solv.dt;
				}
				timesum2 += solv.dt;	//台風時間作動
				solv.Storm_p->SetTime( timesum2 );
			}
		}

		clock_t ts = clock();
		if ( i > half_loopnum )
		{
			if ( !csv2obj )
			{
				if ( RunUp )
				{
					if (solv.ele_area)
					{
						if ( solv.maxheight_cur < stopHeight && solv.maxheight_w_cur2 < stopHeight )
						{
							solv.log_printf("波高:%.2f以下になったため終了\n", stopHeight);
							stat = 0;
							break;
						}
					}else
					{
						if ( solv.wmax < stopHeight )
						{
							solv.log_printf("波高:%.2f以下になったため終了\n", stopHeight);
							stat = 0;
							break;
						}
					}
				}else
				{
					if ( solv.ele_area )
					{
						if ( solv.maxheight_cur < stopHeight )
						{
							solv.log_printf("波高:%.2f以下になったため終了\n", stopHeight);
							stat = 0;
							break;
						}
					}else
					{
						if ( solv.wmax < stopHeight )
						{
							solv.log_printf("波高:%.2f以下になったため終了\n", stopHeight);
							stat = 0;
							break;
						}
					}
				}
			}
		}

		sprintf(fname, "%s%soutput\\output_W%06d.csv", drive, dir, render_count);
		if ( solv.RunUp && i == loopnum-1 ) OutputRunUpImage = i;
		if ( !csv2obj )
		{ 
			std::vector<int> next = solv.get_next_initial_wave_list(timesum);
			if (impact && next.size())
			{
				//printf("－ーーーーーーーーーーーーーーー計算スキップ\n");
				/* empty */
			}else
			{
				impact_dummy_mode = 0;
				if ( (stat = solv.Iterat(1)) < 0 )
				{
					printf("error.\n");
					break;
				}
			}

			if ( isRender(i, render_step, timesum_diff) )
			{
				if ( !isDiskFreeSpaceOK(drive, MIN_DISK_FREE_SPACE) )
				{
					solv.log_printf("Warning: DiskFreeSpace\n");
				}
				OutputLastCounter = i;
				grid.Output(fname, render_z_scale);
			}
			if ( realtimeImage && (isRender(i, render_step, timesum_diff)  ))
			{
				//遡上域の画像出力(putimageCount毎に出力)
				if ( putimageCount%2 == 0 ) OutputRunUpImage=1;
				putimageCount++;
				if ( storm_surge_only != 1 )
				{
					CreateImage(solv, solv.Data->w, render_count, colorbar_min, colorbar_max, colorbar_max, drive, dir, exe_drive, exe_dir, smooth_flg, smooth_num, smooth, ColorMap, colormap, timesum, longitude, latitude);
				}else
				{
					CreateImage(solv, solv.Data->w, render_count, colorbar_min, colorbar_max, colorbar_max, drive, dir, exe_drive, exe_dir, smooth_flg, smooth_num, smooth, ColorMap, colormap, timesum, longitude, latitude);
				}
				//Storm.dump(TimeSum, i, grid.iX, grid.jY);
			}

			if ( (vtk_export || ogjfile) && (isRender(i, render_step, timesum_diff) ))
			{
				double* o = solv.Data->elevation;
				int* t = solv.Data->topog;
				//if ( !solv.RunUp )
				{
					o = 0;
					t = 0;
				}
				double* w_hight = 0;
				double* alp = new double[grid.iX*grid.jY];
				//if ( solv.RunUp )
				{
					w_hight = new double[grid.iX*grid.jY];
#pragma omp parallel for OMP_SCHEDULE
					for ( int ii = 0; ii < grid.jY; ii++ )
					{
						for (int jj = 0; jj < grid.iX; jj++)
						{
							alp[grid.iX*ii + jj] = 0.0;

							//海域
							if (solv.Data->topog[grid.iX*ii + jj] != 0)
							{
								alp[grid.iX*ii + jj] = 1.0;
								w_hight[grid.iX*ii + jj] = solv.Data->w[grid.iX*ii + jj];
							}

							//陸地への遡上
							if (solv.Data->topog[grid.iX*ii + jj] == 0)
							{
								if (solv.isWet(ii, jj))
								{
									alp[grid.iX*ii + jj] = 0.5;
									w_hight[grid.iX*ii + jj] = Z_SCALE_ELV*solv.Data->elevation[grid.iX*ii + jj] / Z_SCALE_WAV + solv.Data->w[grid.iX*ii + jj];
								}
								else
								{
									alp[grid.iX*ii + jj] = 0.0;
									w_hight[grid.iX*ii + jj] = 0.0;

									//最初から陸地が低い場合
									if (solv.Data->elevation[grid.iX*ii + jj] <= 1.0 / Z_SCALE_WAV)
									{
										w_hight[grid.iX*ii + jj] = Z_SCALE_ELV*solv.Data->elevation[grid.iX*ii + jj] / Z_SCALE_WAV - 1.0 / Z_SCALE_WAV;
									}
								}
							}

							if ( 0 )
							{
								char* depthdown = getenv("DEPTH_DOWN");
								double depth_down = 0.0;
								if (depthdown) depth_down = atof(depthdown);

								//スケールの違いで本当は海底露出しないのに露出してしまう場合
								if (solv.Data->topog[grid.iX*ii + jj] && w_hight[grid.iX*ii + jj] * Z_SCALE_WAV + 1.0 / Z_SCALE_WAV < (-solv.Data->h[grid.iX*ii + jj] + depth_down) * Z_SCALE_DPT)
								{
									alp[grid.iX*ii + jj] = 1.0;
									w_hight[grid.iX*ii + jj] = (-solv.Data->h[grid.iX*ii + jj] + depth_down) * Z_SCALE_DPT / Z_SCALE_WAV + 1.0 / Z_SCALE_WAV;
								}
							}
						}
					}
				}

				CsvToObj obj(solv.Data->w, solv.iX, solv.jY, o,t);
				obj.dx = solv.dx;
				obj.dy = solv.dy;
				obj.r_dx = solv.Data->r_dx;
				obj.exp_scale = 1;
				obj.alph = alp;
				//丸みを付ける
				if ( latitude_flag ) obj.latitude = latitude;
				if ( longitude_flag ) obj.longitude = longitude;
				obj.zscale = Z_SCALE_WAV;
				obj.zscale2 = Z_SCALE_ELV;
				obj.zscale3 = Z_SCALE_DPT;
				if ( obj_Material[2] ) obj.Color(1,19,104);


				VertexColor* vertexcolor = new VertexColor[grid.iX*grid.jY];	
				obj.vertex_color = vertexcolor;
				obj.colormap = &wave_colormap;

#pragma omp parallel for OMP_SCHEDULE
				for ( int ii = 0; ii < grid.jY; ii++ )
				{
					for ( int jj = 0; jj < grid.iX; jj++ )
					{
						double h = (obj.Vertex(ii,jj) - 0.0)/(4.0 - 0.0);
						if ( h < 0.0 ) h = 0.0;
						if ( h > 1.0 ) h = 1.0;
						int color_index = (int)(h*(double)wave_colormap.W()-1);
						if ( color_index < 0 ) color_index = 0;
						obj.vertexColor(ii,jj).r = wave_colormap.cell(wave_colormap.H()/2, color_index).r;
						obj.vertexColor(ii,jj).g = wave_colormap.cell(wave_colormap.H()/2, color_index).g;
						obj.vertexColor(ii,jj).b = wave_colormap.cell(wave_colormap.H()/2, color_index).b;
						obj.vertexColor(ii,jj).id = color_index;
					}
				}
				sprintf(fname, "%s%sobj\\output_%06d.obj", drive, dir, render_count);
				obj.smooth_limit = 1;
				obj.globalScale = global_scale;
				if ( obj_normalize )
				{
					obj.normalization = true;
				}
				if ( !obj_Material[2] ) obj.vertex_color = 0;
				if ( solv.Storm_p )
				{
					obj.vectorIndex = solv.Storm_p->windVector();
				}
				if ( objsmooth  )
				{
					if ( ogjfile ) obj.Output(fname,3, 0, grid.h, w_hight);
					if (vtk_export)
					{
						sprintf(fname, "%s%sobj\\vtk\\output_%06d.vtk", drive, dir, render_count);
						obj.vtk_export = 3;
						obj.Output(fname, 3, 0, grid.h, w_hight);
					}

				}else
				{
					if ( ogjfile ) obj.Output(fname,0, 0, grid.h, w_hight);
					if (vtk_export)
					{
						sprintf(fname, "%s%sobj\\vtk\\output_%06d.vtk", drive, dir, render_count);
						obj.vtk_export = 3;
						obj.Output(fname, 0, 0, grid.h, w_hight);
					}
				}
				delete [] vertexcolor;
				if ( solv.Storm_p && ogjfile)
				{
					if ( obj.vectorIndex ) solv.Storm_p->windVectorDelete( obj.vectorIndex );
				}

				//if ( solv.RunUp )
				{
					delete [] w_hight;
				}
				delete [] alp;
				if ( obj_standerd && ogjfile)
				{
					sprintf(fname, "%s%sobj\\output_%06d.obj", drive, dir, render_count);
					TempBuffer cmd;
					sprintf(cmd.p, "%s%s%s %s %s", exe_drive, exe_dir, "OBJConvFormat.exe -fcol ", fname, fname);
					if ( obj_Material[2] ) cmdProcess(cmd.p);
				}

			}

		}else
		{
			if ( /*ogjfile && */(isRender(i, render_step, timesum_diff) ))
			{
				double* ww = ReadCsv(fname, grid.iX, grid.jY);
				if ( ww == NULL )
				{
					break;
				}
				double* o = solv.Data->elevation;
				int* t = solv.Data->topog;
				//if ( !solv.RunUp )
				{
					o = 0;
					t = 0;
				}
				double* w_hight = 0;
				double* alp = new double[grid.iX*grid.jY];
				//if ( solv.RunUp )
				{
					w_hight = new double[grid.iX*grid.jY];
#pragma omp parallel for OMP_SCHEDULE
					for ( int ii = 0; ii < grid.jY; ii++ )
					{
						for (int jj = 0; jj < grid.iX; jj++)
						{
							alp[grid.iX*ii + jj] = 0.0;

							//海域
							if (solv.Data->topog[grid.iX*ii + jj] != 0)
							{
								alp[grid.iX*ii + jj] = 1.0;
								w_hight[grid.iX*ii + jj] = solv.Data->w[grid.iX*ii + jj];
							}

							//陸地への遡上
							if (solv.Data->topog[grid.iX*ii + jj] == 0)
							{
								if (solv.isWet(ii, jj))
								{
									alp[grid.iX*ii + jj] = 0.5;
									w_hight[grid.iX*ii + jj] = Z_SCALE_ELV*solv.Data->elevation[grid.iX*ii + jj] / Z_SCALE_WAV + solv.Data->w[grid.iX*ii + jj];
								}
								else
								{
									alp[grid.iX*ii + jj] = 0.0;
									w_hight[grid.iX*ii + jj] = 0.0;

									//最初から陸地が低い場合
									if (solv.Data->elevation[grid.iX*ii + jj] <= 1.0 / Z_SCALE_WAV)
									{
										w_hight[grid.iX*ii + jj] = Z_SCALE_ELV*solv.Data->elevation[grid.iX*ii + jj] / Z_SCALE_WAV - 1.0 / Z_SCALE_WAV;
									}
								}
							}

							if ( 0 )
							{
								char* depthdown = getenv("DEPTH_DOWN");
								double depth_down = 0.0;
								if (depthdown) depth_down = atof(depthdown);

								//スケールの違いで本当は海底露出しないのに露出してしまう場合
								if (solv.Data->topog[grid.iX*ii + jj] && w_hight[grid.iX*ii + jj] * Z_SCALE_WAV + 1.0 / Z_SCALE_WAV < (-solv.Data->h[grid.iX*ii + jj] + depth_down) * Z_SCALE_DPT)
								{
									alp[grid.iX*ii + jj] = 1.0;
									w_hight[grid.iX*ii + jj] = (-solv.Data->h[grid.iX*ii + jj] + depth_down) * Z_SCALE_DPT / Z_SCALE_WAV + 1.0 / Z_SCALE_WAV;
								}
							}
						}
					}
				}
				CsvToObj obj(ww, grid.iX, grid.jY, o, t);
				obj.dx = solv.dx;
				obj.dy = solv.dy;
				obj.r_dx = solv.Data->r_dx;
				obj.exp_scale = 1;
				//丸みを付ける
				if ( latitude_flag ) obj.latitude = latitude;
				if ( longitude_flag ) obj.longitude = longitude;
				obj.zscale = Z_SCALE_WAV;
				obj.zscale2 = Z_SCALE_ELV;
				obj.zscale3 = Z_SCALE_DPT;
				if ( obj_Material[2] ) obj.Color(1,19,104);

				VertexColor* vertexcolor = new VertexColor[grid.iX*grid.jY];	
				obj.vertex_color = vertexcolor;
				obj.colormap = &wave_colormap;

#pragma omp parallel for OMP_SCHEDULE
				for ( int ii = 0; ii < grid.jY; ii++ )
				{
					for ( int jj = 0; jj < grid.iX; jj++ )
					{
						double h = (obj.Vertex(ii,jj) - 0.0)/(4.0 - 0.0);
						if ( h < 0.0 ) h = 0.0;
						if ( h > 1.0 ) h = 1.0;
						int color_index = (int)(h*(double)wave_colormap.W()-1);
						if ( color_index < 0 ) color_index = 0;
						obj.vertexColor(ii,jj).r = wave_colormap.cell(wave_colormap.H()/2, color_index).r;
						obj.vertexColor(ii,jj).g = wave_colormap.cell(wave_colormap.H()/2, color_index).g;
						obj.vertexColor(ii,jj).b = wave_colormap.cell(wave_colormap.H()/2, color_index).b;
						obj.vertexColor(ii,jj).id = color_index;
					}
				}
				sprintf(fname, "%s%sobj\\output_%06d.obj", drive, dir, render_count);
				printf("file[%s]\n", fname);
				obj.smooth_limit = 1;
				obj.globalScale = global_scale;
				if ( obj_normalize )
				{
					obj.normalization = true;
				}
				if ( !obj_Material[2] ) obj.vertex_color = 0;
				if ( objsmooth )
				{
					if ( ogjfile ) obj.Output(fname,3,0, grid.h,w_hight);
					if (vtk_export)
					{
						sprintf(fname, "%s%sobj\\vtk\\output_%06d.vtk", drive, dir, render_count);
						obj.vtk_export = 3;
						obj.Output(fname, 3, 0, grid.h, w_hight);
					}
				}else
				{
					if ( ogjfile ) obj.Output(fname,0,0, grid.h,w_hight);
					if (vtk_export)
					{
						sprintf(fname, "%s%sobj\\vtk\\output_%06d.vtk", drive, dir, render_count);
						obj.vtk_export = 3;
						obj.Output(fname, 0, 0, grid.h, w_hight);
					}
				}
				delete [] vertexcolor;
				delete [] ww;
				//if ( solv.RunUp )
				{
					delete [] w_hight;
				}
				delete [] alp;
				if ( obj_standerd && ogjfile )
				{
					sprintf(fname, "%s%sobj\\output_%06d.obj", drive, dir, render_count);
					TempBuffer cmd;
					sprintf(cmd.p, "%s%s%s %s %s", exe_drive, exe_dir, "OBJConvFormat.exe -fcol ", fname, fname);
					if ( obj_Material[2] ) cmdProcess(cmd.p);
				}

			}
			printf("                    \r[%04d/%04d]", i+1, loopnum);
			continue;
		}

		if ( samplingline.GetImage() && samplingline.H() && samplingline.W() && (sampling < 0 || (sampling >= 0 && sampling <= timesum+solv.dt)))
		{
			bool pt_sampling = point_sampling;

			sprintf(fname, "%s%ssampling\\sampling_W%06d.dat", drive, dir, render_count);
			if (isRender(i, render_step, timesum_diff) )
			{
				FILE* fp = fopen(fname, "w");
				if ( fp )
				{
					for (int ii = 0; ii < samplingline.H(); ii++)
					{
						for (int jj = 0; jj < samplingline.W(); jj++)
						{

							if ( point_sampling && samplingpoint.GetImage())
							{
								if (samplingpoint.cell(ii, jj).r == 0 && samplingpoint.cell(ii, jj).g == 0 && samplingpoint.cell(ii, jj).b == 0)
								{
									double w = -9999999.0;
									for (int ki = -10; ki <= 10; ki++)
									{
										if (ii + ki < 0 || ii + ki >= grid.jY) continue;
										for (int kj = -10; kj <= 10; kj++)
										{
											if (jj + kj < 0 || jj + kj >= grid.iX) continue;
											if ( w < grid.W(ii, jj)) w = grid.W(ii, jj);
										}
									}
									fprintf(fp, "%.4f\n", w);
								}
							}else
							{
								if (samplingline.cell(ii, jj).r == 0 && samplingline.cell(ii, jj).g == 0 && samplingline.cell(ii, jj).b == 0)
								{
									fprintf(fp, "%.4f\n", grid.W(ii, jj));
								}
								break;
							}
						}
					}
					fclose(fp);
				}
			}
			if (isRender(i, render_step, timesum_diff)  && pt_sampling)
			{
				sprintf(fname, "%s%ssampling\\sampling_point.dat", drive, dir);
				FILE* fp = fopen(fname, "w");
				if ( fp )
				{
					for (int k = 0; k < loopnum; k++)
					{
						char buf[90];
						sprintf(fname, "%s%soutput\\time%06d.txt", drive, dir, k);
						FILE* fp3 = fopen(fname, "r");
						if (fp3 == NULL ) continue;
						fgets(buf, 256, fp3);
						char* p = strchr(buf, '\n');
						if (p) *p = '\0';
						fprintf(fp, "%s\t", buf);
						fclose(fp3);

						sprintf(fname, "%s%ssampling\\sampling_W%06d.dat", drive, dir, k);
						FILE* fp2 = fopen(fname, "r");
						if (fp2 == NULL) continue;
						fgets(buf, 256, fp2);
						fprintf(fp, "%s", buf);
						fclose(fp2);
					}
					fclose(fp);
				}
			}

			if ( sampling > 0 && sampling <= timesum+solv.dt )
			{
				sampling = 0;
				solv.logClose();
				return 0;
			}
		}

		{
			int h = (int)((timesum*R_HOUR));
			int m = (int)((timesum - 3600.0*(double)h)*R_MIN);
			int s = timesum - 3600*h - 60.0*m;
	
			if ( solv.Storm_p )
			{
				if( !solv.Storm_p->isSetup() && storm_surge_only == 0 && timesum2 < tsunami_start_time )
				{
					h = (int)((timesum2*R_HOUR));
					m = (int)((timesum2 - 3600.0*(double)h)*R_MIN);
					s = timesum2 - 3600*h - 60.0*m;
				}
			}

			if ( isRender(i, render_step, timesum_diff)  )
			{
				sprintf(fname, "%s%soutput\\time%06d.txt", drive, dir, render_count);
				FILE* fp = fopen(fname, "w");

				if ( solv.Storm_p && storm_surge_only == 1 )
				{
					Storm_Point a = solv.Storm_p->get();

					if ( a.flg1970 >= 0 )
					{
						time_t t = a.tm;
						char fmt[128];
						strftime(fmt, 128, "%Y", localtime(&t));
						int year;
						if ( a.flg1970 )
						{
							year = atoi(fmt)-20;
							strftime(fmt, 128, "%b.%d %H:%M:%S", localtime(&a.tm));
						}else
						{
							year = atoi(fmt);
							strftime(fmt, 128, "%b.%d %H:%M:%S", localtime(&a.tm));
						}
						if ( solv.Storm_p->JST )
						{
							fprintf(fp, "JST %d.%s\n", year, fmt);
						}else
						{
							fprintf(fp, "UTC %d.%s\n", year, fmt);
						}
					}else
					{
						fprintf(fp, "%02d:%02d:%02d", h, m, s);
					}
				}else
				{
					fprintf(fp, "%02d:%02d:%02d", h, m, s);
				}
				if (topography_data_mask2_bmp.GetImage())
				{
					fprintf(fp, "\n%s", GOOGLE_MAP_DATA_COPY_LIGHT);
				}
				fclose(fp);
			}

			if ( impact_dummy_mode )
			{
				/* empty */
			}else
			{
				//printf("[%04d/%04d] time [%02d:%02d:%02d]Max[(%3.2f) %3.2f %3.2f] 浸水%3.2f 遡上%3.2f MAX%3.2f\n", i+1, loopnum, h, m, s, solv.maxheight_cur, solv.wmax, solv.maxheight, solv.maxheight_w_cur2,solv.maxheight_cur2,solv.maxheight2);
				solv.log_printf("[%04d/%04d] time [%02d:%02d:%02d]Max[(%3.2f) %3.2f %3.2f] 浸水%3.2f 遡上%3.2f MAX%3.2f\n", i+1, loopnum, h, m, s, solv.maxheight_cur, solv.wmax, solv.maxheight, solv.maxheight_w_cur2,solv.maxheight_cur2,solv.maxheight2);

				if ( isRender(i, render_step, timesum_diff)  )
				{
					int wi = solv.maxheightPos[0], wj = solv.maxheightPos[1];
					if (wi >= 0 && wj >= 0)
					{
						if ( solv.Data->T(wi,wj) == 0 )
						{
							solv.log_printf("Max(陸地標高:%.3f 水位:%.3f -> 水深:%.3f\n", solv.Data->ELV(wi,wj), solv.Data->ELV(wi,wj)+solv.Data->W(wi,wj), solv.Data->W(wi,wj));
						}
					}
				}
				if ( solv.Storm_p && isRender(i, render_step, timesum_diff) )
				{
					solv.log_printf("[%04d/%04d] Max wind speed:%.2f[m/s] %.2f[hPh]\n", i+1, loopnum, solv.Storm_p->MaxBreeze(), solv.Storm_p->get().pc);
				}
			}
		}

		if ( solv.Storm_p )
		{
			solv.Storm_p->StartUpCount();
			if ( solv.Storm_p->isSetup() )
			{
				loopnum++;
				half_loopnum = loopnum/2;
				continue;
			}
			if ( storm_surge_only == 0 && timesum2 < tsunami_start_time )
			{
				loopnum++;
				half_loopnum = loopnum/2;
				continue;
			}
		}

		//これから変異がはじまる断層をピックアップ
		std::vector<int> displacementList;
		displacementList = solv.get_start_initial_wave_list(timesum);

		solv.dt = dt_org;
		if (displacementList.size())
		{
			for (int k = 0; k < displacementList.size(); k++)
			{
				solv.log_printf("%s[%d]START\n", (!impact) ? "地殻変動" : "隕石衝突", displacementList[k]);
				//津波初期波源は無視（高潮だけ計算)
				if (storm_surge_only == 1)
				{
					solv.log_printf("%s[%d]=>キャンセル\n", (!impact) ? "地殻変動" : "隕石衝突", displacementList[k]);
				}
			}
			solv.dt = tau_dt;

			//津波初期波源は無視（高潮だけ計算)
			if (storm_surge_only == 1)
			{
				solv.dt = dt_org;
			}
			else
			{
				loopnum += tau_n;
				half_loopnum = loopnum / 2;
			}
		}

		//変移開始状態の場合
		displacementList = solv.get_initial_wave_list(timesum);

		if (displacementList.size() && storm_surge_only != 1)
		{
			//printf("initial_wave_list %d\n", solv.initial_wave_list.size());

			solv.dt = tau_dt;

			double *www = 0;
			if (CrustalMovement)
			{
				www = new double[grid.iX*grid.jY];
				memset(www, '\0', sizeof(double)*grid.iX*grid.jY);
			}

			const int sz = displacementList.size();
			for (int k = 0; k < sz; k++)
			{
				solv.w_init = 0;
				if (solv.initial_wave_list[displacementList[k]].displacement_count <= solv.initial_wave_list[displacementList[k]].displacement_num)
				{
					solv.w_init = 1;
					double inv_tau_n = 1.0 / (double)solv.initial_wave_list[displacementList[k]].displacement_num;
#pragma omp parallel for OMP_SCHEDULE
					for (int ii__ = 0; ii__ < grid.iX*grid.jY; ii__++)
					//for (int ii = ZERO_AREA_WD; ii < grid.jY - ZERO_AREA_WD; ii++)
					{
						int ii = ii__ / solv.iX;
						int jj = ii__ % solv.iX;
						if (ii < ZERO_AREA_WD || ii >= grid.jY-ZERO_AREA_WD || jj < ZERO_AREA_WD || jj >= grid.iX-ZERO_AREA_WD)
						{
							continue;
						}

						//for (int jj = ZERO_AREA_WD; jj < grid.iX - ZERO_AREA_WD; jj++)
						{
							grid.w[ii*grid.iX + jj] += solv.initial_wave_list[displacementList[k]].w_last[ii*grid.iX + jj] * inv_tau_n;
							if (CrustalMovement)
							{
								www[ii*grid.iX + jj] = grid.w_start_org[ii*grid.iX + jj] * inv_tau_n;
							}
							if (!impact_dummy_mode)solv.WaveHeightBounrayCondition(solv.Data, ii, jj);
						}
					}
				}
				if (solv.initial_wave_list[displacementList[k]].displacement_count == solv.initial_wave_list[displacementList[k]].displacement_num)
				{
					if (storm_surge_only != 1)
					{
						delete [] solv.initial_wave_list[displacementList[k]].w_last;
						solv.initial_wave_list[displacementList[k]].w_last = NULL;
						solv.log_printf("%s[%d]END\n", (!impact) ? "地殻変動" : "隕石衝突", displacementList[k]);
					}
				}

			}
			if (isRender(i, render_step, timesum_diff)  && CrustalMovement)
			{
				outputCrustalMovement(solv, grid, www, timesum, Z_SCALE_WAV, Z_SCALE_DPT, crustalMovement_count, exe_drive, exe_dir, drive, dir, latitude_flag, latitude, longitude_flag, longitude);
			}
			if (CrustalMovement) delete[] www;

			if (!solv.is_exist_initial_wave_list())
			{
				solv.log_printf("全ての地殻変動が終了\n");
				solv.w_init = -1;
			}
		}


		//static int riverInitCnt = 0;
		//static int* riverInitFlg = 0;
		//if ( riverInitFlg == 0 )
		//{
		//	riverInitFlg = new int[grid.iX*grid.jY];
		//	memset(riverInitFlg, '\0', sizeof(int)*grid.iX*grid.jY);
		//}
		//riverInitCnt++;



		//河川
		if (solv.riverInfo.existRiver())
		{
			const int absorbingZone_ = solv.absorbingZone;
			const int IX = grid.iX;
			const int JY = grid.jY;


			if (solv.river_running_time < timesum && !solv.riverInfo.isInitEnd())
			{
				solv.log_printf("河川初期化終了\n");
				solv.riverInfo.InitEnd(i);
			}

			int rainflg = 0;
#pragma omp parallel for OMP_SCHEDULE
			for (int ii__ = 0; ii__ < IX*JY; ii__++)
			//for (int ii = ZERO_AREA_WD; ii < JY - ZERO_AREA_WD; ii++)
			{
				int ii = ii__ / IX;
				int jj = ii__ % IX;
				if (ii < ZERO_AREA_WD || ii >= JY-ZERO_AREA_WD || jj < ZERO_AREA_WD || jj >= IX-ZERO_AREA_WD)
				{
					continue;
				}

				//for (int jj = ZERO_AREA_WD; jj < IX - ZERO_AREA_WD; jj++)
				{
					if (grid.topog[ii*IX + jj] == 0)
					{

						int inflow_id = 0;
						//河川の湧き出し口
						if ((inflow_id = solv.riverInfo.isInflow(ii, jj) - 1) >= 0 && !solv.riverInfo.isInitEnd())
						{
							double dw = 1.0;
							if (IsAbsorbingZoneBounray(ii, jj))
							{
								dw = solv.absorbingZoneBounray(ii, jj, -1);
							}

							//if (grid.w[ii*IX + jj] >= solv.riverInfo.inflowList[inflow_id].flow)
							//{
							//	continue;
							//}
							//湧き出し水位
							//if (riverInitFlg[ii*IX + jj] == 0)
							//{
								grid.w[ii*IX + jj] = solv.riverInfo.inflowList[inflow_id].flow;
							//	riverInitFlg[ii*IX + jj] = 1;
							//}


							double q = sqrt(grid.M(ii, jj)*grid.M(ii, jj) + grid.N(ii, jj)*grid.N(ii, jj));

							//流出方向
							if (q > 1.0e-10 && solv.riverInfo.inflowList[inflow_id].dir >= 0.0)
							{
								double x = cos(solv.riverInfo.inflowList[inflow_id].dir / RAD);
								double y = sin(solv.riverInfo.inflowList[inflow_id].dir / RAD);

								grid.M(ii, jj) = x*q;
								grid.N(ii, jj) = y*q;
							}

							//流出速度
							if (solv.riverInfo.inflowList[inflow_id].velocity > 0.0 && q > 1.0e-10)
							{
								double d = grid.W(ii, jj);

								/*
									velocity = sqrt( (M/d)^2 + (N/d)^2 ) = sqrt( (M)^2 + (N)^2 )/d

									velocity' = sqrt( (c*M)^2 + (c*N)^2 )/d = c*sqrt( (M)^2 + (N)^2 )/d = c*q/d

									c =  velocity'*d/q
									*/
								double c = solv.riverInfo.inflowList[inflow_id].velocity*d / q;

								grid.M(ii, jj) *= c;
								grid.N(ii, jj) *= c;
							}
						}


						//河路
						int river_id = 0;
						if ((river_id = solv.riverInfo.isRiver(ii, jj) - 1) >= 0)
						{
							double dw = 1.0;
							if (IsAbsorbingZoneBounray(ii, jj))
							{
								dw = solv.absorbingZoneBounray(ii, jj, -1);
							}

							double depth = solv.riverInfo.riverList[river_id].initflow;
							if (solv.riverInfo.Flag[ii*IX + jj] == 1 && !solv.riverInfo.isInitEnd())
							{
								double d = grid.W(ii, jj);

								double q = sqrt(grid.M(ii, jj)*grid.M(ii, jj) + grid.N(ii, jj)*grid.N(ii, jj));
								
								if (d < 0.01)
								{
									grid.W(ii, jj) = 0.0;
									solv.RunupConditionFlag[ii*IX + jj] = IS_DRY;
									solv.riverInfo.Flag[ii*IX + jj] = 0;
									q = 0.0;
								}

								//流速
								if ( q > 1.0e-10)
								{
									/*
									velocity = sqrt( (M/d)^2 + (N/d)^2 ) = sqrt( (M)^2 + (N)^2 )/d

									velocity' = sqrt( (c*M)^2 + (c*N)^2 )/d = c*sqrt( (M)^2 + (N)^2 )/d = c*q/d

									c =  velocity'*d/q
									*/
									if (d > 0.01)
									{

										grid.W(ii, jj) = d;
										double v = q / d;
										if (v < solv.riverInfo.riverList[river_id].velocity && solv.riverInfo.riverList[river_id].velocity > 0.0)
										{
											double c = solv.riverInfo.riverList[river_id].velocity*d / q;

											grid.M(ii, jj) *= c;
											grid.N(ii, jj) *= c;
										}
									} 
								}
							}
						}

						//降雨の考慮
						if ( solv.isWet(ii, jj) && solv.Data->W(ii, jj) > 0.01 && solv.riverInfo.rainfallIntensityConst.start >= 0 && solv.riverInfo.rainfallIntensityConst.end > 0)
						{
							//河川初期化から降水開始
							if (i*solv.dt >= solv.riverInfo.rainfallIntensityConst.start)
							{
								if ((i - 1)*solv.dt < solv.riverInfo.rainfallIntensityConst.start)
								{
									if (rainflg == 0 ) solv.log_printf("降雨開始\n");
									rainflg++;
								}
								//降水終了まで
								if (i*solv.dt <= solv.riverInfo.rainfallIntensityConst.end)
								{
									//mm->m h->s
									grid.w[ii*IX + jj] += ((solv.riverInfo.rainfallIntensityConst.rainfall*0.01 / 60.0) / 60.0)*solv.dt;
								}
								if ((i-1)*solv.dt < solv.riverInfo.rainfallIntensityConst.end)
								{
									if (i*solv.dt > solv.riverInfo.rainfallIntensityConst.end)
									{
										if (rainflg == 0) solv.log_printf("降雨終了\n");
										rainflg++;
									}
								}
							}
						}
					}
				}
			}
		}


		if ( impact_dummy_mode ) goto LOOP_CONTINUE;

		float victim_tot1 = 0;
		float victim_tot2 = 0;
		float outflow_number = 0;
		float all_destruction = 0;		//全壊
		float partial_destruction = 0;	//半壊
		float inundation = 0;				//床上浸水
		float under = 0;					//床下浸水
		float evacuees_number = 0;		//避難所避難者数(発災当日～発災2日後)
		double sediment = 0.0;			//堆積物

		if ( solv.RunUp && damage_estimation )
		{
			const int ysz = grid.jY;
			const int xsz = grid.iX;
			const AbeNoguera* prm = solv.Data;

#pragma omp parallel for reduction(+:victim_tot1,victim_tot2,outflow_number,all_destruction,partial_destruction,inundation,under,sediment,evacuees_number)  OMP_SCHEDULE
			for (int ii__ = 0; ii__ < grid.iX*grid.jY; ii__++)
			//for ( int ii = 0; ii < ysz; ii++ )
			{
				int ii = ii__ / grid.iX;
				int jj = ii__ % grid.iX;
				if (ii < ZERO_AREA_WD || ii >= grid.jY - ZERO_AREA_WD || jj < ZERO_AREA_WD || jj >= grid.iX - ZERO_AREA_WD)
				{
					continue;
				}
				//for ( int jj = 0; jj < xsz; jj++ )
				{
					const double w = prm->W(ii,jj);
					if ( prm->T(ii,jj) == 0 )
					{
						if ( solv.riverInfo.isRiver_or_Inflow(ii,jj))
						{
							continue;
						}
						if ( solv.isWet(ii,jj) && w >= 0.3)
						{
							if ( storm_surge_only == 1)
							{
								victim[ii*xsz+jj].eval(w,0);
							}else
							{
								victim[ii*xsz+jj].eval(w,timesum*R_MIN);
							}
							evacuees_number += victim[ii*xsz+jj].average_population_density_adp*0.666;
						}
						victim_tot1 += victim[ii*xsz+jj].Apd();
						victim_tot2 += victim[ii*xsz+jj].N_Apd();
						sediment += victim[ii*xsz+jj].sediment;
						under += victim[ii*xsz+jj].under;
						inundation += victim[ii*xsz+jj].inundation;

						//全壊・半壊は流速が落ちるため海岸からHOUSE_DEST_BOUNDの範囲とする
						int stat = 0;
						for ( int ki = -HOUSE_DEST_BOUND; ki < HOUSE_DEST_BOUND; ki++ )
						{
							if ( ii+ki < 0 || ii+ki >= ysz ) continue;
							for ( int kj = -HOUSE_DEST_BOUND; kj < HOUSE_DEST_BOUND; kj++ )
							{
								if ( jj+kj < 0 || jj+kj >= xsz ) continue;
								if ( solv.riverInfo.isRiver_or_Inflow(ii+ki,jj+kj))
								{
									continue;
								}
								stat += prm->T(ii+ki,jj+kj);
							}
						}
						if ( stat )
						{
							outflow_number += victim[ii*xsz+jj].outflow_number;
							all_destruction += victim[ii*xsz+jj].all_destruction;
							partial_destruction += victim[ii*xsz+jj].partial_destruction;
						}
					}
				}
			}
		}

		if (chkpointList.size())
		{
			const int sz = chkpointList.size();
			for ( int k = 0; k < sz; k++ )
			{
				chkpointList[k].victim1 = chkpointList[k].victim2 = 0.0;
			}
			const int ysz = grid.jY;
			const int xsz = grid.iX;
			const int runup = solv.RunUp;
			const AbeNoguera* prm = solv.Data;

			const int IX = xsz;
			const double* w__ = prm->w;
			const int* t__ = prm->topog;
			const double* elv__ = prm->elevation;

			const int num_threads       = omp_get_max_threads()+1;

			int* setFlg = new int[sz];
			int* setFlg2 = new int[sz];
			memset(setFlg, '\0', sizeof(int)*sz);
			
			int** pos = new int*[2*sz];
			double** w_th = new double*[sz];
			float** victim1_sum = new float*[sz];
			float** victim2_sum = new float*[sz];

			for ( int ii = 0; ii < sz; ii++ )
			{
				w_th[ii] = new double[num_threads];
				pos[2*ii] = new int[num_threads];
				pos[2*ii+1] = new int[num_threads];
				victim1_sum[ii] = new float[num_threads];
				victim2_sum[ii] = new float[num_threads];
				for ( int jj = 0; jj < num_threads; jj++ )
				{
					w_th[ii][jj] = chkpointList[ii].wmax;
					victim1_sum[ii][jj] = victim2_sum[ii][jj] = 0;
				}
			}
			//地形から
#pragma omp parallel for OMP_SCHEDULE
			for (int ii__ = 0; ii__ < grid.iX*grid.jY; ii__++)
			//for ( int ii = 1; ii < ysz-1; ii++ )
			{
				int ii = ii__ / grid.iX;
				int jj = ii__ % grid.iX;
				if (ii < ZERO_AREA_WD || ii >= grid.jY - ZERO_AREA_WD || jj < ZERO_AREA_WD || jj >= grid.iX - ZERO_AREA_WD)
				{
					continue;
				}
				//for ( int jj = 1; jj < xsz-1; jj++ )
				{
					double www = W__(ii,jj);
					const bool wet = solv.isWet(ii,jj);
					const int topo_flg = T__(ii,jj);
					const double elv = ELV__(ii,jj);

					if (topo_flg != 0)
					{
						continue;
					}
#if 0
					for (int iii = -1; iii <= 1; iii++)
						for (int jjj = -1; jjj <= 1; jjj++)
							if (W__(ii + iii, jj + jjj) > www) www = W__(ii + iii, jj + jjj);
#endif
					//そのチェックポイントを示すチェックリストを
					const int id = omp_get_thread_num();
					for (int k = 0; k < sz; k++)
					{
						const ChkPointMarker* marker = &(chkpointList[k]);
						if ( abs(marker->xx - jj) <= 1 && abs(marker->yy - ii) <= 1)
						{
							//見つけたらその位置における浸水値から遡上高さに換算する
							double ww = www;
							if (runup && wet)
							{
								ww = ww + elv;
							}
							//その位置のチェックリストに設定されている値よりも大きいなら更新する
							if (w_th[k][id] < ww)
							{
								setFlg[k] = 1;
								w_th[k][id] = ww;
								pos[2 * k][id] = ii;
								pos[2 * k + 1][id] = jj;
							}
							victim1_sum[k][id] += victim[ii*xsz + jj].Apd();
							victim2_sum[k][id] += victim[ii*xsz + jj].N_Apd();
						}
					}
				}
			}

			for ( int k = 0; k < sz; k++ )
			{
				ChkPointMarker* marker = &(chkpointList[k]);

				if (setFlg[k] == 1)
				{
					double wmax = w_th[k][0];
					int p[2] = {pos[2*k][0], pos[2*k+1][0]};
					//スレッド毎に持ち帰った値を比較して本当の最大値となった情報を取得する
					for ( int jj = 0; jj < num_threads-1; jj++ )
					{
						if ( wmax <= w_th[k][jj+1] )
						{
							wmax = w_th[k][jj+1];
							p[0] = pos[2*k][jj+1];
							p[1] = pos[2*k+1][jj+1];
						}
					}
					
					marker->wmax = wmax;
					int h = (int)((timesum*R_HOUR));
					int m = (int)((timesum - 3600.0*(double)h)*R_MIN);
					int s = timesum - 3600*h - 60.0*m;
					sprintf(marker->t, "%02d:%02d:%02d", h, m, s);
								
					if (runup && T__(p[0],p[1]) == 0 )
					{
						marker->runup = ELV__(p[0],p[1]);
					}else
					{
						marker->runup = 0.0;
					}
					if ( marker->s[0] == '\0' && marker->wmax >= 0.2)
					{
						sprintf(marker->s, "%02d:%02d:%02d", h, m, s);
					}
					marker->wrk = 1;
				}

				int victim1_sumall = 0;
				int victim2_sumall = 0;
				for ( int jj = 0; jj < num_threads; jj++ )
				{
					victim1_sumall += victim1_sum[k][jj];
					victim2_sumall += victim2_sum[k][jj];
				}
				delete [] victim1_sum[k];
				delete [] victim2_sum[k];

				marker->victim1 = victim1_sumall;
				marker->victim2 = victim2_sumall;

				delete [] w_th[k];
				delete [] pos[2*k];
				delete [] pos[2*k+1];
			}
			delete [] victim1_sum;
			delete [] victim2_sum;
			delete [] w_th;
			delete [] pos;
			delete [] setFlg;
			if (chkpointList.size() && isRender(i, render_step, timesum_diff)  )
			{
				TempBuffer fname;
				sprintf(fname.p, "%s%s%s_%s_相田指標評価.csv", drive, dir, solv.RunUp?"遡上考慮あり":"遡上考慮無し", IDname);
				FILE* fp = fopen(fname.p, "w");

				sprintf(fname.p, "%s%s%s_%s_相田指標評価(参考値).csv", drive, dir, solv.RunUp?"遡上考慮あり":"遡上考慮無し", IDname);
				FILE* fp2 = fopen(fname.p, "w");
				if (fp)
				{
					if (runup )
					{
						if(fp)fprintf(fp, "痕跡ランク,場所,計算値(遡上高),痕跡,比,標高,第1波到達時間(20cm),最大波高到達時間,緯度,経度\n");
						if(fp2)fprintf(fp2, "痕跡ランク,場所,計算値(遡上高),痕跡,比,標高,第1波到達時間(20cm),最大波高到達時間,緯度,経度\n");
					}else
					{
						if(fp)fprintf(fp, "痕跡ランク,場所,計算値,痕跡,比,標高,第1波到達時間(20cm),最大波高到達時間,緯度,経度\n");
						if(fp2)fprintf(fp2, "痕跡ランク,場所,計算値,痕跡,比,標高,第1波到達時間(20cm),最大波高到達時間,緯度,経度\n");
					}
				}
				std::vector<float> Ki;
				const int sz = chkpointList.size();
#ifdef USE_OPENMP_4
				int* cond = new int[sz];
				int* skipp = new int[sz];
				float* ki_wrk = new float[sz];

#pragma omp parallel for OMP_SCHEDULE
				for ( int k = 0; k < sz; k++ )
				{
					ChkPointMarker* marker = &(chkpointList[k]);
					cond[k] = 0;
					if ( marker->wmax > 0.01 )
					{
						float ki = marker->trace/(marker->wmax+solv.tide_level_offset);
						ki_wrk[k] = ki;
						cond[k] = 1;

						skipp[k] = 0;
						const int rank = marker->rank+'A';
						
						if (rank == 'C' || rank == 'D')
						{
							if (ki > 1.6 || ki < 0.4) skipp[k] = 1;
						}
						if ( exclusionRange[0] > 0.0 && exclusionRange[1] > 0.0 )
						{
							if ( rank == '@'+'A' )
							{
								if ( ki > 1.5 ) skipp[k] = 1;
								if ( ki < 0.5 ) 
								{
									skipp[k] = 1;
									marker->ref = true;
								}
							}else
							if ( rank == 'A' )
							{
								if ( ki > exclusionRange[1] ) skipp[k] = 1;
								if ( ki < exclusionRange[0] ) 
								{
									skipp[k] = 1;
									marker->ref = true;
								}
							}else if ( rank == 'B' )
							{
								if ( ki > exclusionRange[1] ) skipp[k] = 1;
								if ( ki < exclusionRange[0] ) 
								{
									skipp[k] = 1;
									marker->ref = true;
								}
							}else
							{
								if ( ki > exclusionRange[1] ) skipp[k] = 1;
								if ( ki < exclusionRange[0] ) 
								{
									skipp[k] = 1;
									marker->ref = true;
								}
							}
						}
					}
				}

				for ( int k = 0; k < sz; k++ )
				{
					if ( cond[k] == 0 ) continue;
					bool put_chkpoint = false;
					ChkPointMarker* marker = &(chkpointList[k]);

					if ( marker->wrk )
					{
						put_chkpoint = true;
						int rank = marker->rank+'A';
						if ( rank == '@'+'A' ) rank = '@';
						solv.log_printf("!%s%c:%-28s %-6.2f 痕跡:%-6.2f k:%-6.2f\n\t到達[%-9s] Max[%-9s]\n", (marker->s[0]!='\0')?"●":"", (marker->rank != 999)?rank:'?', marker->name.c_str(), marker->wmax+solv.tide_level_offset, marker->trace, ki_wrk[k], marker->s, marker->t);
					}else
					{
						put_chkpoint = true;
						//solv.log_printf("%s%c:%-28s %-6.2f 痕跡:%-6.2f k:%-6.2f\n\t到達[%-9s] Max[%-9s]\n", (chkpointList[k].s[0]!='\0')?"●":"", (chkpointList[k].rank != 999)?chkpointList[k].rank+'A':'?', chkpointList[k].name.c_str(), chkpointList[k].wmax+solv.tide_level_offset, chkpointList[k].trace, ki_wrk[k], chkpointList[k].s, chkpointList[k].t);
					}
					if ( !skipp[k] && !marker->ref)
					{
						Ki.push_back(ki_wrk[k]);
						int rank = chkpointList[k].rank+'A';
						if ( rank == '@'+'A' ) rank = '@';
						//fprintf(fp, "%c,%s,%6.2f,%6.2f,%6.2f,%-6.2f,%s,%s\n",(chkpointList[k].rank != 999)?chkpointList[k].rank+'A':'?', chkpointList[k].name.c_str(), chkpointList[k].wmax+solv.tide_level_offset, chkpointList[k].trace, ki, chkpointList[k].runup, chkpointList[k].s, chkpointList[k].t);
						//fprintf(fp, "%c,%s,%.16f,%.16f,%.16f,%-.16f,%s,%s,%.16f,%.16f\n",(chkpointList[k].rank != 999)?chkpointList[k].rank+'A':'?', chkpointList[k].name.c_str(), chkpointList[k].wmax+solv.tide_level_offset, chkpointList[k].trace, ki, chkpointList[k].runup, chkpointList[k].s, chkpointList[k].t, log(ki), log(ki)*log(ki));
						if ( fp )fprintf(fp, "%c,%s,%.16f,%.16f,%.16f,%-.16f,%s,%s,%f,%f\n", (marker->rank != 999)?rank:'?', marker->name.c_str(), marker->wmax+solv.tide_level_offset, marker->trace, ki_wrk[k], marker->runup, marker->s, marker->t,marker->lat,marker->lon);
					}
					if ( fp2 )
					{
						int rank = chkpointList[k].rank+'A';
						if ( rank == '@'+'A' ) rank = '@';
						fprintf(fp2, "%c,%s,%.16f,%.16f,%.16f,%-.16f,%s,%s,%f,%f\n", (marker->rank != 999)?rank:'?', marker->name.c_str(), marker->wmax+solv.tide_level_offset, marker->trace, ki_wrk[k], marker->runup, marker->s, marker->t,marker->lat,marker->lon);
					}
					if ( runup && put_chkpoint)
					{
						//solv.log_printf("    人的被害総数 (防災意識低い):%d (防災意識高い):%d\n\n", chkpointList[k].victim2,chkpointList[k].victim1);
					}
					marker->wrk = 0;
				}
				delete [] skipp;
				delete [] ki_wrk;
				delete [] cond;
#else
				for ( int k = 0; k < sz; k++ )
				{
					bool put_chkpoint = false;
					if ( chkpointList[k].wmax > 0.01 )
					{
						float ki = chkpointList[k].trace/(chkpointList[k].wmax+solv.tide_level_offset);

						bool skipp = false;
						switch(chkpointList[k].rank+'A')
						{
						default:
						case 'C':
							if ( ki > 1.5f ) skipp = true;
							if ( ki < 0.65f ) 
							{
								skipp = true;
								chkpointList[k].ref = true;
							}
							break;
						case 'B':
							if ( ki > 2.31f ) skipp = true;
							if ( ki < 0.5f ) 
							{
								skipp = true;
								chkpointList[k].ref = true;
							}
							break;
						case 'A':
							if ( ki > 2.31f ) skipp = true;
							if ( ki < 0.5f ) 
							{
								skipp = true;
								chkpointList[k].ref = true;
							}
							break;
						}

						if ( chkpointList[k].wrk )
						{
							put_chkpoint = true;
							solv.log_printf("!%s%c:%-28s %-6.2f 痕跡:%-6.2f k:%-6.2f\n\t到達[%-9s] Max[%-9s]\n", (chkpointList[k].s[0]!='\0')?"●":"", (chkpointList[k].rank != 999)?chkpointList[k].rank+'A':'?', chkpointList[k].name.c_str(), chkpointList[k].wmax+solv.tide_level_offset, chkpointList[k].trace, ki, chkpointList[k].s, chkpointList[k].t);
						}else
						{
							put_chkpoint = true;
							solv.log_printf("%s%c:%-28s %-6.2f 痕跡:%-6.2f k:%-6.2f\n\t到達[%-9s] Max[%-9s]\n", (chkpointList[k].s[0]!='\0')?"●":"", (chkpointList[k].rank != 999)?chkpointList[k].rank+'A':'?', chkpointList[k].name.c_str(), chkpointList[k].wmax+solv.tide_level_offset, chkpointList[k].trace, ki, chkpointList[k].s, chkpointList[k].t);
						}
						if ( !skipp && !chkpointList[k].ref)
						{
							Ki.push_back(ki);
							//fprintf(fp, "%c,%s,%6.2f,%6.2f,%6.2f,%-6.2f,%s,%s\n",(chkpointList[k].rank != 999)?chkpointList[k].rank+'A':'?', chkpointList[k].name.c_str(), chkpointList[k].wmax+solv.tide_level_offset, chkpointList[k].trace, ki, chkpointList[k].runup, chkpointList[k].s, chkpointList[k].t);
							//fprintf(fp, "%c,%s,%.16f,%.16f,%.16f,%-.16f,%s,%s,%.16f,%.16f\n",(chkpointList[k].rank != 999)?chkpointList[k].rank+'A':'?', chkpointList[k].name.c_str(), chkpointList[k].wmax+solv.tide_level_offset, chkpointList[k].trace, ki, chkpointList[k].runup, chkpointList[k].s, chkpointList[k].t, log(ki), log(ki)*log(ki));
							if ( fp )fprintf(fp, "%c,%s,%.16f,%.16f,%.16f,%-.16f,%s,%s,%f,%f\n", (chkpointList[k].rank != 999)?chkpointList[k].rank+'A':'?', chkpointList[k].name.c_str(), chkpointList[k].wmax+solv.tide_level_offset, chkpointList[k].trace, ki, chkpointList[k].runup, chkpointList[k].s, chkpointList[k].t,chkpointList[k].lat,chkpointList[k].lon);
						}
						if ( fp2 )fprintf(fp2, "%c,%s,%.16f,%.16f,%.16f,%-.16f,%s,%s,%f,%f\n", (chkpointList[k].rank != 999)?chkpointList[k].rank+'A':'?', chkpointList[k].name.c_str(), chkpointList[k].wmax+solv.tide_level_offset, chkpointList[k].trace, ki, chkpointList[k].runup, chkpointList[k].s, chkpointList[k].t,chkpointList[k].lat,chkpointList[k].lon);
					}else
					{
						//solv.log_printf("****(%s%c:%-28s %-6.2f 痕跡:%-6.2f\n\t到達[%-9s] Max[%-9s])\n", (chkpointList[k].s[0]!='\0')?"●":"", chkpointList[k].rank+'A', chkpointList[k].name.c_str(), chkpointList[k].wmax+solv.tide_level_offset, chkpointList[k].trace, chkpointList[k].s, chkpointList[k].t);
					}
					if ( solv.RunUp && put_chkpoint)
					{
						solv.log_printf("    人的被害総数 (防災意識低い):%d (防災意識高い):%d\n\n", chkpointList[k].victim2,chkpointList[k].victim1);
					}
					chkpointList[k].wrk = 0;
				}
#endif
				float K = 0.0f;
				float k = 0.0f;
				const int sz2 = Ki.size();
				if ( sz2 )
				{
					solv.log_printf("n=%d 潮位補正:%+3.2f 初期波源補正:x%.2f\n", sz2, solv.tide_level_offset, solv.Data->w_scale[0]);

#pragma omp parallel for reduction(+:K) reduction(+:k) OMP_SCHEDULE
					for ( int ii = 0; ii < sz2; ii++ )
					{
						//printf("K[%d] %f\n", k, Ki[ii]);
						const float s = log(Ki[ii]); 
						K += s;
						k += s*s;
					}
					float logK = K/(float)sz2;
					float logk = 0.0f;
					
					solv.K_Value = exp(logK);
					solv.log_printf("\n(1)適合度の評価(幾何平均0.8～1.2) K=%.3f (log K:%.3f)", solv.K_Value, logK);
					if ( 0.80 <= solv.K_Value && solv.K_Value <= 1.2 ) solv.log_printf("(適合)\n");
					else solv.log_printf("\n");

					if (k - sz2*logK*logK > 0.0f)
					{
						logk = sqrt((k - sz2*logK*logK)/sz2);
						solv.k_Value = exp(logk);
						solv.log_printf("(2)適合度の評価(幾何標準偏差<1.6) k=%.3f (log k:%.3f)", solv.k_Value, logk);
						if ( solv.k_Value <= 1.6 ) solv.log_printf("(適合)\n");
						else solv.log_printf("\n");
					}
					if (fp)
					{
						fprintf(fp, "\n");
						fprintf(fp, "%s,適合度の評価(幾何平均0.8～1.2)Κ,%.3f,0,0,0,0\n", ( 0.80 <= solv.K_Value && solv.K_Value <= 1.2 )?"○":"×", solv.K_Value);
						fprintf(fp, "%s,適合度の評価(幾何標準偏差<1.6)κ,%.3f,0,0,0,0\n", ( solv.k_Value <= 1.6 )?"○":"×",  solv.k_Value);
						fprintf(fp, "サンプル数,%d\n", sz2);
						fprintf(fp, "sum(logK),%f\n", K);
						fprintf(fp, "sum(logK^2),%f\n", k);
					}
				}
				if ( fp ) fclose(fp);
				if ( fp2 ) fclose(fp2);
			}
		}
		if ( solv.RunUp && isRender(i, render_step, timesum_diff)  && damage_estimation )
		{
			solv.Victim_tot1 = victim_tot1;
			solv.Victim_tot2 = victim_tot2;
			solv.outflow_number = outflow_number;			//流出
			solv.all_destruction = all_destruction;			//全壊
			solv.partial_destruction = partial_destruction;	//半壊
			solv.inundation = inundation;					//床上浸水
			solv.under = under;								//床下浸水
			solv.evacuees_number = evacuees_number;
			solv.sediment = sediment;

			solv.log_printf("人的被害総数 (防災意識低い):%d (防災意識高い):%d\n", (int)victim_tot2, (int)victim_tot1);
			solv.log_printf("建造物流出被害総数:%d\n", (int)outflow_number);
			solv.log_printf("建造物被害総数 全壊:%d 半壊:%d 床上浸水:%d 床下浸水:%d\n", 
				(int)all_destruction, (int)partial_destruction, (int)inundation, (int)under);
			solv.log_printf("避難所避難者数:%d 避難所外避難者:%d\n", 
				(int)evacuees_number, (int)((double)evacuees_number*0.5));

			double tot_sediment = 0.0001*(WOODEN_RATE*(all_destruction+0.5*partial_destruction)*FLOOR_AREA_AVERAGE/RUBBLE_WEGHT_BAS1+
				(1.0-WOODEN_RATE)*(all_destruction+0.5*partial_destruction)*FLOOR_AREA_AVERAGE/RUBBLE_WEGHT_BAS2);
			double flood_trash = 0.0001*inundation*15.0;

			solv.log_printf("建造物瓦礫量:%.2f万トン\n",tot_sediment);
			solv.log_printf("水害ごみ量:%.2f万トン\n",flood_trash);
			solv.log_printf("瓦礫量:%.2f万トン 処理費用:%.3f億円\n",tot_sediment+flood_trash, 0.0001*RUBBLE_CODST*(tot_sediment+flood_trash));
			solv.log_printf("堆積物量:%.2f万トン\n\n", 0.0001*sediment);
		}

LOOP_CONTINUE:	;
		//printf(" time:%d[ms]\n", (clock()-ts));
		calc_time += (clock()-ts);
		if ( i == loopnum-1 ) stat = 0;
		if ( isRender(i, render_step, timesum_diff) )
		{
			render_count++;
			timesum_diff = 0.0;
		}
	}

	if ( !csv2obj && solv.RunUp && realtimeImage)
	{
		if ( storm_surge_only != 1 )
		{
			CreateImage(solv, solv.Data->w,  OutputLastCounter, colorbar_min, colorbar_max, colorbar_max, drive, dir, exe_drive, exe_dir, smooth_flg, smooth_num, smooth, ColorMap, colormap, timesum, longitude, latitude);
		}
		else
		{
			CreateImage(solv, solv.Data->w, OutputLastCounter, colorbar_min, colorbar_max, colorbar_max, drive, dir, exe_drive, exe_dir, smooth_flg, smooth_num, smooth, ColorMap, colormap, timesum, longitude, latitude);
		}
	}

	if ( victim ) delete [] victim;
	if ( solv.d2vale ) delete []  solv.d2vale;
	if ( csv2obj )
	{
		solv.log_printf("3Dデータ変換完了\n");
		solv.logClose();
		return 0;
	}

	if ( water_depth_and_elevation ) solv.log_printf("[水深+標高]一括データを使用\n");
	solv.log_printf("■%s[time %d[ms]\n", IDname, calc_time);
	solv.log_printf("最大波高Max:%3.3f[m] Min:%3.3f[m]\n", solv.wmax, solv.wmin);
	solv.log_printf("到達最大津波高さMax[%d,%d]:%3.3f[m]\n", solv.maxheightPos[0], solv.maxheightPos[1], solv.maxheight);
	{
		int wi = solv.maxheightPos[0], wj = solv.maxheightPos[1];
		if (wi >= 0 && wj >= 0)
		{
			if ( solv.Data->T(wi,wj) == 0 )
			{
				solv.log_printf("Max(陸地標高:%.3f 水位:%.3f -> 水深:%.3f\n", solv.Data->ELV(wi,wj), solv.Data->ELV(wi,wj)+solv.Data->W(wi,wj), solv.Data->W(wi,wj));
			}
		}
	}
	int ii, jj;
	double maxheight = solv.MaxHeight(ii, jj);
	solv.log_printf("計算最終時刻での到達最大津波高さMax[%d,%d]:%3.3f[m]\n", ii, jj, maxheight);
	if ( chkpointList.size())
	{
		solv.log_printf("(1)適合度の評価(幾何平均0.8～1.2) K=%.2f %s\n", solv.K_Value, ( 0.80 <= solv.K_Value && solv.K_Value <= 1.2 )?"○":"×");
		solv.log_printf("(2)適合度の評価(幾何標準偏差<1.6) k=%.2f %s\n", solv.k_Value, ( solv.k_Value <= 1.6 )?"○":"×");
	}
	if ( solv.RunUp && damage_estimation)
	{
		solv.log_printf("人的被害総数 (防災意識低い):%d (防災意識高い):%d\n", (int)solv.Victim_tot2, (int)solv.Victim_tot1);
		solv.log_printf("家屋流出被害総数:%d \n\n", (int)solv.outflow_number);
		solv.log_printf("建造物被害総数 全壊:%d 半壊:%d 床上浸水:%d 床下浸水:%d\n", 
			(int)solv.all_destruction, (int)solv.partial_destruction, (int)solv.inundation, solv.under);
		solv.log_printf("避難所避難者数:%d 避難所外避難者:%d\n", 
			solv.evacuees_number, (int)((double)solv.evacuees_number*0.5));

		double tot_sediment = 0.0001*(WOODEN_RATE*(solv.all_destruction+0.5*solv.partial_destruction)*FLOOR_AREA_AVERAGE/RUBBLE_WEGHT_BAS1+
			(1.0-WOODEN_RATE)*(solv.all_destruction+0.5*solv.partial_destruction)*FLOOR_AREA_AVERAGE/RUBBLE_WEGHT_BAS2);
		double flood_trash = 0.0001*solv.inundation*15.0;

		solv.log_printf("建造物瓦礫量:%.2f万トン\n",tot_sediment);
		solv.log_printf("水害ごみ量:%.2f万トン\n",flood_trash);
		solv.log_printf("瓦礫量:%.2f万トン 処理費用:%.3f億円\n",tot_sediment+flood_trash, 0.0001*RUBBLE_CODST*(tot_sediment+flood_trash));
		solv.log_printf("堆積物量:%.2f万トン\n\n", 0.0001*solv.sediment);
	}
	solv.log_printf("%s", (stat == 0)?"■■■■ 正常終了 ■■■■\n":"■■■■ !!エラー終了!! ■■■■\n");
	solv.log_printf("=========== シミュレーション END=================\n\n");

	{
		TempBuffer fname;
		sprintf(fname.p, "%s%soutput\\Closed_%06d", drive, dir, OutputLastCounter);
		FILE* fp = fopen(fname.p, "w");
		fprintf(fp, "%d\n", stat);
		fclose(fp);
		sprintf(fname.p, "%s%simage\\Closed_%06d", drive, dir, OutputLastCounter);
		fp = fopen(fname.p, "w");
		fprintf(fp, "%d\n", stat);
		fclose(fp);
	}


	if (kml_export)
	{
		TempBuffer fname(_MAX_FNAME);
		sprintf(fname.p, "%s%sgoogle_earth\\%s_sim.kml", drive, dir, IDname);
		FILE* fp = fopen(fname.p, "w");
		if (fp)
		{
			fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
			fprintf(fp, "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n");
			fprintf(fp, "<Folder>\n");
			fprintf(fp, "  <name>Animation</name>\n");

			TempBuffer fname2(_MAX_FNAME);
			TempBuffer buf;
			for (int i = 1; i < kml_export; i++)
			{
				sprintf(fname2.p, "%s%sgoogle_earth\\%s_image_%06d", drive, dir, IDname, i);
				FILE* fp2 = fopen(fname2.p, "r");
				if (fp2)
				{
					while (fgets(buf.p, 256, fp2) != NULL)
					{
						fprintf(fp, "%s", buf.p);
					}
					fclose(fp2);
					_unlink(fname2.p);
				}
				else
				{
					break;
				}
			}
			fprintf(fp, "  </Folder>\n");
			fprintf(fp, "</kml>\n");
			fclose(fp);
		}
	}

	if ( stat < 0 )
	{
		solv.logClose();
		return -1;
	}

	{
		TempBuffer fname;
		sprintf(fname.p, "%s%soutput_outline.txt", drive, dir);
		FILE* fp = fopen(fname.p, "w");
		fprintf(fp, "%f\n", solv.wmin);
		fprintf(fp, "%f\n", solv.wmax);
		fprintf(fp, "%f\n", solv.w_max);
		fclose(fp);
	}

create_image_onry:	;
	//if (realtimeImage == 0 || realtimeImage == 2)
	//{
	//	printf("=========== 画像生成 START =================\n");
	//	for ( int i = 0; i < loopnum; i+=render_step )
	//	{
	//		CreateImage(solv, 0, i, solv.wmin, (0.8*solv.wmax+1.3*solv.w_max)*0.5, solv.wmax, drive, dir, exe_drive, exe_dir, smooth_flg, smooth_num, smooth, ColorMap, colormap, timesum, longitude, latitude);
	//		printf("            \r[%d/%d]", i+1, loopnum);
	//	}
	//	printf("\n");
	//	printf("=========== 画像生成 END =================\n");
	//	printf("\n");
	//}

render_code_only:	;
	//RenderCode( loopnum, render_step, drive, dir);
	
	if ( solv.dispersion_correction_coefficient ) delete [] solv.dispersion_correction_coefficient;
	if ( solv.Breaking_point ) delete [] solv.Breaking_point;
	if ( solv.Breaking_factor ) delete [] solv.Breaking_factor;

	solv.CheckerBordCheckTerm();
	if ( solv.RunUp )
	{
		if ( solv.RunUpMarker ) delete [] solv.RunUpMarker;
		solv.RunUpMarker = 0;
		runup_color_level0.Clear();
		runup_color_level.Clear();
	}
	if ( CrustalMovementData )
	{
		delete [] CrustalMovementData;
		CrustalMovementData = 0;
	}
	if ( CrustalMovementDiff )
	{
		delete [] CrustalMovementDiff;
		CrustalMovementDiff = 0;
	}
	if ( topography_data_mask_bmp.GetImage() == NULL )
	{
		topography_data_mask_bmp.Clear();
	}
	if ( topography_data_mask2_bmp.GetImage() == NULL )
	{
		topography_data_mask2_bmp.Clear();
	}
	if (topography_data_edit.GetImage() == NULL)
	{
		topography_data_edit.Clear();
	}
	
	delete [] topography_data;
	delete [] water_depth_data;
	delete [] topography_data_mask;
	delete [] latitude_data;
	delete [] manning_data;
	delete [] elevation_data;
	delete [] samplingline_data;
	delete [] samplingpoint_data;
	delete[] logfilename;
	delete [] Initial_wave_data[0];
	delete [] Initial_wave_data[1];
	delete [] Initial_wave_data[2];

	solv.riverInfo.Clear();
	solv.DeleteGapFlg();
	solv.DeleteCoriolisFactor();
	solv.DeleteBoundaryBandMap();
	solv.DeleteStormInfo();
	solv.DeleteInviolability();
	solv.logClose();
	return 0;
}


void CreateImage(Solver& solv, double* solv_w, int i, double min, double max, double max2, char* drive, char* dir, char* drive2, char* dir2, bool smooth_flg, int smooth_num, double smooth[3][3], BitMap& ColorMap, bool colormap, double timesum, double* lon, double* lat)
{
	char fname[512];

	if ( topography_data_mask_bmp.GetImage() == NULL )
	{
		sprintf(fname, "%s%stopography_data_mask.bmp", drive, dir);
		topography_data_mask_bmp.Read(fname);
	}
	if ( topography_data_mask2_bmp.GetImage() == NULL )	//google map等
	{
		sprintf(fname, "%s%stopography_data_mask2.bmp", drive, dir);
		topography_data_mask2_bmp.Read(fname);
	}

	BitMap bmp3;
	bmp3.Copy(topography_data_mask_bmp);

	if ( topography_data_mask2_bmp.GetImage() )
	{
#pragma omp parallel for OMP_SCHEDULE
		for ( int i = 0; i < solv.Data->jY; i++ )
		{
			for ( int j = 0; j < solv.Data->iX; j++ )
			{
				if (solv.Data->T(i, j) != 0)
				{
					continue;
				}
				//if ( topography_data_mask2_bmp.cell(i,j).r >90 && topography_data_mask2_bmp.cell(i,j).g < 90 && topography_data_mask2_bmp.cell(i,j).b < 90)
				//{
				//	continue;
				//}
				bmp3.cell(i,j) = topography_data_mask2_bmp.cell(i,j);
			}
		}
	}

	if ( colormap && BitMap::colortableNum == 0 )
	{
		BitMap colorMap;
		sprintf(fname, "%s%s..\\colormap\\colormap.bmp", drive2, dir2);
		colorMap.Read(fname);

		ColorMap.ColorTable(colorMap);
	}

//#pragma omp parallel for
	{
		char fname[_MAX_FNAME];	// ファイル名
		BitMap bmp2;
		if ( solv_w == NULL )
		{
			sprintf(fname, "%s%soutput\\output_W%06d.csv", drive, dir, i);
			bmp2.ReadCsv(fname, min, max);
		}else
		{
			bmp2.ReadCsv(solv_w, solv.iX, solv.jY, min, max);
		}

		solv.Data->BitmapMask(bmp3);
		//bmp2.ToGrayScale();
		//bmp2.Reverse();
		bmp2.ToGrayScale_and_Reverse();

		int w, h;
		double* z = 0;
		
		if ( solv_w == NULL )
		{
			z = ReadCsv( fname, w, h);
		}else
		{
			z = solv_w;
		}
		const AbeNoguera* prm = solv.Data;

		//if ( solv.RunUp )
		//{
		//	#pragma omp parallel for
		//	for ( int i = 0; i < solv.Data->jY; i++ )
		//	{
		//		for ( int j = 0; j < solv.Data->iX; j++ )
		//		{
		//			if ( solv.Data->topog[i*solv.Data->iX+j] == 0 && fabs(z[i*solv.Data->iX+j]) > 0.1)
		//			{
		//				z[i*solv.Data->iX+j] += solv.Data->elevation[i*solv.Data->iX+j];
		//			}
		//		}
		//	}
		//}

		const int xsz = prm->iX;
		const int ysz = prm->jY;
		const int IX = prm->iX;

		const bool river_exist = solv.riverInfo.existRiver();
		//if (river_exist && solv_w)
		//{
		//	double* solv_ww = solv_w;
		//	z = new double[prm->iX*prm->jY];
		//	memcpy(z, solv_ww, sizeof(double)*prm->iX*prm->jY);
		//	solv_w = NULL;
		//}

		if ( colormap )
		{
//#pragma omp parallel for OMP_SCHEDULE
//			for (int i = 0; i < ysz; i++)
//			{
//				for (int j = 0; j < xsz; j++)
//				{
//					if (solv.riverInfo.isRiver_or_Inflow(i, j))
//					{
//						z[IX*i + j] = z[IX*i + j] - solv.riverInfo.WaterDepth(i,j);
//					}
//				}
//			}

			bmp2.ColorLevel(min, max, z, 0.0, NULL, prm->topog, prm->elevation);
		}

		solv.Data->BitmapMask(bmp2, bmp3, z);

		const Rgb rgbZero(0,0,0);


#if 0
		//遡上先端（先に書いておくと先端だけが表示で残る
		if ( 1 && solv.RunUp )
		{
#pragma omp parallel for OMP_SCHEDULE
			for ( int i = 0; i < ysz; i++ )
			{
				for ( int j = 0; j < xsz; j++ )
				{
					if ( solv.RunupConditionFlag[i*IX+j] == IS_WET)
					{
						bmp2.cell(i,j) = Rgb(250, 255, 250);
					}
				}
			}
		}
#endif

		if ( solv.RunUp )
		{
			const double xx = (double)runup_color_level.W()/2.0;
			const int yy = runup_color_level.H()/2;
			const int yy0 = runup_color_level0.H()/2;

			const double drydepth = 0.01;		

#pragma omp parallel for OMP_SCHEDULE
			for ( int i = 0; i < ysz; i++ )
			{
				for ( int j = 0; j < xsz; j++ )
				{
					double river_depth = 0.0;

					//if (solv.riverInfo.isRiver_or_Inflow(i, j))
					//{
					//	continue;
					//}
					if ( solv.RunUpMarker[i*IX+j] >= 0 )
					{
						double a;
						int color_index = -1;

						double zz = z[i*IX + j];
						//if (solv.riverInfo.isRiver_or_Inflow(i, j))
						//{
						//	zz = z[IX*i + j] - solv.riverInfo.WaterDepth(i,j);
						//}

						if ( fabs(zz+solv.tide_level_offset- river_depth) <= 1.0 )
						{
							a = (zz+solv.tide_level_offset-drydepth - river_depth)/(1.0 -drydepth);

							if ( a < 0.0 ) a = 0.0;
							else if ( a > 1.0 ) a = 1.0;
							color_index = (int)(a*xx-1);
						}else
						{
							a = (zz+solv.tide_level_offset-drydepth- river_depth)/(10.0 -drydepth);
							if ( a < 0.0 ) a = 0.0;
							else if ( a > 1.0 ) a = 1.0;
							color_index = (int)(xx)+(int)(a*(double)runup_color_level.W()/2.0);
						}

						if ( color_index >= runup_color_level.W() ) color_index = runup_color_level.W()-1;
						if ( color_index < 0.0 ) color_index = 0;
						if ( solv.RunUpMarker[i*IX+j] < color_index )
						{
							solv.RunUpMarker[i*IX+j] = color_index;
						}
						double alp = 0.50;
						//if ( z[i*IX+j] > drydepth )
						//{
						//	alp = 0.85;
						//}
						const Rgb& ff = runup_color_level.cell(yy, color_index);
						const Rgb& gg = bmp3.cell(i,j);
						double r = (double)ff.r*alp + (double)gg.r*(1.0-alp);
						double g = (double)ff.g*alp + (double)gg.g*(1.0-alp);
						double b = (double)ff.b*alp + (double)gg.b*(1.0-alp);
						//if ( z[i*IX+j] <= drydepth )
						//{
						//	alp = 0.85;
						//	Rgb& rgb = bmp3.cell(i,j);
						//	r = 131.0*alp + (double)rgb.r*(1.0-alp);
						//	g = 107.0*alp + (double)rgb.g*(1.0-alp);
						//	b = 101.0*alp + (double)rgb.b*(1.0-alp);
						//}
						
						if ( r > 255.0 ) r = 255.0;
						else if ( r <0.0 ) r = 0.0;
						if ( g > 255.0 ) g = 255.0;
						else if ( g <0.0 ) g = 0.0;
						if ( b > 255.0 ) b = 255.0;
						else if ( b <0.0 ) b = 0.0;
						bmp2.cell(i,j) = Rgb(r, g, b);
					}else
					if ( prm->topog[i*IX+j] == 0 && fabs(z[i*IX+j]) > drydepth)
					{
						double zz = z[i*IX + j];
						//if (solv.riverInfo.isRiver_or_Inflow(i, j))
						//{
						//	zz = z[IX*i + j] - solv.riverInfo.WaterDepth(i,j);
						//}
						double a = (zz+solv.tide_level_offset - drydepth- river_depth)/(10.0-drydepth);
						int color_index = (int)(a*(double)runup_color_level0.W()-1);

						if ( color_index >= runup_color_level0.W() ) color_index = runup_color_level0.W()-1;
						if ( color_index < 0.0 ) color_index = 0;
						bmp2.cell(i,j) = runup_color_level0.cell(yy0, color_index);
						solv.RunUpMarker[i*IX+j] = color_index;
					}
				}
			}
		}

		//海底露出
		if ( 0 )
		{
			const double xx = (double)runup_color_level.W()/2.0;
			const int yy = runup_color_level.H()/2;
			const int yy0 = runup_color_level0.H()/2;

#pragma omp parallel for OMP_SCHEDULE
			for ( int i = 0; i < ysz; i++ )
			{
				for ( int j = 0; j < xsz; j++ )
				{
					double d = solv.Data->W(i,j)+solv.Data->H(i,j);
					if ( solv.Data->T(i,j) != 0 && d < 40.0)
					{

						int color_index = (solv.Data->H(i,j)-solv.hmax)/(solv.hmax - solv.hmin);
						if ( color_index < 0 ) color_index = 0;
						if ( color_index >= 255 ) color_index = 255;

						color_index = 255-color_index;
						double rr = 24*(color_index/255.0);
						double gg = 56*(color_index/255.0);
						double bb = 57*(color_index/255.0);

						Rgb exposure(rr, gg, bb);

						double alp = 0.8 - 0.79*(d/40.0);

						const Rgb& ff = bmp3.cell(i,j);
						double r = (double)exposure.r*alp + (double)ff.r*(1.0-alp);
						double g = (double)exposure.g*alp + (double)ff.g*(1.0-alp);
						double b = (double)exposure.b*alp + (double)ff.b*(1.0-alp);
						bmp2.cell(i,j) = Rgb(r, g, b);
					}
				}
			}
		}

		if (getenv("MAX_MARKER"))
		{
			int wi = solv.maxheightPos[0], wj = solv.maxheightPos[1];
			if (wi >= 0 && wj >= 0)
			{
				for (int ii = -30; ii <= 30; ii++)
				{
					if (wi + ii < 0 || wi + ii >= xsz) continue;
					if (ii == 0) continue;
					bmp2.cell(wi + ii, wj + 0) = Rgb(255, 0, 0);
				}
				for (int jj = -30; jj <= 30; jj++)
				{
					if (wj + jj < 0 || wj + jj >= ysz) continue;
					if (jj == 0) continue;
					bmp2.cell(wi + 0, wj + jj) = Rgb(255, 0, 0);
				}
			}

			double wwmax = -999999999.0;
			wi = -1, wj = -1;
//#pragma omp parallel for OMP_SCHEDULE	//NG!!並列化しちゃいけない
			for (int i = 0; i < ysz; i++)
			{
				for (int j = 0; j < xsz; j++)
				{
					double d = solv.Data->W(i, j) + solv.Data->H(i, j);
					if (wwmax < solv.Data->W(i, j))
					{
						wi = i;
						wj = j;
						wwmax = solv.Data->W(i, j);
					}
				}
			}

			if (wi >= 0 && wj >= 0)
			{
				for (int ii = -30; ii <= 30; ii++)
				{
					if (wi + ii < 0 || wi + ii >= ysz) continue;
					if (ii == 0) continue;
					bmp2.cell(wi + ii, wj + 0) = Rgb(255, 255, 255);
				}
				for (int jj = -30; jj <= 30; jj++)
				{
					if (wj + jj < 0 || wj + jj >= xsz) continue;
					if (jj == 0) continue;
					bmp2.cell(wi + 0, wj + jj) = Rgb(255, 255, 255);
				}
			}
		}

		//砕波点
		if ( 1 && solv.Breaking_point )
		{
			double alp = 0.6;
#pragma omp parallel for OMP_SCHEDULE
			for ( int i = 0; i < ysz; i++ )
			{
				for ( int j = 0; j < xsz; j++ )
				{
					if ( solv.Breaking_point[i*IX+j] == 1 && solv.Data->T(i,j) != 0)
					{
						Rgb exposure(255, 255, 255);
						const Rgb& gg = bmp2.cell(i,j);
						double r = (double)exposure.r*alp + (double)gg.r*(1.0-alp);
						double g = (double)exposure.g*alp + (double)gg.g*(1.0-alp);
						double b = (double)exposure.b*alp + (double)gg.b*(1.0-alp);
						bmp2.cell(i,j) = Rgb(r, g, b);
					}
				}
			}
		}


		//チェッカーボード数値振動検出の可視化用
		int check_item = OUTPUT_VALUE;
		if (  solv.CheckerBord[check_item] && checker_bord_error_debug )
		{
			BitMap chk_color_level;
			sprintf(fname, "%s%s..\\colormap\\chk_colormap.bmp", drive2, dir2);
			chk_color_level.Read(fname);
			printf("check\n");
#pragma omp parallel for OMP_SCHEDULE
			for ( int i = 0; i < ysz; i++ )
			{
				for ( int j = 0; j < xsz; j++ )
				{
					if ( solv.CheckerBord[check_item][i*IX+j] >= 1 )
					{
						//printf("check %d\n", solv.CheckerBord[i*IX+j]);
						double a;
						int color_index = -1;

						a = solv.CheckerBord[check_item][i*IX+j]/(10.0);
						if ( a < 0.0 ) a = 0.0;
						else if ( a > 1.0 ) a = 1.0;
						color_index = (int)(a*(double)chk_color_level.W())-1;

						if ( color_index >= chk_color_level.W() ) color_index = chk_color_level.W()-1;
						if ( color_index < 0.0 ) color_index = 0;
						double alp = 0.2;
						const Rgb& ff = bmp2.cell(i,j);
						const Rgb& gg = chk_color_level.cell(chk_color_level.H()/2, color_index);
						double r = (double)gg.r*alp + (double)ff.r*(1.0-alp);
						double g = (double)gg.g*alp + (double)ff.g*(1.0-alp);
						double b = (double)gg.b*alp + (double)ff.b*(1.0-alp);
						
						if ( r > 255.0 ) r = 255.0;
						else if ( r <0.0 ) r = 0.0;
						if ( g > 255.0 ) g = 255.0;
						else if ( g <0.0 ) g = 0.0;
						if ( b > 255.0 ) b = 255.0;
						else if ( b <0.0 ) b = 0.0;
						bmp2.cell(i,j) = Rgb(r, g, b);
						//bmp2.cell(i,j) = Rgb(255, 255, 255);
					}
				}
			}
		}

		if ( !kml_export && solv_w == 0 ) delete [] z;

		sprintf(fname, "%s%simage\\output_W%06d.bmp", drive, dir, i);

		if ( solv.Storm_p )
		{
			int windvector = 0;
			char* env = getenv("WINDVECTOR_PLOT");
			if ( env ) windvector = atoi(env);

			int dmy;
			Storm_Point a = solv.Storm_p->get(solv.Storm_p->GetTime(), dmy);
			double lon = a.lon;
			double lat = a.lat;
			
			int x = (lon - solv.Storm_p->longitude[0])*(double)(xsz-1)/(solv.Storm_p->longitude[1] - solv.Storm_p->longitude[0]);
			int y = (lat - solv.Storm_p->latitude[2])*(double)(ysz-1)/(solv.Storm_p->latitude[0] - solv.Storm_p->latitude[2]);
			
			for ( int kk = -3; kk < 3; kk++ )
			{
				if ( y+kk < 0 || y+kk >= bmp2.H() ) continue;
				for ( int ll = -3; ll < 3; ll++ )
				{
					if ( x+ll < 0 || x+ll >= bmp2.W() ) continue;
					bmp2.cell(y+kk, x+ll) = Rgb(255, 255, 255);
				}
			}
			if ( windvector ) solv.Storm_p->windVector(bmp2);
			
			double s = sqrt(solv.dx*solv.dx + solv.dy*solv.dy);
			double r = a.rm / s;

			double rr = solv.Storm_p->StormArea(25);	//暴風域
			rr /= s;

			double rrr = solv.Storm_p->StormArea(15);	//強風域
			rrr /= s;

			for (double t = 0; t < 360.0; t += 0.2)
			{
				double co = cos(t*M_PI / 180.0);
				double si = sin(t*M_PI / 180.0);

				int j = x + (int)(r*co + 0.5);
				int i = y + (int)(r*si + 0.5);
				if (i < 0 || i >= bmp2.H() || j < 0 || j >= bmp2.W())
				{
					continue;
				}
				bmp2.cell(i, j) = Rgb(255.0 / 4.0, 0, 0);

				j = x + (int)(rr*co + 0.5);
				i = y + (int)(rr*si + 0.5);
				if (i < 0 || i >= bmp2.H() || j < 0 || j >= bmp2.W())
				{
					continue;
				}
				bmp2.cell(i, j) = Rgb(255.0, 0, 0);
			}

		}

		{
			int mnvector = 0;
			char* env = getenv("VECTOR_PLOT");
			if ( env ) mnvector = atoi(env);
			if ( mnvector )
			{
				MNVector(&solv, bmp2, lat, lon, mnvector);
			}
		}

		if (debug_marker)
		{
#pragma omp parallel for OMP_SCHEDULE
			for (int i = 1; i < ysz - 1; i++)
			{
				for (int j = 1; j < xsz - 1; j++)
				{
					if (debug_marker[i*IX + j])
					{
#if 0
						bmp2.cell(i, j) = Rgb(1, 0, 0);
						for (int k = -1; k <= 1; k++)
							for (int kk = -1; kk <= 1; kk++)
								bmp2.cell(i + kk, j + k) = Rgb(255 * debug_marker[i*IX + j], 0, 0);
#endif
						debug_marker[i*IX + j] = 0.0f;
					}
				}
			}
		}

		if (topography_data_mask2_bmp.GetImage())
		{
#pragma omp parallel for OMP_SCHEDULE
			for (int i = 0; i < solv.Data->jY; i++)
			{
				for (int j = 0; j < solv.Data->iX; j++)
				{
					if (topography_data_mask2_bmp.cell(i, j).r == 255 && topography_data_mask2_bmp.cell(i, j).g == 255 && topography_data_mask2_bmp.cell(i, j).b == 255)
					{
						bmp2.cell(i, j) = topography_data_mask2_bmp.cell(i, j);
						continue;
					}
				}
			}
		}
		//吸収領域を削除
		//if ( absorbingZone ) bmp2.Offset(-absorbingZone-1);
		bmp2.Write(fname);

		utf8str utf8;
		if ( kml_export )
		{
#pragma omp parallel for OMP_SCHEDULE
			for ( int i = 0; i < ysz; i++ )
			{
				for ( int j = 0; j < xsz; j++ )
				{
					//if (solv.riverInfo.isRiver_or_Inflow(i, j))
					//{
					//	continue;
					//}
					if ( prm->T(i,j) == 0 && fabs(z[i*IX+j]) <= 0.01)
					{
						bmp2.cell(i,j) = rgbZero;
					}
				}
			}
			if ( solv_w == 0 ) delete [] z;

			sprintf(fname, "%s%sgoogle_earth\\%s_output_W%06d.bmp", drive, dir, IDname, kml_export);
			bmp2.Write(fname);

#ifdef USE_STB_IMAGE
			{
				unsigned char* pixels;
				int width;
				int height;
				int bpp;
				pixels = stbi_load(fname, &width, &height, &bpp, 0);

				char fname2[_MAX_FNAME];
				sprintf(fname2, "%s%sgoogle_earth\\%s_output_W%06d.png", drive, dir, IDname, kml_export);

				unsigned char* pixels2 = stbi__convert_format(pixels, bpp, 4, width, height);
				for (int i = 0; i < height; i++)
				{
					for (int j = 0; j < width; j++)
					{
						int pos = (i*width + j);
						if (pixels2[pos * 4 + 0] == 0 && pixels2[pos * 4 + 1] == 0 && pixels2[pos * 4 + 2] == 0)
						{
							pixels2[pos * 4 + 3] = 0;
						}
						else
						{
							pixels2[pos * 4 + 3] = 255;
						}
					}
				}
				stbi_write_png(fname2, width, height, 4, pixels2, width*4);
				if (pixels == pixels2)
				{
					stbi_image_free(pixels);
				}
				else
				{
					stbi_image_free(pixels2);
				}
				_unlink(fname);
			}
#endif
			int h1 = (int)((kml_start_time*R_HOUR));
			int m1 = (int)((kml_start_time - 3600.0*(double)h1)*R_MIN);
			int s1 = kml_start_time - 3600*h1 - 60.0*m1;

			int h2 = (int)((timesum*R_HOUR));
			int m2 = (int)((timesum - 3600.0*(double)h2)*R_MIN);
			int s2 = timesum - 3600*h2 - 60.0*m2;

			sprintf(fname, "%s%sgoogle_earth\\%s_image_%06d", drive, dir, IDname, kml_export);
			FILE* fp = fopen(fname, "w");
			utf8.fprintf(fp,
				"<GroundOverlay>\n"
				"   <name>image</name>\n"
				"   <TimeSpan>\n"
				"       <begin>2012-12-31T%02d:%02d:%02d</begin>\n"
				"       <end>2012-12-31T%02d:%02d:%02d</end>\n"
				"   </TimeSpan>\n"
				"   <Icon>\n"
				"       <href>%s_output_W%06d.png</href>\n"
				"   </Icon>\n"
				"   <LatLonBox>\n"
				"      <north>%f</north>\n"
                "      <south>%f</south>\n"
				"      <east>%f</east>\n"
				"      <west>%f</west>\n"
				"   </LatLonBox>\n"
				"</GroundOverlay>\n",
				h1,m1,s1,
				h2,m2,s2,
				IDname,
				kml_export,
				lat[0]*RAD, lat[2]*RAD, lon[0]*RAD, lon[1]*RAD);
			fclose(fp);

			kml_export++;
			kml_start_time = timesum;


			if (kml_export % 101 == 0)
			{
				char fname[_MAX_FNAME];
				sprintf(fname, "%s%sgoogle_earth\\%s_sim.kml", drive, dir, IDname);
				FILE* fp = fopen(fname, "w");
				if (fp)
				{
					fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
					fprintf(fp, "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n");
					fprintf(fp, "<Folder>\n");
					fprintf(fp, "  <name>Animation</name>\n");

					char fname2[_MAX_FNAME];
					char buf[256];
					for (int i = 1; i < kml_export; i++)
					{
						sprintf(fname2, "%s%sgoogle_earth\\%s_image_%06d", drive, dir, IDname, i);
						FILE* fp2 = fopen(fname2, "r");
						if (fp2)
						{
							while (fgets(buf, 256, fp2) != NULL)
							{
								fprintf(fp, "%s", buf);
							}
							fclose(fp2);
						}
						else
						{
							break;
						}
					}
					fprintf(fp, "  </Folder>\n");
					fprintf(fp, "</kml>\n");
					fclose(fp);
				}
			}
		}

		if ( OutputRunUpImage && solv.RunUp )
		{
			if ( !runup_elv_bitmap_make )
			{
				ElvContourRunUpBitmpa(solv, 40.0, drive, dir, drive2, dir2);
				runup_elv_bitmap_make = 1;
			}

			BitMap bmp3;
			sprintf(fname, "%s%schk\\%s_elevation_contour_%.2f.bmp", drive, dir, IDname, 40.0);
			bmp3.Read(fname);

			const int y = runup_color_level.H()/2;
#pragma omp parallel for OMP_SCHEDULE
			for ( int i = 0; i < ysz; i++ )
			{
				for ( int j = 0; j < xsz; j++ )
				{
					if ( prm->T(i,j) == 0 )
					{
						//if (solv.riverInfo.isRiver_or_Inflow(i, j))
						//{
						//	bmp3.cell(i, j) = bmp2.cell(i, j);
						//	continue;
						//}
						int color_index = solv.RunUpMarker[i*IX+j];
						if ( color_index >= 0 )
						{
							Rgb rgb = runup_color_level.cell(y, color_index);

							const double alp = 0.05;
							double r = (double)bmp3.cell(i,j).r*alp + (double)rgb.r*(1.0-alp);
							double g = (double)bmp3.cell(i,j).g*alp + (double)rgb.g*(1.0-alp);
							double b = (double)bmp3.cell(i,j).b*alp + (double)rgb.b*(1.0-alp);
						
							if ( r > 255.0 ) r = 255.0;
							else if ( r <0.0 ) r = 0.0;
							if ( g > 255.0 ) g = 255.0;
							else if ( g <0.0 ) g = 0.0;
							if ( b > 255.0 ) b = 255.0;
							else if ( b <0.0 ) b = 0.0;
							bmp3.cell(i,j) = Rgb(r, g, b);

						}
					}else
					{
						bmp3.cell(i,j) = Rgb(128, 128, 128);
					}
				}
			}
			//sprintf(fname, "%s%srunup_image\\output_W%06d.bmp", drive, dir, i);
			sprintf(fname, "%s%srunup_image\\%s_浸水域.bmp", drive, dir, IDname);
			bmp3.Write(fname);
			OutputRunUpImage=0;

			//KML用
#pragma omp parallel for OMP_SCHEDULE
			for ( int i = 0; i < ysz; i++ )
			{
				for ( int j = 0; j < xsz; j++ )
				{
					if ( prm->T(i,j) == 0 )
					{
						//if (solv.riverInfo.isRiver_or_Inflow(i, j))
						//{
						//	bmp3.cell(i, j) = bmp2.cell(i, j);
						//	continue;
						//}
						int color_index = solv.RunUpMarker[i*IX+j];
						if ( color_index < 0 )
						{
							bmp3.cell(i,j) = rgbZero;
						}
					}else
					{
						bmp3.cell(i,j) = rgbZero;
					}
				}
			}
			sprintf(fname, "%s%sgoogle_earth\\%s_runup_area.bmp", drive, dir, IDname);
			bmp3.Write(fname);
#ifdef USE_STB_IMAGE
			{
				unsigned char* pixels;
				int width;
				int height;
				int bpp;
				pixels = stbi_load(fname, &width, &height, &bpp, 0);

				char fname2[_MAX_FNAME];
				sprintf(fname2, "%s%sgoogle_earth\\%s_runup_area.png", drive, dir, IDname);
				unsigned char* pixels2 = stbi__convert_format(pixels, bpp, 4, width, height);
				for (int i = 0; i < height; i++)
				{
					for (int j = 0; j < width; j++)
					{
						int pos = (i*width + j);
						if (pixels2[pos * 4 + 0] == 0 && pixels2[pos * 4 + 1] == 0 && pixels2[pos * 4 + 2] == 0)
						{
							pixels2[pos * 4 + 3] = 0;
						}
						else
						{
							pixels2[pos * 4 + 3] = 255;
						}
					}
				}
				stbi_write_png(fname2, width, height, 4, pixels2, width * 4);
				if (pixels == pixels2)
				{
					stbi_image_free(pixels);
				}
				else
				{
					stbi_image_free(pixels2);
				}

				_unlink(fname);
			}
#endif
			sprintf(fname, "%s%sgoogle_earth\\%s_runup_area.kml", drive, dir, IDname);
			FILE* fp = fopen(fname, "w");
			utf8.fprintf(fp, 
				"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
				"<kml xmlns=\"http://earth.google.com/kml/2.2\">\n"
				"<Folder><name>runup_area[%s]</name>\n"
				"<GroundOverlay>\n"
				"   <name>image</name>\n"
				"   <Icon>\n"
				"       <href>%s_runup_area.png</href>\n"
				"   </Icon>\n"
				"   <LatLonBox>\n"
				"      <north>%f</north>\n"
                "      <south>%f</south>\n"
				"      <east>%f</east>\n"
				"      <west>%f</west>\n"
				"   </LatLonBox>\n"
				"</GroundOverlay>\n"
				"  </Folder>\n"
				"</kml>\n",
				IDname, IDname, lat[0] * RAD, lat[2] * RAD, lon[0] * RAD, lon[1] * RAD
			);
			fclose(fp);

		}

		//PovRay用
		//sprintf(fname, "%s%soutput\\output_W%06d.csv", drive, dir, i);
		//bmp2.ReadCsv(fname, min, max2);
		//if ( smooth_flg )
		//{
		//	for ( int k = 0; k < smooth_num; k++ )
		//	{
		//		bmp2.convolve_smooth(solv.Data->topog, smooth);
		//	}
		//}
		//solv.Data->BitmapMaskTopog(bmp2);
		//bmp2.ToGrayScale();
		//sprintf(fname, "%s%spov\\output_W%06d.bmp", drive, dir, number);
		//bmp2.Write(fname);

		static int stormBmpDump = 0;
		if (!stormBmpDump && solv.Storm_p)
		{
			//solv.Storm_p->dumpBmp( drive, dir, IDname,topography_data_mask_bmp,colormap_default);
			//solv.Storm_p->dumpBmpWaterLevelRaising(drive, dir, IDname,topography_data_mask_bmp,colormap_default);
			solv.Storm_p->dumpBmpCenter(drive, dir, IDname,topography_data_mask_bmp,colormap_default);
			stormBmpDump = 1;
		}

		if (0)
		{
			BitMap bmp3;
			bmp3.Copy(topography_data_mask_bmp);
			int index = i;
			sprintf(fname, "%s%schk\\%s_AtmosphericPressure%06d.bmp", drive, dir, IDname, i);

			double pmax, pmin;
			int storm_id;
			double* pres = Storm.Pressure( NULL, prm->iX, prm->jY, pmin, pmax, storm_id);

#pragma omp parallel for OMP_SCHEDULE
			for ( int i = 0; i < ysz; i++ )
			{
				for ( int j = 0; j < xsz; j++ )
				{
					double Pa = pres[i*xsz+j];
					int color = (colormap_default.H()-1)*(Pa-pmin)/(pmax - pmin);
					if ( color < 0 ) color = 0;
					if ( color >colormap_default.H()) color = colormap_default.H()-1;

					color = (colormap_default.H()-1) - color;
					Rgb rgb = colormap_default.cell(color, colormap_default.W()/2);

					const double alp = 0.65;
					double r = (double)bmp3.cell(i,j).r*alp + (double)rgb.r*(1.0-alp);
					double g = (double)bmp3.cell(i,j).g*alp + (double)rgb.g*(1.0-alp);
					double b = (double)bmp3.cell(i,j).b*alp + (double)rgb.b*(1.0-alp);
						
					if ( r > 255.0 ) r = 255.0;
					else if ( r <0.0 ) r = 0.0;
					if ( g > 255.0 ) g = 255.0;
					else if ( g <0.0 ) g = 0.0;
					if ( b > 255.0 ) b = 255.0;
					else if ( b <0.0 ) b = 0.0;
					bmp3.cell(i,j) = Rgb(r, g, b);
				}
			}
			bmp3.Write(fname);
			delete [] pres;
		}
	}


#if 0
	{
		char fname[_MAX_FNAME];	// ファイル名
		BitMap bmp2;
		sprintf(fname, "%s%soutput\\output_M%06d.csv", drive, dir, render_count);
		bmp2.ReadCsv(fname, 0, 100);

		solv.Data->BitmapMask(bmp3);
		bmp2.ToGrayScale();
		bmp2.Reverse();

		int w, h;
		double* z = ReadCsv( fname, w, h);

		if ( colormap )
		{
			//bmp2.ColorLevel(solv.wmin, solv.wmax, z, 0.001, maskcolor);
			bmp2.ColorLevel(0, 100, z, 0.0, NULL);
		}
		//bmp2.convolve_smooth();

		solv.Data->BitmapMask(bmp2, bmp3, z);
		free(z);

		sprintf(fname, "%s%simage\\output_M%06d.bmp", drive, dir, render_count);
			
		//吸収領域を削除
		//if ( absorbingZone ) bmp2.Offset(-absorbingZone-1);

		bmp2.Write(fname);
	}

	{
		char fname[_MAX_FNAME];	// ファイル名
		BitMap bmp2;
		sprintf(fname, "%s%soutput\\output_N%06d.csv", drive, dir, i);
		bmp2.ReadCsv(fname, 0, 100);

		solv.Data->BitmapMask(bmp3);
		bmp2.ToGrayScale();
		bmp2.Reverse();

		int w, h;
		double* z = ReadCsv( fname, w, h);

		if ( colormap )
		{
			//bmp2.ColorLevel(solv.wmin, solv.wmax, z, 0.001, maskcolor);
			bmp2.ColorLevel(0, 100, z, 0.0, NULL);
		}
		//bmp2.convolve_smooth();

		solv.Data->BitmapMask(bmp2, bmp3, z);
		free(z);

		sprintf(fname, "%s%simage\\output_N%06d.bmp", drive, dir, render_count);
			
		//吸収領域を削除
		//if ( absorbingZone ) bmp2.Offset(-absorbingZone-1);

		bmp2.Write(fname);
	}
#endif
}

void ElvContourBitmpa(Solver& solv, double zmin, double zmax, char* drive, char* dir, char* exe_drive, char* exe_dir)
{
	char fname[_MAX_FNAME];	// ファイル名
	BitMap bmp;
	BitMap elv_color_level;

	sprintf(fname, "%s%s..\\colormap\\elv_colormap.bmp", exe_drive, exe_dir);
	//printf("%s\n", fname);

	elv_color_level.Read(fname);

	bmp.Create(solv.iX, solv.jY);
	#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 0; i < solv.jY; i++ )
	{
		for ( int j = 0; j < solv.iX; j++ )
		{
			if ( solv.Data->T(i,j) == 0 )
			{
				int color_index;

				if ( solv.Data->ELV(i,j) > zmax ) color_index = elv_color_level.W()-1;
				else
				{
					color_index = ((solv.Data->ELV(i,j) - zmin)/(zmax-zmin))*elv_color_level.W();
				}
				if ( color_index >= elv_color_level.W() ) color_index = elv_color_level.W()-1;
				if ( color_index < 0.0 ) color_index = 0;
				bmp.cell(i,j) = elv_color_level.cell(elv_color_level.H()/2, color_index);
			}else
			{
				bmp.cell(i,j).r = bmp.cell(i,j).g = bmp.cell(i,j).b = 0;
			}
		}
	}
	sprintf(fname, "%s%schk\\%s_elevation_contour_%.2f.bmp", drive, dir, IDname, zmax);
	printf("標高コンタ(%.2f以下)[%s]\n", zmax, fname);
	bmp.Write(fname);
}

void ElvContourBitmpa1(Solver& solv, double zmin, double zmax, char* drive, char* dir, char* exe_drive, char* exe_dir)
{
	char fname[_MAX_FNAME];	// ファイル名
	BitMap bmp;
	BitMap dpt_color_level;

	sprintf(fname, "%s%s..\\colormap\\depth_colormap .bmp", exe_drive, exe_dir);
	//printf("%s\n", fname);

	dpt_color_level.Read(fname);

	bmp.Create(solv.iX, solv.jY);
	#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 0; i < solv.jY; i++ )
	{
		for ( int j = 0; j < solv.iX; j++ )
		{
			if ( solv.Data->T(i,j) != 0 )
			{
				int color_index;

				color_index = ((solv.Data->H(i,j) - 0.0)/(zmax))*dpt_color_level.W();

				if ( color_index >= dpt_color_level.W() ) color_index = dpt_color_level.W()-1;
				if ( color_index < 0.0 ) color_index = 0;
				bmp.cell(i,j) = dpt_color_level.cell(dpt_color_level.H()/2, color_index);
			}else
			{
				bmp.cell(i,j).r = bmp.cell(i,j).g = bmp.cell(i,j).b = 0;
			}
		}
	}
	sprintf(fname, "%s%schk\\%s_depth_contour_%.2f.bmp", drive, dir, IDname, zmax);
	printf("水深コンタ(%.2f以下)[%s]\n", zmax, fname);
	bmp.Write(fname);
}

void ElvContourBitmpa2(Solver& solv, double zmin, double zmax, char* drive, char* dir, char* exe_drive, char* exe_dir)
{
	char fname[_MAX_FNAME];	// ファイル名
	BitMap bmp;
	BitMap elv_color_level;
	BitMap dpt_color_level;

	printf("depth %f zmax %f\n", zmin, zmax);

	sprintf(fname, "%s%s..\\colormap\\elevation_colormap.bmp", exe_drive, exe_dir);
	//printf("%s\n", fname);

	elv_color_level.Read(fname);

	sprintf(fname, "%s%s..\\colormap\\depth_colormap .bmp", exe_drive, exe_dir);
	//printf("%s\n", fname);

	dpt_color_level.Read(fname);

	bmp.Create(solv.iX, solv.jY);
	const int ysz = solv.jY;
	const int xsz = solv.iX;

#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 0; i < ysz; i++ )
	{
		for ( int j = 0; j < xsz; j++ )
		{
			if ( solv.Data->T(i,j) == 0 )
			{
				int color_index;

				color_index = ((solv.Data->ELV(i,j) - 0.0)/(zmax-0.0))*elv_color_level.W();

				if ( color_index >= elv_color_level.W() ) color_index = elv_color_level.W()-1;
				if ( color_index < 0.0 ) color_index = 0;
				bmp.cell(i,j) = elv_color_level.cell(elv_color_level.H()/2, color_index);
			}else
			{
				int color_index;

				color_index = ((solv.Data->H(i,j) - 0.0)/(zmin))*dpt_color_level.W();

				if ( color_index >= dpt_color_level.W() ) color_index = dpt_color_level.W()-1;
				if ( color_index < 0.0 ) color_index = 0;
				bmp.cell(i,j) = dpt_color_level.cell(dpt_color_level.H()/2, color_index);
			}
		}
	}
	sprintf(fname, "%s%schk\\%s_contour_%.2f.bmp", drive, dir, IDname, zmax);
	printf("水深・標高コンタ(%.2f～%.2f以下)[%s]\n", zmin, zmax, fname);
	bmp.Write(fname);
}

void ElvContourRunUpBitmpa(Solver& solv, double zmax, char* drive, char* dir, char* exe_drive, char* exe_dir)
{
	char fname[_MAX_FNAME];	// ファイル名
	BitMap bmp;
	BitMap elv_color_level;

	sprintf(fname, "%s%s..\\colormap\\runup_elv_colormap.bmp", exe_drive, exe_dir);
	//printf("%s\n", fname);

	elv_color_level.Read(fname);

	const int ysz = solv.jY;
	const int xsz = solv.iX;

	bmp.Create(solv.iX, solv.jY);
	
#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 0; i < ysz; i++ )
	{
		for ( int j = 0; j < xsz; j++ )
		{
			if ( solv.Data->T(i,j) == 0 )
			{
				int color_index;

				if ( solv.Data->ELV(i,j) > zmax ) color_index = elv_color_level.W()-1;
				else
				{
					color_index = ((solv.Data->ELV(i,j))/(zmax))*elv_color_level.W();
				}
				if ( color_index >= elv_color_level.W() ) color_index = elv_color_level.W()-1;
				if ( color_index < 0.0 ) color_index = 0;
				bmp.cell(i,j) = elv_color_level.cell(elv_color_level.H()/2, color_index);
			}else
			{
				bmp.cell(i,j).r = bmp.cell(i,j).g = bmp.cell(i,j).b = 0;
			}
		}
	}
	sprintf(fname, "%s%schk\\%s_elevation_contour_%.2f.bmp", drive, dir, IDname, zmax);
	printf("標高コンタ(%.2f以下)[%s]\n", zmax, fname);
	bmp.Write(fname);
}

void ElvContourBitmpa3(Solver& solv, double zmin, double zmax, char* drive, char* dir, char* exe_drive, char* exe_dir)
{
	char fname[_MAX_FNAME];	// ファイル名
	BitMap bmp;
	BitMap dpt_color_level;

	sprintf(fname, "%s%s..\\colormap\\colormap_default.bmp", exe_drive, exe_dir);
	//printf("%s\n", fname);

	dpt_color_level.Read(fname);

	bmp.Create(solv.iX, solv.jY);
	const int ysz = solv.jY;
	const int xsz = solv.iX;

#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 1; i < ysz-1; i++ )
	{
		for ( int j = 1; j < xsz-1; j++ )
		{
			
			int color_index = 70;

			if ( solv.GAP(i,j) & GAP_1)
			{
				double dd;
				if ( solv.Data->T(i,j) == 0 )
				{
					dd = solv.Data->ELV(i, j);
				}else
				{
					dd = -solv.Data->H(i, j);
				}

				double max = -999999999.0;
				for ( int k = -1; k <= 1; k++ )
				{
					for ( int kk = -1; kk <= 1; kk++ )
					{
						double d;
						if ( solv.Data->T(i+k,j+kk) == 0 )
						{
							d = solv.Data->ELV((i+k), j+kk);
						}else
						{
							d = -solv.Data->H((i+k), j+kk);
						}
						d = fabs(d - dd);
						if ( d > max ) max = d;
					}
				}
				color_index = ((max - 0.0)/(3.0*zmax))*dpt_color_level.W();
			}
#if 0
			if ( color_index >= dpt_color_level.W() ) color_index = dpt_color_level.W()-1;
			if ( color_index < 0.0 ) color_index = 0;
			bmp.cell(i,j) = dpt_color_level.cell(dpt_color_level.H()/2, color_index);
#else
			if ( solv.GAP(i,j) & GAP_4 ) color_index = 0.7*(dpt_color_level.W()-1);
			if ( solv.GAP(i,j) & GAP_1 ) color_index = dpt_color_level.W()-1;
			bmp.cell(i,j) = dpt_color_level.cell(dpt_color_level.H()/2, color_index);
#endif
			if ( solv.Data->T(i,j) == 0 )
			{
				bmp.cell(i,j).r = 128;
				bmp.cell(i,j).g = 128;
				bmp.cell(i,j).b = 128;
				if ( solv.GAP(i,j) & GAP_4 )
				{
					color_index = 0.3*(dpt_color_level.W()-1);
					bmp.cell(i,j) = dpt_color_level.cell(dpt_color_level.H()/2, color_index);
				}
				if ( solv.GAP(i,j) & GAP_1 )
				{
					color_index = 0.5*(dpt_color_level.W()-1);
					bmp.cell(i,j) = dpt_color_level.cell(dpt_color_level.H()/2, color_index);
				}
			}
		}
	}
	sprintf(fname, "%s%schk\\%s_DifferenceLevel_%.2f.bmp", drive, dir, IDname, zmax);
	printf("段差コンタ(%.2f～%.2f以下)[%s]\n", zmin, zmax, fname);
	bmp.Write(fname);
}

void RenderCode(int loopnum, int render_step, char* drive, char* dir)
{
	char fname[512];

	char* rendersrc = new char[100000];
	sprintf(fname, "%s%srender_template.pov", drive, dir);
	FILE* fp = fopen(fname, "r");
	if ( fp == NULL )
	{
		delete [] rendersrc;
		return;
	}

	{
		int c;
		int i = 0;
		while( (c = fgetc(fp)) != EOF )
		{
			rendersrc[i] = c;
			i++;
		}
		rendersrc[i] = '\0';
	}
	fclose(fp);


	printf("=========== Povrayコード生成 START =================\n");
	for ( int i = 0; i < loopnum; i+=render_step )
	{
		sprintf(fname, "%s%spov\\output_W%06d.pov", drive, dir, i);

		FILE* fp = fopen(fname, "w");
		fprintf(fp, rendersrc, i);
		fclose(fp);
		printf("                    \r[%04d/%04d]", i+1, loopnum);
	}
	printf("=========== Povrayコード生成 END =================\n");
	printf("\n");

	printf("=========== レンダリングバッチ生成 START =================\n");
	sprintf(fname, "%s%sRENDER.bat", drive, dir);
	fp = fopen(fname, "w");
	fprintf(fp, "call outputBMPtoPNG.bat %s\n", "water_depth_data");
	for ( int i = 0; i < loopnum; i+=render_step )
	{
		sprintf(fname, "output_W%06d", i);
		fprintf(fp, "call outputBMPtoPNG.bat %s\n", fname);
	}
	for ( int i = 0; i < loopnum; i+=render_step )
	{
		sprintf(fname, "output_W%06d", i);
		fprintf(fp, "call povray.bat %s\n", fname);
		printf("                    \r[%04d/%04d]", i+1, loopnum);
	}
	fprintf(fp, "call cnv_move.bat\n");
	fclose(fp);
	printf("==> RENDER.bat 生成\n");
	printf("=========== レンダリングバッチ生成 END =================\n");
	printf("\n");
	delete [] rendersrc;
}



void outputCrustalMovement(Solver& solv, AbeNoguera& grid, double* www, int timesum, double scale, double depth_scale, int& crustalMovement_count, char* exe_drive, char* exe_dir, char* drive, char* dir, int latitude_flag, double* latitude, int longitude_flag, double* longitude)
{
	char fname[512];
	{
		int h = (int)((timesum*R_HOUR));
		int m = (int)((timesum - 3600.0*(double)h)*R_MIN);
		int s = timesum - 3600*h - 60.0*m;
	
		sprintf(fname, "%s%sTerrain\\time%06d.txt", drive, dir, crustalMovement_count);
		FILE* fp = fopen(fname, "w");
		fprintf(fp, "%02d:%02d:%02d", h, m, s);
		fclose(fp);
	}

	//地形と海底を接続した形状を出力
	double hmin = -10000.0;
	double hmax = 10000.0;

	BitMap depthColor;
	sprintf(fname, "%s%s..\\colormap\\%s", exe_drive, exe_dir, "depth_colormap_gray.bmp");
	depthColor.Read(fname);

	BitMap elevationColor;
	sprintf(fname, "%s%s..\\colormap\\%s", exe_drive, exe_dir, "elevation_colormap_gray.bmp");
	elevationColor.Read(fname);

	sprintf(fname, "%s%sTerrain\\output_%06d.obj", drive, dir, crustalMovement_count);
	crustalMovement_count++;

	if ( CrustalMovementData == 0)
	{
		CrustalMovementData = new double[grid.iX*grid.jY];
		CrustalMovementDiff = new double[grid.iX*grid.jY];
		memset(CrustalMovementDiff, '\0', sizeof(double)*grid.iX*grid.jY);

#pragma omp parallel for OMP_SCHEDULE
		for ( int i = 0; i < grid.jY; i++ )
		{
			for ( int j = 0; j < grid.iX; j++ )
			{
				CrustalMovementData[i*grid.iX+j] = -grid.h[i*grid.iX+j];
				if ( grid.topog[i*grid.iX+j] == 0 )
				{
					CrustalMovementData[i*grid.iX+j] = grid.elevation[i*grid.iX+j];
				}
				CrustalMovementData[i*grid.iX+j] -= solv.Data->W(i,j);
			}
		}
	}else
	{
#pragma omp parallel for OMP_SCHEDULE
		for ( int i = 0; i < grid.jY; i++ )
		{
			for ( int j = 0; j < grid.iX; j++ )
			{
				CrustalMovementData[i*grid.iX+j] += www[i*grid.iX+j]*scale;
				CrustalMovementDiff[i*grid.iX+j] += www[i*grid.iX+j];
			}
		}
	}



	CsvToObj obj3(CrustalMovementData, grid.iX, grid.jY, 0, 0);
	obj3.dx = solv.dx;
	obj3.dy = solv.dy;
	obj3.r_dx = solv.Data->r_dx;
	//丸みを付ける
	if ( latitude_flag ) obj3.latitude = latitude;
	if ( longitude_flag ) obj3.longitude = longitude;
	obj3.zscale =depth_scale;
	obj3.Color(109,87,56);
		
	VertexColor* vertexcolor = new VertexColor[grid.iX*grid.jY];	
	obj3.vertex_color = vertexcolor;

#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 0; i < grid.jY; i++ )
	{
		for ( int j = 0; j < grid.iX; j++ )
		{
			if ( grid.topog[i*grid.iX+j] != 0 )
			{
				double h = (-obj3.Vertex(i,j) - hmin)/(hmax - hmin);
				if ( h < 0.0 ) h = 0.0;
				if ( h > 1.0 ) h = 1.0;

				int color_index = (int)(h*(double)depthColor.W()-1);
				if ( color_index < 0 ) color_index = 0;
				if ( color_index >= depthColor.W() )
				{
					obj3.vertexColor(i,j).r = 255;
					obj3.vertexColor(i,j).g = 255;
					obj3.vertexColor(i,j).b = 255;
				}else
				{
					obj3.vertexColor(i,j).r = depthColor.cell(depthColor.H()/2, color_index).r;
					obj3.vertexColor(i,j).g = depthColor.cell(depthColor.H()/2, color_index).g;
					obj3.vertexColor(i,j).b = depthColor.cell(depthColor.H()/2, color_index).b;
				}
			}else
			{
				double h = (obj3.Vertex(i,j) - hmin)/(hmax - hmin);
				if ( h < 0.0 ) h = 0.0;
				if ( h > 1.0 ) h = 1.0;

				int color_index = (int)(h*(double)elevationColor.W()-1);
				if ( color_index < 0 ) color_index = 0;
				if ( color_index >= elevationColor.W() )
				{
					obj3.vertexColor(i,j).r = 255;
					obj3.vertexColor(i,j).g = 255;
					obj3.vertexColor(i,j).b = 255;
				}else
				{
					obj3.vertexColor(i,j).r = elevationColor.cell(elevationColor.H()/2, color_index).r;
					obj3.vertexColor(i,j).g = elevationColor.cell(elevationColor.H()/2, color_index).g;
					obj3.vertexColor(i,j).b = elevationColor.cell(elevationColor.H()/2, color_index).b;
				}
			}
#if 0
			if ( www )
			{
				double a = 0.99;
				double h = (CrustalMovementDiff[i*grid.iX+j] - (-3.5))/(3.5 - (-3.5));
				if ( h < 0.0 ) h = 0.0;
				if ( h > 1.0 ) h = 1.0;
				
				double b = exp(-300.0*pow(CrustalMovementDiff[i*grid.iX+j],2.0));
				//double b = 0.0;
				
				int color_index = (int)(h*(double)colormap_default.W()-1);
				if ( color_index < 0 ) color_index = 0;
				obj3.vertexColor(i,j).r = b*a*obj3.vertexColor(i,j).r+(1.0-b*a)*colormap_default.cell(colormap_default.H()/2, color_index).r;
				obj3.vertexColor(i,j).g = b*a*obj3.vertexColor(i,j).g+(1.0-b*a)*colormap_default.cell(colormap_default.H()/2, color_index).g;
				obj3.vertexColor(i,j).b = b*a*obj3.vertexColor(i,j).b+(1.0-b*a)*colormap_default.cell(colormap_default.H()/2, color_index).b;
				//obj3.vertexColor(i,j).id = color_index;
			}
#endif
		}
	}
	obj3.Output(fname, 0);
	delete [] vertexcolor;
}


void MNVector(Solver* solver, BitMap& bmp, double* latitude, double* longitude, int pitch=8)
{
	AbeNoguera* prm = ((Solver*)solver)->Data;

	float wmax = -9999999.0f;
	const int xsz = prm->iX;
	const int ysz = prm->jY;
	const int IX = prm->iX;
	const int JY = prm->jY;

	//const double dlo = (longitude[1] - longitude[0]) / (double)(bmp.W() - 1);
	//const double dla = (latitude[0] - latitude[2]) / (double)(bmp.H() - 1);

	//const float maxNM = 10.0f;				//ベクトル最大ピクセル長をMAXとする最大風速
	const float vector_length_max = 10;	//ベクトル最大ピクセル長

	//for (int i = 0; i < ysz; i++)
	//{
	//	for (int j = 0; j < xsz; j++)
	//	{
	//		if (maxNM < fabs(prm->M(i, j))) maxNM = prm->M(i, j);
	//		if (maxNM < fabs(prm->N(i, j))) maxNM = prm->N(i, j);
	//	}
	//}

	BitMap copyBmp;
	copyBmp.Copy(bmp);
	{
		int step = pitch;
		if (step < 0) step = 8;

#pragma omp parallel for OMP_SCHEDULE
		for (int i = solver->absorbingZone + step; i < ysz - solver->absorbingZone; i += step)
		{
			for (int j = solver->absorbingZone + step; j < xsz - solver->absorbingZone; j += step)
			{
				//double lon = j*dlo + longitude[0];
				//double lat = i*dla + latitude[2];

				//ベクトルの計算
				float ww = sqrt(prm->M(i, j)*prm->M(i, j) + prm->N(i, j)*prm->N(i, j));
				if (ww < 0.01f)
				{
					continue;
				}

				//風ベクトルの単位化
				float xx = prm->M(i, j) / ww;
				float yy = prm->N(i, j) / ww;

				//ベクトルのピクセル長
				float length = vector_length_max;
				//float length = vector_length_max*ww / maxNM;
				//if (length > vector_length_max) length = vector_length_max;

				float dt = 1.0 / length;
				float t = 0.0f;

				while ( /*length > 16.0f &&*/ t <= length)
				{
					int x = (int)(j + xx*t);
					int y = (int)(i + yy*t);
					if (x < solver->absorbingZone || x > IX - solver->absorbingZone || y < solver->absorbingZone || y > JY - solver->absorbingZone)
					{
						t += dt;
						continue;
					}
					bmp.cell(y, x).r = 255;
					bmp.cell(y, x).g = 255;
					bmp.cell(y, x).b = 0;

					float alp = 0.8*t / length;
					float r = (float)copyBmp.cell(y, x).r*alp + (float)bmp.cell(i, j).r*(1.0 - alp);
					float g = (float)copyBmp.cell(y, x).g*alp + (float)bmp.cell(i, j).g*(1.0 - alp);
					float b = (float)copyBmp.cell(y, x).b*alp + (float)bmp.cell(i, j).b*(1.0 - alp);
					if (r < 0) r = 0.0f;
					if (g < 0) g = 0.0f;
					if (b < 0) b = 0.0f;
					if (r > 255) r = 255.0f;
					if (g > 255) g = 255.0f;
					if (b > 255) b = 0;
					bmp.cell(y, x).r = r;
					bmp.cell(y, x).g = g;
					bmp.cell(y, x).b = b;
					t += dt;
				}
			}
		}
	}
}

void ElvContourBitmpaTmp(Solver& solv, int kk, int* flag, char* drive, char* dir, char* exe_drive, char* exe_dir)
{
	char fname[_MAX_FNAME];	// ファイル名
	BitMap bmp;

	bmp.Create(solv.iX, solv.jY);
#pragma omp parallel for OMP_SCHEDULE
	for (int i = 0; i < solv.jY; i++)
	{
		for (int j = 0; j < solv.iX; j++)
		{
			bmp.cell(i, j).r = bmp.cell(i, j).g = bmp.cell(i, j).b = 128;
			if (flag[i*solv.iX + j] == 1) bmp.cell(i, j) = Rgb(255, 0, 0);
			if (flag[i*solv.iX + j] == 2) bmp.cell(i, j) = Rgb(0, 255, 0);
			if (flag[i*solv.iX + j] == 3) bmp.cell(i, j) = Rgb(0, 0, 255);
			if (flag[i*solv.iX + j] == 4) bmp.cell(i, j) = Rgb(255, 255, 255);
		}
	}
	sprintf(fname, "%s%schk\\%s_flag_%d.bmp", drive, dir, IDname, kk);
	bmp.Write(fname);
}


