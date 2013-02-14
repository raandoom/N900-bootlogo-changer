#include "mainwindow.h"
#include <QDebug>

bool extractLoaderAndSecondaryFromMtd()
{
#ifdef Q_WS_MAEMO_5
    QProcess *shell = new QProcess;
    QString command = "sh -c \"echo dd if=/dev/mtd0 of=%1 | root\"";
    command = command.arg(mtdOutPath);
    shell->start(command);
    shell->waitForFinished();
    shell->close();
    delete shell;
    shell = 0;
#endif
    // get xloader and secondary
    // xloader size = 14848 bytes, from 0
    // secondary size = 109440 bytes, from xloader + 0x600

    QFile mtd (mtdOutPath);
    QFile xloader (xloaderPath);
    QFile secondary (secondaryPath);

    if (mtd.open(QIODevice::ReadOnly) == false)
    {
        // error
        return false;
    }
    if (xloader.open(QIODevice::WriteOnly) == false)
    {
        // error
        mtd.close();
        return false;
    }
    if (secondary.open(QIODevice::WriteOnly) == false)
    {
        // error
        mtd.close();
        xloader.close();
        return false;
    }

    QDataStream xloaderStream(&xloader);
    QDataStream secondaryStream(&secondary);

    QByteArray mtdArray = mtd.readAll();
    xloaderStream.writeRawData(mtdArray, 0x3A00); // 14848 bytes

    mtd.seek(0x3A00 + 0x600);
    mtdArray = mtd.readAll();
    secondaryStream.writeRawData(mtdArray, 0x1AB80); // 109440 bytes

    mtd.close();
    xloader.close();
    secondary.close();

    return true;
}


void MainWindow::patchNolo()
{
    applyPatchBtn->setDisabled(true);

    applyLog->append("");
    applyLog->append("Patching started...");

    if (!extractLoaderAndSecondaryFromMtd())
    {
        applyLog->append("Error when extracting 'xloader' and 'secondary'!");
        return;
    }

    QFile secondary (secondaryPath);
    if (secondary.open(QIODevice::ReadWrite) == false)
    {
        applyLog->append("Can't open secondary for patching!");
        return;
    }
    QByteArray secAr = secondary.readAll();


    if (bgCheck->isChecked())
    {
        // required:
        // echo -ne '\x17\xE0' | dd of=my_secondary.bin bs=1 seek=36834 conv=notrunc
        // echo -ne '\xC0\x46\x40\x4A' | dd of=my_secondary.bin bs=1 seek=36574 conv=notrunc

        secAr[36834] = 0x17;
        secAr[36835] = 0xE0;

        secAr[36574] = 0xC0;
        secAr[36575] = 0x46;
        secAr[36576] = 0x40;
        secAr[36577] = 0x4A;

        // color:
        // echo -ne '\x00\x00\x00\x00' | dd of=my_secondary.bin bs=1 seek=36836 conv=notrunc

        ushort color = convertQColorToShort(bgColor);
        secAr[36836] = (uchar)(color & 0xFF);
        secAr[36837] = (uchar)(color >> 8);
        secAr[36838] = (uchar)(color & 0xFF);
        secAr[36839] = (uchar)(color >> 8);

        // Example:
        // 0xF800 is RED, it is mean '\x00\xF8\x00\xF8'
        // 0x07E0 is GREEN, it is mean '\xE0\x07\xE0\x07'
        // 0x001F is BLUE, it is mean '\x1F\x00\x1F\x00'

        applyLog->append("Background color patched...");
    }

    if (logoCheck->isChecked())
    {
        // required:
        // echo -ne '\x5A\xA3\x59\x88\x1B\x88\xBF\xF8\x3E\x61' | dd of=my_secondary.bin bs=1 seek=33234 conv=notrunc
        // echo -ne '\x3C\x4B' | dd of=my_secondary.bin bs=1 seek=33584 conv=notrunc

        secAr[33234] = 0x5A;
        secAr[33235] = 0xA3;
        secAr[33236] = 0x59;
        secAr[33237] = 0x88;
        secAr[33238] = 0x1B;
        secAr[33239] = 0x88;
        secAr[33240] = 0xBF;
        secAr[33241] = 0xF8;
        secAr[33242] = 0x3E;
        secAr[33243] = 0x61;

        secAr[33584] = 0x3C;
        secAr[33585] = 0x4B;

        // image:
        // dd if=logo.bin of=my_secondary.bin bs=1 seek=75940 conv=notrunc

        QFile logo(logoOutPath);
        if (logo.open(QIODevice::ReadOnly) == false)
        {
            applyLog->append("Can't open compressed logo for patching!");
            secondary.close();
            return;
        }
        QByteArray logoAr = logo.readAll();
        ushort logoSize = logoAr.size();

        secAr.replace(75940, logoSize, logoAr);

        // size:
        // echo -ne '\xF6\x19' | dd of=my_secondary.bin bs=1 seek=33562 conv=notrunc

        secAr[33562] = (uchar)(logoSize & 0xFF);
        secAr[33563] = (uchar)(logoSize >> 8);

        // width:
        // echo -ne '\xE6\x01' | dd of=my_secondary.bin bs=1 seek=33596 conv=notrunc

        QImage logoImage(logoPath);

        ushort logoW = logoImage.width();
        secAr[33596] = (uchar)(logoW & 0xFF);
        secAr[33597] = (uchar)(logoW >> 8);

        // height:
        // echo -ne '\x8A\x00' | dd of=my_secondary.bin bs=1 seek=33598 conv=notrunc

        ushort logoH = logoImage.height();
        secAr[33598] = (uchar)(logoH & 0xFF);
        secAr[33599] = (uchar)(logoH >> 8);

        logo.close();

        applyLog->append("Central logo patched...");
    }

    if (usbCheck->isChecked())
    {
        // image:
        // dd if=usb.bin of=my_secondary.bin bs=1 seek=82596 conv=notrunc

        QFile usb(usbOutPath);
        if (usb.open(QIODevice::ReadOnly) == false)
        {
            applyLog->append("Can't open compressed usb for patching!");
            secondary.close();
            return;
        }
        QByteArray usbAr = usb.readAll();
        ushort usbSize = usbAr.size();

        secAr.replace(82596, usbSize, usbAr);

        // size:
        // size = 0xXYZ, this mean '\x40\xF2\xYZ\xX6'
        // echo -ne '\x40\xF2\x41\x46' | dd of=my_secondary.bin bs=1 seek=33252 conv=notrunc

        secAr[33252] = 0x40;
        secAr[33253] = 0xF2;
        secAr[33254] = (uchar)(usbSize & 0xFF);
        secAr[33255] = (uchar)(((usbSize & 0xF00) >> 4) + 6);

        // width:
        // echo -ne '\x66' | dd of=my_secondary.bin bs=1 seek=33246 conv=notrunc

        QImage usbImage(usbPath);

        uchar usbW = usbImage.width();
        secAr[33246] = usbW;

        // height:
        // echo -ne '\x2C\x21' | dd of=my_secondary.bin bs=1 seek=33250 conv=notrunc

        uchar usbH = usbImage.height();
        secAr[33250] = usbH;
        secAr[33251] = 0x21;

        usb.close();

        applyLog->append("USB icon patched...");
    }

    if (rdCheck->isChecked())
    {
        // image:
        // dd if=rd.bin of=secondary.bin bs=1 seek=71976 conv=notrunc

        QFile rd(rdOutPath);
        if (rd.open(QIODevice::ReadOnly) == false)
        {
            applyLog->append("Can't open compressed rd for patching!");
            secondary.close();
            return;
        }
        QByteArray rdAr = rd.readAll();
        ushort rdSize = rdAr.size();

        secAr.replace(71976, rdSize, rdAr);

        // size:
        // Template string: '\x40\xFk\xYZ\xX6'
        // k = 2, if size < 0x800 bytes (2048 bytes). Further, use XYZ values like in usb.
        // k = 6, if size >= 0x800 bytes (2048 bytes). In this case, value 0x800 will be added to your XYZ value.
        // Example: size = 2753 bytes = 0xAC1 bytes = (0x800 + 0x2C1) bytes, k=6, '\x40\xF6\xC1\x26'.
        // echo -ne '\x40\xF6\xC1\x26' | dd of=my_secondary.bin bs=1 seek=33226 conv=notrunc

        secAr[33226] = 0x40;

        uchar k;
        if (rdSize < 0x800) k = 2;
        else k = 6;
        secAr[33227] = (uchar)(0xF0 + k);

        secAr[33228] = (uchar)(rdSize & 0xFF);

        secAr[33229] = (uchar)(((rdSize & 0xF00) >> 4) + 6);

        // width:
        // echo -ne '\x40' | dd of=my_secondary.bin bs=1 seek=33222 conv=notrunc

        QImage rdImage(rdPath);

        uchar rdW = rdImage.width();
        secAr[33222] = rdW;

        // height:
        // echo -ne '\x36' | dd of=my_secondary.bin bs=1 seek=33224 conv=notrunc

        uchar rdH = rdImage.height();
        secAr[33224] = rdH;

        rd.close();

        applyLog->append("R&D icon patched...");
    }

    QDataStream secStream(&secondary);
    secondary.seek(0);
    secStream.writeRawData(secAr, secAr.size());
    secondary.close();

    applyLog->append("Flashing NOLO...");
    flashNolo();

    applyLog->append("Custom NOLO flashed successfully!");
}

void MainWindow::flashNolo()
{
#ifdef Q_WS_MAEMO_5
    shell = new QProcess;
    QString command = "sh -c \"echo %1 | root\"";
    command = command.arg(scriptPath);
    shell->start(command);
    shell->waitForFinished();
    shell->close();
    delete shell;
    shell = 0;
#endif
}
