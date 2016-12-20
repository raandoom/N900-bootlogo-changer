#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QMenuBar>

#ifdef Q_WS_MAEMO_5
#include <QAbstractKineticScroller>
#endif

#define sizeStep 20
#define sizeInc 10
//==============================================================================
// Create MainWindow
//==============================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    bgColor (QColor(255,255,255)),
    logoPath(logoDefaultPath),
    usbPath (usbDefaultPath),
    rdPath  (rdDefaultPath)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("BootLogo Changer for Maemo 5");

    shell = 0;

    rdShow = false;

    QMenuBar *menu = menuBar();
    enableAnySizes = new QAction("Allow any image sizes", this);
    enableAnySizes->setCheckable(true);
    connect(enableAnySizes, SIGNAL(triggered(bool)),
            this, SLOT(changeImageLimit(bool)));
    menu->addAction(enableAnySizes);
    menu->addAction("About", this, SLOT(showAbout()));
    menu->addAction("About Qt", qApp, SLOT(aboutQt()));

    ui->stack->setCurrentWidget(ui->centralPage);

    // Central/Left/Right layouts
    ui->rightTopLayout->setContentsMargins(sizeInc, 0, 0, 0);

    // Right buttons
    // Background button
    ui->bgButton->setFixedWidth(ui->bgButton->sizeHint().width() + sizeStep);
    connect(ui->bgButton, SIGNAL(clicked()), this, SLOT(bgBtnClicked()));
    ui->bgButton->installEventFilter(this);
    ui->rightTopLayout->setAlignment(ui->bgButton, Qt::AlignVCenter | Qt::AlignRight);
    // Logo button
    ui->logoButton->setFixedWidth(ui->bgButton->sizeHint().width() + sizeStep);
    connect(ui->logoButton, SIGNAL(clicked()), this, SLOT(logoBtnClicked()));
    ui->logoButton->installEventFilter(this);
    ui->rightTopLayout->setAlignment(ui->logoButton, Qt::AlignVCenter | Qt::AlignRight);
    // USB button
    ui->usbButton->setFixedWidth(ui->bgButton->sizeHint().width() + sizeStep);
    connect(ui->usbButton, SIGNAL(clicked()), this, SLOT(usbBtnClicked()));
    ui->usbButton->installEventFilter(this);
    ui->rightTopLayout->setAlignment(ui->usbButton, Qt::AlignVCenter | Qt::AlignRight);
    // RD button
    ui->rdButton->setFixedWidth(ui->bgButton->sizeHint().width() + sizeStep);
    connect(ui->rdButton, SIGNAL(clicked()), this, SLOT(rdBtnClicked()));
    ui->rdButton->installEventFilter(this);
    ui->rightTopLayout->setAlignment(ui->rdButton, Qt::AlignVCenter | Qt::AlignRight);
    // Preview button
    connect(ui->previewButton, SIGNAL(clicked()), this, SLOT(showPreview()));
    // Apply button
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applyBtnClicked()));

    // Create info widgets for buttons
    // Create Background info widget
    createBgInfo();
    // Create Logo info widget
    createLogoInfo();
    // Create USB info widget
    createUsbInfo();
    // Create RD info widget
    createRdInfo();
    // Create Preview widget
    createPreview();
    // Create Apply widget
    createApply();
}
void MainWindow::changeImageLimit(bool state)
{
    if (state)
    {
        int button =
                QMessageBox::warning(this, "WARNING!",
                                     "Use this option on your own risk!\n"
                                     "You try to DISABLE LIMITATIONS for image sizes.\n"
                                     "Default sizes is: logo - 416x72, usb - 40x40, r&d - 64x54.\n"
                                     "It will allow you use images with width and height less than default image.\n"
                                     "This images can brick your phone, but you can unbrick it with Cold Flashing (xloader.bin and secondary.bin are required).\n"
                                     "Do you realy want to DISABLE LIMITATIONS?",
                                     QMessageBox::Yes,
                                     QMessageBox::No);
        if (button == QMessageBox::Yes)
            enableAnySizes->setChecked(true);
        else if (button == QMessageBox::No)
            enableAnySizes->setChecked(false);

    }
    else
    {
        QMessageBox::warning(this, "WARNING!",
                             "Width and height limitations are enabled now.\n"
                             "With this option applicattion does not allow you to use images with sizes less than default.\n"
                             "Default sizes is: 416x72 for logo, 40x40 for usb, 64x54 for r&d.\n"
                             );
    }
}
void MainWindow::showAbout()
{
    QDialog *about = new QDialog(this);
    about->setAttribute(Qt::WA_DeleteOnClose);
    about->setWindowTitle("About");

    QVBoxLayout *aboutLayout = new QVBoxLayout;
    QLabel *appName = new QLabel("BootLogo Changer 1.0", about);
    QFont font = appName->font();
    font.setPointSize(32);
    font.setBold(true);
    appName->setFont(font);
    appName->setAlignment(Qt::AlignCenter);
    aboutLayout->addWidget(appName);

    QLabel *aboutText = new QLabel("(c) 2013 Sergey Savkin", about);
    aboutLayout->addWidget(aboutText);

    about->setLayout(aboutLayout);
    about->show();
}
//==============================================================================
// Event filter
//==============================================================================
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // if disabled Background button clicked
    //    if (obj == bgBtn && bgBtn->isEnabled() == false)
    if (obj == ui->bgButton && ui->bgButton->isChecked() == false)
    {
        if (event->type() == QEvent::MouseButtonRelease)
        {
            emit bgBtnClicked();
            return true;
        }
        else return false;
    }
    // if disabled Logo button clicked
    //    else if (obj == logoBtn && logoBtn->isEnabled() == false)
    else if (obj == ui->logoButton && ui->logoButton->isChecked() == false)
    {
        if (event->type() == QEvent::MouseButtonRelease)
        {
            emit logoBtnClicked();
            return true;
        }
        else return false;
    }
    // if disabled USB button clicked
    //    else if (obj == usbBtn && usbBtn->isEnabled() == false)
    else if (obj == ui->usbButton && ui->usbButton->isChecked() == false)
    {
        if (event->type() == QEvent::MouseButtonRelease)
        {
            emit usbBtnClicked();
            return true;
        }
        else return false;
    }
    // if RD button clicked
    //    else if (obj == rdBtn && rdBtn->isEnabled() == false)
    else if (obj == ui->rdButton && ui->rdButton->isChecked() == false)
    {
        if (event->type() == QEvent::MouseButtonRelease)
        {
            emit rdBtnClicked();
            return true;
        }
        else return false;
    }
    else if (obj == previewClose)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            closePreview();
            return true;
        }
        else return false;
    }
    else return QMainWindow::eventFilter(obj, event);
}
//==============================================================================
// Enable/Disable Buttons/Widgets by click
//==============================================================================
// Background button clicked
void MainWindow::bgBtnClicked()
{
    if (ui->bgButton->isChecked() == false)
    {
        if (ui->bgButton->width() == ui->logoButton->width() &&
                ui->logoButton->width() == ui->usbButton->width() &&
                ui->usbButton->width() == ui->rdButton->width())
        {
            // if first push
            ui->centralStack->setCurrentWidget(ui->bgInfoPage);
            ui->rightTopLayout->setContentsMargins(0,0,0,0);
            ui->bgButton->setFixedWidth(ui->bgButton->width() + sizeInc);
        }
        else
        {
            if (ui->bgButton->width() < ui->logoButton->width())
            {
                ui->logoButton->setFixedWidth(ui->logoButton->width() - sizeInc);
            }
            if (ui->bgButton->width() < ui->usbButton->width())
            {
                ui->usbButton->setFixedWidth(ui->usbButton->width() - sizeInc);
            }
            if (ui->bgButton->width() < ui->rdButton->width())
            {
                ui->rdButton->setFixedWidth(ui->rdButton->width() - sizeInc);
            }
            ui->bgButton->setFixedWidth(ui->bgButton->sizeHint().width() + sizeStep + sizeInc);
        }

        ui->logoButton->setChecked(false);
        ui->usbButton->setChecked(false);
        ui->rdButton->setChecked(false);

        ui->centralStack->setCurrentWidget(ui->bgInfoPage);
        ui->bgButton->setChecked(true);
    }
}
// Logo button clicked
void MainWindow::logoBtnClicked()
{
    if (ui->logoButton->isChecked() == false)
    {
        if (ui->bgButton->width() == ui->logoButton->width() &&
                ui->logoButton->width() == ui->usbButton->width() &&
                ui->usbButton->width() == ui->rdButton->width())
        {
            // if first push
            ui->centralStack->setCurrentWidget(ui->logoInfoPage);
            ui->rightTopLayout->setContentsMargins(0,0,0,0);
            ui->logoButton->setFixedWidth(ui->logoButton->width() + sizeInc);
        }
        else
        {
            if (ui->logoButton->width() < ui->bgButton->width())
            {
                ui->bgButton->setFixedWidth(ui->bgButton->width() - sizeInc);
            }
            if (ui->logoButton->width() < ui->usbButton->width())
            {
                ui->usbButton->setFixedWidth(ui->usbButton->width() - sizeInc);
            }
            if (ui->logoButton->width() < ui->rdButton->width())
            {
                ui->rdButton->setFixedWidth(ui->rdButton->width() - sizeInc);
            }
            ui->logoButton->setFixedWidth(ui->bgButton->sizeHint().width() + sizeStep + sizeInc);
        }

        ui->bgButton->setChecked(false);
        ui->usbButton->setChecked(false);
        ui->rdButton->setChecked(false);

        ui->centralStack->setCurrentWidget(ui->logoInfoPage);
        ui->logoButton->setChecked(true);
    }
    logoPreview->setPalette(QPalette(bgColor));
}
// USB button clicked
void MainWindow::usbBtnClicked()
{
    if (ui->usbButton->isChecked() == false)
    {
        if (ui->bgButton->width() == ui->logoButton->width() &&
                ui->logoButton->width() == ui->usbButton->width() &&
                ui->usbButton->width() == ui->rdButton->width())
        {
            // if first push
            ui->centralStack->setCurrentWidget(ui->usbInfoPage);
            ui->rightTopLayout->setContentsMargins(0,0,0,0);
            ui->usbButton->setFixedWidth(ui->usbButton->width() + sizeInc);
        }
        else
        {
            if (ui->usbButton->width() < ui->bgButton->width())
            {
                ui->bgButton->setFixedWidth(ui->bgButton->width() - sizeInc);
            }
            if (ui->usbButton->width() < ui->logoButton->width())
            {
                ui->logoButton->setFixedWidth(ui->logoButton->width() - sizeInc);
            }
            if (ui->usbButton->width() < ui->rdButton->width())
            {
                ui->rdButton->setFixedWidth(ui->rdButton->width() - sizeInc);
            }
            ui->usbButton->setFixedWidth(ui->bgButton->sizeHint().width() + sizeStep + sizeInc);
        }

        ui->bgButton->setChecked(false);
        ui->logoButton->setChecked(false);
        ui->rdButton->setChecked(false);

        ui->centralStack->setCurrentWidget(ui->usbInfoPage);
        ui->usbButton->setChecked(true);
    }
    usbPreview->setPalette(QPalette(bgColor));
}
// RD button clicked
void MainWindow::rdBtnClicked()
{
    if (ui->rdButton->isChecked() == false)
    {
        if (ui->bgButton->width() == ui->logoButton->width() &&
                ui->logoButton->width() == ui->usbButton->width() &&
                ui->usbButton->width() == ui->rdButton->width())
        {
            // if first push
            ui->centralStack->setCurrentWidget(ui->rdInfoPage);
            ui->rightTopLayout->setContentsMargins(0,0,0,0);
            ui->rdButton->setFixedWidth(ui->rdButton->width() + sizeInc);
        }
        else
        {
            if (ui->rdButton->width() < ui->bgButton->width())
            {
                ui->bgButton->setFixedWidth(ui->bgButton->width() - sizeInc);
            }
            if (ui->rdButton->width() < ui->logoButton->width())
            {
                ui->logoButton->setFixedWidth(ui->logoButton->width() - sizeInc);
            }
            if (ui->rdButton->width() < ui->usbButton->width())
            {
                ui->usbButton->setFixedWidth(ui->usbButton->width() - sizeInc);
            }
            ui->rdButton->setFixedWidth(ui->bgButton->sizeHint().width() + sizeStep + sizeInc);
        }

        ui->bgButton->setChecked(false);
        ui->logoButton->setChecked(false);
        ui->usbButton->setChecked(false);

        ui->centralStack->setCurrentWidget(ui->rdInfoPage);
        ui->rdButton->setChecked(true);
    }
    rdPreview->setPalette(QPalette(bgColor));
}
// Preview button clicked
void MainWindow::showPreview()
{
    ui->previewPage->setPalette(QPalette(QColor(bgColor)));
    previewLogoLabel->setPixmap(QPixmap::fromImage(QImage(logoPath)));
    previewLogoLabel->resize(previewLogoLabel->sizeHint());
    previewLogoLabel->move((ui->applyPage->width() - previewLogoLabel->width()) / 2,
                           (ui->previewPage->height() - previewLogoLabel->height()) / 2);
    previewUsbLabel->setPixmap(QPixmap::fromImage(QImage(usbPath)));
    previewUsbLabel->resize(previewUsbLabel->sizeHint());
    previewUsbLabel->move(ui->previewPage->width() - previewUsbLabel->width() - 14, 14);
    if (rdShow)
    {
        previewRdLabel->setPixmap(QPixmap::fromImage(QImage(rdPath)));
        previewRdLabel->resize(previewRdLabel->sizeHint());
        previewRdLabel->move((ui->previewPage->width() - previewRdLabel->width()) / 2,
                             ui->previewPage->height() - previewRdLabel->height() - 140);
    }
    else previewRdLabel->clear();
#ifdef Q_WS_MAEMO_5
    showFullScreen();
#endif
    ui->stack->setCurrentWidget(ui->previewPage);
}
// Apply button clicked
void MainWindow::applyBtnClicked()
{
#ifdef Q_WS_MAEMO_5
    showFullScreen();
#endif
    ui->stack->setCurrentWidget(ui->applyPage);
}
//==============================================================================
// Create frames
//==============================================================================
//--- Background ---------------------------------------------------------------
void MainWindow::createBgInfo()
{
    QVBoxLayout *bgInfoL = new QVBoxLayout;
    ui->bgInfoPage->setLayout(bgInfoL);
    QHBoxLayout *bgInfoTopL = new QHBoxLayout;
    bgInfoTopL->setSpacing(0);
    bgInfoTopL->setMargin(0);
    bgInfoL->addLayout(bgInfoTopL);

    bgCheck = new QCheckBox("Change background color", ui->bgInfoPage);
    bgCheck->setFixedWidth(bgCheck->sizeHint().width());
    connect(bgCheck, SIGNAL(clicked(bool)), this, SLOT(bgSetEnabled(bool)));
    bgInfoTopL->addWidget(bgCheck);
    bgDefaultBtn = new QPushButton("Set default", ui->bgInfoPage);
    connect(bgDefaultBtn, SIGNAL(clicked()), this, SLOT(bgSetDefault()));
    bgInfoTopL->addWidget(bgDefaultBtn);

    bgSetAsBox = new QComboBox(ui->bgInfoPage);
    bgSetAsBox->addItem("Show color in HEX as RGB16");
    bgSetAsBox->addItem("Show color in HEX as RGB24");
    bgSetAsBox->addItem("Show color in DEC as RGB24");
    bgSetAsBox->setCurrentIndex(2);
    connect(bgSetAsBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(bgLineChangeRegExp(int)));
    bgInfoL->addWidget(bgSetAsBox);

    QHBoxLayout *bgColorEditLayout = new QHBoxLayout;
    bgColorLine = new QLineEdit(ui->bgInfoPage);
    bgColorLine->setText(convertShortToQString(convertQColorToShort(bgColor)));
    connect(bgColorLine, SIGNAL(textChanged(QString)),
            this, SLOT(bgLineChanged(QString)));
    bgColorEditLayout->addWidget(bgColorLine);

    bgRedBox = new QSpinBox(ui->bgInfoPage);
    bgRedBox->setRange(0, 255);
    bgRedBox->setValue(255);
    bgRedBox->setWrapping(true);
    bgRedBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    connect(bgRedBox, SIGNAL(valueChanged(int)),
            this, SLOT(bgRedBoxChanged(int)));
    bgRedBox->hide();
    bgGreenBox = new QSpinBox(ui->bgInfoPage);
    bgGreenBox->setRange(0, 255);
    bgGreenBox->setValue(255);
    bgGreenBox->setWrapping(true);
    bgGreenBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    connect(bgGreenBox, SIGNAL(valueChanged(int)),
            this, SLOT(bgGreenBoxChanged(int)));
    bgGreenBox->hide();
    bgBlueBox = new QSpinBox(ui->bgInfoPage);
    bgBlueBox->setRange(0, 255);
    bgBlueBox->setValue(255);
    bgBlueBox->setWrapping(true);
    bgBlueBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    connect(bgBlueBox, SIGNAL(valueChanged(int)),
            this, SLOT(bgBlueBoxChanged(int)));
    bgBlueBox->hide();

    bgRedLabel = new QLabel("R:", ui->bgInfoPage);
    bgRedLabel->setFixedWidth(bgRedLabel->sizeHint().width());
    bgRedLabel->hide();
    bgColorEditLayout->addWidget(bgRedLabel);
    bgColorEditLayout->addWidget(bgRedBox);

    bgGreenLabel = new QLabel("G:", ui->bgInfoPage);
    bgGreenLabel->setFixedWidth(bgGreenLabel->sizeHint().width());
    bgGreenLabel->hide();
    bgColorEditLayout->addWidget(bgGreenLabel);
    bgColorEditLayout->addWidget(bgGreenBox);

    bgBlueLabel = new QLabel("B:", ui->bgInfoPage);
    bgBlueLabel->setFixedWidth(bgBlueLabel->sizeHint().width());
    bgBlueLabel->hide();
    bgColorEditLayout->addWidget(bgBlueLabel);
    bgColorEditLayout->addWidget(bgBlueBox);

    bgInfoL->addLayout(bgColorEditLayout);

    QFrame *bgColorPreviewFrame = new QFrame(ui->bgInfoPage);
    bgColorPreviewFrame->setFrameStyle(QFrame::StyledPanel);

    QVBoxLayout *bgColorPreviewLayout = new QVBoxLayout;
    bgColorPreviewLayout->setSpacing(0);
    bgColorPreviewLayout->setMargin(0);
    bgColorPreviewFrame->setLayout(bgColorPreviewLayout);

    bgColorPreview = new QLabel(ui->bgInfoPage);
    bgColorPreview->setAutoFillBackground(true);
    bgColorPreview->setPalette(QPalette(bgColor));
    bgColorPreviewLayout->addWidget(bgColorPreview);

    bgInfoL->addWidget(bgColorPreviewFrame);

    bgSetColorBtn = new QPushButton("Choose color", ui->bgInfoPage);
    connect(bgSetColorBtn, SIGNAL(clicked()), this, SLOT(bgChooseColor()));
    bgInfoL->addWidget(bgSetColorBtn);

    bgLineChangeRegExp(bgSetAsBox->currentIndex());
    bgSetEnabled(false);
}
// enable/disable
void MainWindow::bgSetEnabled(bool state)
{
    bgDefaultBtn->setEnabled(state);
    bgSetAsBox->setEnabled(state);
    bgColorLine->setEnabled(state);
    bgRedLabel->setEnabled(state);
    bgRedBox->setEnabled(state);
    bgGreenLabel->setEnabled(state);
    bgGreenBox->setEnabled(state);
    bgBlueLabel->setEnabled(state);
    bgBlueBox->setEnabled(state);
    bgColorPreview->setEnabled(state);
    bgSetColorBtn->setEnabled(state);
}
// set default
void MainWindow::bgSetDefault()
{
    bgColor = QColor(255,255,255);
    bgSetValuesFromQColor(bgColor);
}
// set line regexp
void MainWindow::bgLineChangeRegExp(int index)
{
    QRegExpValidator *validator = new QRegExpValidator(bgColorLine);
    switch (index)
    {
    case 0 : // hex rgb16
    {
        bgRedLabel->hide();
        bgRedBox->hide();
        bgGreenLabel->hide();
        bgGreenBox->hide();
        bgBlueLabel->hide();
        bgBlueBox->hide();
        bgColorLine->show();
        validator->setRegExp(QRegExp("0x[0-9A-Fa-f]{1,4}"));
        bgColorLine->setValidator(validator);
        bgColorLine->setText(convertShortToQString(
                                 convertQColorToShort(bgColor)));
        break;
    }
    case 1 : // hex rgb24
    {
        bgRedLabel->hide();
        bgRedBox->hide();
        bgGreenLabel->hide();
        bgGreenBox->hide();
        bgBlueLabel->hide();
        bgBlueBox->hide();
        bgColorLine->show();
        validator->setRegExp(QRegExp("0x[0-9A-Fa-f]{1,6}"));
        bgColorLine->setValidator(validator);
        bgColorLine->setText(convertQColorToQString(bgColor));
        break;
    }
    case 2 : // dec pallete
    {
        bgColorLine->hide();
        bgRedLabel->show();
        bgRedBox->show();
        bgGreenLabel->show();
        bgGreenBox->show();
        bgBlueLabel->show();
        bgBlueBox->show();
        bgRedBox->setValue(bgColor.red());
        bgGreenBox->setValue(bgColor.green());
        bgBlueBox->setValue(bgColor.blue());
        break;
    }
    }

}
// set color from line
void MainWindow::bgLineChanged(QString text)
{
    switch (bgSetAsBox->currentIndex())
    {
    case 0 :
    {
        bgColor = convertShortToQColor(text.toUShort(0,16));
        break;
    }
    case 1 :
    {
        bgColor = convertIntToQColor(text.toUInt(0,16));
        break;
    }
    case 2 :
    {
        break;
    }
    }
    bgColorPreview->setPalette(QPalette(bgColor));
}
// set color from spinboxes
void MainWindow::bgRedBoxChanged(int value)
{
    bgColor.setRed(value);
    bgColorPreview->setPalette(QPalette(bgColor));
}
void MainWindow::bgGreenBoxChanged(int value)
{
    bgColor.setGreen(value);
    bgColorPreview->setPalette(QPalette(bgColor));
}
void MainWindow::bgBlueBoxChanged(int value)
{
    bgColor.setBlue(value);
    bgColorPreview->setPalette(QPalette(bgColor));
}
// choose color
void MainWindow::bgChooseColor()
{
    QColor color =
            QColorDialog::getColor(bgColor, ui->bgInfoPage, "Choose background color");
    if (color.isValid() == false)
        return;

    bgColor = color;
    bgSetValuesFromQColor(bgColor);
}
// set values from color
void MainWindow::bgSetValuesFromQColor(QColor color)
{
    bgColorPreview->setPalette(QPalette(color));
    switch (bgSetAsBox->currentIndex())
    {
    case 0 :
    {
        bgColorLine->
                setText(convertShortToQString(convertQColorToShort(color)));
        break;
    }
    case 1 :
    {
        bgColorLine->setText(convertQColorToQString(color));
        break;
    }
    case 2 :
    {
        bgRedBox->setValue(color.red());
        bgGreenBox->setValue(color.green());
        bgBlueBox->setValue(color.blue());
    }
    }
}
//--- Logo ---------------------------------------------------------------------
void MainWindow::createLogoInfo()
{
    QVBoxLayout *logoInfoL = new QVBoxLayout;
    ui->logoInfoPage->setLayout(logoInfoL);
    QHBoxLayout *logoInfoTopL = new QHBoxLayout;
    logoInfoTopL->setSpacing(0);
    logoInfoTopL->setMargin(0);
    logoInfoL->addLayout(logoInfoTopL);

    logoCheck = new QCheckBox("Change central logo", ui->logoInfoPage);
    logoCheck->setFixedWidth(bgCheck->width());
    connect(logoCheck, SIGNAL(clicked(bool)), this, SLOT(logoSetEnabled(bool)));
    logoInfoTopL->addWidget(logoCheck);
    logoDefaultBtn = new QPushButton("Set default", ui->logoInfoPage);
    connect(logoDefaultBtn, SIGNAL(clicked()), this, SLOT(logoSetDefault()));
    logoInfoTopL->addWidget(logoDefaultBtn);

    QHBoxLayout *logoFileNameLayout = new QHBoxLayout;
    logoFileNameLabel = new QLabel("File name: ", ui->logoInfoPage);
    logoFileNameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    logoFileName = new QLabel(QFileInfo(logoPath).fileName(), ui->logoInfoPage);
    logoFileName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QFont italic = logoFileName->font();
    italic.setItalic(true);
    logoFileName->setFont(italic);
    logoFileNameLayout->addWidget(logoFileNameLabel);
    logoFileNameLayout->addWidget(logoFileName);

    logoInfoL->addLayout(logoFileNameLayout);

    QFrame *logoPreviewFrame = new QFrame(ui->logoInfoPage);
    logoPreviewFrame->setFrameStyle(QFrame::StyledPanel);

    QVBoxLayout *logoPreviewLayout = new QVBoxLayout;
    logoPreviewLayout->setSpacing(0);
    logoPreviewLayout->setMargin(0);
    logoPreviewFrame->setLayout(logoPreviewLayout);

    logoPreview = new QLabel(ui->logoInfoPage);
    logoPreview->setAutoFillBackground(true);
    logoPreview->setPalette(QPalette(bgColor));
    logoPreview->setAlignment(Qt::AlignCenter);
    logoPreview->setPixmap(QPixmap::fromImage(QImage(logoPath)));
    logoPreviewLayout->addWidget(logoPreview);

    logoInfoL->addWidget(logoPreviewFrame);

    logoSelectBtn = new QPushButton("Choose image", ui->logoInfoPage);
    connect(logoSelectBtn, SIGNAL(clicked()), this, SLOT(logoSelectImage()));
    logoInfoL->addWidget(logoSelectBtn);

    logoSetEnabled(false);
}
// enable/disable
void MainWindow::logoSetEnabled(bool state)
{
    logoDefaultBtn->setEnabled(state);
    logoFileNameLabel->setEnabled(state);
    logoFileName->setEnabled(state);
    logoPreview->setEnabled(state);
    logoSelectBtn->setEnabled(state);
}
// set default
void MainWindow::logoSetDefault()
{
    logoPath = logoDefaultPath;
    logoPreview->setPixmap(QPixmap::fromImage(QImage(logoPath)));
}
// choose logo image
void MainWindow::logoSelectImage()
{
    QString tempPath =
            QFileDialog::getOpenFileName(ui->logoInfoPage,
                                         "Select *.bmp (with rgb565 or rgb888):",
                                         QDir::homePath()
                                     #ifdef Q_WS_MAEMO_5
                                         + "/MyDocuments"
                                     #endif
                                         ,
                                         "*.bmp"
                                         );
    if (tempPath.isEmpty())
        return;

    if (bmpCheckFormat(tempPath) == false)
    {
        QMessageBox::warning(ui->logoInfoPage, "Error!",
                             QFileInfo(tempPath).fileName() +
                             " is not a *.bmp image with "
                             "16bit (rgb565) or 24bit (rgb888) palette.");
        return;
    }

    QImage logo(tempPath);
    if (enableAnySizes->isChecked() == false)
    {
        if ((logo.width() < 416) || (logo.height() < 72))
        {
            QMessageBox::warning(this, "Error!",
                                 "Selected image has width or/and height less than default image!\n"
                                 "Default size for logo is 416x72.\n"
                                 "If you want to use this image anyway - enable any image sizes from the menu bar.");
            return;
        }
    }

    logoPath = tempPath;
    logoPreview->setPixmap(QPixmap::fromImage(logo));
    logoFileName->setText(QFileInfo(logoPath).fileName());
}
//--- USB ----------------------------------------------------------------------
void MainWindow::createUsbInfo()
{
    QVBoxLayout *usbInfoL = new QVBoxLayout;
    ui->usbInfoPage->setLayout(usbInfoL);
    QHBoxLayout *usbInfoTopL = new QHBoxLayout;
    usbInfoTopL->setSpacing(0);
    usbInfoTopL->setMargin(0);
    usbInfoL->addLayout(usbInfoTopL);

    usbCheck = new QCheckBox("Change USB icon", ui->usbInfoPage);
    usbCheck->setFixedWidth(bgCheck->width());
    connect(usbCheck, SIGNAL(clicked(bool)), this, SLOT(usbSetEnabled(bool)));
    usbInfoTopL->addWidget(usbCheck);
    usbDefaultBtn = new QPushButton("Set default", ui->usbInfoPage);
    connect(usbDefaultBtn, SIGNAL(clicked()), this, SLOT(usbSetDefault()));
    usbInfoTopL->addWidget(usbDefaultBtn);

    QHBoxLayout *usbFileNameLayout = new QHBoxLayout;
    usbFileNameLabel = new QLabel("File name: ", ui->usbInfoPage);
    usbFileNameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    usbFileName = new QLabel(QFileInfo(usbPath).fileName(), ui->usbInfoPage);
    usbFileName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QFont italic = usbFileName->font();
    italic.setItalic(true);
    usbFileName->setFont(italic);
    usbFileNameLayout->addWidget(usbFileNameLabel);
    usbFileNameLayout->addWidget(usbFileName);

    usbInfoL->addLayout(usbFileNameLayout);

    QFrame *usbPreviewFrame = new QFrame(ui->usbInfoPage);
    usbPreviewFrame->setFrameStyle(QFrame::StyledPanel);

    QVBoxLayout *usbPreviewLayout = new QVBoxLayout;
    usbPreviewLayout->setSpacing(0);
    usbPreviewLayout->setMargin(0);
    usbPreviewFrame->setLayout(usbPreviewLayout);

    usbPreview = new QLabel(ui->usbInfoPage);
    usbPreview->setAutoFillBackground(true);
    usbPreview->setPalette(QPalette(bgColor));
    usbPreview->setAlignment(Qt::AlignCenter);
    usbPreview->setPixmap(QPixmap::fromImage(QImage(usbPath)));
    usbPreviewLayout->addWidget(usbPreview);

    usbInfoL->addWidget(usbPreviewFrame);

    usbSelectBtn = new QPushButton("Choose image", ui->usbInfoPage);
    connect(usbSelectBtn, SIGNAL(clicked()), this, SLOT(usbSelectImage()));
    usbInfoL->addWidget(usbSelectBtn);

    usbSetEnabled(false);
}
// enable / disable
void MainWindow::usbSetEnabled(bool state)
{
    usbDefaultBtn->setEnabled(state);
    usbFileNameLabel->setEnabled(state);
    usbFileName->setEnabled(state);
    usbPreview->setEnabled(state);
    usbSelectBtn->setEnabled(state);
}
// set default
void MainWindow::usbSetDefault()
{
    usbPath = usbDefaultPath;
    usbPreview->setPixmap(QPixmap::fromImage(QImage(usbPath)));
}
// choose logo image
void MainWindow::usbSelectImage()
{
    QString tempPath =
            QFileDialog::getOpenFileName(ui->usbInfoPage,
                                         "Select *.bmp (with rgb565 or rgb888):",
                                         QDir::homePath()
                                     #ifdef Q_WS_MAEMO_5
                                         + "/MyDocuments"
                                     #endif
                                         ,
                                         "*.bmp"
                                         );
    if (tempPath.isEmpty())
        return;

    if (bmpCheckFormat(tempPath) == false)
    {
        QMessageBox::warning(ui->usbInfoPage, "Error!",
                             QFileInfo(tempPath).fileName() +
                             " is not a *.bmp image with "
                             "16bit (rgb565) or 24bit (rgb888) palette.");
        return;
    }

    QImage usb(tempPath);
    if (enableAnySizes->isChecked() == false)
    {
        if ((usb.width() < 40) || (usb.height() < 40))
        {
            QMessageBox::warning(this, "Error!",
                                 "Selected image has width or/and height less than default image!\n"
                                 "Default size for usb is 40x40.\n"
                                 "If you want to use this image anyway - enable any image sizes from the menu bar.");
            return;
        }
    }

    usbPath = tempPath;
    usbPreview->setPixmap(QPixmap::fromImage(usb));
    usbFileName->setText(QFileInfo(usbPath).fileName());
}
//--- RD -----------------------------------------------------------------------
void MainWindow::createRdInfo()
{
    QVBoxLayout *rdInfoL = new QVBoxLayout;
    ui->rdInfoPage->setLayout(rdInfoL);
    QHBoxLayout *rdInfoTopL = new QHBoxLayout;
    rdInfoTopL->setSpacing(0);
    rdInfoTopL->setMargin(0);
    rdInfoL->addLayout(rdInfoTopL);

    rdCheck = new QCheckBox("Change R&D icon", ui->rdInfoPage);
    rdCheck->setFixedWidth(bgCheck->width());
    connect(rdCheck, SIGNAL(clicked(bool)), this, SLOT(rdSetEnabled(bool)));
    rdInfoTopL->addWidget(rdCheck);
    rdDefaultBtn = new QPushButton("Set default", ui->rdInfoPage);
    connect(rdDefaultBtn, SIGNAL(clicked()), this, SLOT(rdSetDefault()));
    rdInfoTopL->addWidget(rdDefaultBtn);

    rdShowOnPreview = new QCheckBox("Show on preview", ui->rdInfoPage);
    connect(rdShowOnPreview, SIGNAL(clicked(bool)),
            this, SLOT(rdSetShowOnPreview(bool)));
    rdInfoL->addWidget(rdShowOnPreview);

    QHBoxLayout *rdFileNameLayout = new QHBoxLayout;
    rdFileNameLabel = new QLabel("File name: ", ui->rdInfoPage);
    rdFileNameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    rdFileName = new QLabel(QFileInfo(rdPath).fileName(), ui->rdInfoPage);
    rdFileName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QFont italic = rdFileName->font();
    italic.setItalic(true);
    rdFileName->setFont(italic);
    rdFileNameLayout->addWidget(rdFileNameLabel);
    rdFileNameLayout->addWidget(rdFileName);

    rdInfoL->addLayout(rdFileNameLayout);

    QFrame *rdPreviewFrame = new QFrame(ui->rdInfoPage);
    rdPreviewFrame->setFrameStyle(QFrame::StyledPanel);

    QVBoxLayout *rdPreviewLayout = new QVBoxLayout;
    rdPreviewLayout->setSpacing(0);
    rdPreviewLayout->setMargin(0);
    rdPreviewFrame->setLayout(rdPreviewLayout);

    rdPreview = new QLabel(ui->rdInfoPage);
    rdPreview->setAutoFillBackground(true);
    rdPreview->setPalette(QPalette(bgColor));
    rdPreview->setAlignment(Qt::AlignCenter);
    rdPreview->setPixmap(QPixmap::fromImage(QImage(rdPath)));
    rdPreviewLayout->addWidget(rdPreview);

    rdInfoL->addWidget(rdPreviewFrame);

    rdSelectBtn = new QPushButton("Choose image", ui->rdInfoPage);
    connect(rdSelectBtn, SIGNAL(clicked()), this, SLOT(rdSelectImage()));
    rdInfoL->addWidget(rdSelectBtn);

    rdSetEnabled(false);
}
// enable/ disable
void MainWindow::rdSetEnabled(bool state)
{
    rdDefaultBtn->setEnabled(state);
    rdFileNameLabel->setEnabled(state);
    rdFileName->setEnabled(state);
    rdPreview->setEnabled(state);
    rdSelectBtn->setEnabled(state);
}
// set default
void MainWindow::rdSetDefault()
{
    rdPath = rdDefaultPath;
    rdPreview->setPixmap(QPixmap::fromImage((QImage(rdPath))));
}
// choose rd image
void MainWindow::rdSelectImage()
{
    QString tempPath =
            QFileDialog::getOpenFileName(ui->rdInfoPage,
                                         "Select *.bmp (with rgb565 or rgb888):",
                                         QDir::homePath()
                                     #ifdef Q_WS_MAEMO_5
                                         + "/MyDocuments"
                                     #endif
                                         ,
                                         "*.bmp"
                                         );
    if (tempPath.isEmpty())
        return;

    if (bmpCheckFormat(tempPath) == false)
    {
        QMessageBox::warning(ui->rdInfoPage, "Error!",
                             QFileInfo(tempPath).fileName() +
                             " is not a *.bmp image with "
                             "16bit (rgb565) or 24bit (rgb888) palette.");
        return;
    }

    QImage rd(tempPath);
    if (enableAnySizes->isChecked() == false)
    {
        if ((rd.width() < 64) || (rd.height() < 54))
        {
            QMessageBox::warning(this, "Error!",
                                 "Selected image has width or/and height less than default image!\n"
                                 "Default size for r&d is 64x54.\n"
                                 "If you want to use this image anyway - enable any image sizes from the menu bar.");
            return;
        }
    }

    rdPath = tempPath;
    rdPreview->setPixmap(QPixmap::fromImage(rd));
    rdFileName->setText(QFileInfo(rdPath).fileName());
}

void MainWindow::rdSetShowOnPreview(bool state)
{
    rdShow = state;
}
//--- Preview ------------------------------------------------------------------
// create preview
void MainWindow::createPreview()
{
    ui->previewPage->setFixedSize(800, 480);
    ui->previewPage->setAutoFillBackground(true);
    ui->previewPage->setPalette(QPalette(QColor(bgColor)));
    previewClose = new QPushButton("Tap here to close preview...", ui->previewPage);
    previewClose->installEventFilter(this);
    previewClose->setFixedSize(previewClose->sizeHint());
    previewClose->move((ui->previewPage->width() - previewClose->width()) / 2,
                       ui->previewPage->height() - previewClose->height());
    previewClose->setFlat(true);
    previewClose->setAutoFillBackground(true);

    previewLogoLabel = new QLabel(ui->previewPage);
    QImage logoImage;

    logoImage = QImage(logoPath);

    previewLogoLabel->setPixmap(QPixmap::fromImage(logoImage));
    previewLogoLabel->resize(previewLogoLabel->sizeHint());
    previewLogoLabel->move((ui->previewPage->width() - previewLogoLabel->width()) / 2,
                           (ui->previewPage->height() - previewLogoLabel->height()) / 2);

    previewUsbLabel = new QLabel(ui->previewPage);
    QImage usbImage;

    usbImage = QImage(usbPath);

    previewUsbLabel->setPixmap(QPixmap::fromImage(usbImage));
    previewUsbLabel->resize(previewUsbLabel->sizeHint());
    previewUsbLabel->move(ui->previewPage->width() - previewUsbLabel->width() - 14, 14);

    previewRdLabel = new QLabel(ui->previewPage);
    QImage rdImage;

    rdImage = QImage(rdPath);

    previewRdLabel->setPixmap(QPixmap::fromImage(rdImage));
    previewRdLabel->resize(previewRdLabel->sizeHint());
    previewRdLabel->move((ui->previewPage->width() - previewRdLabel->width()) / 2,
                         ui->previewPage->height() - previewRdLabel->height() - 140);

}
// close preview window
void MainWindow::closePreview()
{
    showNormal();
    ui->stack->setCurrentWidget(ui->centralPage);
}
//--- Apply --------------------------------------------------------------------
// create apply
void MainWindow::createApply()
{
    applyLog = new QTextEdit(ui->applyPage);
    applyLog->setCursorWidth(0);
    applyLog->setTextInteractionFlags(Qt::TextSelectableByMouse);
    applyLog->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ui->scrollArea->setWidget(applyLog);
    ui->scrollArea->setWidgetResizable(true);

#ifdef Q_WS_MAEMO_5
    QAbstractKineticScroller *scroller;
    scroller = applyLog->property("kineticScroller").value<QAbstractKineticScroller*>();
    scroller->setMode(QAbstractKineticScroller::PushMode);
    scroller->setOvershootPolicy(QAbstractKineticScroller::OvershootAlwaysOff);
    scroller->setEnabled(true);
#endif

    connect(ui->applyCheckBtn, SIGNAL(clicked()), this, SLOT(checkAll()));
    connect(ui->applyPatchBtn, SIGNAL(clicked()), this, SLOT(patchNolo()));
    connect(ui->applyReturnBtn, SIGNAL(clicked()), this, SLOT(closeApply()));
}

void MainWindow::checkAll()
{
    ui->applyCheckBtn->setDisabled(true);

    uint bpp;
    QString tempPath;
    // if nothing
    if (!bgCheck->isChecked() &&
            !logoCheck->isChecked() &&
            !usbCheck->isChecked() &&
            !rdCheck->isChecked())
    {
        applyLog->append("There is no selected items for applying.");
        applyLog->append("Noyhing to do, return and try again.");
        return;
    }
    // bg
    if (bgCheck->isChecked())
    {
        applyLog->append(QString("<b>Background color:</b> #").
                         append(QString("%1%2%3").
                                arg(bgColor.red(),2,16,QLatin1Char('0')).
                                arg(bgColor.green(),2,16,QLatin1Char('0')).
                                arg(bgColor.blue(),2,16,QLatin1Char('0')).toUpper()));
    }
    else
    {
        applyLog->append("<b>Background color:</b> disabled.");
    }
    // logo
    if (logoCheck->isChecked())
    {
        bool converted = false;

        applyLog->append("<b>Logo image:</b> " + logoPath);
        bpp = bmpGetBpp(logoPath);
        if (bpp == 16) // if 16bpp
        {
            converted = false;
            applyLog->append("bpp: 16b");
        }
        else if (bpp == 24) // if 24bpp
            applyLog->append("bpp: 24b - will be converted to 16b");
        else
        {
            applyLog->append("error: logo image is not 16bpp or 24bpp");
            applyLog->append("return and try to use correct image");
            return;
        }

        if (bpp == 24) // convert if ok and 24bpp
        {
            applyLog->append("converting logo...");
            tempPath = appFolder + QFileInfo(logoPath).fileName() + ".temp";
            if (bmp24to16(logoPath, tempPath, ORIENT_FROM_TOP))
            {
                converted = true;
                applyLog->append("logo successfuly converted to 16bpp");
            }
            else
            {
                applyLog->append("error when converting logo to 16bpp");
                applyLog->append("return and try to fix problem");
                return;
            }
        }
        else if (bmpOrient(logoPath) != ORIENT_FROM_TOP) // check orient for 16bpp image
        {
            converted = true;
            tempPath = appFolder + QFileInfo(logoPath).fileName() + ".temp";
            bmp16Turn(logoPath, tempPath);
        }

        // compress logo if all ok
        uint size; // max size is 6646
        if (converted)
        {
            if (!bmpCompresRle16(tempPath, logoOutPath))
            {
                applyLog->append("can't open logo image!");
                applyLog->append("return and try to fix problem");
                return;
            }
        }
        else
        {
            if (!bmpCompresRle16(logoPath, logoOutPath))
            {
                applyLog->append("can't open logo image!");
                applyLog->append("return and try to fix problem");
                return;
            }
        }
        size = QFile(logoOutPath).size();

        // check logo size
        if (size > MAX_LOGO_SIZE)
        {
            applyLog->append("size of compressed logo is larger than 6646 bytes");
            applyLog->append("return and try to fix problem");
            return;
        }
        else
        {
            applyLog->append(QString("RLE size: %1 bytes. <b>OK!</b>").arg(size));
        }
    }
    else
    {
        applyLog->append("<b>Logo image:</b> disabled.");
    }
    // usb
    if (usbCheck->isChecked())
    {
        bool converted = false;

        applyLog->append("<b>USB icon:</b> " + usbPath);
        bpp = bmpGetBpp(usbPath);
        if (bpp == 16) // if 16bpp
        {
            converted = false;
            applyLog->append("bpp: 16b");
        }
        else if (bpp == 24) // if 24bpp
            applyLog->append("bpp: 24b - will be converted to 16b");
        else
        {
            applyLog->append("error: usb image is not 16bpp or 24bpp");
            applyLog->append("return and try to use correct image");
            return;
        }

        if (bpp == 24) // convert if ok and 24bpp
        {
            applyLog->append("converting usb...");
            tempPath = appFolder + QFileInfo(usbPath).fileName() + ".temp";
            if (bmp24to16(usbPath, tempPath, ORIENT_FROM_TOP))
            {
                converted = true;
                applyLog->append("usb successfuly converted to 16bpp");
            }
            else
            {
                applyLog->append("error when converting usb to 16bpp");
                applyLog->append("return and try to fix problem");
                return;
            }
        }
        else if (bmpOrient(usbPath) != ORIENT_FROM_TOP) // check orient for 16bpp image
        {
            converted = true;
            tempPath = appFolder + QFileInfo(usbPath).fileName() + ".temp";
            bmp16Turn(usbPath, tempPath);
        }

        // compress usb if all ok
        uint size; // max size is 1089
        if (converted)
        {
            if (!bmpCompresRle16(tempPath, usbOutPath))
            {
                applyLog->append("can't open usb image!");
                applyLog->append("return and try to fix problem");
                return;
            }
        }
        else
        {
            if (!bmpCompresRle16(usbPath, usbOutPath))
            {
                applyLog->append("can't open usb image!");
                applyLog->append("return and try to fix problem");
                return;
            }
        }
        size = QFile(usbOutPath).size();

        // check usb size
        if (size > MAX_USB_SIZE)
        {
            applyLog->append("size of compressed usb is larger than 1089 bytes");
            applyLog->append("return and try to fix problem");
            return;
        }
        else
        {
            applyLog->append(QString("RLE size: %1 bytes. <b>OK!</b>").arg(size));
        }
    }
    else
    {
        applyLog->append("<b>USB icon:</b> disabled.");
    }
    // rd
    if (rdCheck->isChecked())
    {
        bool converted = false;

        applyLog->append("<b>R&D icon:</b> " + rdPath);
        bpp = bmpGetBpp(rdPath);
        if (bpp == 16) // if 16bpp
        {
            converted = false;
            applyLog->append("bpp: 16b");
        }
        else if (bpp == 24) // if 24bpp
            applyLog->append("bpp: 24b - will be converted to 16b");
        else
        {
            applyLog->append("error: rd image is not 16bpp or 24bpp");
            applyLog->append("return and try to use correct image");
            return;
        }

        if (bpp == 24) // convert if ok and 24bpp
        {
            applyLog->append("converting rd...");
            tempPath = appFolder + QFileInfo(rdPath).fileName() + ".temp";
            if (bmp24to16(rdPath, tempPath, ORIENT_FROM_TOP))
            {
                converted = true;
                applyLog->append("rd successfuly converted to 16bpp");
            }
            else
            {
                applyLog->append("error when converting rd to 16bpp");
                applyLog->append("return and try to fix problem");
                return;
            }
        }
        else if (bmpOrient(rdPath) != ORIENT_FROM_TOP) // check orient for 16bpp image
        {
            converted = true;
            tempPath = appFolder + QFileInfo(rdPath).fileName() + ".temp";
            bmp16Turn(rdPath, tempPath);
        }

        // compress rd if all ok
        uint size; // max size is 3954
        if (converted)
        {
            if (!bmpCompresRle16(tempPath, rdOutPath))
            {
                applyLog->append("can't open r&d image!");
                applyLog->append("return and try to fix problem");
                return;
            }
        }
        else
        {
            if (!bmpCompresRle16(rdPath, rdOutPath))
            {
                applyLog->append("can't open logo image!");
                applyLog->append("return and try to fix problem");
                return;
            }
        }
        size = QFile(rdOutPath).size();

        // check rd size
        if (size > MAX_RD_SIZE)
        {
            applyLog->append("size of compressed logo is larger than 3954 bytes");
            applyLog->append("return and try to fix problem");
            return;
        }
        else
        {
            applyLog->append(QString("RLE size: %1 bytes. <b>OK!</b>").arg(size));
        }
    }
    else
    {
        applyLog->append("<b>R&D icon:</b> disabled.");
    }

    applyLog->append("");
    applyLog->append("All OK! you can continue.");
    applyLog->append("Click <b>Apply</b> for patching NOLO.");

    ui->applyPatchBtn->setEnabled(true);
}

void MainWindow::closeApply()
{
    if (QFile(appFolder + QFileInfo(logoPath).fileName() + ".temp").exists())
        QFile(appFolder + QFileInfo(logoPath).fileName() + ".temp").remove();
    if (QFile(appFolder + QFileInfo(usbPath).fileName() + ".temp").exists())
        QFile(appFolder + QFileInfo(usbPath).fileName() + ".temp").remove();
    if (QFile(appFolder + QFileInfo(rdPath).fileName() + ".temp").exists())
        QFile(appFolder + QFileInfo(rdPath).fileName() + ".temp").remove();

    applyLog->clear();
    ui->applyCheckBtn->setEnabled(true);
    showNormal();
    ui->stack->setCurrentWidget(ui->centralPage);
}
