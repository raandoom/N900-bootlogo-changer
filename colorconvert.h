#ifndef COLORCONVERT_H
#define COLORCONVERT_H
#include <QColor>

QColor convertShortToQColor (ushort rgb565);
ushort convertQColorToShort(QColor color);
QColor convertIntToQColor(uint color888);
uint convertQColorToInt(QColor color888);
QString convertShortToQString(ushort color565);
QString convertQColorToQString(QColor color888);

#endif // COLORCONVERT_H
