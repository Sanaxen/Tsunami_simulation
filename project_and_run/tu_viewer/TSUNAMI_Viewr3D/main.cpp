//コンソールを出さない
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#include "GL/glew.h"
#ifdef _WIN64
#pragma comment(lib, "libs\\64bit\\glew32.lib")
#else
#pragma comment(lib, "libs\\32bit\\glew32.lib")
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string>
#include <vector>

#include "offscreen.h"
#if defined(WIN32)
#  include "GL/freeglut.h"
#  include "GL/glext.h"
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/glut.h>
#endif

#include "glsl.h"

#define USE_GLUI	10

#if USE_GLUI
#include <GL/glui.h>
#endif

#include "vob.hpp"
#include "ObjLoader.h"
#include "glslobj.hpp"

#include "Font.h"

#include "bitmaptexture.h"

int resizeFlg = 0;
int TerrainMode = 0;
char DataDir[512];
char IDname[256];

#define USE_VBO
#ifdef USE_VBO
#define VBO1	1
#define VBO2	1
#else
#define VBO1	0
#define VBO2	0
#endif

//#define GLS_SHADER

char colormapName[128];
int no_vbo = 0;

wchar_t colormapMinMax[2][32];
int colormapDefine = 0;

int ViewPort[4];
GLubyte* AllocReadPixelsImageBuffer(size_t size);
void FreeReadPixelsImageBuffer();
int SavePixelsImage(char* fileName, int viewport[4], int offscreen_capture);
BOOL IsFileAlreadyOpen(char *filename);

#define ORTHO_VIEW	10
int Perspective = 0;

GLSLObj *glsl;
VOBobject *vob;
Obj *data;
float zoom = 0.85f;
float shiftX = 0.0f, shiftY = 0.0f;
int mouseX = 0, mouseY = 0;
int width, height;
bool animation = false;

char* ObjData = NULL;
char* ShaderCode = NULL;

int capture = -1;
int alphablend = 0;
int waterdepth = 1;
int wave = 1;
int earth = 1;
int end_exit_number = -1;
int axis = 1;
int windvec = 1;
int colorbar = 1;
int scenario=0;

int   main_window;
float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float view_rotate2[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float view_rotate3[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float obj_pos[] = { 0.0, 0.0, 0.0 };

#if USE_GLUI
GLUI *glui, *glui2;
float sphere_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float torus_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

GLUI_Spinner *spinner;
GLUI_Spinner *spinner1;
GLUI_Spinner *spinner2;
//GLUI_Translation *trans_water_depth;
//GLUI_Translation *trans_wave;

GLUI_Spinner* trans_water_depth_spinner;
GLUI_Spinner* trans_wave_spinner;

GLUI_Rotation *view_rot2;
GLUI_Rotation *view_rot3;

#endif

int offscreen_capture = 1;
int glui_area_hide = 0;

BitmapFont g_Font1;	// 日本語フォント表示用.
BitmapFont g_Font2;	// 日本語フォント表示用.
BitmapFont g_Font3;	// 日本語フォント表示用.
BitmapFont g_Font4;	// 日本語フォント表示用.
char font2FileName[512];

BitMapTexture* Texture1;

/*
** 光源
*/
static const GLfloat lightpos1[] = { 0.0, 15.0, -10.0, 1.0 }; /* 位置　　　 */
static const GLfloat lightpos2[] = { 0.0, 0.0, 15.0, 1.0 }; /* 位置　　　 */
static const GLfloat lightcol[] = { 1.0, 1.0, 1.0, 1.0 }; /* 直接光強度 */
static const GLfloat lightamb[] = { 0.5, 0.5, 0.5, 0.6 }; /* 環境光強度 */

/* トラックボール処理用関数の宣言 */
#include "trackball.h"

static float offset1 = 0.0;
static float offset2 = 0.0;


void draw_axes( float scale )
{
  if ( !axis ) return;
  glDisable( GL_LIGHTING );

  glPushMatrix();
  glScalef( scale, scale, scale );

  glBegin( GL_LINES );
 
  glColor3f( 1.0, 0.0, 0.0 );
  glVertex3f( .8f, 0.05f, 0.0 );  glVertex3f( 1.0, 0.25f, 0.0 ); /* Letter X*/
  glVertex3f( 0.8f, .25f, 0.0 );  glVertex3f( 1.0, 0.05f, 0.0 );
  glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 1.0, 0.0, 0.0 ); /* X axis */

  glColor3f( 0.0, 1.0, 0.0 );
  glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 1.0, 0.0 ); /* Y axis */

  glColor3f( 0.0, 0.0, 1.0 );
  glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 0.0, 1.0 ); /* Z axis  */
  glEnd();

  glPopMatrix();

  glEnable( GL_LIGHTING );
}

typedef struct windVector_
{
	float s[3];
	float e[3];
} windVector;

std::vector<windVector> windVectorList;

Obj* elevation_data = 0;
Obj* water_depth_data = 0;
Obj* earth_data = 0;
Obj* output = 0;
int cur_counter = -1;
int counter = 0;
int counter_step = 0;

int light1 = 0;
int light2 = 1;

extern int colorskipp;
extern double ZScale;
double _ZScale = 1.0;
extern int wave_height;

void draw_vector( float scale )
{
 
	const int sz = windVectorList.size();
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_COLOR_MATERIAL);
	for ( int i = 0; i < sz; i++ )
	{
		glPointSize(3.0f);
		glBegin(GL_POINTS);
		glColor4f( 1.0, 0.8, 0.9, 0.7f );
		glVertex3fv(windVectorList[i].s);
		glEnd();

		glBegin( GL_LINES );
		glColor4f( 1.0, 0.8, 0.9, 0.7f );
		glVertex3fv(windVectorList[i].s);
		glColor4f( 1.0, 1.0, 1.0, 0.1f );
		glVertex3fv(windVectorList[i].e);
		glEnd();
	}
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
}

static char* setlocale_retval = 0;

void Render2D()
{
	char text[280];
	wchar_t wtext[280];

	char copylighttext[280];
	wchar_t copylightwtext[280];

	char fname[256];

	if (!setlocale_retval)
	{
		setlocale_retval = setlocale(LC_CTYPE, "jpn");
	}

	int t = cur_counter;
	if ( t < 0 ) t = 0;
	sprintf(fname, "%s\\output\\time%06d.txt", DataDir, t);
	if ( TerrainMode )
	{
		sprintf(fname, "%s\\Terrain\\time%06d.txt", DataDir, t);
	}
	FILE* fp = fopen(fname, "r");
	if ( fp )
	{
		fgets(text, 256, fp);
		//printf("text %s\n", text);
		char* p = strchr(text, '\n');
		if ( p ) *p = '\0';

		int len = strlen(text);
		len = mbstowcs(wtext, text, len+1);

		char tmp[80];
		//fgets(tmp, 256, fp);
		//printf("tmp %s\n", text);
		if (fgets(copylighttext, 256, fp) != NULL)
		{
			//printf("copylighttext %s\n", copylighttext);
			char* p = strchr(copylighttext, '\n');
			if (p) *p = '\0';

			int len = strlen(copylighttext);
			len = mbstowcs(copylightwtext, copylighttext, len + 1);
		}
		fclose(fp);
	}

	if ( resizeFlg )
	{
		::glGetIntegerv( GL_VIEWPORT, ViewPort);   // get location and size of viewport
	}
	const unsigned int width = ViewPort[2];   // viewport width
	const unsigned int height = ViewPort[3];  //  viewport height

	//　3D　→　2D
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);
	//glOrtho(0.0, width, height, 0.0, 1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	//
	// 文字列描画.
	//
	glEnable(GL_COLOR_MATERIAL);
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	glRasterPos2i(3, 3);
	g_Font1.DrawStringW(L" ");	//dummy
	g_Font2.DrawStringW(L" ");	//dummy
	g_Font3.DrawStringW(L" ");	//dummy

	//glRasterPos2i(30, 30);
	//g_Font.DrawStringW(L"日本語表示サンプルプログラム");
	//glRasterPos2i(30, 55);
	//g_Font.DrawStringW(L"OpenGLのウィンドウ上に日本語を表示しています");
	glRasterPos2i(30, 33);
	//g_Font.DrawStringW(L"現在の時間:%s", wtext);
	g_Font1.DrawStringW(L"Time:");
	
	glRasterPos2i(95, 33);
	{
		char* time_str = NULL;
		char* zone = strstr( text, "JST ");
		if ( zone == NULL) zone = strstr(text, "UTC ");
		if ( zone )
		{
			zone[3] = '\0';
			time_str = text+4;
		}else
		{
			time_str = text;
		}
		if ( zone )
		{
			size_t newsize = strlen(zone) + 1;
			wchar_t * wcstring = new wchar_t[newsize];
			size_t convertedChars = 0;
			mbstowcs_s(&convertedChars, wcstring, newsize, zone, _TRUNCATE);

			g_Font2.DrawStringW( wcstring);
			glRasterPos2i(95+40, 33);
			g_Font2.DrawStringA( time_str);
			delete [] wcstring;
		}else
		{
			g_Font2.DrawStringA( text);
		}
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glRasterPos2i(95+40+250, 33);
	g_Font4.DrawStringW(copylightwtext);
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

	if (cur_counter >= 0 && colorbar)
	{
		glRasterPos2i(10, height-Texture1->sizeY-10);
		if ( TerrainMode == 0 )
		{
			if ( !colormapDefine)
			{
				g_Font3.DrawStringW( L"0.0");
			}else
			{
				g_Font3.DrawStringW( colormapMinMax[0] );
			}
		}
		else
		{
			if ( !colormapDefine)
			{
				g_Font3.DrawStringW( L"-3.5");
			}else
			{
				g_Font3.DrawStringW( colormapMinMax[0] );
			}
		}

		glRasterPos2i(Texture1->sizeX, height-Texture1->sizeY-10);
		if ( TerrainMode == 0 )
		{
			if ( !colormapDefine)
			{
				g_Font3.DrawStringW( L"4.0");
			}else
			{
				g_Font3.DrawStringW( colormapMinMax[1] );
			}
		}
		else
		{
			if ( !colormapDefine)
			{
				g_Font3.DrawStringW( L"3.5");
			}else
			{
				g_Font3.DrawStringW( colormapMinMax[1] );
			}
		}
		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);//テクスチャ有効
		glBindTexture(GL_TEXTURE_2D, Texture1->texture);
		Texture1->TexSet();

		//glEnable(GL_ALPHA_TEST);//アルファテスト開始
		glBegin(GL_QUADS);
		 glTexCoord2f(0.0f, 0.0f); glVertex2d(10 , height);//左下
		 glTexCoord2f(0.0f, 1.0f); glVertex2d(10 ,  height-Texture1->sizeY);//左上
		 glTexCoord2f(1.0f, 1.0f); glVertex2d(10+Texture1->sizeX ,  height-Texture1->sizeY);//右上
		 glTexCoord2f(1.0f, 0.0f); glVertex2d(10+Texture1->sizeX , height);//右下
		 glEnd();
		//glDisable(GL_ALPHA_TEST);//アルファテスト終了
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);//テクスチャ無効
	}
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

	//　2D　→　3D
	glPopMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
}


/*
** 初期化
*/
static void init(void)
{
	char thisModuleFileName[_MAX_PATH];
	//実行中のプロセスのフルパス名を取得する
	GetModuleFileNameA(NULL, thisModuleFileName, sizeof(thisModuleFileName));

	char exe_drive[_MAX_DRIVE];	// ドライブ名
	char exe_dir[_MAX_DIR];		// ディレクトリ名
	char exe_fname[_MAX_FNAME];	// ファイル名
	char exe_ext[_MAX_EXT];		// 拡張子

	_splitpath( thisModuleFileName, exe_drive, exe_dir, exe_fname, exe_ext );

	char texname[512];
	sprintf(texname, "%s%s..\\colormap\\%s", exe_drive,exe_dir, colormapName);
	if ( TerrainMode )
	{
		sprintf(texname, "%s%s..\\colormap\\colormap_default.bmp", exe_drive,exe_dir);
	}
	Texture1 = new BitMapTexture(texname);
	
	g_Font1.CreateW(L"ＭＳ ゴシック", 24);
	g_Font4.CreateW(L"ＭＳ ゴシック", 10);

	{
		sprintf(font2FileName, "%s%s..\\Font\\7barSPBd.ttf", exe_drive,exe_dir);
		FILE* fp = fopen(font2FileName, "r");
		if ( fp == NULL ) printf("font NULL\n");
		else fclose(fp);
	}
	//printf("%d\n", AddFontResourceEx("c:\\7barSPBd.ttf", FR_PRIVATE, NULL));
	//RemoveFontResource(font2FileName);exit(0);

	printf("%d\n", AddFontResource(font2FileName));
	g_Font2.CreateA("7barSPBd", 24);
	g_Font3.CreateW(L"HGP創英角ｺﾞｼｯｸUB", 16);

#ifdef GLS_SHADER
	glsl = new GLSLObj;		//シェーダー環境作成
	glsl->LoadShader("c:\\tmp\\refract");
	glsl->CompileShader();
	glsl->AttachShader();
	glsl->LinkProgram();
#endif

	vob = new VOBobject;	//VOB環境作成

  	/* 形状データオブジェクトの作成 */
	if ( elevation_data == NULL )
	{
		char fname[512];
		sprintf(fname, "%s\\obj\\elevation_data.obj", DataDir);
		if ( TerrainMode )
		{
			sprintf(fname, "%s\\Terrain\\output_000000.obj", DataDir);
		}
		elevation_data = new Obj(fname, (no_vbo ? 0:VBO1), 0.0f);
	}
	Obj::minmaxFix = 1;
	if ( water_depth_data == NULL && TerrainMode == 0)
	{
		char fname[512];
		colorskipp = 1;
		sprintf(fname, "%s\\obj\\water_depth_data.obj", DataDir);
		water_depth_data = new Obj(fname, (no_vbo ? 0:VBO1), 0.0f);
		colorskipp = 0;
	}
#if 0
	if ( earth_data == NULL )
	{
		char fname[512];
		int rgb[3] = {3,3,172};
		sprintf(fname, "%s\\obj\\Earth.obj", DataDir);
		earth_data = new Obj(fname, 0, 0.0f, 0.5);
	}
#else
	char fname[512];
	sprintf(fname, "%s\\obj\\Earth.obj", DataDir);
	FILE* fp = fopen(fname, "r");
	if ( fp )
	{
		fclose(fp);
		earth_data = new Obj;
	}
#endif


	/* 初期設定 */
	glClearColor(0.00, 0.00, 0.00, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* 光源の初期設定 */
	//if (!ShaderCode )
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcol);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightcol);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);

		glEnable(GL_LIGHT1);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lightcol);
		glLightfv(GL_LIGHT1, GL_SPECULAR, lightcol);
		glLightfv(GL_LIGHT1, GL_AMBIENT, lightamb);
		//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	}

	if ( TerrainMode == 0 ) elevation_data->draw();
	if ( TerrainMode == 0 ) water_depth_data->draw();

	if ( earth && earth_data)
	{
#if 0
		earth_data->drawOutLines();
#else
		earth_data->drawEarth();
#endif
	}
	draw_axes(.52f);
}

/*
** シーンの描画
*/
static void scene(void)
{
	static const GLfloat diffuse[] = { 0.6, 0.6, 0.9, 0.2 };
	static const GLfloat specular1[] = { 0.05, 0.04, 0.04, 0.0 };
	static const GLfloat specular2[] = { 0.4, 0.4, 0.4, 0.0 };
	static const GLfloat specular3[] = { 0.4, 0.4, 0.4, 0.0 };
 
	///* 材質の設定 */
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular1);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0f);
//*	

	if ( TerrainMode == 0) elevation_data->draw();
	if ( waterdepth && TerrainMode == 0)
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular2);
		water_depth_data->draw();
	}
	if ( output && wave )
	{
		static const GLfloat diffuse[] = { 0.6, 0.6, 0.9, 0.2 };
		static const GLfloat specular[] = { 0.85, 0.85, 0.85, 0.0 };

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 110.0f);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
#ifdef GLS_SHADER
		glsl->UseProgram();
#endif	
		if ( alphablend )glEnable(GL_BLEND);
		output->draw();
		if ( alphablend )glDisable(GL_BLEND);
		
#ifdef GLS_SHADER
		glsl->UseProgramOff();
#endif
	}
	if ( windvec ) draw_vector(1.0f);

#if 10
	if ( earth  && earth_data)
	{
		static const GLfloat diffuse[] = { 0.6, 0.6, 0.9, 0.2 };
		static const GLfloat specular[] = { 0.85, 0.85, 0.85, 0.0 };

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 110.0f);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
#if 0
		earth_data->drawOutLines();
#else
		earth_data->drawEarth();
#endif		
		glDisable(GL_BLEND);
	}
#endif
//*/
	draw_axes(.52f);
}

void display();

void SaveImage_PPM(char* fname)
{ // save current screen buffe image to the ppm file
  int viewport[4];
  ::glGetIntegerv( GL_VIEWPORT, viewport);   // get location and size of viewport
  const unsigned int width = viewport[2];   // viewport width
  const unsigned int height = viewport[3];  //  viewport height
  
  void* image = malloc(3*width* height);  // allocate buffer (rgb)*width*height

#if 10
  glReadBuffer(GL_FRONT);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3],  GL_RGB, GL_UNSIGNED_BYTE, image);
#else
  ::glPixelStorei(GL_PACK_ALIGNMENT, 1);
  ::glReadPixels(0, 0, width,height, GL_RGB, GL_UNSIGNED_BYTE, image);   // get image
#endif
  // write to file
  FILE* fp = fopen(fname, "w");
  if ( fp )
  {
	  fprintf(fp, "P3\n");   // ascii format
	  fprintf(fp, "%d %d\n", width, height);
	  fprintf(fp, "255\n");  // range of the rgb data
	  char* img = (char*)image;  
	  for(unsigned int ih=0;ih<height;ih++)
	  {    
		  for(unsigned int iw=0;iw<width;iw++)
		  {    
			unsigned int i = (height-1-ih)*width+iw;
			int r = (unsigned char)img[i*3+0];
			int g = (unsigned char)img[i*3+1];
			int b = (unsigned char)img[i*3+2];
			fprintf(fp, "%d %d %d\n", r, g, b);
		  }
	  }
	  fclose(fp);
  }
  if ( image ) free( image );
}

void GetCurrentViewPort()
{
	//if (!offscreen_capture)
	//{
	//	GLUI_Master.auto_set_viewport();
	//}
	glutSetWindow(main_window);
	glutPostRedisplay();

	//if ( resizeFlg == 0 ) return;
	/* 現在のビューポートのサイズを得る */
	GLint view[4];
	glGetIntegerv(GL_VIEWPORT, ViewPort);
	printf("ViewPort %d %d %d %d\n", ViewPort[0], ViewPort[1], ViewPort[2], ViewPort[3]);
	resizeFlg = 0;
}

void load2();
static void resize(int w, int h);
void Capture()
{
	resize(width, height);
#if USE_GLUI
	//glui2->hide();
#endif
	char fname[512];
	GetCurrentViewPort();

	OffScreenRender* offscrennRender = 0;
	if ( offscreen_capture )
	{
		offscrennRender = new OffScreenRender(display, ViewPort);
	}
	printf("CaptureViewPort %d %d %d %d\n", ViewPort[0], ViewPort[1], ViewPort[2], ViewPort[3]);

	printf("Capture...\n");
	sprintf(fname, "%s\\image3D\\output_%06d.bmp", DataDir,cur_counter);
	SavePixelsImage(fname, ViewPort, offscreen_capture);

	//sprintf(fname, "%s\\image3D\\output_%06d.ppm", DataDir,cur_counter);
	//SaveImage_PPM(fname);

	if ( offscreen_capture )
	{
		delete offscrennRender;
	}
#if USE_GLUI
	//if ( glui_area_hide == 0 )glui2->show();
#endif
}

/****************************
** GLUT のコールバック関数 **
****************************/

static void resize(int w, int h);
static void display(void)
{
	resize(width, height);
	glLoadIdentity();
 

	/* 光源の位置を設定 */
	//if (!ShaderCode )
	{
		if ( light1 )
		{
			glEnable(GL_LIGHT0);
			glMultMatrixf( view_rotate2 );

			glLightfv(GL_LIGHT0, GL_POSITION, lightpos1);
		}else
		{
			glDisable(GL_LIGHT0);
		}
		if ( light2 )
		{
			glEnable(GL_LIGHT1);
			glMultMatrixf( view_rotate3 );
			glLightfv(GL_LIGHT1, GL_POSITION, lightpos2);
		}else
		{
			glDisable(GL_LIGHT1);
		}
	}
	glLoadIdentity();

//#if !ORTHO_VIEW
	if ( Perspective )
	{
		/* 視点の移動（物体の方を奥に移動）*/
		glTranslatef(shiftX, shiftY, zoom);
		//printf("zoom %f\n", zoom);
	}
//#endif

#if USE_GLUI
	glTranslatef( obj_pos[0], obj_pos[1], 0 ); 
	glMultMatrixf( view_rotate );
#endif

	/* トラックボール処理による回転 */
	glMultMatrixd(trackballRotation());
  
	/* 画面クリア */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//render_string2D(100,500, "AAAAAAAAAAAA");


	/* シーンの描画 */
	scene();
	glPushMatrix();

	Render2D();
	glPopMatrix();


	/* ダブルバッファリング */
	glutSwapBuffers();
}

static void resize(int w, int h)
{ 
	static float zoom_pre = -10000000.0;
	if ( w == width && h == height && zoom == zoom_pre) return;
	printf("resize()\n");
	zoom_pre = zoom;
	resizeFlg = 1;
    width = w;
	height = h;
	/* トラックボールする範囲 */
	trackballRegion(w, h);
  
	/* ウィンドウ全体をビューポートにする */
	glViewport(0, 0, w, h);
  
	/* 透視変換行列の指定 */
	glMatrixMode(GL_PROJECTION);
  
	/* 透視変換行列の初期化 */
	glLoadIdentity();

//#if !ORTHO_VIEW
	if ( Perspective )
	{
		gluPerspective(48.0, (double)w / (double)h, 0.01, 50.0);
	}else
//#else
	{
	//  float zoom = 1.0;
	if (w <= h) 
		glOrtho(-2.0/zoom, 2.0/zoom,
		-2.0 * (GLfloat) h / (GLfloat) w /zoom, 
		2.0 * (GLfloat) h / (GLfloat) w /zoom,
		-10.0, 10.0); 
	else 
		glOrtho(-2.0 * (GLfloat) w / (GLfloat) h / zoom, 
		2.0 * (GLfloat) w / (GLfloat) h /zoom,
		-2.0/zoom, 2.0/zoom, -10.0, 10.0);
	}
//#endif
	glMatrixMode(GL_MODELVIEW); /* モデルビュー変換行列の初期化 */
	GetCurrentViewPort();
	printf("resize()\n");
}


static void mouse(int button, int state, int x, int y)
{
  switch (button) {
  case GLUT_LEFT_BUTTON:
    switch (state) {
    case GLUT_DOWN:
		mouseX = x;
		mouseY = y;

      /* トラックボール開始 */
      trackballStart(x, y);
      break;
    case GLUT_UP:
      /* トラックボール停止 */
      trackballStop(x, y);
      break;
    default:
      break;
    }
    break;
  case GLUT_MIDDLE_BUTTON:

	resize(width, height);
	display();
	break;

    default:
      break;
  }
}

static void MouseWheel(int wheel_number, int direction, int x, int y)
{
	  //trackballStop(x, y);
	  if ( direction == 1 ) zoom += 0.04f;
	  else zoom -= 0.04f;
    if(zoom > 20.0f) zoom = 20.0f;
    else if(zoom < 0.005f) zoom = 0.005f;
	resize(width, height);
	display();
}

static void motion(int x, int y)
{
  /* トラックボール移動 */
  trackballMotion(x, y);
}

int aaaaaa = 0;
void timer(int t)
{
	printf("timer event !!(%d)\n", counter);
	aaaaaa = 1;
}

int freamNum = 10;
int freamCnt = 0;

std::vector<std::string> ogjFiles;
char* envDELETE_DRAWING_FILES = 0;

void refresh()
{
  /* 画面の描き替え */
	glutSetWindow(main_window);
	glutPostRedisplay();

	if ( counter == cur_counter ) return;

	printf("refresh()\n");
	char fname[512];
	sprintf(fname, "%s\\obj\\output_%06d.obj", DataDir, counter);
	if ( TerrainMode )
	{
		sprintf(fname, "%s\\Terrain\\output_%06d.obj", DataDir, counter);
	}
	if (IsFileAlreadyOpen(fname) )
	{
		return;
	}
	FILE* fp = fopen(fname, "r");
	if ( fp )
	{
		fclose(fp);
		
		sprintf(fname, "%s\\obj\\output_%06d.obj", DataDir, counter);
		if ( TerrainMode )
		{
			sprintf(fname, "%s\\Terrain\\output_%06d.obj", DataDir, counter);
		}
		if (IsFileAlreadyOpen(fname) )
		{
			return;
		}
		Obj* wrk = output;
		output = NULL;

		if ( wrk ) delete wrk;
		wave_height = 1;
		ZScale = _ZScale;
		printf("ZScale %f\n", ZScale);
		if ( TerrainMode )
		{
			wrk = new Obj(fname, (no_vbo ? 0:VBO2), 0.0);
		}else
		{
			if (!envDELETE_DRAWING_FILES)
			{
				envDELETE_DRAWING_FILES = getenv("DELETE_DRAWING_FILES");
			}
			if (envDELETE_DRAWING_FILES && atoi(envDELETE_DRAWING_FILES))
			{
				if ( ogjFiles.size() > 10 )
				{
					for ( int ii = 0; ii < ogjFiles.size(); ii++ )
					{
						_unlink(ogjFiles[ii].c_str());
					}
					ogjFiles.clear();
				}
				ogjFiles.push_back(fname);
			}

			wrk = new Obj(fname, (no_vbo ? 0:VBO2), 0.0, 0.85f, true);
			sprintf(fname, "%s\\obj\\output_%06d.obj.vector", DataDir, counter);
			if (IsFileAlreadyOpen(fname) )
			{
				/* empty */
			}else
			{
				printf("load wind vector\n");
				windVectorList.clear();
				char buf[256];
				FILE* fp = fopen(fname, "r");
				printf("[%s] => %p\n", fname, fp);
				if ( fp )
				{
					while( fgets(buf, 256, fp) != NULL )
					{
						windVector p;
						double s[3], e[3];
						sscanf(buf, "%lf %lf %lf %lf %lf %lf", 
							&(s[0]),&(s[1]),&(s[2]),
							&(e[0]),&(e[1]),&(e[2]));
						p.s[0] = s[0];
						p.s[1] = s[1];
						p.s[2] = s[2];
						p.e[0] = e[0];
						p.e[1] = e[1];
						p.e[2] = e[2];
						wrk->normalizeSize(4.0, p.s[0], p.s[1], p.s[2]);
						wrk->normalizeSize(4.0, p.e[0], p.e[1], p.e[2]);
						windVectorList.push_back(p);
					}
					fclose(fp);
				}
				printf("wind vector %d\n", windVectorList.size());
			}
		}
		wave_height = 0;
		ZScale = 1.0;
		output = wrk;
		printf("counter%d cur_counter %d -> ", counter, cur_counter);
		cur_counter = counter;
		printf("%d\n", cur_counter);
#if USE_GLUI
		spinner->set_int_val(counter);
#endif
		glutSetWindow(main_window);
		resize(width, height);
		load2();
		display();
		glutPostRedisplay();
		if ( capture == 1 ) Capture();
		printf("cur_counter %d -> ", counter);
		counter += counter_step;
		printf("%d\n", counter);

		printf("refresh() end.\n");
		return;
	}
}

int no_data_count = 0;
/*! 
 * アイドルイベント処理関数
 */
void Idle(void)
{
  /* 画面の描き替え */
	glutSetWindow(main_window);
	glutPostRedisplay();

	if ( !animation ) return;

	if ( !aaaaaa ) return;

	aaaaaa = 0;

	int pre_counter = counter;
	refresh();

	glutTimerFunc(4500, timer, 0);

	if ( end_exit_number >= 0 && end_exit_number < counter )
	{
		RemoveFontResource(font2FileName);
		exit(0);
	}

	if ( counter == pre_counter )
	{
		no_data_count++;
	}else
	{
		no_data_count = 0;
	}
	if ( no_data_count > 100 )
	{
		RemoveFontResource(font2FileName);
		exit(0);
	}
	return;

}

void SwitchIdle()
{
	animation = !animation;
	if ( animation ) printf("animation running\n");
	else printf("animation stop\n");

	if ( animation && capture == 1 && offscreen_capture)
	{
		////glutIconifyWindow();
		//glui2->hide();
		//glui_area_hide = 1;
	}
}

static void spetialkey(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_LEFT:
		obj_pos[0] -= 0.005;
		break;
	case GLUT_KEY_RIGHT:
		obj_pos[0] += 0.005;
		break;
	case GLUT_KEY_UP:
		obj_pos[1] += 0.005;
		break;
	case GLUT_KEY_DOWN:
		obj_pos[1] -= 0.005;
		break;

	}
}

static void keyboard(unsigned char key, int x, int y)
{
	printf("key->%c\n", key);
  switch (key) {
  case 'a':
	  SwitchIdle();
	  break;
  case 'c':
	  Capture();
	  break;
  case 'h':
#if USE_GLUI
	  if ( glui_area_hide == 0 ) glui_area_hide =1;
	  else glui_area_hide = 0;
	
	  if ( glui_area_hide ) glui2->hide();
	  else glui2->show();
#endif
	  break;
  case 'z':
	  //printf("zoom %f -> ", zoom);
	  zoom -= 0.005;
	  //printf("%f\n", zoom);
	  display();
	  break;
  case 'Z':
	  //printf("zoom %f -> ", zoom);
	  zoom += 0.005;
	  //printf("%f\n", zoom);
	  display();
	  break;
  case 'q':
  case 'Q':
  case '\033':
    /* ESC か q か Q をタイプしたら終了 */
	RemoveFontResource(font2FileName);
    exit(0);
  default:
    break;
  }
}

float pre_zoom;
float pre_shiftX;
float pre_shiftY;
float pre_obj_pos[3];
float pre_view_rotate[16];
float pre_view_rotate2[16];
float pre_view_rotate3[16];
double pre_trackballRotation[16];

void save()
{
	int x, y;
	char fname[256];
	char fname2[256];
	if ( Perspective )
	{
		sprintf(fname, "%s\\TSUNAMI_Viewr_%s_Perspective.sav", DataDir, IDname);
		sprintf(fname2, "%s\\TSUNAMI_Viewr_%s_Perspective.sav_backup", DataDir, IDname);
	}else
	{
		sprintf(fname, "%s\\TSUNAMI_Viewr_%s_Orthogonal.sav", DataDir, IDname);
		sprintf(fname2, "%s\\TSUNAMI_Viewr_%s_Orthogonal.sav_backup", DataDir, IDname);
	}
	FILE* fp = fopen(fname, "r");
	if ( fp )
	{
		fclose(fp);
		CopyFile(fname, fname2, FALSE);
	}
 	printf("save[%s] =>", fname);
	fp = fopen(fname, "w");
	if ( fp == NULL ) return;

	fprintf(fp, "%d\n", light1);
	fprintf(fp, "%d\n", light2);
	fprintf(fp, "%d\n", alphablend);
	fprintf(fp, "%d\n", axis); printf("axis %d\n", axis);
	fprintf(fp, "%d\n", waterdepth);
	fprintf(fp, "%d\n", wave);
	fprintf(fp, "%d\n", width);
	fprintf(fp, "%d\n", height);
	fprintf(fp, "%f\n", zoom);
	fprintf(fp, "%f\n", shiftX);
	fprintf(fp, "%f\n", shiftY);
	fprintf(fp, "%f\n", shiftY);
	fprintf(fp, "%f\n", obj_pos[0]);
	fprintf(fp, "%f\n", obj_pos[1]);
	fprintf(fp, "%f\n", obj_pos[2]);
	double* r = trackballRotation();
	for ( int i = 0; i < 16; i++ )
	{
		fprintf(fp, "%f\n", r[i]);
	}
	trackball_drgStpos(1, x, y);
	fprintf(fp, "%d %d\n", x, y);
	trackball_Cofpos(1, x, y);
	fprintf(fp, "%d %d\n", x, y);
	r = trackball_getCQ();
	for ( int i = 0; i < 4; i++ )
	{
		fprintf(fp, "%f\n", r[i]);
	}
	r = trackball_getTQ();
	for ( int i = 0; i < 4; i++ )
	{
		fprintf(fp, "%f\n", r[i]);
	}
	r = trackball_getRT();
	for ( int i = 0; i < 16; i++ )
	{
		fprintf(fp, "%f\n", r[i]);
	}

	for ( int i = 0; i < 16; i++ )
	{
		fprintf(fp, "%f\n", view_rotate[i]);
	}
	for ( int i = 0; i < 16; i++ )
	{
		fprintf(fp, "%f\n", view_rotate2[i]);
	}
	for ( int i = 0; i < 16; i++ )
	{
		fprintf(fp, "%f\n", view_rotate3[i]);
	}
	fprintf(fp, "%f\n", offset1);
	fprintf(fp, "%f\n", offset2);
	fprintf(fp, "%d\n", earth);
	fprintf(fp, "%d\n", windvec);
	fprintf(fp, "%d\n", colorbar);
	fclose(fp);
	printf("...done.\n");
}

void save2()
{
	int x, y;
	char fname[256];

	if ( freamCnt == 0 )
	{
		//int ii = 0;
		//int skippMax = 100000;
		//int skippCnt = 0;
		//while(skippCnt < skippMax)
		//{
		//	if ( Perspective )
		//	{
		//		sprintf(fname, "%s\\Scenario\\TSUNAMI_Viewr_%s_Perspective_%03d.sav", DataDir, IDname, ii);
		//	}else
		//	{
		//		sprintf(fname, "%s\\Scenario\\TSUNAMI_Viewr_%s_Orthogonal_%03d.sav", DataDir, IDname, ii);
		//	}
		//	FILE* fp = fopen(fname, "r");
		//	if ( fp != NULL )
		//	{
		//		fclose(fp);
		//		_unlink(fname);
		//		skippCnt = 0;
		//	}else
		//	{
		//		skippCnt++;
		//	}
		//	ii++;
		//}

		if ( Perspective )
		{
			sprintf(fname, "%s\\Scenario\\TSUNAMI_Viewr_%s_Perspective_*.sav", DataDir, IDname);
		}else
		{
			sprintf(fname, "%s\\Scenario\\TSUNAMI_Viewr_%s_Orthogonal_*.sav", DataDir, IDname);
		}
		char cmd[512];
		sprintf(cmd, "del /Q %s", fname);
		system( cmd );
	}


	double dt = 1.0/(double)(freamNum-1);
	double t = 0.0;
	int num = freamNum;

	if ( freamCnt == 0 )
	{
		t = 1.0;
		num = 1;
		printf("アニメーションスタート位置保存\n");
	}
	for ( int k = 0; k < num; k++ )
	{
		if ( Perspective )
		{
			sprintf(fname, "%s\\Scenario\\TSUNAMI_Viewr_%s_Perspective_%03d.sav", DataDir, IDname, freamCnt);
		}else
		{
			sprintf(fname, "%s\\Scenario\\TSUNAMI_Viewr_%s_Orthogonal_%03d.sav", DataDir, IDname, freamCnt);
		}
 		printf("アニメーション[%s]\n", fname);
		FILE* fp = fopen(fname, "w");
		if ( fp == NULL ) return;

		fprintf(fp, "%d\n", light1);
		fprintf(fp, "%d\n", light2);
		fprintf(fp, "%d\n", alphablend);
		fprintf(fp, "%d\n", axis);
		fprintf(fp, "%d\n", waterdepth);
		fprintf(fp, "%d\n", wave);
		fprintf(fp, "%d\n", width);
		fprintf(fp, "%d\n", height);
		fprintf(fp, "%f\n", pre_zoom*(1.0-t)+zoom*t);
		fprintf(fp, "%f\n", pre_shiftX*(1.0-t)+shiftX*t);
		fprintf(fp, "%f\n", pre_shiftY*(1.0-t)+shiftY*t);
		fprintf(fp, "%f\n", pre_shiftY*(1.0-t)+shiftY*t);
		fprintf(fp, "%f\n", pre_obj_pos[0]*(1.0-t)+obj_pos[0]*t);
		fprintf(fp, "%f\n", pre_obj_pos[1]*(1.0-t)+obj_pos[1]*t);
		fprintf(fp, "%f\n", pre_obj_pos[2]*(1.0-t)+obj_pos[2]*t);
		double* r = trackballRotation();
		for ( int i = 0; i < 16; i++ )
		{
			fprintf(fp, "%f\n", pre_trackballRotation[i]*(1.0-t)+r[i]*t);
		}
		trackball_drgStpos(1, x, y);
		fprintf(fp, "%d %d\n", x, y);
		trackball_Cofpos(1, x, y);
		fprintf(fp, "%d %d\n", x, y);
		r = trackball_getCQ();
		for ( int i = 0; i < 4; i++ )
		{
			fprintf(fp, "%f\n", r[i]);
		}
		r = trackball_getTQ();
		for ( int i = 0; i < 4; i++ )
		{
			fprintf(fp, "%f\n", r[i]);
		}
		r = trackball_getRT();
		for ( int i = 0; i < 16; i++ )
		{
			fprintf(fp, "%f\n", r[i]);
		}

		for ( int i = 0; i < 16; i++ )
		{
			fprintf(fp, "%f\n", pre_view_rotate[i]*(1.0-t)+view_rotate[i]*t);
		}
		for ( int i = 0; i < 16; i++ )
		{
			fprintf(fp, "%f\n", pre_view_rotate2[i]*(1.0-t)+view_rotate2[i]*t);
		}
		for ( int i = 0; i < 16; i++ )
		{
			fprintf(fp, "%f\n", pre_view_rotate3[i]*(1.0-t)+view_rotate3[i]*t);
		}
		fprintf(fp, "%f\n", offset1);
		fprintf(fp, "%f\n", offset2);
		fprintf(fp, "%d\n", earth);
		fprintf(fp, "%d\n", windvec);
		fprintf(fp, "%d\n", colorbar);
		fclose(fp);
		freamCnt += counter_step;
		int nn = freamCnt;
		spinner2->set_int_val(nn);
		t += dt;
	}
	pre_zoom = zoom;
	pre_shiftX = shiftX;
	pre_shiftY = shiftY;
	pre_obj_pos[0] = obj_pos[0];
	pre_obj_pos[1] = obj_pos[1];
	pre_obj_pos[2] = obj_pos[2];
	memcpy(pre_view_rotate, view_rotate, 16*sizeof(float));
	memcpy(pre_view_rotate2, view_rotate, 16*sizeof(float));
	memcpy(pre_view_rotate3, view_rotate, 16*sizeof(float));
	memcpy(pre_trackballRotation, trackballRotation(), 16*sizeof(double));

	printf("...done.\n");
}

void load(int flg = 1)
{
	int x, y;
	char fname[256];
	if ( Perspective )
	{
		sprintf(fname, "%s\\TSUNAMI_Viewr_%s_Perspective.sav", DataDir, IDname);
	}else
	{
		sprintf(fname, "%s\\TSUNAMI_Viewr_%s_Orthogonal.sav", DataDir, IDname);
	}
 	printf("load[%s] =>", fname);
	FILE* fp = fopen(fname, "r");

	if ( fp == NULL )
	{
		sprintf(fname, "%s\\TSUNAMI_Viewr_%s.sav", DataDir, IDname);
	 	if ( flg ) printf("load[%s] =>", fname);
		fp = fopen(fname, "r");
	}
	if ( fp == NULL ) return;
	char buf[256];


	pre_zoom = zoom;
	pre_shiftX = shiftX;
	pre_shiftY = shiftY;
	pre_obj_pos[0] = obj_pos[0];
	pre_obj_pos[1] = obj_pos[1];
	pre_obj_pos[2] = obj_pos[2];
	memcpy(pre_view_rotate, view_rotate, 16*sizeof(float));
	memcpy(pre_view_rotate2, view_rotate, 16*sizeof(float));
	memcpy(pre_view_rotate3, view_rotate, 16*sizeof(float));
	memcpy(pre_trackballRotation, trackballRotation(), 16*sizeof(double));

	fgets(buf, 256, fp); sscanf(buf, "%d\n", &light1);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &light2);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &alphablend);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &axis); printf(":axis %d\n", axis);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &waterdepth);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &wave);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &width);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &height);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &zoom);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &shiftX);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &shiftY);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &shiftY);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &obj_pos[0]);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &obj_pos[1]);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &obj_pos[2]);
	double* r = trackballRotation();
	for ( int i = 0; i < 16; i++ )
	{
		fgets(buf, 256, fp); sscanf(buf, "%lf\n", &r[i]);
	}
	fgets(buf, 256, fp); sscanf(buf, "%d %d\n", &x, &y);
	trackball_drgStpos(0, x, y);
	fgets(buf, 256, fp); sscanf(buf, "%d %d\n", &x, &y);
	trackball_Cofpos(0, x, y);
	r = trackball_getCQ();
	for ( int i = 0; i < 4; i++ )
	{
		fgets(buf, 256, fp); sscanf(buf, "%lf\n", &r[i]);
	}
	r = trackball_getTQ();
	for ( int i = 0; i < 4; i++ )
	{
		fgets(buf, 256, fp); sscanf(buf, "%lf\n", &r[i]);
	}
	r = trackball_getRT();
	for ( int i = 0; i < 16; i++ )
	{
		fgets(buf, 256, fp); sscanf(buf, "%lf\n", &r[i]);
	}

	for ( int i = 0; i < 16; i++ )
	{
		fgets(buf, 256, fp); sscanf(buf, "%f\n", &view_rotate[i]);
	}
	for ( int i = 0; i < 16; i++ )
	{
		fgets(buf, 256, fp); sscanf(buf, "%f\n", &view_rotate2[i]);
	}
	for ( int i = 0; i < 16; i++ )
	{
		fgets(buf, 256, fp); sscanf(buf, "%f\n", &view_rotate3[i]);
	}
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &offset1);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &offset2);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &earth);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &windvec);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &colorbar);

#if USE_GLUI
	//trans_water_depth_spinner->set_float_val(offset1);
	//trans_wave_spinner->set_float_val(offset2);
	glui2->sync_live();
#endif

	fclose(fp);
	if ( flg ) printf("...done.\n");

	int ww = width;
	int hh = height;
	width = 100;
	height = 100;
	resize(ww, hh);
	width = ww;
	height = hh;

	glutReshapeWindow(width, height);
	display();
	glutPostRedisplay();

	pre_zoom = zoom;
	pre_shiftX = shiftX;
	pre_shiftY = shiftY;
	pre_obj_pos[0] = obj_pos[0];
	pre_obj_pos[1] = obj_pos[1];
	pre_obj_pos[2] = obj_pos[2];
	memcpy(pre_view_rotate, view_rotate, 16*sizeof(float));
	memcpy(pre_view_rotate2, view_rotate2, 16*sizeof(float));
	memcpy(pre_view_rotate3, view_rotate3, 16*sizeof(float));
	memcpy(pre_trackballRotation, trackballRotation(), 16*sizeof(double));
}

void load2()
{
	if ( !scenario )
	{
		load(0);
		return;
	}

	int x, y;
	char fname[256];
	if ( Perspective )
	{
		sprintf(fname, "%s\\Scenario\\TSUNAMI_Viewr_%s_Perspective_%03d.sav", DataDir, IDname, counter);
	}else
	{
		sprintf(fname, "%s\\Scenario\\TSUNAMI_Viewr_%s_Orthogonal_%03d.sav", DataDir, IDname, counter);
	}
	FILE* fp = fopen(fname, "r");
	if ( fp == NULL )
	{
		return;
	}
 	printf("load2[%s] =>", fname);

	char buf[256];

	fgets(buf, 256, fp); sscanf(buf, "%d\n", &light1);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &light2);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &alphablend);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &axis);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &waterdepth);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &wave);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &width);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &height);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &zoom);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &shiftX);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &shiftY);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &shiftY);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &obj_pos[0]);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &obj_pos[1]);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &obj_pos[2]);
	double* r = trackballRotation();
	for ( int i = 0; i < 16; i++ )
	{
		fgets(buf, 256, fp); sscanf(buf, "%lf\n", &r[i]);
	}
	fgets(buf, 256, fp); sscanf(buf, "%d %d\n", &x, &y);
	trackball_drgStpos(0, x, y);
	fgets(buf, 256, fp); sscanf(buf, "%d %d\n", &x, &y);
	trackball_Cofpos(0, x, y);
	r = trackball_getCQ();
	for ( int i = 0; i < 4; i++ )
	{
		fgets(buf, 256, fp); sscanf(buf, "%lf\n", &r[i]);
	}
	r = trackball_getTQ();
	for ( int i = 0; i < 4; i++ )
	{
		fgets(buf, 256, fp); sscanf(buf, "%lf\n", &r[i]);
	}
	r = trackball_getRT();
	for ( int i = 0; i < 16; i++ )
	{
		fgets(buf, 256, fp); sscanf(buf, "%lf\n", &r[i]);
	}

	for ( int i = 0; i < 16; i++ )
	{
		fgets(buf, 256, fp); sscanf(buf, "%f\n", &view_rotate[i]);
	}
	for ( int i = 0; i < 16; i++ )
	{
		fgets(buf, 256, fp); sscanf(buf, "%f\n", &view_rotate2[i]);
	}
	for ( int i = 0; i < 16; i++ )
	{
		fgets(buf, 256, fp); sscanf(buf, "%f\n", &view_rotate3[i]);
	}
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &offset1);
	fgets(buf, 256, fp); sscanf(buf, "%f\n", &offset2);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &earth);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &windvec);
	fgets(buf, 256, fp); sscanf(buf, "%d\n", &colorbar);

#if USE_GLUI
	//trans_water_depth_spinner->set_float_val(offset1);
	//trans_wave_spinner->set_float_val(offset2);
	glui2->sync_live();
#endif

	fclose(fp);
	printf("...done.\n");

	int ww = width;
	int hh = height;
	width = 100;
	height = 100;
	resize(ww, hh);
	width = ww;
	height = hh;

	glutReshapeWindow(width, height);
	//display();
	//glutPostRedisplay();
}

/*!
 * メインメニュー
 * @param[in] id メニューID
 */
void OnMainMenu(int id)
{
	keyboard((unsigned char)id, 0, 0);
}

/*!
 * GLUTの右クリックメニュー作成
 */
void InitMenu(void)
{
	// メインメニュー
	glutCreateMenu(OnMainMenu);
	glutAddMenuEntry("Toggle animation [s]",		's');
	glutAddMenuEntry(" ------------------------ ",	-1);
	glutAddMenuEntry("Quit [ESC]",					'\033');
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void control_cb( int control )
{
	int n;
	FILE* fp = NULL;
	switch(control)
	{
	case 777:
#if 0
		n = spinner->get_int_val();
		counter = n-(n%counter_step);
		if ( counter < 0 ) counter = 0;
		cur_counter = counter;
		spinner->set_int_val(counter);

		char fname[512];
		sprintf(fname, "%s\\obj\\output_%06d.obj", DataDir, counter);
		if (IsFileAlreadyOpen(fname) )
		{
			break;
		}
		fp = fopen(fname, "r");
		if ( fp )
		{
			fclose(fp);

			sprintf(fname, "%s\\obj\\output_%06d.obj", DataDir, counter);
			if (IsFileAlreadyOpen(fname) )
			{
				break;
			}
			Obj* wrk = output;
			output = NULL;

			if ( wrk ) delete wrk;
			wrk = new Obj(fname, VBO2, 0.0, 0.8f, true);
			output = wrk;
			cur_counter = counter;
			spinner->set_int_val(counter);
			glutSetWindow(main_window);
			glutPostRedisplay();
		}
#endif
		break;
	case 778:
		capture *= -1;
	
		/* 現在のビューポートのサイズを得る */
		GetCurrentViewPort();
		Capture();
		if ( capture == 1 ) printf("Capture ON\n");
		else printf("Capture OFF\n");
		break;
	}
}

void ani(int x)
{
	SwitchIdle();
}


void getIDname()
{
	char fname[512];
	sprintf(fname, "%s\\CalculationParameters.txt", DataDir);
	FILE* fp = fopen(fname, "r");
	if ( fp == NULL ) return;

	char buf[256];
	while( fgets(buf, 256, fp) != NULL )
	{
		if ( strcmp(buf, "ID\n") == 0 )
		{
			fgets(buf, 256, fp);
			char* p = strchr(buf, '\n');
			if (p) *p = '\0';
			strcpy(IDname, buf);
			printf("ID:[%s]\n", IDname);
		}
	}
	fclose(fp);
}

/*
** メインプログラム
*/
int main(int argc, char *argv[])
{
	{
		char* env = getenv("PERSPECTIVE");
		if ( env != 0 && atoi(env) == 1 ) Perspective = 1;
	}
	printf("PERSPECTIVE=%d\n", Perspective);
	if ( Perspective ) zoom = -10.0;

	strcpy(colormapName, "wave_colormap.bmp");
	{
		char* env = getenv("COLORMAP");
		if ( env ) strcpy( colormapName, env);
		printf("colormapName:%s\n", colormapName);
	}
	strcpy(DataDir, ".");
	if ( argc >= 2 ) strcpy(DataDir, argv[1]);
	else GetCurrentDirectory(sizeof(DataDir), DataDir);
	printf("target[%s]\n", DataDir);

	char parameterFile[512];
	sprintf(parameterFile, "%s\\CalculationParameters.txt", DataDir);

	for ( int i = 2; i < argc; i++ )
	{
		if ( strcmp(argv[i], "-offscreen") == 0 )
		{
			offscreen_capture = atoi(argv[i+1]);
			i++;
		}
		if ( strcmp(argv[i], "-end") == 0 )
		{
			end_exit_number = atoi(argv[i+1]);
			i++;
		}
		if ( strcmp(argv[i], "-offset1") == 0 )
		{
			offset1 = atof(argv[i+1]);
			i++;
		}
		if ( strcmp(argv[i], "-offset2") == 0 )
		{
			offset2 = atof(argv[i+1]);
			i++;
		}
		if ( strcmp(argv[i], "-nvbo") == 0 )
		{
			no_vbo = atoi(argv[i+1]);
			i++;
		}
		if ( strcmp(argv[i], "-scale") == 0 )
		{
			ZScale = atof(argv[i+1]);
			printf("scale %f\n", ZScale);
			_ZScale = ZScale;
			i++;
		}
		if ( strcmp(argv[i], "-param") == 0 )
		{
			strcpy(parameterFile, argv[i+1]);
			i++;
		}
	}
	if ( getenv("ZSCALE") ) _ZScale = atof( getenv("ZSCALE"));
	printf("ZSCALE:%f\n",_ZScale);

	if ( getenv("CRUSTAL") ) TerrainMode = atoi( getenv("CRUSTAL"));
	printf("CRUSTAL:%d\n",TerrainMode);
	
	if ( getenv("MOVE_STEP") ) freamNum = atof( getenv("MOVE_STEP"));
	printf("MOVE_STEP:%d\n",freamNum);

	{
		FILE* fp = fopen(parameterFile, "r");
		if ( fp )
		{
			char buf[256];
			while( fgets(buf, 256, fp))
			{
				if ( strncmp(buf, "COLOR_BAR_MIN_MAX\n", 18) == 0 )
				{
					double a,b;
					fgets(buf, 256, fp);
					sscanf(buf, "%lf %lf", &a, &b);
					
					char tmp[32];
					sprintf(tmp,"%.1f", a);

					size_t newsize = strlen(tmp) + 1;
					mbstowcs(colormapMinMax[0], tmp, newsize);

					sprintf(tmp,"%.1f", b);

					newsize = strlen(tmp) + 1;
					mbstowcs(colormapMinMax[1], tmp, newsize);
					colormapDefine = 1;
					printf("COLOR_BAR_MIN_MAX=>%f %f\n", a, b);
					break;
				}
			}
			fclose(fp);
		}
	}
	getIDname();

	if ( TerrainMode )
	{
		do{
			char fname[512];
			if ( TerrainMode )
			{
				sprintf(fname, "%s\\Terrain\\output_%06d.obj", DataDir, 0);
			}
			FILE* fp = fopen(fname, "r");
			if ( fp )
			{
				fclose(fp);
				break;
			}
		}while(1);
	}

	int ts = clock();
	bool counter_step_ok = false;
	while(clock()-ts < 5*60*1000)
	{
		counter_step = 1;
		do{
			char fname[512];
			sprintf(fname, "%s\\obj\\output_%06d.obj", DataDir, counter_step);
			if ( TerrainMode )
			{
				sprintf(fname, "%s\\Terrain\\output_%06d.obj", DataDir, 0);
			}
			FILE* fp = fopen(fname, "r");
			if ( fp )
			{
				fclose(fp);
				counter_step_ok = true;
				break;
			}
			counter_step++;
		}while(counter_step < 10000 );
		if ( counter_step_ok ) break;
	}
	if ( !counter_step_ok )
	{
		RemoveFontResource(font2FileName);
		exit(0);
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
#if USE_GLUI
	main_window = glutCreateWindow(argv[0]);
	GLUI_Master.set_glutReshapeFunc( resize ); 
	GLUI_Master.set_glutMouseFunc( mouse );
#else
	glutCreateWindow(argv[0]);
	glutReshapeFunc(resize);
	glutMouseFunc(mouse);
#endif
	
	int glewstat = glewInit();
	if ( glewstat != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glewstat));
		offscreen_capture= 0;
	}
	glutDisplayFunc(display);
	glutIdleFunc(Idle);
	glutTimerFunc(0, timer, 0);
	glutMotionFunc(motion);
	glutMouseWheelFunc ( MouseWheel ) ;//ホイールコールバック

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(spetialkey);
	
	trackballInit();
	//SwitchIdle(1);
	InitMenu();

	init();

#if USE_GLUI

#if 0
	glui2 = GLUI_Master.create_glui_subwindow( main_window, 
												GLUI_SUBWINDOW_BOTTOM );
	glui2->set_main_gfx_window( main_window );
#else
	glui2 = GLUI_Master.create_glui( "control" );
	glui2->set_main_gfx_window( main_window );
#endif

	GLUI_Panel* panel1 = new GLUI_Panel(glui2, "move");
	GLUI_Rotation *view_rot = new GLUI_Rotation(panel1, "Objects", view_rotate );
	view_rot->set_spin( 1.0 );
	new GLUI_Column( panel1, false );
	//GLUI_Rotation *sph_rot = new GLUI_Rotation(glui2, "undef", sphere_rotate );
	//sph_rot->set_spin( .98 );
	//new GLUI_Column( glui2, false );
	//GLUI_Rotation *tor_rot = new GLUI_Rotation(glui2, "undef", torus_rotate );
	//tor_rot->set_spin( .98 );
	//new GLUI_Column( glui2, false );
	GLUI_Translation *trans_xy = 
	new GLUI_Translation(panel1, "Objects XY", GLUI_TRANSLATION_XY, obj_pos );
	trans_xy->set_speed( .005 );
	new GLUI_Column( panel1, false );
	GLUI_Translation *trans_x = 
	new GLUI_Translation(panel1, "Objects X", GLUI_TRANSLATION_X, obj_pos );
	trans_x->set_speed( .005 );
	new GLUI_Column( panel1, false );
	GLUI_Translation *trans_y = 
	new GLUI_Translation( panel1, "Objects Y", GLUI_TRANSLATION_Y, &obj_pos[1] );
	trans_y->set_speed( .005 );
	new GLUI_Column( panel1, false );
	GLUI_Translation *trans_z = 
	new GLUI_Translation( panel1, "Objects Z", GLUI_TRANSLATION_Z, &zoom );
	trans_z->set_speed( .005 );

	GLUI_Panel* panel2 = new GLUI_Panel(glui2, "capture");
	//new GLUI_Column( glui2, false );
	spinner = new GLUI_Spinner( panel2, "time:", &counter, 777, control_cb);
	spinner->set_int_limits( 0, 900000 );
	spinner->set_alignment( GLUI_ALIGN_RIGHT );
	new GLUI_Button( panel2, "refresh", 0,(GLUI_Update_CB)refresh );
	new GLUI_Button( panel2, "Ani", 0,(GLUI_Update_CB)ani );

	new GLUI_Button( panel2, "cap", 778, control_cb );
	
	new GLUI_Column( panel1, false );
	GLUI_Rollout *options = new GLUI_Rollout(panel2, "Options", true );
	new GLUI_Checkbox( options, "alpha", &alphablend );
	new GLUI_Checkbox( options, "water depth", &waterdepth );
	new GLUI_Checkbox( options, "axis", &axis );
	new GLUI_Checkbox( options, "wave", &wave );
	new GLUI_Checkbox( options, "earth", &earth );
	new GLUI_Checkbox( options, "wind", &windvec );
	new GLUI_Checkbox( options, "bar", &colorbar );
	new GLUI_Column( panel2, false );

	GLUI_Rollout *options2 = new GLUI_Rollout(panel2, "light", true );
	new GLUI_Checkbox( options2, "light1", &light1 );
	new GLUI_Checkbox( options2, "light2", &light2 );
	view_rot2 = new GLUI_Rotation(panel2, "light1", view_rotate2 );
	view_rot2->set_spin( 1.0 );
	view_rot3 = new GLUI_Rotation(panel2, "light2", view_rotate3 );
	view_rot2->set_spin( 1.0 );

	new GLUI_Column( glui2, false );
	new GLUI_Button( glui2, "save", 0,(GLUI_Update_CB)save );
	new GLUI_Button( glui2, "load", 0,(GLUI_Update_CB)load );
	GLUI_Panel* panel3 = new GLUI_Panel(glui2, "scenario");
	spinner1 = new GLUI_Spinner( panel3, "freamNum:", &freamNum, 777, control_cb);
	spinner1->set_int_limits( 0, 900000 );
	spinner1->set_alignment( GLUI_ALIGN_RIGHT );
	spinner2 = new GLUI_Spinner( panel3, "freamCnt:", &freamCnt, 777, control_cb);
	spinner2->set_int_limits( 0, 900000 );
	spinner2->set_alignment( GLUI_ALIGN_RIGHT );
	new GLUI_Button( panel3, "Add", 0,(GLUI_Update_CB)save2 );
	new GLUI_Checkbox( panel3, "Apply", &scenario );
	new GLUI_Column( panel3, false );
	new GLUI_Column( glui2, false );

	//
	//trans_water_depth =	new GLUI_Translation( glui2, "water_depth", GLUI_TRANSLATION_Z, &offset1 );
	//trans_water_depth->set_speed( .001 );
	//trans_wave = new GLUI_Translation( glui2, "wave", GLUI_TRANSLATION_Z, &offset2 );
	//trans_wave->set_speed( .001 );

	//trans_water_depth_spinner = new GLUI_Spinner( glui2, "water_depth:", GLUI_SPINNER_FLOAT, &offset1);
	//trans_wave_spinner = new GLUI_Spinner( glui2, "wave:", GLUI_SPINNER_FLOAT, &offset2);
	//trans_water_depth_spinner->set_speed(0.001);
	//trans_wave_spinner->set_speed(0.001);

	GLUI_Master.auto_set_viewport();
#endif

	glutMainLoop();
	return 0;
}
