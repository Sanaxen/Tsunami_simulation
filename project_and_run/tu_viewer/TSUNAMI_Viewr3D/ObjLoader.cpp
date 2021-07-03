#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>

#include <math.h>
#include "vob.hpp"
#include "ObjLoader.h"
#include "bitmap.h"

float Obj::max[3], Obj::min[3];
float Obj::size[3], Obj::mid[3];
float Obj::daiag;
int Obj::minmaxFix;

extern char colormapName[128];
int wave_height = 0;
int colorskipp = 0;
static int loadobj = 0;
#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

double ZScale = 1.0;
BitMap WaveHeightColor;

void getColorMapBitmap()
{
	if ( WaveHeightColor.GetImage() )
	{
		return;
	}

	char thisModuleFileName[_MAX_PATH];
	//実行中のプロセスのフルパス名を取得する
	GetModuleFileNameA(NULL, thisModuleFileName, sizeof(thisModuleFileName));

	char exe_drive[_MAX_DRIVE];	// ドライブ名
    char exe_dir[_MAX_DIR];		// ディレクトリ名
    char exe_fname[_MAX_FNAME];	// ファイル名
    char exe_ext[_MAX_EXT];		// 拡張子

    _splitpath( thisModuleFileName, exe_drive, exe_dir, exe_fname, exe_ext );

	char fname[256];

	sprintf(fname, "%s%s..\\colormap\\%s", exe_drive, exe_dir, colormapName);
	printf("カラーマップ:%s\n", fname);
	WaveHeightColor.Read(fname);
}

static char* alphablend_case = 0;

/*
** コンストラクタ
*/
Obj::Obj(char *name, int vbo, float offset, float alpha, bool water_scaleFlg, int* rgb)
{
  vert = 0;
  norm = 0;
  fnorm = 0;
  face = 0;
  color = 0;
  use_vbo = vbo;
	set_color = false;
	water_scaleFlg_ = water_scaleFlg;
  if (!loadobj)
  {
	printf("MIN-MAX Init\n");
	Obj::max[0] = -9999999.0;
	Obj::max[1] = -9999999.0;
	Obj::max[2] = -9999999.0;
	Obj::min[0] = 99999999.0;
	Obj::min[1] = 99999999.0;
	Obj::min[2] = 99999999.0;
	Obj::daiag = 1.0;
	loadobj++;
  }
  offset_value = 0.0;

  std::ifstream file(name, std::ios::in | std::ios::binary);
  char buf[1024];
  int i, v, f;

  if (!file) {
    std::cerr << name << " が開けません" << std::endl;
    abort();
  }

  printf("Load [%s]..", name);
  v = f = 0;
 int color_id = 0;
 if(wave_height) getColorMapBitmap();

 float alphablend_case_value = 0;
 if (!alphablend_case)
 {
	 alphablend_case = getenv("ALPHA_BLEND_VALUE");
 }
 if ( alphablend_case ) alphablend_case_value = atof(alphablend_case);

#if 0
  char fname2[512];
  sprintf(fname2, "%s.bin", name);

  int read_stat = Read( fname2, nv, nf );
  if ( read_stat == 0 )
  {
	  v = nv;
	  f = nf;
	  try {
		vert = new vec[nv];
		norm = new vec[nv];
		color = new color_vec[nv];
		fnorm = new vec[nf];
		face = new idx[nf];
	  }
	  catch (std::bad_alloc e) {
		std::cerr << "メモリが足りません" << std::endl;
		abort();
	  }
	  Read(fname2);
	  goto end;
  }
#endif

  /* データの数を調べる */
  while (file.getline(buf, sizeof buf)) {
    if (buf[0] == 'v' && buf[1] == ' ') {
      ++v;
    }
    else if (buf[0] == 'f' && buf[1] == ' ') {
      ++f;
    }
	if ( strstr(buf, "#ZSCALE") )
	{
		sscanf(buf, "#ZSCALE:%f", &zscale);
	}
  }

  nv = v;
  nf = f;

  try {
    vert = new vec[v];
    norm = new vec[v];
	color = new color_vec[v];
    fnorm = new vec[f];
    face = new idx[f];
  }
  catch (std::bad_alloc e) {
    std::cerr << "メモリが足りません" << std::endl;
    abort();
  }

  /* ファイルの巻き戻し */
  file.clear();
  file.seekg(0L, std::ios::beg);

  if ( alphablend_case ) alpha = alphablend_case_value;

  bool alpha2 = false;
  /* データの読み込み */
  v = f = 0;
  while (file.getline(buf, sizeof buf)) {
    if (buf[0] == 'v' && buf[1] == ' ') {
		int n = 0;
		int c[4];
		c[3] = 1.0;
		set_color = true;
		float alpha2val = 1.0;
      n = sscanf(buf, "%*s %f %f %f %d %d %d %d %f", vert[v], vert[v] + 1, vert[v] + 2, c, c+1, c+2, &color_id, &alpha2val);
      if ( n == 8 )
	  {
		  alpha2 = true;
		  if ( alphablend_case ) alpha2val = alphablend_case_value;
	  }

	  if ( n != 8 )
	  {
		  n = sscanf(buf, "%*s %f %f %f %d %d %d %d", vert[v], vert[v] + 1, vert[v] + 2, c, c+1, c+2, &color_id);
		  if ( n != 7 )
		  {
			  color_id = -1;
			  n = sscanf(buf, "%*s %f %f %f %d %d %d", vert[v], vert[v] + 1, vert[v] + 2, c, c+1, c+2);
			  if ( n != 6 )
			  {
				set_color = false;
				sscanf(buf, "%*s %f %f %f", vert[v], vert[v] + 1, vert[v] + 2);
			  }
		  }
	  }
	  vert[v][2] += offset;


	  if ( set_color )
	  {
		  //printf("%d %d %d\n", c[0], c[1], c[2]);
		  if ( !colorskipp )
		  {
			  color[v][0] = c[0]/255.0;
			  color[v][1] = c[1]/255.0;
			  color[v][2] = c[2]/255.0;
			  color[v][3] = alpha;
			  if ( alpha2 ) color[v][3] = alpha2val;
			  if ( wave_height && color_id >= 0 )
			  {
				  color[v][0] = WaveHeightColor.cell(WaveHeightColor.H()/2, color_id).r/255.0;
				  color[v][1] = WaveHeightColor.cell(WaveHeightColor.H()/2, color_id).g/255.0;
				  color[v][2] = WaveHeightColor.cell(WaveHeightColor.H()/2, color_id).b/255.0;
				  //printf("%d %f %f %f\n", color_id, color[v][0], color[v][1], color[v][2]);
			  }
		  }else
		  {
			  //color[v][0] = 21/255.0;
			  //color[v][1] = 54/255.0;
			  //color[v][2] = 130/255.0;
			  color[v][0] = 30/255.0;
			  color[v][1] = 31/255.0;
			  color[v][2] = 36/255.0;
			  color[v][3] = alpha;
			  if ( alpha2 ) color[v][3] = alpha2val;
		  }

		  if ( water_scaleFlg_ )
		  {
			  //if ( vert[v][2] > 0.001 )
			  //{
				 // vert[v][2] *= 5.0;
			  //}
#if 0
			  color[v][0] = c[0]/255.0;
			  color[v][1] = c[1]/255.0;
			  color[v][2] = c[2]/255.0;

			  if ( vert[v][2]/zscale > 0.35 )
			  {
				  double t = (vert[v][2]/zscale - 0.35)/(4.5 - 0.35);
				  if ( t > 1.0 ) t = 1.0;
				  if ( t >= 0.0 )
				  {
					  color[v][0] = (c[0]*(1.0 - t) + 217.0*t)/255.0;
					  color[v][1] = (c[1]*(1.0 - t) + 241.0*t)/255.0;
					  color[v][2] = (c[2]*(1.0 - t) + 255.0*t)/255.0;
				  }
			  }
#endif
		  }

	  }else
	  {
		  if ( rgb )
		  {
			  color[v][0] = (double)rgb[0]/255.0;
			  color[v][1] = (double)rgb[1]/255.0;
			  color[v][2] = (double)rgb[2]/255.0;
		  }else
		  {
			  color[v][0] = 0.6;
			  color[v][1] = 0.6;
			  color[v][2] = 0.6;
		  }
		  color[v][3] = alpha;
	  }
      ++v;
    }
    else if (buf[0] == 'f' && buf[1] == ' ') {
      if (sscanf(buf + 2, "%d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", face[f], face[f] + 1, face[f] + 2) != 3) {
        if (sscanf(buf + 2, "%d//%*d %d//%*d %d//%*d", face[f], face[f] + 1, face[f] + 2) != 3) {
          sscanf(buf + 2, "%d %d %d", face[f], face[f] + 1, face[f] + 2);
        }
      }
      --face[f][0];
      --face[f][1];
      --face[f][2];
      ++f;
    }
  }

  printf("...done\n");


  /* 面法線ベクトルの算出 */
#pragma omp parallel for
  for (i = 0; i < f; ++i) 
  {
    float dx1 = vert[face[i][1]][0] - vert[face[i][0]][0];
    float dy1 = vert[face[i][1]][1] - vert[face[i][0]][1];
    float dz1 = vert[face[i][1]][2] - vert[face[i][0]][2];
    float dx2 = vert[face[i][2]][0] - vert[face[i][0]][0];
    float dy2 = vert[face[i][2]][1] - vert[face[i][0]][1];
    float dz2 = vert[face[i][2]][2] - vert[face[i][0]][2];

    fnorm[i][0] = dy1 * dz2 - dz1 * dy2;
    fnorm[i][1] = dz1 * dx2 - dx1 * dz2;
    fnorm[i][2] = dx1 * dy2 - dy1 * dx2;
  }

  /* 頂点の仮想法線ベクトルの算出 */
  for (i = 0; i < v; ++i) {
    norm[i][0] = norm[i][1] = norm[i][2] = 0.0;
  }
  
#pragma omp parallel for
  for (i = 0; i < f; ++i) 
  {
    norm[face[i][0]][0] += fnorm[i][0];
    norm[face[i][0]][1] += fnorm[i][1];
    norm[face[i][0]][2] += fnorm[i][2];

    norm[face[i][1]][0] += fnorm[i][0];
    norm[face[i][1]][1] += fnorm[i][1];
    norm[face[i][1]][2] += fnorm[i][2];

    norm[face[i][2]][0] += fnorm[i][0];
    norm[face[i][2]][1] += fnorm[i][1];
    norm[face[i][2]][2] += fnorm[i][2];
  }

  /* 頂点の仮想法線ベクトルの正規化 */
#pragma omp parallel for
  for (i = 0; i < v; ++i) {
    float a = sqrt(norm[i][0] * norm[i][0]
                 + norm[i][1] * norm[i][1]
                 + norm[i][2] * norm[i][2]);

    if (a != 0.0) {
      norm[i][0] /= a;
      norm[i][1] /= a;
      norm[i][2] /= a;
    }
  }

  if ( water_scaleFlg_ )
  {
	  printf("wave scale:%f\n", ZScale);
#pragma omp parallel for
	for (i = 0; i < v; ++i) 
	{
		vert[i][0] += norm[i][0]*ZScale;
		vert[i][1] += norm[i][1]*ZScale;
		vert[i][2] += norm[i][2]*ZScale;
	}
  }


  delete [] fnorm;
  fnorm = 0;

#if 0
  Write(fname2);
#endif

end:	;
  normalizeSize(4.0);


#if 10
  if ( use_vbo )
  {
	  printf("use VBO :glGenBuffers\n");
	  /* バッファオブジェクトの名前を4つ作る */
	  glGenBuffers(4, buffers);

	  /* １つ目のバッファオブジェクトに頂点データ配列を転送する */
	  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	  glBufferData(GL_ARRAY_BUFFER, nv * sizeof(vec), vert, (use_vbo==1?GL_STATIC_DRAW:GL_DYNAMIC_DRAW));

	  /* ２つ目のバッファオブジェクトに法線データ配列を転送する */
	  glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	  glBufferData(GL_ARRAY_BUFFER, nv * sizeof(vec), norm, (use_vbo==1?GL_STATIC_DRAW:GL_DYNAMIC_DRAW));

	  /* ３つ目のバッファオブジェクトに頂点のインデックスを転送する */
	  glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	  glBufferData(GL_ARRAY_BUFFER, nv * sizeof(color_vec), color, (use_vbo==1?GL_STATIC_DRAW:GL_DYNAMIC_DRAW));

	  /* ４つ目のバッファオブジェクトに頂点のインデックスを転送する */
	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
	  glBufferData(GL_ELEMENT_ARRAY_BUFFER, nf * sizeof(idx), face, (use_vbo==1?GL_STATIC_DRAW:GL_DYNAMIC_DRAW));

	if (vert) delete[] vert;
	if (norm) delete[] norm;
	if (fnorm) delete[] fnorm;
	if (face) delete[] face;
	if ( color ) delete [] color;
	vert = 0;
	norm = 0;
	fnorm = 0;
	face = 0;
	color = 0;

  }
#endif
  	printf("Create Obj(%x)\n", (int)((void*)this));

}



/*
** デストラクタ
*/
Obj::~Obj()
{
	glFlush();
    if ( use_vbo )
	{
		printf("use VBO :glDeleteBuffers\n");
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDeleteBuffers(4, buffers);
	}

	if (vert) delete[] vert;
	if (norm) delete[] norm;
	if (fnorm) delete[] fnorm;
	if (face) delete[] face;
	if ( color ) delete [] color;
	vert = 0;
	norm = 0;
	fnorm = 0;
	face = 0;
	color = 0;
	printf("Delete Obj(%x)\n", (int)((void*)this));
}


/*
** 図形の表示
*/
void Obj::draw(char* ShaderCode)
{
	if ( use_vbo )
	{
		glEnable(GL_COLOR_MATERIAL);
		/* 頂点データと法線データの配列を有効にする */
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_INDEX_ARRAY);	//<== 

		/* 頂点データの場所を指定する */
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));

		/* 法線データの場所を指定する */
		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
		glColorPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(0));

		/* 頂点のインデックスの場所を指定して図形を描く */
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
		glDrawElements(GL_TRIANGLES, nf * 3, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

		/* 頂点データと法線データの配列を無効にする */
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_INDEX_ARRAY);	//<== 

		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisable(GL_COLOR_MATERIAL);
	}else
	{
		glEnable(GL_COLOR_MATERIAL);
		glShadeModel(GL_SMOOTH);

		if ( water_scaleFlg_ )
		{
			glBegin(GL_TRIANGLES);
			for ( int i = 0; i < nf; i++ )
			{
				glColor4f(color[face[i][0]][0],color[face[i][0]][1],color[face[i][0]][2], color[face[i][0]][3]);
				glVertex3f(vert[face[i][0]][0],vert[face[i][0]][1],vert[face[i][0]][2]);
				glNormal3f(norm[face[i][0]][0],norm[face[i][0]][1],norm[face[i][0]][2]);

				glColor4f(color[face[i][1]][0],color[face[i][1]][1],color[face[i][1]][2],color[face[i][1]][3]);
				glVertex3f(vert[face[i][1]][0],vert[face[i][1]][1],vert[face[i][1]][2]);
				glNormal3f(norm[face[i][1]][0],norm[face[i][1]][1],norm[face[i][1]][2]);

				glColor4f(color[face[i][2]][0],color[face[i][2]][1],color[face[i][2]][2],color[face[i][2]][3]);
				glVertex3f(vert[face[i][2]][0],vert[face[i][2]][1],vert[face[i][2]][2]);
				glNormal3f(norm[face[i][2]][0],norm[face[i][2]][1],norm[face[i][2]][2]);
			}
			glEnd();
		}else
		{
			glBegin(GL_TRIANGLES);
			for ( int i = 0; i < nf; i++ )
			{
				//glNormal3f(fnorm[i][0],fnorm[i][1],fnorm[i][2]);
				glColor4f(color[face[i][0]][0],color[face[i][0]][1],color[face[i][0]][2], color[face[i][0]][3]);
				glVertex3f(vert[face[i][0]][0],vert[face[i][0]][1],vert[face[i][0]][2]);
				glNormal3f(norm[face[i][0]][0],norm[face[i][0]][1],norm[face[i][0]][2]);

				glColor4f(color[face[i][1]][0],color[face[i][1]][1],color[face[i][1]][2],color[face[i][1]][3]);
				glVertex3f(vert[face[i][1]][0],vert[face[i][1]][1],vert[face[i][1]][2]);
				glNormal3f(norm[face[i][1]][0],norm[face[i][1]][1],norm[face[i][1]][2]);

				glColor4f(color[face[i][2]][0],color[face[i][2]][1],color[face[i][2]][2],color[face[i][2]][3]);
				glVertex3f(vert[face[i][2]][0],vert[face[i][2]][1],vert[face[i][2]][2]);
				glNormal3f(norm[face[i][2]][0],norm[face[i][2]][1],norm[face[i][2]][2]);

			}
			glEnd();
		}
		glDisable(GL_COLOR_MATERIAL);
	}
}

void Obj::drawLines(char* ShaderCode)
{
	if ( use_vbo )
	{
		glEnable(GL_COLOR_MATERIAL);
		/* 頂点データと法線データの配列を有効にする */
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_INDEX_ARRAY);	//<== 

		/* 頂点データの場所を指定する */
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));

		/* 法線データの場所を指定する */
		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
		glColorPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(0));

		/* 頂点のインデックスの場所を指定して図形を描く */
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
		glDrawElements(GL_LINES, nf * 3, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

		/* 頂点データと法線データの配列を無効にする */
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_INDEX_ARRAY);	//<== 

		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisable(GL_COLOR_MATERIAL);
	}else
	{
		glEnable(GL_COLOR_MATERIAL);
		glShadeModel(GL_SMOOTH);

		if ( water_scaleFlg_ )
		{
			glBegin(GL_LINES);
			for ( int i = 0; i < nf; i++ )
			{
				glColor4f(color[face[i][0]][0],color[face[i][0]][1],color[face[i][0]][2], color[face[i][0]][3]);
				glVertex3f(vert[face[i][0]][0],vert[face[i][0]][1],vert[face[i][0]][2]);
				glNormal3f(norm[face[i][0]][0],norm[face[i][0]][1],norm[face[i][0]][2]);

				glColor4f(color[face[i][1]][0],color[face[i][1]][1],color[face[i][1]][2],color[face[i][1]][3]);
				glVertex3f(vert[face[i][1]][0],vert[face[i][1]][1],vert[face[i][1]][2]);
				glNormal3f(norm[face[i][1]][0],norm[face[i][1]][1],norm[face[i][1]][2]);

				glColor4f(color[face[i][2]][0],color[face[i][2]][1],color[face[i][2]][2],color[face[i][2]][3]);
				glVertex3f(vert[face[i][2]][0],vert[face[i][2]][1],vert[face[i][2]][2]);
				glNormal3f(norm[face[i][2]][0],norm[face[i][2]][1],norm[face[i][2]][2]);
			}
			glEnd();
		}else
		{
			glBegin(GL_LINES);
			for ( int i = 0; i < nf; i++ )
			{
				//glNormal3f(fnorm[i][0],fnorm[i][1],fnorm[i][2]);
				glColor4f(color[face[i][0]][0],color[face[i][0]][1],color[face[i][0]][2], color[face[i][0]][3]);
				glVertex3f(vert[face[i][0]][0],vert[face[i][0]][1],vert[face[i][0]][2]);
				glNormal3f(norm[face[i][0]][0],norm[face[i][0]][1],norm[face[i][0]][2]);

				glColor4f(color[face[i][1]][0],color[face[i][1]][1],color[face[i][1]][2],color[face[i][1]][3]);
				glVertex3f(vert[face[i][1]][0],vert[face[i][1]][1],vert[face[i][1]][2]);
				glNormal3f(norm[face[i][1]][0],norm[face[i][1]][1],norm[face[i][1]][2]);

				glColor4f(color[face[i][2]][0],color[face[i][2]][1],color[face[i][2]][2],color[face[i][2]][3]);
				glVertex3f(vert[face[i][2]][0],vert[face[i][2]][1],vert[face[i][2]][2]);
				glNormal3f(norm[face[i][2]][0],norm[face[i][2]][1],norm[face[i][2]][2]);

			}
			glEnd();
		}
		glDisable(GL_COLOR_MATERIAL);
	}
}

void Obj::drawOutLines(char* ShaderCode)
{
		glEnable(GL_COLOR_MATERIAL);
		glShadeModel(GL_SMOOTH);

		glBegin(GL_LINES);
		for ( int i = 0; i < nf/2; i++ )
		{
			glColor4f(color[face[2*i][0]][0],color[face[2*i][0]][1],color[face[2*i][0]][2], color[face[2*i][0]][3]);
			glVertex3f(vert[face[2*i][0]][0],vert[face[2*i][0]][1],vert[face[2*i][0]][2]);
			glNormal3f(norm[face[2*i][0]][0],norm[face[2*i][0]][1],norm[face[2*i][0]][2]);

			glColor4f(color[face[2*i][1]][0],color[face[2*i][1]][1],color[face[2*i][1]][2],color[face[2*i][1]][3]);
			glVertex3f(vert[face[2*i][1]][0],vert[face[2*i][1]][1],vert[face[2*i][1]][2]);
			glNormal3f(norm[face[2*i][1]][0],norm[face[2*i][1]][1],norm[face[2*i][1]][2]);

			glColor4f(color[face[2*i][2]][0],color[face[2*i][2]][1],color[face[2*i][2]][2],color[face[2*i][2]][3]);
			glVertex3f(vert[face[2*i][2]][0],vert[face[2*i][2]][1],vert[face[2*i][2]][2]);
			glNormal3f(norm[face[2*i][2]][0],norm[face[2*i][2]][1],norm[face[2*i][2]][2]);

			glColor4f(color[face[2*i+1][2]][0],color[face[2*i+1][2]][1],color[face[2*i+1][2]][2],color[face[2*i+1][2]][3]);
			glVertex3f(vert[face[2*i+1][2]][0],vert[face[2*i+1][2]][1],vert[face[2*i+1][2]][2]);
			glNormal3f(norm[face[2*i+1][2]][0],norm[face[2*i+1][2]][1],norm[face[2*i+1][2]][2]);

			glColor4f(color[face[2*i][0]][0],color[face[2*i][0]][1],color[face[2*i][0]][2], color[face[2*i][0]][3]);
			glVertex3f(vert[face[2*i][0]][0],vert[face[2*i][0]][1],vert[face[2*i][0]][2]);
			glNormal3f(norm[face[2*i][0]][0],norm[face[2*i][0]][1],norm[face[2*i][0]][2]);
		}
		glEnd();
		glDisable(GL_COLOR_MATERIAL);
}

void Obj::drawEarth(char* ShaderCode)
{
	double c = 1.0;

	//printf("draw Earth\n");
	double pai = 3.14159265358979323;
	int hh = (int)(180.0/c);
	int ww = (int)(360.0/c);

	//hh = 360;ww=360;
	//printf("hh %d ww %d\n", hh, ww);
	double t = 1.0/(double)(hh-1);
	double s = 1.0/(double)(ww-1);

	double longi[2];
	double latit[2];

	longi[0] = 0.0;
	longi[1] = 2.0*pai;

	latit[0] = -pai*0.5;
	latit[1] = pai*0.5;

	double z2 = 0.0;
	const double f84 = 1.0/298.257223563;
	const double a = 6378137.000;
	const double e2 = f84*(2.0 - f84);

	glEnable(GL_COLOR_MATERIAL);
	//glShadeModel(GL_SMOOTH);

	
	for ( int i = 0; i < hh; i++ )
	{
		glBegin(GL_LINE_STRIP);
		for ( int j = 0; j <= ww; j++ )
		{
			double lat;
			double lon;
			double ee;
			double N;
			float x, y, zz;

			lat = latit[0]*(1.0 - i*t) + latit[1]*i*t;
			lon = longi[0]*(1.0 - (j%ww)*s) + longi[1]*(j%ww)*s;				
			ee = 1.0 - e2*sin(lat)*sin(lat);
			if ( ee < 0.0 )
			{
				ee = 0.0;
				printf("---\n");
			}
			N = a/sqrt(ee);

			x = (N+z2)*cos(lat)*cos(lon);
			y = (N+z2)*cos(lat)*sin(lon);
			zz = (N*(1.0 -e2)+z2)*sin(lat);
			 
			normalizeSize(4.0, x, y, zz);
			glColor4f(0.6f, 0.6f,0.6f, 0.6f);
			glVertex3f(x, y, zz);
		}
		glEnd();
	}

	for ( int j = 0; j < ww; j++ )
	{
		glBegin(GL_LINE_STRIP);
		for ( int i = 0; i <= hh; i++ )
		{
			double lat;
			double lon;
			double ee;
			double N;
			float x, y, zz;

			lat = latit[0]*(1.0 - (i%hh)*t) + latit[1]*(i%hh)*t;
			lon = longi[0]*(1.0 - j*s) + longi[1]*j*s;				
			ee = 1.0 - e2*sin(lat)*sin(lat);
			if ( ee < 0.0 )
			{
				ee = 0.0;
				printf("---\n");
			}
			N = a/sqrt(ee);

			x = (N+z2)*cos(lat)*cos(lon);
			y = (N+z2)*cos(lat)*sin(lon);
			zz = (N*(1.0 -e2)+z2)*sin(lat);
			 
			normalizeSize(4.0, x, y, zz);
			glColor4f(0.6f, 0.6f,0.6f, 0.6f);
			glVertex3f(x, y, zz);
		}
		glEnd();
	}
	glDisable(GL_COLOR_MATERIAL);
}


void Obj::normalizeSize(float length)
{
  if ( !minmaxFix)
  {
	  printf("MIN-MAX Setting!!\n");
	  for(int i=0; i<3; i++)
	  {
		for(int j=0; j<nv; j++)
		{
		  float v = vert[j][i];
		  if(v > max[i])
			max[i] = v;
		  else if(v < min[i])
			min[i] = v;
		}
		size[i] = max[i] - min[i];
		mid[i] = 0.5f*(max[i] + min[i]);
	  }
  }

  float s = size[0];
  if(size[1] > s)
    s = size[1];
  if(size[2] > s)
    s = size[2];
  float scale = length/s;
  
#pragma omp parallel for
  for(int i=0; i<3; i++)
    for(int j=0; j<nv; j++)
      vert[j][i] = scale*(vert[j][i] - mid[i]);

  daiag = sqrt(size[0]*size[0] + size[1]*size[1] + size[2]*size[2]);
}

void Obj::normalizeSize(float length, float& x, float& y, float& z)
{

  float s = size[0];
  if(size[1] > s)
    s = size[1];
  if(size[2] > s)
    s = size[2];
  float scale = length/s;
  
  x = scale*(x - mid[0]);
  y = scale*(y - mid[1]);
  z = scale*(z - mid[2]);
}
