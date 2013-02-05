#include "mainwindow.h"
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
    QMainWindow(parent)
{
    shell = 0;

    bgColor = QColor(255,255,255);
    logoPath = logoDefaultPath;
    usbPath = usbDefaultPath;
    rdPath = rdDefaultPath;
    rdShow = false;

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("BootLogo Changer for Maemo 5");

    QMenuBar *menu = menuBar();
    menu->addAction("About", this, SLOT(showAbout()));
    menu->addAction("About Qt", qApp, SLOT(aboutQt()));

    stackWidget = new QStackedWidget(this);
    setCentralWidget(stackWidget);
    centralW = new QWidget(this);
    stackWidget->addWidget(centralW);
    stackWidget->setCurrentWidget(centralW);

    // Central/Left/Right layouts
    QHBoxLayout *centralL = new QHBoxLayout;
    centralL->setSpacing(0);
    centralL->setMargin(0);
    QVBoxLayout *rightL = new QVBoxLayout;
    rightL->setSpacing(0);
    rightL->setMargin(0);
    rightTopL = new QVBoxLayout;
    rightTopL->setSpacing(0);
    rightTopL->setContentsMargins(sizeInc, 0, 0, 0);
    QVBoxLayout *rightBottomL = new QVBoxLayout;
    rightBottomL->setSpacing(0);
    rightBottomL->setMargin(0);
    QVBoxLayout *leftL = new QVBoxLayout;
    leftL->setSpacing(0);
    leftL->setMargin(0);
    // Frame for left widgets
    QFrame *frame = new QFrame(this);
    frame->setFrameStyle(QFrame::StyledPanel);
    QVBoxLayout *frameL = new QVBoxLayout();
    frameL->setSpacing(0);
    frameL->setMargin(0);
    frame->setLayout(frameL);
    leftL->addWidget(frame);
    // Set layouts
    centralW->setLayout(centralL);
    centralL->addLayout(leftL, 1);
    centralL->addLayout(rightL);
    rightL->addLayout(rightTopL);
    rightL->addLayout(rightBottomL);

    // Right buttons
    // Background button
    bgBtn = new QPushButton("Background color", this);
    bgBtn->setFixedWidth(bgBtn->sizeHint().width() + sizeStep);
    bgBtn->setDisabled(true);
    connect(bgBtn, SIGNAL(clicked()), this, SLOT(bgBtnClicked()));
    bgBtn->installEventFilter(this);
    rightTopL->addWidget(bgBtn, 0, Qt::AlignRight);
    // Logo button
    logoBtn = new QPushButton("Logo image", this);
    logoBtn->setFixedWidth(bgBtn->sizeHint().width() + sizeStep);
    logoBtn->setDisabled(true);
    connect(logoBtn, SIGNAL(clicked()), this, SLOT(logoBtnClicked()));
    logoBtn->installEventFilter(this);
    rightTopL->addWidget(logoBtn, 0, Qt::AlignRight);
    // USB button
    usbBtn = new QPushButton("USB icon", this);
    usbBtn->setFixedWidth(bgBtn->sizeHint().width() + sizeStep);
    usbBtn->setDisabled(true);
    connect(usbBtn, SIGNAL(clicked()), this, SLOT(usbBtnClicked()));
    usbBtn->installEventFilter(this);
    rightTopL->addWidget(usbBtn, 0, Qt::AlignRight);
    // RD button
    rdBtn = new QPushButton("R&D icon", this);
    rdBtn->setFixedWidth(bgBtn->sizeHint().width() + sizeStep);
    rdBtn->setDisabled(true);
    connect(rdBtn, SIGNAL(clicked()), this, SLOT(rdBtnClicked()));
    rdBtn->installEventFilter(this);
    rightTopL->addWidget(rdBtn, 0, Qt::AlignRight | Qt::AlignTop);
    // Preview button
    previewBtn = new QPushButton("Preview", this);
    //    previewBtn->setDisabled(true);
    connect(previewBtn, SIGNAL(clicked()), this, SLOT(showPreview()));
    rightBottomL->addWidget(previewBtn, 0, Qt::AlignBottom);
    // Apply button
    applyBtn = new QPushButton("Apply", this);
    applyBtn->setEnabled(true);
    connect(applyBtn, SIGNAL(clicked()), this, SLOT(applyBtnClicked()));
    rightBottomL->addWidget(applyBtn);

    // Show start widget;
    createStartInfo();
    frameL->addWidget(startInfoW);

    // Create info widgets for buttons
    // Create Background info widget
    createBgInfo();
    frameL->addWidget(bgInfoW);
    // Create Logo info widget
    createLogoInfo();
    frameL->addWidget(logoInfoW);
    // Create USB info widget
    createUsbInfo();
    frameL->addWidget(usbInfoW);
    // Create RD info widget
    createRdInfo();
    frameL->addWidget(rdInfoW);
    // Create Preview widget
    createPreview();
    stackWidget->addWidget(previewW);
    // Create Apply widget
    createApply();
    stackWidget->addWidget(applyW);
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
    if (obj == bgBtn && bgBtn->isEnabled() == false)
    {
        if (event->type() == QEvent::MouseButtonRelease)
        {
            emit bgBtnClicked();
            return true;
        }
        else return false;
    }
    // if disabled Logo button clicked
    else if (obj == logoBtn && logoBtn->isEnabled() == false)
    {
        if (event->type() == QEvent::MouseButtonRelease)
        {
            emit logoBtnClicked();
            return true;
        }
        else return false;
    }
    // if disabled USB button clicked
    else if (obj == usbBtn && usbBtn->isEnabled() == false)
    {
        if (event->type() == QEvent::MouseButtonRelease)
        {
            emit usbBtnClicked();
            return true;
        }
        else return false;
    }
    // if RD button clicked
    else if (obj == rdBtn && rdBtn->isEnabled() == false)
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
    if (bgBtn->isEnabled() == false)
    {
        if (bgBtn->width() == logoBtn->width() &&
                logoBtn->width() == usbBtn->width() &&
                usbBtn->width() == rdBtn->width())
        {
            // if first push
            startInfoW->hide();
            rightTopL->setContentsMargins(0,0,0,0);
            bgBtn->setFixedWidth(bgBtn->width() + sizeInc);
        }
        else
        {
            if (bgBtn->width() < logoBtn->width())
            {
                logoBtn->setFixedWidth(logoBtn->width() - sizeInc);
            }
            if (bgBtn->width() < usbBtn->width())
            {
                usbBtn->setFixedWidth(usbBtn->width() - sizeInc);
            }
            if (bgBtn->width() < rdBtn->width())
            {
                rdBtn->setFixedWidth(rdBtn->width() - sizeInc);
            }
            bgBtn->setFixedWidth(bgBtn->width() + sizeInc);
        }
        bgInfoW->show();
        bgBtn->setEnabled(true);
        if (logoInfoW->isVisible() == true) logoInfoW->hide();
        logoBtn->setDisabled(true);
        if (usbInfoW->isVisible() == true) usbInfoW->hide();
        usbBtn->setDisabled(true);
        if (rdInfoW->isVisible() == true) rdInfoW->hide();
        rdBtn->setDisabled(true);
    }
}
// Logo button clicked
void MainWindow::logoBtnClicked()
{
    if (logoBtn->isEnabled() == false)
    {
        if (bgBtn->width() == logoBtn->width() &&
                logoBtn->width() == usbBtn->width() &&
                usbBtn->width() == rdBtn->width())
        {
            // if first push
            startInfoW->hide();
            rightTopL->setContentsMargins(0,0,0,0);
            logoBtn->setFixedWidth(logoBtn->width() + sizeInc);
        }
        else
        {
            if (logoBtn->width() < bgBtn->width())
            {
                bgBtn->setFixedWidth(bgBtn->width() - sizeInc);
            }
            if (logoBtn->width() < usbBtn->width())
            {
                usbBtn->setFixedWidth(usbBtn->width() - sizeInc);
            }
            if (logoBtn->width() < rdBtn->width())
            {
                rdBtn->setFixedWidth(rdBtn->width() - sizeInc);
            }
            logoBtn->setFixedWidth(bgBtn->width() + sizeInc);
        }
        if (bgInfoW->isVisible() == true) bgInfoW->hide();
        bgBtn->setDisabled(true);
        logoInfoW->show();
        logoBtn->setEnabled(true);
        if (usbInfoW->isVisible() == true) usbInfoW->hide();
        usbBtn->setDisabled(true);
        if (rdInfoW->isVisible() == true) rdInfoW->hide();
        rdBtn->setDisabled(true);
    }
    logoPreview->setPalette(QPalette(bgColor));
}
// USB button clicked
void MainWindow::usbBtnClicked()
{
    if (usbBtn->isEnabled() == false)
    {
        if (bgBtn->width() == logoBtn->width() &&
                logoBtn->width() == usbBtn->width() &&
                usbBtn->width() == rdBtn->width())
        {
            // if first push
            startInfoW->hide();
            rightTopL->setContentsMargins(0,0,0,0);
            usbBtn->setFixedWidth(usbBtn->width() + sizeInc);
        }
        else
        {
            if (usbBtn->width() < bgBtn->width())
            {
                bgBtn->setFixedWidth(bgBtn->width() - sizeInc);
            }
            if (usbBtn->width() < logoBtn->width())
            {
                logoBtn->setFixedWidth(logoBtn->width() - sizeInc);
            }
            if (usbBtn->width() < rdBtn->width())
            {
                rdBtn->setFixedWidth(rdBtn->width() - sizeInc);
            }
            usbBtn->setFixedWidth(bgBtn->width() + sizeInc);
        }
        if (bgInfoW->isVisible() == true) bgInfoW->hide();
        bgBtn->setDisabled(true);
        if (logoInfoW->isVisible() == true) logoInfoW->hide();
        logoBtn->setDisabled(true);
        usbInfoW->show();
        usbBtn->setEnabled(true);
        if (rdInfoW->isVisible() == true) rdInfoW->hide();
        rdBtn->setDisabled(true);
    }
    usbPreview->setPalette(QPalette(bgColor));
}
// RD button clicked
void MainWindow::rdBtnClicked()
{
    if (rdBtn->isEnabled() == false)
    {
        if (bgBtn->width() == logoBtn->width() &&
                logoBtn->width() == usbBtn->width() &&
                usbBtn->width() == rdBtn->width())
        {
            // if first push
            startInfoW->hide();
            rightTopL->setContentsMargins(0,0,0,0);
            rdBtn->setFixedWidth(rdBtn->width() + sizeInc);
        }
        else
        {
            if (rdBtn->width() < bgBtn->width())
            {
                bgBtn->setFixedWidth(bgBtn->width() - sizeInc);
            }
            if (rdBtn->width() < logoBtn->width())
            {
                logoBtn->setFixedWidth(logoBtn->width() - sizeInc);
            }
            if (rdBtn->width() < usbBtn->width())
            {
                usbBtn->setFixedWidth(usbBtn->width() - sizeInc);
            }
            rdBtn->setFixedWidth(bgBtn->width() + sizeInc);
        }
        if (bgInfoW->isVisible() == true) bgInfoW->hide();
        bgBtn->setDisabled(true);
        if (logoInfoW->isVisible() == true) logoInfoW->hide();
        logoBtn->setDisabled(true);
        if (usbInfoW->isVisible() == true) usbInfoW->hide();
        usbBtn->setDisabled(true);
        rdInfoW->show();
        rdBtn->setEnabled(true);
    }
    rdPreview->setPalette(QPalette(bgColor));
}
// Preview button clicked
void MainWindow::showPreview()
{
    previewW->setPalette(QPalette(QColor(bgColor)));
    previewLogoLabel->setPixmap(QPixmap::fromImage(QImage(logoPath)));
    previewLogoLabel->resize(previewLogoLabel->sizeHint());
    previewLogoLabel->move((previewW->width() - previewLogoLabel->width()) / 2,
                           (previewW->height() - previewLogoLabel->height()) / 2);
    previewUsbLabel->setPixmap(QPixmap::fromImage(QImage(usbPath)));
    previewUsbLabel->resize(previewUsbLabel->sizeHint());
    previewUsbLabel->move(previewW->width() - previewUsbLabel->width() - 14, 14);
    if (rdShow)
    {
        previewRdLabel->setPixmap(QPixmap::fromImage(QImage(rdPath)));
        previewRdLabel->resize(previewRdLabel->sizeHint());
        previewRdLabel->move((previewW->width() - previewRdLabel->width()) / 2,
                             previewW->height() - previewRdLabel->height() - 140);
    }
    else previewRdLabel->clear();
#ifdef Q_WS_MAEMO_5
    showFullScreen();
#endif
    stackWidget->setCurrentWidget(previewW);
}
// Apply button clicked
void MainWindow::applyBtnClicked()
{
#ifdef Q_WS_MAEMO_5
    showFullScreen();
#endif
    stackWidget->setCurrentWidget(applyW);
}
//==============================================================================
// Create frames
//==============================================================================
//--- Start --------------------------------------------------------------------
void MainWindow::createStartInfo()
{
    startInfoW = new QWidget(this);
    QVBoxLayout *startInfoL = new QVBoxLayout();
    startInfoL->setMargin(0);
    startInfoL->setSpacing(0);
    startInfoW->setLayout(startInfoL);
    QLabel *startHeadLabel = new QLabel("\n[<b>N900 BootLogo Changer</b>]\n",
                                        startInfoW);
    startInfoL->addWidget(startHeadLabel, 0, Qt::AlignCenter);
    QString startDescText("With this application you can easily "
                          "customize bootscreen of your N900.\n"
                          "Press some button on the right side of "
                          "the screen for start.\n"
                          "Enjoy!");
    QLabel *startDescLabel = new QLabel(startInfoW);
    startDescLabel->setWordWrap(true);
    startDescLabel->setAlignment(Qt::AlignCenter);
    startDescLabel->setText(startDescText);
    startInfoL->addWidget(startDescLabel, 0, Qt::AlignTop | Qt::AlignHCenter);
}
//--- Background ---------------------------------------------------------------
void MainWindow::createBgInfo()
{
    bgInfoW = new QWidget(this);
    bgInfoW->hide();
    QVBoxLayout *bgInfoL = new QVBoxLayout;
    //    bgInfoL->setSpacing(0);
    //    bgInfoL->setMargin(0);
    bgInfoW->setLayout(bgInfoL);
    QHBoxLayout *bgInfoTopL = new QHBoxLayout;
    bgInfoTopL->setSpacing(0);
    bgInfoTopL->setMargin(0);
    bgInfoL->addLayout(bgInfoTopL);

    bgCheck = new QCheckBox("Change background color", bgInfoW);
    bgCheck->setFixedWidth(bgCheck->sizeHint().width());
    connect(bgCheck, SIGNAL(clicked(bool)), this, SLOT(bgSetEnabled(bool)));
    bgInfoTopL->addWidget(bgCheck);
    bgDefaultBtn = new QPushButton("Set default", bgInfoW);
    connect(bgDefaultBtn, SIGNAL(clicked()), this, SLOT(bgSetDefault()));
    bgInfoTopL->addWidget(bgDefaultBtn);

    bgSetAsBox = new QComboBox(bgInfoW);
    bgSetAsBox->addItem("Show color in HEX as RGB16");
    bgSetAsBox->addItem("Show color in HEX as RGB24");
    bgSetAsBox->addItem("Show color in DEC as RGB24");
    bgSetAsBox->setCurrentIndex(2);
    connect(bgSetAsBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(bgLineChangeRegExp(int)));
    bgInfoL->addWidget(bgSetAsBox);

    QHBoxLayout *bgColorEditLayout = new QHBoxLayout;
    bgColorLine = new QLineEdit(bgInfoW);
    bgColorLine->setText(convertShortToQString(convertQColorToShort(bgColor)));
    connect(bgColorLine, SIGNAL(textChanged(QString)),
            this, SLOT(bgLineChanged(QString)));
    bgColorEditLayout->addWidget(bgColorLine);

    bgRedBox = new QSpinBox(bgInfoW);
    bgRedBox->setRange(0, 255);
    bgRedBox->setValue(255);
    bgRedBox->setWrapping(true);
    bgRedBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    connect(bgRedBox, SIGNAL(valueChanged(int)),
            this, SLOT(bgRedBoxChanged(int)));
    bgRedBox->hide();
    bgGreenBox = new QSpinBox(bgInfoW);
    bgGreenBox->setRange(0, 255);
    bgGreenBox->setValue(255);
    bgGreenBox->setWrapping(true);
    bgGreenBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    connect(bgGreenBox, SIGNAL(valueChanged(int)),
            this, SLOT(bgGreenBoxChanged(int)));
    bgGreenBox->hide();
    bgBlueBox = new QSpinBox(bgInfoW);
    bgBlueBox->setRange(0, 255);
    bgBlueBox->setValue(255);
    bgBlueBox->setWrapping(true);
    bgBlueBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    connect(bgBlueBox, SIGNAL(valueChanged(int)),
            this, SLOT(bgBlueBoxChanged(int)));
    bgBlueBox->hide();

    bgRedLabel = new QLabel("R:", bgInfoW);
    bgRedLabel->setFixedWidth(bgRedLabel->sizeHint().width());
    bgRedLabel->hide();
    bgColorEditLayout->addWidget(bgRedLabel);
    bgColorEditLayout->addWidget(bgRedBox);

    bgGreenLabel = new QLabel("G:", bgInfoW);
    bgGreenLabel->setFixedWidth(bgGreenLabel->sizeHint().width());
    bgGreenLabel->hide();
    bgColorEditLayout->addWidget(bgGreenLabel);
    bgColorEditLayout->addWidget(bgGreenBox);

    bgBlueLabel = new QLabel("B:", bgInfoW);
    bgBlueLabel->setFixedWidth(bgBlueLabel->sizeHint().width());
    bgBlueLabel->hide();
    bgColorEditLayout->addWidget(bgBlueLabel);
    bgColorEditLayout->addWidget(bgBlueBox);

    bgInfoL->addLayout(bgColorEditLayout);

    QFrame *bgColorPreviewFrame = new QFrame(bgInfoW);
    bgColorPreviewFrame->setFrameStyle(QFrame::StyledPanel);

    QVBoxLayout *bgColorPreviewLayout = new QVBoxLayout;
    bgColorPreviewLayout->setSpacing(0);
    bgColorPreviewLayout->setMargin(0);
    bgColorPreviewFrame->setLayout(bgColorPreviewLayout);

    bgColorPreview = new QLabel(bgInfoW);
    bgColorPreview->setAutoFillBackground(true);
    bgColorPreview->setPalette(QPalette(bgColor));
    bgColorPreviewLayout->addWidget(bgColorPreview);

    bgInfoL->addWidget(bgColorPreviewFrame);

    bgSetColorBtn = new QPushButton("Choose color", bgInfoW);
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
            QColorDialog::getColor(bgColor, bgInfoW, "Choose background color");
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
    logoInfoW = new QWidget(this);
    logoInfoW->hide();
    QVBoxLayout *logoInfoL = new QVBoxLayout;
    //    logoInfoL->setSpacing(0);
    //    logoInfoL->setMargin(0);
    logoInfoW->setLayout(logoInfoL);
    QHBoxLayout *logoInfoTopL = new QHBoxLayout;
    logoInfoTopL->setSpacing(0);
    logoInfoTopL->setMargin(0);
    logoInfoL->addLayout(logoInfoTopL);

    logoCheck = new QCheckBox("Change central logo", logoInfoW);
    logoCheck->setFixedWidth(bgCheck->width());
    connect(logoCheck, SIGNAL(clicked(bool)), this, SLOT(logoSetEnabled(bool)));
    logoInfoTopL->addWidget(logoCheck);
    logoDefaultBtn = new QPushButton("Set default", logoInfoW);
    connect(logoDefaultBtn, SIGNAL(clicked()), this, SLOT(logoSetDefault()));
    logoInfoTopL->addWidget(logoDefaultBtn);

    QHBoxLayout *logoFileNameLayout = new QHBoxLayout;
    logoFileNameLabel = new QLabel("File name: ", logoInfoW);
    logoFileNameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    logoFileName = new QLabel(QFileInfo(logoPath).fileName(), logoInfoW);
    logoFileName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QFont italic = logoFileName->font();
    italic.setItalic(true);
    logoFileName->setFont(italic);
    logoFileNameLayout->addWidget(logoFileNameLabel);
    logoFileNameLayout->addWidget(logoFileName);

    logoInfoL->addLayout(logoFileNameLayout);

    QFrame *logoPreviewFrame = new QFrame(logoInfoW);
    logoPreviewFrame->setFrameStyle(QFrame::StyledPanel);

    QVBoxLayout *logoPreviewLayout = new QVBoxLayout;
    logoPreviewLayout->setSpacing(0);
    logoPreviewLayout->setMargin(0);
    logoPreviewFrame->setLayout(logoPreviewLayout);

    logoPreview = new QLabel(logoInfoW);
    logoPreview->setAutoFillBackground(true);
    logoPreview->setPalette(QPalette(bgColor));
    logoPreview->setAlignment(Qt::AlignCenter);
    logoPreview->setPixmap(QPixmap::fromImage(QImage(logoPath)));
    logoPreviewLayout->addWidget(logoPreview);

    logoInfoL->addWidget(logoPreviewFrame);

    logoSelectBtn = new QPushButton("Choose image", logoInfoW);
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
            QFileDialog::getOpenFileName(logoInfoW,
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
        QMessageBox::warning(logoInfoW, "Error!",
                             QFileInfo(tempPath).fileName() +
                             " is not a *.bmp image with "
                             "16bit (rgb565) or 24bit (rgb888) palette.");
        return;
    }

    logoPath = tempPath;
    logoPreview->setPixmap(QPixmap::fromImage(QImage(logoPath)));
    logoFileName->setText(QFileInfo(logoPath).fileName());
}
//--- USB ----------------------------------------------------------------------
void MainWindow::createUsbInfo()
{
    usbInfoW = new QWidget(this);
    usbInfoW->hide();
    QVBoxLayout *usbInfoL = new QVBoxLayout;
    //    usbInfoL->setSpacing(0);
    //    usbInfoL->setMargin(0);
    usbInfoW->setLayout(usbInfoL);
    QHBoxLayout *usbInfoTopL = new QHBoxLayout;
    usbInfoTopL->setSpacing(0);
    usbInfoTopL->setMargin(0);
    usbInfoL->addLayout(usbInfoTopL);

    usbCheck = new QCheckBox("Change USB icon", usbInfoW);
    usbCheck->setFixedWidth(bgCheck->width());
    connect(usbCheck, SIGNAL(clicked(bool)), this, SLOT(usbSetEnabled(bool)));
    usbInfoTopL->addWidget(usbCheck);
    usbDefaultBtn = new QPushButton("Set default", usbInfoW);
    connect(usbDefaultBtn, SIGNAL(clicked()), this, SLOT(usbSetDefault()));
    usbInfoTopL->addWidget(usbDefaultBtn);

    QHBoxLayout *usbFileNameLayout = new QHBoxLayout;
    usbFileNameLabel = new QLabel("File name: ", usbInfoW);
    usbFileNameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    usbFileName = new QLabel(QFileInfo(usbPath).fileName(), usbInfoW);
    usbFileName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QFont italic = usbFileName->font();
    italic.setItalic(true);
    usbFileName->setFont(italic);
    usbFileNameLayout->addWidget(usbFileNameLabel);
    usbFileNameLayout->addWidget(usbFileName);

    usbInfoL->addLayout(usbFileNameLayout);

    QFrame *usbPreviewFrame = new QFrame(usbInfoW);
    usbPreviewFrame->setFrameStyle(QFrame::StyledPanel);

    QVBoxLayout *usbPreviewLayout = new QVBoxLayout;
    usbPreviewLayout->setSpacing(0);
    usbPreviewLayout->setMargin(0);
    usbPreviewFrame->setLayout(usbPreviewLayout);

    usbPreview = new QLabel(usbInfoW);
    usbPreview->setAutoFillBackground(true);
    usbPreview->setPalette(QPalette(bgColor));
    usbPreview->setAlignment(Qt::AlignCenter);
    usbPreview->setPixmap(QPixmap::fromImage(QImage(usbPath)));
    usbPreviewLayout->addWidget(usbPreview);

    usbInfoL->addWidget(usbPreviewFrame);

    usbSelectBtn = new QPushButton("Choose image", usbInfoW);
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
            QFileDialog::getOpenFileName(usbInfoW,
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
        QMessageBox::warning(logoInfoW, "Error!",
                             QFileInfo(tempPath).fileName() +
                             " is not a *.bmp image with "
                             "16bit (rgb565) or 24bit (rgb888) palette.");
        return;
    }

    usbPath = tempPath;
    usbPreview->setPixmap(QPixmap::fromImage(QImage(usbPath)));
    usbFileName->setText(QFileInfo(usbPath).fileName());
}
//--- RD -----------------------------------------------------------------------
void MainWindow::createRdInfo()
{
    rdInfoW = new QWidget(this);
    rdInfoW->hide();
    QVBoxLayout *rdInfoL = new QVBoxLayout;
    //    rdInfoL->setSpacing(0);
    //    rdInfoL->setMargin(0);
    rdInfoW->setLayout(rdInfoL);
    QHBoxLayout *rdInfoTopL = new QHBoxLayout;
    rdInfoTopL->setSpacing(0);
    rdInfoTopL->setMargin(0);
    rdInfoL->addLayout(rdInfoTopL);

    rdCheck = new QCheckBox("Change R&D icon", rdInfoW);
    rdCheck->setFixedWidth(bgCheck->width());
    connect(rdCheck, SIGNAL(clicked(bool)), this, SLOT(rdSetEnabled(bool)));
    rdInfoTopL->addWidget(rdCheck);
    rdDefaultBtn = new QPushButton("Set default", rdInfoW);
    connect(rdDefaultBtn, SIGNAL(clicked()), this, SLOT(rdSetDefault()));
    rdInfoTopL->addWidget(rdDefaultBtn);

    rdShowOnPreview = new QCheckBox("Show on preview", rdInfoW);
    connect(rdShowOnPreview, SIGNAL(clicked(bool)),
            this, SLOT(rdSetShowOnPreview(bool)));
    rdInfoL->addWidget(rdShowOnPreview);

    QHBoxLayout *rdFileNameLayout = new QHBoxLayout;
    rdFileNameLabel = new QLabel("File name: ", rdInfoW);
    rdFileNameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    rdFileName = new QLabel(QFileInfo(rdPath).fileName(), rdInfoW);
    rdFileName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QFont italic = rdFileName->font();
    italic.setItalic(true);
    rdFileName->setFont(italic);
    rdFileNameLayout->addWidget(rdFileNameLabel);
    rdFileNameLayout->addWidget(rdFileName);

    rdInfoL->addLayout(rdFileNameLayout);

    QFrame *rdPreviewFrame = new QFrame(rdInfoW);
    rdPreviewFrame->setFrameStyle(QFrame::StyledPanel);

    QVBoxLayout *rdPreviewLayout = new QVBoxLayout;
    rdPreviewLayout->setSpacing(0);
    rdPreviewLayout->setMargin(0);
    rdPreviewFrame->setLayout(rdPreviewLayout);

    rdPreview = new QLabel(rdInfoW);
    rdPreview->setAutoFillBackground(true);
    rdPreview->setPalette(QPalette(bgColor));
    rdPreview->setAlignment(Qt::AlignCenter);
    rdPreview->setPixmap(QPixmap::fromImage(QImage(rdPath)));
    rdPreviewLayout->addWidget(rdPreview);

    rdInfoL->addWidget(rdPreviewFrame);

    rdSelectBtn = new QPushButton("Choose image", rdInfoW);
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
            QFileDialog::getOpenFileName(rdInfoW,
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
        QMessageBox::warning(logoInfoW, "Error!",
                             QFileInfo(tempPath).fileName() +
                             " is not a *.bmp image with "
                             "16bit (rgb565) or 24bit (rgb888) palette.");
        return;
    }

    rdPath = tempPath;
    rdPreview->setPixmap(QPixmap::fromImage(QImage(rdPath)));
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
    previewW = new QWidget(this);
    previewW->setFixedSize(800, 480);
    previewW->setAutoFillBackground(true);
    previewW->setPalette(QPalette(QColor(bgColor)));
    previewClose = new QPushButton("Tap here to close preview...", previewW);
    previewClose->installEventFilter(this);
    previewClose->setFixedSize(previewClose->sizeHint());
    previewClose->move((previewW->width() - previewClose->width()) / 2,
                       previewW->height() - previewClose->height());
    previewClose->setFlat(true);
    previewClose->setAutoFillBackground(true);

    previewLogoLabel = new QLabel(previewW);
    QImage logoImage;

    logoImage = QImage(logoPath);

    previewLogoLabel->setPixmap(QPixmap::fromImage(logoImage));
    previewLogoLabel->resize(previewLogoLabel->sizeHint());
    previewLogoLabel->move((previewW->width() - previewLogoLabel->width()) / 2,
                           (previewW->height() - previewLogoLabel->height()) / 2);

    previewUsbLabel = new QLabel(previewW);
    QImage usbImage;

    usbImage = QImage(usbPath);

    previewUsbLabel->setPixmap(QPixmap::fromImage(usbImage));
    previewUsbLabel->resize(previewUsbLabel->sizeHint());
    previewUsbLabel->move(previewW->width() - previewUsbLabel->width() - 14, 14);

    previewRdLabel = new QLabel(previewW);
    QImage rdImage;

    rdImage = QImage(rdPath);

    previewRdLabel->setPixmap(QPixmap::fromImage(rdImage));
    previewRdLabel->resize(previewRdLabel->sizeHint());
    previewRdLabel->move((previewW->width() - previewRdLabel->width()) / 2,
                         previewW->height() - previewRdLabel->height() - 140);

}
// close preview window
void MainWindow::closePreview()
{
    showNormal();
    stackWidget->setCurrentWidget(centralW);
}
//--- Apply --------------------------------------------------------------------
// create apply
void MainWindow::createApply()
{
    applyW = new QWidget(this);
    QVBoxLayout *applyL = new QVBoxLayout;
    applyW->setLayout(applyL);

    applyLog = new QTextEdit(applyW);
    applyLog->setCursorWidth(0);
    applyLog->setTextInteractionFlags(Qt::TextSelectableByMouse);
    applyLog->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    QScrollArea *scrollArea = new QScrollArea(applyW);
    scrollArea->setWidget(applyLog);
    scrollArea->setWidgetResizable(true);

#ifdef Q_WS_MAEMO_5
    QAbstractKineticScroller *scroller;
    scroller = applyLog->property("kineticScroller").value<QAbstractKineticScroller*>();
    scroller->setMode(QAbstractKineticScroller::PushMode);
    scroller->setOvershootPolicy(QAbstractKineticScroller::OvershootAlwaysOff);
    scroller->setEnabled(true);
#endif

    applyL->addWidget(scrollArea);

    QHBoxLayout *applyLogBtnL = new QHBoxLayout;
    applyCheckBtn = new QPushButton("Check", applyW);
    connect(applyCheckBtn, SIGNAL(clicked()), this, SLOT(checkAll()));
    applyPatchBtn = new QPushButton("Patch", applyW);
    connect(applyPatchBtn, SIGNAL(clicked()), this, SLOT(patchNolo()));
    applyPatchBtn->setDisabled(true);
    applyReturn = new QPushButton("Return", applyW);
    connect(applyReturn, SIGNAL(clicked()), this, SLOT(closeApply()));
    applyLogBtnL->addWidget(applyCheckBtn);
    applyLogBtnL->addWidget(applyPatchBtn);
    applyLogBtnL->addWidget(applyReturn);

    applyL->addLayout(applyLogBtnL);
}

void MainWindow::checkAll()
{
    applyCheckBtn->setDisabled(true);

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
        applyLog->append("<b>Logo image:</b> " + logoPath);
        bpp = bmpGetBpp(logoPath);
        if (bpp == 16) // if 16bpp
        {
            isLogoConvreted = false;
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
                isLogoConvreted = true;
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
            isLogoConvreted = true;
            tempPath = appFolder + QFileInfo(logoPath).fileName() + ".temp";
            bmp16Turn(logoPath, tempPath);
        }

        // compress logo if all ok
        uint size; // max size is 6646
        if (isLogoConvreted)
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
        applyLog->append("<b>USB icon:</b> " + usbPath);
        bpp = bmpGetBpp(usbPath);
        if (bpp == 16) // if 16bpp
        {
            isUsbConverted = false;
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
                isUsbConverted = true;
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
            isUsbConverted = true;
            tempPath = appFolder + QFileInfo(usbPath).fileName() + ".temp";
            bmp16Turn(usbPath, tempPath);
        }

        // compress usb if all ok
        uint size; // max size is 1089
        if (isUsbConverted)
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
        applyLog->append("<b>R&D icon:</b> " + rdPath);
        bpp = bmpGetBpp(rdPath);
        if (bpp == 16) // if 16bpp
        {
            isRdConverted = false;
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
                isRdConverted = true;
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
            isRdConverted = true;
            tempPath = appFolder + QFileInfo(rdPath).fileName() + ".temp";
            bmp16Turn(rdPath, tempPath);
        }

        // compress rd if all ok
        uint size; // max size is 3954
        if (isRdConverted)
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

    applyPatchBtn->setEnabled(true);
}

void MainWindow::closeApply()
{
    applyLog->clear();
    applyCheckBtn->setEnabled(true);
    showNormal();
    stackWidget->setCurrentWidget(centralW);
}
