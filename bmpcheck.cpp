#include "bmpcheck.h"
#include <QFile>
#include <QDataStream>

#include <QDebug>

#define MAX_GROUP_SIZE 0x7F
#define REPEAT_MASK 0x80

// check for bmp header with rgb16 or rgb24
bool bmpCheckFormat(QString path)
{
    QFile image(path);
    if (image.open(QIODevice::ReadOnly) == false)
        return false;

    QDataStream stream(&image);
    stream.setByteOrder(QDataStream::LittleEndian);

    ushort temp;
    stream >> temp;
    if (temp != BMP_HEADER) // header
    {
        image.close();
        return false;
    }
    image.seek(BMP_BPP_OFF);
    stream >> temp;
    image.close();
    if (temp != 16 && temp != 24) // color
    {
        return false;
    }

    return true;
}
// get bmp palette
uint bmpGetBpp(QString path)
{
    QFile image(path);
    if (image.open(QIODevice::ReadOnly) == false)
        return 0;

    QDataStream stream(&image);
    stream.setByteOrder(QDataStream::LittleEndian);

    ushort temp;
    image.seek(BMP_BPP_OFF);
    stream >> temp;
    image.close();
    return temp;
}
// get bmp orient
uint bmpOrient(QString path)
{
    QFile image(path);
    image.open(QIODevice::ReadOnly);
    QDataStream stream(&image);
    stream.setByteOrder(QDataStream::LittleEndian);

    int height;

    image.seek(BMP_HEIGHT_OFF);
    stream >> height;
    image.close();

    if (height < 0) return ORIENT_FROM_TOP;
    else return ORIENT_FROM_BOTTOM;
}
// init struct
void initStruct(uchar* data, uint count)
{
    while (count !=0)
    {
        count--;
        data[count] = 0;
    }
}
// convert
bool bmp24to16(QString srcPath, QString destPath, int orient)
{
    bool turn;

    QFile readf(srcPath);
    QFile writef(destPath);
    if (readf.open(QIODevice::ReadOnly) == false)
        return false;
    if (writef.open(QIODevice::WriteOnly) == false)
    {
        readf.close();
        return false;
    }
    QDataStream streamIn(&readf);
    streamIn.setByteOrder(QDataStream::LittleEndian);
    QDataStream streamOut(&writef);
    streamOut.setByteOrder(QDataStream::LittleEndian);

    BITMAPFILEHEADER image24;
    initStruct((uchar*)&image24, 14);
    // make bmp header for 24
    streamIn >> image24.bfType;
    streamIn >> image24.bfSize;
    streamIn >> image24.bfReserved1;
    streamIn >> image24.bfReserved2;
    streamIn >> image24.bfOffBits;

    BITMAPINFOHEADER info24;
    initStruct((uchar*)&info24, 40);
    // make bmp info for 24
    streamIn >> info24.biSize;
    streamIn >> info24.biWidth;
    streamIn >> info24.biHeight;
    streamIn >> info24.biPlanes;
    streamIn >> info24.biBitCount;
    streamIn >> info24.biCompression;
    streamIn >> info24.biSizeImage;
    streamIn >> info24.biXPelsPerMeter;
    streamIn >> info24.biYPelsPerMeter;
    streamIn >> info24.biClrUsed;
    streamIn >> info24.biClrImportant;

    // check zero-padding
    int srcPadding = info24.biWidth % 4;
    int destPadding = (info24.biWidth * 2) % 4;

    BITMAPFILEHEADER image16;
    initStruct((uchar*)&image16, 14);
    // make bmp header for 16
    image16.bfOffBits = 0x42;
    image16.bfReserved2 = image24.bfReserved2;
    image16.bfReserved1 = image24.bfReserved1;
    image16.bfSize =
            ((info24.biWidth * 2) + destPadding) * qAbs(info24.biHeight) +
            image16.bfOffBits;
    image16.bfType = image24.bfType;

    BITMAPINFOHEADER info16;
    initStruct((uchar*)&info16, 40);
    // make bmp info for 16
    info16.biSize = 0x28;
    info16.biWidth = info24.biWidth;

    if ((orient == ORIENT_DEFAULT) ||
            ((orient == ORIENT_FROM_TOP) && (info24.biHeight < 0)) ||
            ((orient == ORIENT_FROM_BOTTOM) && (info24.biHeight > 0)))
    {
        turn = false;
        info16.biHeight = info24.biHeight;
    }
    else
    {
        turn = true;
        info16.biHeight = - info24.biHeight;
    }

    info16.biPlanes = 1;
    info16.biBitCount = 0x10;
    info16.biCompression = 3;
    info16.biSizeImage = image16.bfSize - image16.bfOffBits;
    info16.biXPelsPerMeter = info24.biXPelsPerMeter;
    info16.biYPelsPerMeter = info24.biYPelsPerMeter;
    info16.biClrUsed = 0;
    info16.biClrImportant = 0;

    streamOut << image16.bfType;
    streamOut << image16.bfSize;
    streamOut << image16.bfReserved1;
    streamOut << image16.bfReserved2;
    streamOut << image16.bfOffBits;

    streamOut << info16.biSize;
    streamOut << info16.biWidth;
    streamOut << info16.biHeight;
    streamOut << info16.biPlanes;
    streamOut << info16.biBitCount;
    streamOut << info16.biCompression;
    streamOut << info16.biSizeImage;
    streamOut << info16.biXPelsPerMeter;
    streamOut << info16.biYPelsPerMeter;
    streamOut << info16.biClrUsed;
    streamOut << info16.biClrImportant;
    streamOut << 0xF800; // red
    streamOut << 0x7E0; // green
    streamOut << 0x1F; // blue

    // start convert
    int h = qAbs(info24.biHeight);
    unsigned char r, g, b;
    unsigned short rgb16;

    readf.seek(image24.bfOffBits);
    if (turn)
        writef.seek(image16.bfOffBits +
                    ((info16.biWidth * 2) + destPadding) * (h - 1));
    else
        writef.seek(image16.bfOffBits);

    while (h != 0)
    {
        unsigned int w = info24.biWidth;
        while (w != 0)
        {
            streamIn >> b;
            streamIn >> g;
            streamIn >> r;
            r = r >> 3;
            g = g >> 2;
            b = b >> 3;
            rgb16 = (r << 11) + (g << 5) + b;
            streamOut << rgb16;
            w--;
        }
        if (srcPadding != 0)
        {
            unsigned char trash;
            for (int i = 0; i < srcPadding; i++) // read unneeded bytes to trash
            {
                streamIn >> trash;
            }
        }
        if (destPadding != 0)
        {
            unsigned char p = 0;
            for (int i = 0; i < destPadding; i++)
            {
                streamOut << p;
            }
        }
        h--;
        if (turn)
            writef.seek(image16.bfOffBits +
                        ((info16.biWidth * 2) + destPadding) * (h - 1));
    }
    readf.close();
    writef.close();
    return true;
}
// turn 16bpp image
bool bmp16Turn(QString srcPath, QString destPath)
{
    QFile readf(srcPath);
    QFile writef(destPath);
    if (readf.open(QIODevice::ReadOnly) == false)
        return false;
    if (writef.open(QIODevice::WriteOnly) == false)
    {
        readf.close();
        return false;
    }
    QDataStream streamIn(&readf);
    streamIn.setByteOrder(QDataStream::LittleEndian);
    QDataStream streamOut(&writef);
    streamOut.setByteOrder(QDataStream::LittleEndian);

    BITMAPFILEHEADER imageSrc;
    initStruct((uchar*)&imageSrc, 14);
    // make bmp header for src 16
    streamIn >> imageSrc.bfType;
    streamIn >> imageSrc.bfSize;
    streamIn >> imageSrc.bfReserved1;
    streamIn >> imageSrc.bfReserved2;
    streamIn >> imageSrc.bfOffBits;

    BITMAPINFOHEADER infoSrc;
    initStruct((uchar*)&infoSrc, 40);
    // make bmp info for src 16
    streamIn >> infoSrc.biSize;
    streamIn >> infoSrc.biWidth;
    streamIn >> infoSrc.biHeight;
    streamIn >> infoSrc.biPlanes;
    streamIn >> infoSrc.biBitCount;
    streamIn >> infoSrc.biCompression;
    streamIn >> infoSrc.biSizeImage;
    streamIn >> infoSrc.biXPelsPerMeter;
    streamIn >> infoSrc.biYPelsPerMeter;
    streamIn >> infoSrc.biClrUsed;
    streamIn >> infoSrc.biClrImportant;

    // check zero-padding
    int padding = (infoSrc.biWidth * 2) % 4;

    BITMAPFILEHEADER imageDest;
    initStruct((uchar*)&imageDest, 14);
    // make bmp header for dest 16
    imageDest.bfOffBits = 0x42;
    imageDest.bfReserved2 = imageSrc.bfReserved2;
    imageDest.bfReserved1 = imageSrc.bfReserved1;
    imageDest.bfSize =
            ((infoSrc.biWidth * 2) + padding) * qAbs(infoSrc.biHeight) +
            imageDest.bfOffBits;
    imageDest.bfType = imageSrc.bfType;

    BITMAPINFOHEADER infoDest;
    initStruct((uchar*)&infoDest, 40);
    // make bmp info for dest 16
    infoDest.biSize = 0x28;
    infoDest.biWidth = infoSrc.biWidth;
    infoDest.biHeight = - infoSrc.biHeight;
    infoDest.biPlanes = infoSrc.biPlanes;
    infoDest.biBitCount = infoSrc.biBitCount;
    infoDest.biCompression = 3;
    infoDest.biSizeImage = infoSrc.biSizeImage;
    infoDest.biXPelsPerMeter = infoSrc.biXPelsPerMeter;
    infoDest.biYPelsPerMeter = infoSrc.biYPelsPerMeter;
    infoDest.biClrUsed = infoSrc.biClrUsed;
    infoDest.biClrImportant = infoSrc.biClrImportant;

    streamOut << imageDest.bfType;
    streamOut << imageDest.bfSize;
    streamOut << imageDest.bfReserved1;
    streamOut << imageDest.bfReserved2;
    streamOut << imageDest.bfOffBits;

    streamOut << infoDest.biSize;
    streamOut << infoDest.biWidth;
    streamOut << infoDest.biHeight;
    streamOut << infoDest.biPlanes;
    streamOut << infoDest.biBitCount;
    streamOut << infoDest.biCompression;
    streamOut << infoDest.biSizeImage;
    streamOut << infoDest.biXPelsPerMeter;
    streamOut << infoDest.biYPelsPerMeter;
    streamOut << infoDest.biClrUsed;
    streamOut << infoDest.biClrImportant;
    streamOut << 0xF800; // red
    streamOut << 0x7E0; // green
    streamOut << 0x1F; // blue

    // start turn
    int h = qAbs(infoSrc.biHeight);
    ushort pixel;

    readf.seek(imageSrc.bfOffBits);
    writef.seek(imageDest.bfOffBits +
                ((infoDest.biWidth * 2) + padding) * (h - 1));

    while (h != 0)
    {
        uint w = infoSrc.biWidth;
        while (w != 0)
        {
            streamIn >> pixel;
            streamOut << pixel;
            w--;
        }
        if (padding != 0)
        {
            streamIn >> pixel;
            streamOut << pixel;
        }
        h--;
        writef.seek(imageDest.bfOffBits +
                    ((infoDest.biWidth * 2) + padding) * (h - 1));
    }
    readf.close();
    writef.close();
    return true;
}

//--- compress to rle
// count repeated elements
uint repeatNum(uint elements, QByteArray* src)
{
    uint num = 1; // there is minimum 2 elements already repeated
    if (elements == 2) // if there is last 2 elements
    {
        num++;
        return num;
    }

    char* data = src->data();
    while ((ushort)data[0] == (ushort)data[2])
    {
        if ((elements - num) == 1) // if this is last 2 elements
        {
            num++;
            return num;
        }
        data = data + 2;
        num++;
    }
    return num;
}
// count different elements
uint differNum(uint elements, QByteArray* src)
{
    uint num = 1; // there is minimum 2 elements already different
    if (elements == 2) // if there is last 2 elements
    {
        num++;
        return num;
    }

    char* data = src->data();
    while ((ushort)data[2] != (ushort)data[4])
        // if 2nd and 3rd elements are the same - different elements ends
    {
        if ((elements - num) == 1)
        {
            num = num + 2;
            return num;
        }
        data = data + 2;
        num++;
    }
    return num;
}
// write differ group
void differGroupWrite(uchar num, QByteArray* src, QDataStream* file)
{
    *file << num;
    while (num > 0)
    {
        *file << (uchar)src->at(1);
        *file << (uchar)src->at(0);
        src->remove(0, 2);
        num--;
    }
    return;
}

// count group for elements
uint groupNum(uint num)
{
    uint group = num / MAX_GROUP_SIZE;
    if (group * MAX_GROUP_SIZE != num)
        group++; // need 1 more group
    return group;
}
// compress
bool bmpCompresRle16(QString srcPath, QString destPath)
{
    uint width;
    int height;
    int padding;
    uint headerSize; // bmp header size
    uint elements; // number of ushorts in bmp raw
    uint group, num; // number of groups and elements in groups

    QFile srcImage(srcPath);
    if (srcImage.open(QIODevice::ReadOnly) == false)
        return false;
    QFile destImage(destPath);
    if (destImage.open(QIODevice::WriteOnly) == false)
    {
        srcImage.close();
        return false;
    }
    QDataStream srcStream(&srcImage);
    srcStream.setByteOrder(QDataStream::LittleEndian);
    QDataStream destStream(&destImage);
    destStream.setByteOrder(QDataStream::LittleEndian);

    srcImage.seek(BMP_WIDTH_OFF); // get width for count elements
    srcStream >> width;

    srcImage.seek(BMP_HEIGHT_OFF); // get height for checking sign
    srcStream >> height;
    height = qAbs(height);

    srcImage.seek(BMP_RAW_OFF); // get raw offset
    srcStream >> headerSize;
    srcImage.seek(headerSize);

    elements = width * height;
    QByteArray src = srcStream.device()->
            peek(srcImage.size() - headerSize);
    // remove zero-padding from byte array
    padding = (width * 2) % 4;
    if (padding != 0)
    {
        for (int i = 0; i < height; i++)
        {
            src.remove((width * 2) * (i + 1), padding);
        }
    }

    while (elements > 0) // when 0, there is no more elements
    {
        if (elements == 1) // if it is the last element
        {
            destStream << (uchar) 1;
            destStream << (uchar)src.at(1);
            destStream << (uchar)src.at(0);
            destStream << (uchar) 0;
            return true;
        }

        if ((ushort)src.at(0) == (ushort)src.at(2)) // if elements the same
        {
            num = repeatNum(elements, &src); // count repeated elements
            group = groupNum(num); // get group number
            if (num > MAX_GROUP_SIZE) // if there is more than 1 group
            {
                uint t_group = group;
                while (t_group > 1) // dont write last group
                {
                    destStream << (uchar)(MAX_GROUP_SIZE + REPEAT_MASK);
                    destStream << (uchar)src.at(1);
                    destStream << (uchar)src.at(0);
                    t_group--;
                }
            }
            // get and write elements of last (one) group
            destStream << (uchar)(num - (group - 1) * MAX_GROUP_SIZE + REPEAT_MASK);
            destStream << (uchar)src.at(1);
            destStream << (uchar)src.at(0);
            src.remove(0, num * 2);
        }
        else // if elements different
        {
            num = differNum(elements, &src); // count different elements
            group = groupNum(num); // get group number
            if (num > MAX_GROUP_SIZE) // if there is more than 1 group
            {
                uint t_group = group;
                while (t_group > 1) // write group by group, but not last one
                {
                    differGroupWrite(
                                MAX_GROUP_SIZE,
                                &src,
                                &destStream);
                    t_group--;
                }
                differGroupWrite(
                            (num - (group - 1) * MAX_GROUP_SIZE),
                            &src,
                            &destStream);
            }
            else // if there is one group
            {
                differGroupWrite(num, &src, &destStream);
            }
        }
        elements = elements - num;
    }

    destStream << (uchar) 0;

    srcImage.close();
    destImage.close();
    return true;
}
