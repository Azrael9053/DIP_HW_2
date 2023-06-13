/*
 * 四電機四B B10812109 朱晏廷
 * DIP_homework_2
 * main.cpp
 */
#include <iostream>

#include "bmp.h"
#include "img_process.h"

using namespace std;

int R[MaxBMPSizeX][MaxBMPSizeY]; // MaxBMPSizeX and MaxBMPSizeY are defined in "bmp.h"
int G[MaxBMPSizeX][MaxBMPSizeY];
int B[MaxBMPSizeX][MaxBMPSizeY];
int r[MaxBMPSizeX][MaxBMPSizeY];
int g[MaxBMPSizeX][MaxBMPSizeY];
int b[MaxBMPSizeX][MaxBMPSizeY];


int main(int argc, char *argv[]) {
    int width, height;
    int i, j;
    int Noise[2][2]={{123,123},{131,131}};

    open_bmp("./test images/skin_det_1.bmp", R, G, B, width, height);
    skin_detect(R, G, B, width, height);
    save_bmp("./test images/skin_det_result.bmp", r, g, b);
    close_bmp();

    open_bmp("./test images/periodic_noise3.bmp", R, G, B, width, height);
    Freq_Filter(R, Noise, 2, width, height);
    save_bmp("./test images/periodic_denoise.bmp", r, r, r);
    close_bmp();

    open_bmp("./test images/noisy_rectangle.bmp", R, G, B, width, height);
    erode(R, width, height);
    erode(R, width, height);
    erode(R, width, height);
    erode(R, width, height);
    erode(R, width, height);
    erode(R, width, height);


    dilate(R, width, height);
    dilate(R, width, height);
    dilate(R, width, height);
    dilate(R, width, height);
    dilate(R, width, height);
    dilate(R, width, height);
    save_bmp("./test images/denoisy_rectangle.bmp", r, r, r);
    close_bmp();


}