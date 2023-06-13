//
// Created by user on 2023/5/24.
//

#ifndef DIP_HW2_IMG_PROCESS_H
#define DIP_HW2_IMG_PROCESS_H

#include "bmp.h"
#include "FFT.h"

#define MaxBMPSizeX    1024
#define MaxBMPSizeY    1024
#define filter_size 11
#define half_size (filter_size >> 1)

extern int r[MaxBMPSizeX][MaxBMPSizeY];
extern int g[MaxBMPSizeX][MaxBMPSizeY];
extern int b[MaxBMPSizeX][MaxBMPSizeY];
extern int R[MaxBMPSizeX][MaxBMPSizeY];
extern int G[MaxBMPSizeX][MaxBMPSizeY];
extern int B[MaxBMPSizeX][MaxBMPSizeY];

void skin_detect(int bmp_r[MaxBMPSizeX][MaxBMPSizeY], int bmp_g[MaxBMPSizeX][MaxBMPSizeY],
                 int bmp_b[MaxBMPSizeX][MaxBMPSizeY], int &width, int &height);

void Freq_Img(int bmp_r[MaxBMPSizeX][MaxBMPSizeY], int &width, int &height);

void Freq_Filter(int bmp_r[MaxBMPSizeX][MaxBMPSizeY], int Noise[][2], int Num, int &width, int &height);

void erode(int bmp_r[MaxBMPSizeX][MaxBMPSizeY], int &width, int &height);

void dilate(int bmp_r[MaxBMPSizeX][MaxBMPSizeY], int &width, int &height);

#endif //DIP_HW2_IMG_PROCESS_H
