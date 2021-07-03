#ifndef _CSVREADWRITE_HPP
#define _CSVREADWRITE_HPP

#define LINELENGMAX	(4096*100)

double* ReadCsv( char* filename, int& w, int& h);
int WriteCsv(char* filename, int w, int h, double* data, double scale);
int WriteCsv2(char* filename, int w, int h, double* data1, double* data2, double scale);
int WriteCsv(char* filename, int w, int h, int* data, double scale);

#endif
