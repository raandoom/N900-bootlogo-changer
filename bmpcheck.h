#ifndef BMPCHECK_H
#define BMPCHECK_H

#include "bmp.h"
#include <QString>

#define ORIENT_DEFAULT      0
#define ORIENT_FROM_TOP     1
#define ORIENT_FROM_BOTTOM  2

#define BMP_BPP_OFF         28
#define BMP_HEIGHT_OFF      22
#define BMP_WIDTH_OFF       18
#define BMP_RAW_OFF         10
#define BMP_HEADER          0x4D42

uint bmpOrient(QString path);
bool bmpCheckFormat(QString path);
uint bmpGetBpp(QString path);
bool bmp24to16(QString srcPath, QString destPath, int orient);
bool bmp16Turn(QString srcPath, QString destPath);
bool bmpCompresRle16(QString srcPath, QString destPath);

#endif // BMPCHECK_H
