#ifndef OBJ_H
#define OBJ_H

typedef float vec[3];
typedef float color_vec[4];
typedef unsigned int idx[3];

inline int SunflowSC( char* drive, char* dir, char *filename, int vtxnum, double *vtx, int trinum, int *triid, double*uv, double* normal );

class Obj {
  GLuint buffers[4];
  int use_vbo;
public:
  static float max[3], min[3];
  static float mid[3];
  static float daiag;
  static float size[3];

  float zscale;
  bool water_scaleFlg_;

  bool set_color;

  int nv, nf;
  vec *vert, *norm, *fnorm;
  color_vec* color;
  idx *face;
  static int minmaxFix;

  float offset_value;

  Obj()
  {};

  Obj(char *file, int vbo, float offset, float alpha=1.0, bool water_scaleFlg=false, int* rgb=0);
  virtual ~Obj();
  void draw(char* ShaderCode=NULL);
  void drawLines(char* ShaderCode=NULL);
  void drawOutLines(char* ShaderCode=NULL);
  void drawEarth(char* ShaderCode=NULL);

  void normalizeSize(float length);
  void normalizeSize(float length, float& x, float& y, float& z);

  void Write(char* fname)
  {
	  FILE* fp = fopen(fname, "wb");
	  fwrite(&this->nv, sizeof(int), 1, fp);
	  fwrite(&this->nf, sizeof(int), 1, fp);
	  //fwrite(this->min, sizeof(float), 3, fp);
	  //fwrite(this->max, sizeof(float), 3, fp);
	  //fwrite(&this->daiag, sizeof(float), 1, fp);
	  //fwrite(this->size, sizeof(float), 3, fp);
	  //fwrite(this->mid, sizeof(float), 3, fp);
	  fwrite(&this->set_color, sizeof(bool), 1, fp);
	  fwrite(this->vert, sizeof(vec), nv, fp);
	  fwrite(this->color, sizeof(color_vec), nv, fp);
	  fwrite(this->norm, sizeof(vec), nv, fp);
	  fwrite(this->face, sizeof(idx), nf, fp);
	  fwrite(this->fnorm, sizeof(vec), nf, fp);
	  fclose(fp);
  }

  int Read(char* fname, int& nnv, int& nnf)
  {

	  FILE* fp = fopen(fname, "rb");
	  if ( fp == NULL )
	  {
		  return -1;
	  }
	  fread(&this->nv, sizeof(int), 1, fp);
	  fread(&this->nf, sizeof(int), 1, fp);
	  fclose(fp);
	  return 0;
  }

  int Read(char* fname)
  {
	  FILE* fp = fopen(fname, "rb");
	  if ( fp == NULL )
	  {
		  return -1;
	  }
	  fread(&this->nv, sizeof(int), 1, fp);
	  fread(&this->nf, sizeof(int), 1, fp);
	  //fread(this->min, sizeof(float), 3, fp);
	  //fread(this->max, sizeof(float), 3, fp);
	  //fread(&this->daiag, sizeof(float), 1, fp);
	  //fread(this->size, sizeof(float), 3, fp);
	  //fread(this->mid, sizeof(float), 3, fp);
	  fread(&this->set_color, sizeof(bool), 1, fp);
	  fread(this->vert, sizeof(vec), nv, fp);
	  fread(this->color, sizeof(color_vec), nv, fp);
	  fread(this->norm, sizeof(vec), nv, fp);
	  fread(this->face, sizeof(idx), nf, fp);
	  fread(this->fnorm, sizeof(vec), nf, fp);
	  fclose(fp);
	  return 0;
  }

};



#endif
