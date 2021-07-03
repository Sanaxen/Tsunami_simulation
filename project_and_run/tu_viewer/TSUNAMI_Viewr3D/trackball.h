/*
** 簡易トラックボール処理
*/
extern void trackballInit(void);
extern void trackballRegion(int w, int h);
extern void trackballStart(int x, int y);
extern void trackballMotion(int x, int y);
extern void trackballStop(int x, int y);
extern double *trackballRotation(void);

void trackball_drgStpos(int flg, int& x, int& y);
void trackball_Cofpos(int flg, int& x, int& y);
double* trackball_getCQ();
double* trackball_getTQ();
double* trackball_getRT();

