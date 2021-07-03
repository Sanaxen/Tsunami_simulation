#ifndef GL_SCREENSHOT_H_INCLUDED
#define GL_SCREENSHOT_H_INCLUDED

// ビットマップのヘッダ
struct BITMAPFILEHEADER_ {
  //char bfType1;
  //char bfType2;
  unsigned long  bfSize;
  unsigned short bfReserved1;
  unsigned short bfReserved2;
  unsigned long  bfOffBits;
};

struct BITMAPINFOHEADER_ {
    unsigned long  biSize;
    long           biWidth;
    long           biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned long  biCompression;
    unsigned long  biSizeImage;
    long           biXPixPerMeter;
    long           biYPixPerMeter;
    unsigned long  biClrUsed;
    unsigned long  biClrImporant;
};

class gl_screenshot {
public:
	gl_screenshot();
	~gl_screenshot();

	bool screenshot(const char* filename, int bpp=32);
	
};

#endif
