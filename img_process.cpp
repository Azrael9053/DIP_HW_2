//
// Created by user on 2023/5/24.
//

#include "img_process.h"
#include <iostream>
#include <cmath>

float data[MaxBMPSizeX * MaxBMPSizeY * 2]; // a long 1D array to keep the image and its spectrum
float sp_re[MaxBMPSizeX][MaxBMPSizeY]; // real part of the spectrum
float sp_im[MaxBMPSizeX][MaxBMPSizeY]; // imaginary part of the spectrum
double tmp[MaxBMPSizeX][MaxBMPSizeY];
float min = 1E99;
float max = -1E99;

// Function to convert RGB color values to HSV color space
void rgbToHsv(int pixel_r, int pixel_g, int pixel_b, double &h, double &s, double &v) {
    double red = pixel_r / 255.0;
    double green = pixel_g / 255.0;
    double blue = pixel_b / 255.0;

    double maxVal = std::max(red, std::max(green, blue));
    double minVal = std::min(red, std::min(green, blue));
    double diff = maxVal - minVal;

    // Calculate hue
    if (diff == 0) {
        h = 0; // undefined
    } else if (maxVal == red) {
        h = 60 * ((green - blue) / diff);
    } else if (maxVal == green) {
        h = 60 * ((blue - red) / diff + 2);
    } else if (maxVal == blue) {
        h = 60 * ((red - green) / diff + 4);
    }
    if (h < 0) {
        h += 360;
    }

    // Calculate saturation
    if (maxVal == 0) {
        s = 0;
    } else {
        s = (diff / maxVal) * 100;
    }

    // Calculate value
    v = maxVal * 100;
}

// Function to detect skin color in an RGB image using HSV color space
void skin_detect(int bmp_r[MaxBMPSizeX][MaxBMPSizeY], int bmp_g[MaxBMPSizeX][MaxBMPSizeY],
                 int bmp_b[MaxBMPSizeX][MaxBMPSizeY], int &width, int &height) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double h, s, v;
            rgbToHsv(bmp_r[x][y], bmp_g[x][y], bmp_b[x][y], h, s, v);

            // Define skin color range in HSV color space
            bool isSkinColor = (h >= 6 && h <= 45) && (s >= 10 && s <= 68) && (v >= 75 && v <= 100);

            if (isSkinColor) {
                // Set skin pixels to white (255)
                r[x][y] = 255;
                g[x][y] = 255;
                b[x][y] = 255;
            } else {
                // Set non-skin pixels to black (0)
                r[x][y] = 0;
                g[x][y] = 0;
                b[x][y] = 0;
            }
        }
    }
}

void spectrum_shift(int mm) {
    for (int i = 0; i < mm; i++) {
        for (int j = 0; j < mm; j++) {
            //------------------------ the real part
            tmp[i][j] = sp_re[i][j]; // upper-left <-> lower-right
            sp_re[i][j] = sp_re[mm + i][mm + j];
            sp_re[mm + i][mm + j] = tmp[i][j];
            tmp[i][j] = sp_re[i][mm + j]; // upper-right <-> lower-left
            sp_re[i][mm + j] = sp_re[mm + i][j];
            sp_re[mm + i][j] = tmp[i][j];
            //--------------------------- the imag. part
            tmp[i][j] = sp_im[i][j]; // upper-left <-> lower-right
            sp_im[i][j] = sp_im[mm + i][mm + j];
            sp_im[mm + i][mm + j] = tmp[i][j];
            tmp[i][j] = sp_im[i][mm + j]; // upper-right <-> lower-left
            sp_im[i][mm + j] = sp_im[mm + i][j];
            sp_im[mm + i][j] = tmp[i][j];
        }
    }
}

void Freq_Img(int bmp_r[MaxBMPSizeX][MaxBMPSizeY], int &width, int &height) {
    int i, j;
    int mm = width >> 1;
    // convert to the long 1D array
    int ptr = 0; // pointer for the data array
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            data[ptr] = bmp_r[i][j];     // real part of the input data
            data[ptr + 1] = 0;         // imaginary part of the input data
            ptr += 2;
        }
    }
    fft2(data, width, 1); // perform the forward fft2

    // fetch the 2D spectrum from the long 1-D spectrum array
    ptr = 0;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            sp_re[i][j] = data[ptr];    // real part of the 2D spectrum
            sp_im[i][j] = data[ptr + 1];  // imaginary part of the 2D spectrum
            ptr += 2;
        }
    }

    spectrum_shift(mm);

    for (j = height - 1; j > -1; j--) {
        for (i = 0; i < width; i++) {
            tmp[i][j] = log(sqrt(pow(sp_re[i][j], 2) + pow(sp_im[i][j], 2)) + 1);
            if (max < tmp[i][j]) {
                max = tmp[i][j];
            }
            if (min > tmp[i][j]) {
                min = tmp[i][j];
            }
        }
    }
    float sf;
    sf = 255 / (max - min);
    printf("%f \t %f \t %f \n", max, min, sf);
    for (j = height - 1; j > -1; j--) {
        for (i = 0; i < width; i++) {
            r[i][j] = tmp[i][j] * sf;
        }
    }
}

void Freq_Filter(int bmp_r[MaxBMPSizeX][MaxBMPSizeY], int Noise[][2], int Num, int &width, int &height) {
    int i, j, x, y;
    int mm = width >> 1;
    int n = 4;
    double D0 = 3;

    // convert to the long 1D array
    int ptr = 0; // pointer for the data array
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            data[ptr] = bmp_r[i][j];     // real part of the input data
            data[ptr + 1] = 0;         // imaginary part of the input data
            ptr += 2;
        }
    }
    fft2(data, width, 1); // perform the forward fft2

    // fetch the 2D spectrum from the long 1-D spectrum array
    ptr = 0;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            sp_re[i][j] = data[ptr];    // real part of the 2D spectrum
            sp_im[i][j] = data[ptr + 1];  // imaginary part of the 2D spectrum
            ptr += 2;
        }
    }

    spectrum_shift(mm);

    for (int N = 0; N < Num; N++) {
        x = Noise[N][1];
        y = width - Noise[N][0];
        for (j = height - 1; j > -1; j--) {
            for (i = 0; i < width; i++) {
                sp_re[i][j] *= 1 - 1 / (1 + pow((pow((x - i), 2) + pow((y - j), 2)) / pow(D0, 2), n));
            }
        }
    }

    spectrum_shift(mm);
    // convert to the long 1D array
    ptr = 0; // pointer for the data array
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            data[ptr] = sp_re[i][j];
            data[ptr + 1] = sp_im[i][j];
            ptr += 2;
        }
    }

    fft2(data, width, -1); // perform the inverse fft2

    // convert back to the 2D image
    ptr = 0; // pointer for the data array
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            sp_re[i][j] = data[ptr];
            ptr += 2;
        }
    }
    for (j = height - 1; j > -1; j--) {
        for (i = 0; i < width; i++) {
            r[i][j] = sp_re[i][j];
        }
    }
}

void erode(int bmp_r[MaxBMPSizeX][MaxBMPSizeY], int &width, int &height) {
    int i, j, u, v;
    int filter[filter_size][filter_size];
    for (i = 0; i < filter_size; i++) {
        for (j = 0; j < filter_size; j++) {
            if (pow((i - half_size), 2) + pow((j - half_size), 2) <= pow((half_size + 0.5), 2)) {
                filter[i][j] = 1;
            } else {
                filter[i][j] = 0;
            }
        }
    }
    for (j = height - 1; j > -1; j--) {
        for (i = 0; i < width; i++) {
            r[i][j] = 255;
            for (u = 0; u < filter_size && r[i][j] == 255; u++) {
                for (v = 0; v < filter_size && r[i][j] == 255; v++) {
                    if ((i + u - half_size) >= 0 && (i + u - half_size) < width && (j + v - half_size) >= 0 &&
                        (j + v - half_size) < height) {
                        if (bmp_r[i + u - half_size][j + v - half_size] < 127 && filter[u][v] == 1) {
                            r[i][j] = 0;
                        }
                    } else {
                        r[i][j] = 0;
                    }
                }
            }
        }
    }
    for(i = 0; i < width; i++) {
        for(j = 0; j < height; j++) {
            R[i][j] = r[i][j];
        }
    }
}

void dilate(int bmp_r[MaxBMPSizeX][MaxBMPSizeY], int &width, int &height) {
    int i, j, u, v;
    int filter[filter_size][filter_size];
    for (i = 0; i < filter_size; i++) {
        for (j = 0; j < filter_size; j++) {
            filter[i][j] = 1;
        }
    }
    for (j = height - 1; j > -1; j--) {
        for (i = 0; i < width; i++) {
            r[i][j] = 0;
        }
    }
    for (j = height - 1; j > -1; j--) {
        for (i = 0; i < width; i++) {
            if (bmp_r[i][j] >= 127) {
                for (u = 0; u < filter_size; u++) {
                    for (v = 0; v < filter_size; v++) {
                        if ((i + u - half_size) >= 0 && (i + u - half_size) < width && (j + v - half_size) >= 0 &&
                            (j + v - half_size) < height) {
                            r[i + u - half_size][j + v - half_size] = 255;
                        }
                    }
                }
            }
        }
    }
    for(i = 0; i < width; i++) {
        for(j = 0; j < height; j++) {
            R[i][j] = r[i][j];
        }
    }
}