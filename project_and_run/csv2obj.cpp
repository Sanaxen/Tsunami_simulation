#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "csv2obj.h"

double CsvToObj::max[3]={-99999999.0,-99999999.0,-99999999.0};
double CsvToObj::min[3]={ 99999999.0, 99999999.0, 99999999.0};
double CsvToObj::mid[3]={ 0.0, 0.0, 0.0};
double CsvToObj::size[3]={ 99999999.0, 99999999.0, 99999999.0};
double CsvToObj::length = 5.0;
double CsvToObj::normalize_scale = 1.0;

