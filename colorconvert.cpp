#include <QColor>

uchar table5[32] = \
{  0,   8,  16,  25,  33,  41,  49,  58,  66,  74,  82,  90,  99, 107, 115, 123,\
   132, 140, 148, 156, 165, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255
};
uchar table6[64] = \
{  0,   4,   8,  12,  16,  20,  24,  28,  32,  36,  40,  45,  49,  53,  57,  61,\
   65,  69,  73,  77,  81,  85,  89,  93,  97, 101, 105, 109, 113, 117, 121, 125,\
   130, 134, 138, 142, 146, 150, 154, 158, 162, 166, 170, 174, 178, 182, 186, 190,\
   194, 198, 202, 206, 210, 215, 219, 223, 227, 231, 235, 239, 243, 247, 251, 255
};
uchar convertColor8ToColor565(uchar color8,
                              uchar *table,
                              int tableSize)
{
    uchar index;
    for (index = 0; index < tableSize; index++)
    {
        if (color8 == table[index])
            return index;
        if (color8 < table[index])
        {
            ushort leftDelta = color8 - table[index - 1];
            ushort rightDelta = table[index] - color8;

            if (leftDelta < rightDelta) return index - 1;
            else return index;
        }
    }
    return index;
}
// short to qcolor
QColor convertShortToQColor (ushort rgb565)
{
    ushort redMask = 0xF800;
    ushort greenMask = 0x7E0;
    ushort blueMask = 0x1F;
    uchar red = (rgb565 & redMask) >> 11;
    uchar green = (rgb565 & greenMask) >> 5;
    uchar blue = rgb565 & blueMask;
    return QColor(table5[red], table6[green], table5[blue]);
}
// qcolor to short
ushort convertQColorToShort(QColor color)
{
    uchar red8 = color.red();
    uchar green8 = color.green();
    uchar blue8 = color.blue();
    uchar red5 = convertColor8ToColor565(red8, &table5[0], 32);
    uchar green6 = convertColor8ToColor565(green8, &table6[0], 64);
    uchar blue5 = convertColor8ToColor565(blue8, &table5[0], 32);
    return (red5 << 11) + (green6 << 5) + blue5;
}
// int to qcolor
QColor convertIntToQColor(uint color888)
{
    return QColor((color888 & 0xFF0000) >> 16,
                  (color888 & 0xFF00) >> 8,
                  color888 & 0xFF);
}
// qcolor to int
uint convertQColorToInt(QColor color888)
{
    return (color888.red() << 16) +
            (color888.green() << 8) +
            color888.blue();
}
// short to qstring
QString convertShortToQString(ushort color565)
{
    return QString("%1").
            arg(color565, 4, 16, QLatin1Char('0')).
            toUpper().prepend("0x");
}
// qcolor to qstring
QString convertQColorToQString(QColor color888)
{
    return QString("%1").
            arg(convertQColorToInt(color888), 6, 16, QLatin1Char('0')).
            toUpper().prepend("0x");
}
