#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "colorconvert.h"
#include "bmpcheck.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QEvent>
#include <QProcess>
#include <QFileInfo>

#ifdef Q_WS_MAEMO_5
#define appFolder "/opt/n900-bootlogo-changer/"
#define logoDefaultPath "/opt/n900-bootlogo-changer/logo.original.bmp"
#define logoOutPath "/opt/n900-bootlogo-changer/logo.out"
#define usbDefaultPath "/opt/n900-bootlogo-changer/usb.original.bmp"
#define usbOutPath "/opt/n900-bootlogo-changer/usb.out"
#define rdDefaultPath "/opt/n900-bootlogo-changer/rd.original.bmp"
#define rdOutPath "/opt/n900-bootlogo-changer/rd.out"
#define mtdOutPath "/opt/n900-bootlogo-changer/mtd"
#define xloaderPath "/opt/n900-bootlogo-changer/xloader"
#define secondaryPath "/opt/n900-bootlogo-changer/secondary"
#define scriptPath "/opt/n900-bootlogo-changer/flashnolo.sh"
#else
#define appFolder "./"
#define logoDefaultPath "./logo.original.bmp"
#define logoOutPath "./logo.out"
#define usbDefaultPath "./usb.original.bmp"
#define usbOutPath "./usb.out"
#define rdDefaultPath "./rd.original.bmp"
#define rdOutPath "./rd.out"
#define mtdOutPath "./mtd"
#define xloaderPath "./xloader"
#define secondaryPath "./secondary"
#define scriptPath "./flashnolo.sh"
#endif

#define MAX_LOGO_SIZE 6646
#define MAX_USB_SIZE 1089
#define MAX_RD_SIZE 3954


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow()
    {
        if (shell != 0) delete shell;
        if (QFile(mtdOutPath).exists()) QFile(mtdOutPath).remove();
        if (QFile(xloaderPath).exists()) QFile(xloaderPath).remove();
        if (QFile(secondaryPath).exists()) QFile(secondaryPath).remove();

        if (QFile(logoOutPath).exists()) QFile(logoOutPath).remove();
        if (QFile(usbOutPath).exists()) QFile(usbOutPath).remove();
        if (QFile(rdOutPath).exists()) QFile(rdOutPath).remove();

        if (QFile(appFolder + QFileInfo(logoPath).fileName() + ".temp").exists())
            QFile(appFolder + QFileInfo(logoPath).fileName() + ".temp").remove();
        if (QFile(appFolder + QFileInfo(usbPath).fileName() + ".temp").exists())
            QFile(appFolder + QFileInfo(usbPath).fileName() + ".temp").remove();
        if (QFile(appFolder + QFileInfo(rdPath).fileName() + ".temp").exists())
            QFile(appFolder + QFileInfo(rdPath).fileName() + ".temp").remove();
    }

private:
    QColor bgColor;
    QString logoPath;
    bool isLogoConvreted;
    QString usbPath;
    bool isUsbConverted;
    QString rdPath;
    bool isRdConverted;

    QAction *enableAnySizes;

    QStackedWidget *stackWidget;
    QWidget *centralW;
    QVBoxLayout *rightTopL;
    QWidget *startInfoW;
    // Background frame
    QWidget *bgInfoW;
    QCheckBox *bgCheck;
    QPushButton *bgDefaultBtn;
    QComboBox *bgSetAsBox;
    QLineEdit *bgColorLine;
    QLabel *bgRedLabel;
    QSpinBox *bgRedBox;
    QLabel *bgGreenLabel;
    QSpinBox *bgGreenBox;
    QLabel *bgBlueLabel;
    QSpinBox *bgBlueBox;
    QLabel *bgColorPreview;
    QPushButton *bgSetColorBtn;
    // Logo frame
    QWidget *logoInfoW;
    QCheckBox *logoCheck;
    QPushButton *logoDefaultBtn;
    QLabel *logoFileNameLabel;
    QLabel *logoFileName;
    QLabel *logoPreview;
    QPushButton *logoSelectBtn;
    // USB frame
    QWidget *usbInfoW;
    QCheckBox *usbCheck;
    QPushButton *usbDefaultBtn;
    QLabel *usbFileNameLabel;
    QLabel *usbFileName;
    QLabel *usbPreview;
    QPushButton *usbSelectBtn;
    // RD frame
    QWidget *rdInfoW;
    QCheckBox *rdCheck;
    QPushButton *rdDefaultBtn;
    QCheckBox *rdShowOnPreview;
    bool rdShow;
    QLabel *rdFileNameLabel;
    QLabel *rdFileName;
    QLabel *rdPreview;
    QPushButton *rdSelectBtn;
    // Preview
    QWidget *previewW;
    QLabel *previewLogoLabel;
    QLabel *previewUsbLabel;
    QLabel *previewRdLabel;
    // Apply
    QWidget *applyW;
    QTextEdit *applyLog;
    QPushButton *applyCheckBtn;
    QPushButton *applyPatchBtn;
    QPushButton *applyReturn;
    // Right buttons
    QPushButton *bgBtn;
    QPushButton *logoBtn;
    QPushButton *usbBtn;
    QPushButton *rdBtn;
    QPushButton *previewBtn;
    QPushButton *applyBtn;
    QPushButton *previewClose;

    // Process
    QProcess *shell;

private slots:
    void changeImageLimit(bool state);
    void showAbout();

    bool eventFilter(QObject *obj, QEvent *event);
    // Right buttons
    void bgBtnClicked();
    void logoBtnClicked();
    void usbBtnClicked();
    void rdBtnClicked();
    void showPreview();
    void applyBtnClicked();
    // Create frames
    void createStartInfo();
    void createBgInfo();
    void createLogoInfo();
    void createUsbInfo();
    void createRdInfo();
    void createPreview();
    void createApply();
    // Background frame
    void bgSetEnabled(bool state);
    void bgSetDefault();
    void bgLineChangeRegExp(int index);
    void bgLineChanged(QString text);
    void bgRedBoxChanged(int value);
    void bgGreenBoxChanged(int value);
    void bgBlueBoxChanged(int value);
    void bgChooseColor();
    void bgSetValuesFromQColor(QColor color);
    // Logo frame
    void logoSetEnabled(bool state);
    void logoSetDefault();
    void logoSelectImage();
    // USB frame
    void usbSetEnabled(bool state);
    void usbSetDefault();
    void usbSelectImage();
    // RD frame
    void rdSetEnabled(bool state);
    void rdSetDefault();
    void rdSetShowOnPreview(bool state);
    void rdSelectImage();
    // Preview
    void closePreview();
    // Apply
    void checkAll();
    void patchNolo();
    void flashNolo();
    void closeApply();

signals:
    
public slots:
    
};

#endif // MAINWINDOW_H
