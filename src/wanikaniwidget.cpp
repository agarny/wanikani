/*******************************************************************************

Copyright Alan Garny

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************/

#include "wanikani.h"
#include "wanikaniwidget.h"

//==============================================================================

#include <QCloseEvent>
#include <QColorDialog>
#include <QDate>
#include <QFile>
#include <QMenu>
#include <QSettings>
#include <QTextStream>

//==============================================================================

#include "ui_wanikaniwidget.h"

//==============================================================================

static const auto SettingsFileName      = QStringLiteral("FileName");
static const auto SettingsApiKey        = QStringLiteral("ApiKey");
static const auto SettingsCurrentKanjis = QStringLiteral("CurrentKanjis");
static const auto SettingsInterval      = QStringLiteral("Interval");
static const auto SettingsFontName      = QStringLiteral("FontName");
static const auto SettingsBoldFont      = QStringLiteral("BoldFont");
static const auto SettingsItalicsFont   = QStringLiteral("ItalicsFont");
static const auto SettingsColor         = QStringLiteral("Color%1%2");

//==============================================================================

static const auto LinkStyle = "color: rgb(103, 103, 103); outline: 0px; text-decoration: none;";

//==============================================================================

WaniKaniWidget::WaniKaniWidget(WaniKani *pWaniKani) :
    mGui(new Ui::WaniKaniWidget),
    mInitializing(true),
    mFileName(QString()),
    mWaniKani(pWaniKani),
    mColors(QMap<QPushButton *, QRgb>())
{
    // Set up our GUI

    mGui->setupUi(this);

    setMinimumSize(QSize(1024, 768));

    connect(mGui->currentKanjisRadioButton, SIGNAL(clicked()),
            this, SLOT(updateLevels()));
    connect(mGui->allKanjisRadioButton, SIGNAL(clicked()),
            this, SLOT(updateLevels()));

    for (int i = 1; i <= 6; ++i) {
        for (int j = 1; j <= 2; ++j) {
            connect(qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsLayout)->itemAtPosition(i, j)->widget()), SIGNAL(clicked()),
                    this, SLOT(updatePushButtonColor()));
        }
    }

    // Some about information

    QFile versionFile(":/version");

    versionFile.open(QIODevice::ReadOnly);

    QTextStream stream(&versionFile);
    QString version = stream.readAll();

    versionFile.close();

    int currentYear = QDate::currentDate().year();

    mGui->aboutValue->setText("<span style=\"font-size: 17pt;\"><strong><a href=\"https://github.com/agarny/wanikani\" style=\""+QString(LinkStyle)+"\">WaniKani "+version+"</a></strong></span><br/>"
                              "by<br/>"
                              "<span style=\"font-size: 13pt;\"><strong><em><a href=\"https://github.com/agarny\" style=\""+QString(LinkStyle)+"\">Alan Garny</a></em></strong></span><br/>"
                              "<em>Copyright 2016"+((currentYear > 2016)?QString("-%1").arg(currentYear):QString())+"</em>");

    // Retrieve our settings and handle a click on our foreground/background
    // push buttons

    on_resetAllPushButton_clicked(true);

    mInitializing = false;
}

//==============================================================================

WaniKaniWidget::~WaniKaniWidget()
{
    // Keep track of our settings

    QSettings settings;

    settings.setValue(SettingsFileName, mFileName);
    settings.setValue(SettingsApiKey, mGui->apiKeyValue->text());
    settings.setValue(SettingsCurrentKanjis, mGui->currentKanjisRadioButton->isChecked());
    settings.setValue(SettingsInterval, mGui->intervalSpinBox->value());
    settings.setValue(SettingsFontName, mGui->fontComboBox->currentText());
    settings.setValue(SettingsBoldFont, mGui->boldFontCheckBox->isChecked());
    settings.setValue(SettingsItalicsFont, mGui->italicsFontCheckBox->isChecked());

    for (int i = 1; i <= 6; ++i) {
        for (int j = 1; j <= 2; ++j)
            settings.setValue(SettingsColor.arg(i).arg(j), mColors.value(qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsLayout)->itemAtPosition(i, j)->widget())));
    }
}

//==============================================================================

QString WaniKaniWidget::fileName() const
{
    // Return our file name

    return mFileName;
}

//==============================================================================

void WaniKaniWidget::setFileName(const QString &pFileName)
{
    // Set our file name

    mFileName = pFileName;
}

//==============================================================================

QString WaniKaniWidget::apiKey() const
{
    // Return our API key

    return mGui->apiKeyValue->text();
}

//==============================================================================

void WaniKaniWidget::updateSrsDistributionPalettes()
{
    // Update the palette of our different SRS distribution information

    QPalette palette;

    palette.setColor(QPalette::Window, mGui->apprenticeBackgroundPushButton->palette().color(QPalette::Button));
    palette.setColor(QPalette::WindowText, mGui->apprenticeForegroundPushButton->palette().color(QPalette::Button));

    mGui->apprenticeValue->setPalette(palette);

    palette.setColor(QPalette::Window, mGui->guruBackgroundPushButton->palette().color(QPalette::Button));
    palette.setColor(QPalette::WindowText, mGui->guruForegroundPushButton->palette().color(QPalette::Button));

    mGui->guruValue->setPalette(palette);

    palette.setColor(QPalette::Window, mGui->masterBackgroundPushButton->palette().color(QPalette::Button));
    palette.setColor(QPalette::WindowText, mGui->masterForegroundPushButton->palette().color(QPalette::Button));

    mGui->masterValue->setPalette(palette);

    palette.setColor(QPalette::Window, mGui->enlightenedBackgroundPushButton->palette().color(QPalette::Button));
    palette.setColor(QPalette::WindowText, mGui->enlightenedForegroundPushButton->palette().color(QPalette::Button));

    mGui->enlightenedValue->setPalette(palette);

    palette.setColor(QPalette::Window, mGui->burnedBackgroundPushButton->palette().color(QPalette::Button));
    palette.setColor(QPalette::WindowText, mGui->burnedForegroundPushButton->palette().color(QPalette::Button));

    mGui->burnedValue->setPalette(palette);
}

//==============================================================================

void WaniKaniWidget::updateUserInformation(const QString &pUserName,
                                           const QPixmap &pGravatar,
                                           const QString &pLevel,
                                           const QString &pTitle,
                                           const QString &pApprentice,
                                           const QString &pGuru,
                                           const QString &pMaster,
                                           const QString &pEnlightened,
                                           const QString &pBurned)
{
    // Update our user information

    QPixmap gravatar = pUserName.isEmpty()?QPixmap(":/warning"):pGravatar;

    mGui->gravatarValue->setPixmap(gravatar.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    if (!pUserName.isEmpty()) {
        mGui->userInformationValue->setText("<center>"
                                            "    <span style=\"font-size: 15pt;\"><strong><a href=\"https://www.wanikani.com/community/people/"+pUserName+"\" style=\""+QString(LinkStyle)+"\">"+pUserName+"</a></strong> of Sect <strong>"+pTitle+"</strong></span><br/>"
                                            "    <span style=\"font-size: 11pt;\"><strong><em>(Level "+pLevel+")</em></strong></span>"
                                            "</center>");

        updateSrsDistributionPalettes();

        mGui->apprenticeValue->setText("<img src=\":/apprentice\" width=32 height=32><br/>"+pApprentice);
        mGui->guruValue->setText("<img src=\":/guru\" width=32 height=32><br/>"+pGuru);
        mGui->masterValue->setText("<img src=\":/master\" width=32 height=32><br/>"+pMaster);
        mGui->enlightenedValue->setText("<img src=\":/enlightened\" width=32 height=32><br/>"+pEnlightened);
        mGui->burnedValue->setText("<img src=\":/burned\" width=32 height=32><br/>"+pBurned);

        mGui->userInformationValue->show();
        mGui->apprenticeValue->show();
        mGui->guruValue->show();
        mGui->masterValue->show();
        mGui->enlightenedValue->show();
        mGui->burnedValue->show();
    } else {
        mGui->userInformationValue->hide();
        mGui->apprenticeValue->hide();
        mGui->guruValue->hide();
        mGui->masterValue->hide();
        mGui->enlightenedValue->hide();
        mGui->burnedValue->hide();
    }
}

//==============================================================================

int WaniKaniWidget::interval() const
{
    // Return our interval

    return mGui->intervalSpinBox->value();
}

//==============================================================================

bool WaniKaniWidget::currentKanjis() const
{
    // Return whether we are to display only our current levels

    return mGui->currentKanjisRadioButton->isChecked();
}

//==============================================================================

QString WaniKaniWidget::fontName() const
{
    // Return our font name

    return mGui->fontComboBox->currentText();
}

//==============================================================================

bool WaniKaniWidget::boldFont() const
{
    // Return whether our font is to be bold

    return mGui->boldFontCheckBox->isChecked();
}

//==============================================================================

bool WaniKaniWidget::italicsFont() const
{
    // Return whether our font is to be in italics

    return mGui->italicsFontCheckBox->isChecked();
}

//==============================================================================

QColor WaniKaniWidget::color(const int &pRow, const int &pColumn) const
{
    // Return whether our font is to be in italics

    QRgb rgba = mColors.value(qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsLayout)->itemAtPosition(pRow, pColumn)->widget()));

    return QColor(qRed(rgba), qGreen(rgba), qBlue(rgba), qAlpha(rgba));
}

//==============================================================================

void WaniKaniWidget::closeEvent(QCloseEvent *pEvent)
{
    // Hide ourselves rather than close ourselves

#ifdef Q_OS_OSX
    if (!pEvent->spontaneous() || !isVisible())
        return;
#endif

    hide();

    pEvent->ignore();
}

//==============================================================================

void WaniKaniWidget::on_apiKeyValue_returnPressed()
{
    // Update our user's information and Kanjis (and therefore our wallpaper)

    mWaniKani->updateUserInformation();
    mWaniKani->updateKanjis(true);
}

//==============================================================================

void WaniKaniWidget::on_intervalSpinBox_valueChanged(int pInterval)
{
    // Update our timer's interval

    if (!mInitializing)
        mWaniKani->updateInterval(pInterval);
}

//==============================================================================

void WaniKaniWidget::on_forceUpdateButton_clicked()
{
    // Update our Kanjis (and therefore our wallpaper)

    mWaniKani->updateKanjis(true);
}

//==============================================================================

void WaniKaniWidget::on_fontComboBox_currentTextChanged(const QString &pFontName)
{
    Q_UNUSED(pFontName);

    // Force the update our wallpaper

    if (!mInitializing)
        mWaniKani->updateWallpaper(true);
}

//==============================================================================

void WaniKaniWidget::on_boldFontCheckBox_clicked()
{
    // Force the update our wallpaper

    if (!mInitializing)
        mWaniKani->updateWallpaper(true);
}

//==============================================================================

void WaniKaniWidget::on_italicsFontCheckBox_clicked()
{
    // Force the update our wallpaper

    if (!mInitializing)
        mWaniKani->updateWallpaper(true);
}

//==============================================================================

void WaniKaniWidget::on_swapPushButton_clicked()
{
    // Swap the foreground and background colours, but leaving the alpha values
    // untouched

    for (int i = 1; i <= 6; ++i) {
        QPushButton *fgPushButton = qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsLayout)->itemAtPosition(i, 1)->widget());
        QPushButton *bgPushButton = qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsLayout)->itemAtPosition(i, 2)->widget());
        QRgb fgColor = mColors.value(fgPushButton);
        QRgb bgColor = mColors.value(bgPushButton);

        setPushButtonColor(fgPushButton, qRgba(qRed(bgColor), qGreen(bgColor), qBlue(bgColor), qAlpha(fgColor)));
        setPushButtonColor(bgPushButton, qRgba(qRed(fgColor), qGreen(fgColor), qBlue(fgColor), qAlpha(bgColor)));
    }

    updateSrsDistributionPalettes();

    mWaniKani->updateWallpaper(true);
}

//==============================================================================

void WaniKaniWidget::on_resetAllPushButton_clicked(const bool &pRetrieveSettingsOnly)
{
    // Retrieve all of our settings after having reset some of them, if
    // requested

    QSettings settings;

    if (mInitializing) {
        mFileName = settings.value(SettingsFileName).toString();

        mGui->apiKeyValue->setText(settings.value(SettingsApiKey).toString());
    }

    if (!pRetrieveSettingsOnly) {
        mInitializing = true;

        settings.clear();
    }

    if (settings.value(SettingsCurrentKanjis, true).toBool())
        mGui->currentKanjisRadioButton->setChecked(true);
    else
        mGui->allKanjisRadioButton->setChecked(true);

    mGui->intervalSpinBox->setValue(settings.value(SettingsInterval).toInt());

    static const QColor Colors[6][2] = { { "#606060", "#60808080"},
                                         { "#606060", "#60dd0093"},
                                         { "#606060", "#60882d9e"},
                                         { "#606060", "#60294ddb"},
                                         { "#606060", "#600093dd"},
                                         { "#606060", "#60fbc042"} };

    QString fontName = settings.value(SettingsFontName).toString();

    mGui->fontComboBox->setCurrentText(fontName);
    mGui->boldFontCheckBox->setChecked(settings.value(SettingsBoldFont).toBool());
    mGui->italicsFontCheckBox->setChecked(settings.value(SettingsItalicsFont).toBool());

    for (int i = 1; i <= 6; ++i) {
        for (int j = 1; j <= 2; ++j) {
            QPushButton *pushButton = qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsLayout)->itemAtPosition(i, j)->widget());
            QRgb color = settings.value(SettingsColor.arg(i).arg(j), Colors[i-1][j-1].rgba()).toUInt();

            setPushButtonColor(pushButton, color);
        }
    }

    if (fontName.isEmpty()) {
#if defined(Q_OS_WIN)
        mGui->fontComboBox->setCurrentText("MS Mincho");
#elif defined(Q_OS_LINUX)
        mGui->fontComboBox->setCurrentText("Droid Sans Fallback");
#elif defined(Q_OS_MAC)
        mGui->fontComboBox->setCurrentText("Hiragino Mincho Pro");
#else
    #error Unsupported platform
#endif
    }

    if (!pRetrieveSettingsOnly) {
        mInitializing = false;

        updateSrsDistributionPalettes();

        mWaniKani->updateKanjis(true);
    }
}

//==============================================================================

void WaniKaniWidget::on_closeToolButton_clicked()
{
    // Close ourselves

    mWaniKani->close();
}

//==============================================================================

void WaniKaniWidget::updateLevels()
{
    // Update the levels to display

    if (!mInitializing)
        mWaniKani->updateKanjis(true);
}

//==============================================================================

void WaniKaniWidget::updatePushButtonColor()
{
    // Update the background colour of the given push button

    QPushButton *pushButton = qobject_cast<QPushButton *>(sender());
    QColorDialog  colorDialog;

    colorDialog.setCurrentColor(pushButton->palette().color(QPalette::Button));
    colorDialog.setOption(QColorDialog::ShowAlphaChannel);

    if (colorDialog.exec() == QDialog::Accepted) {
        setPushButtonColor(pushButton, colorDialog.currentColor().rgba());

        updateSrsDistributionPalettes();

        mWaniKani->updateWallpaper(true);
    }

    // We will have been hidden when showing the colour dialog, so show us back

    mWaniKani->showWidget();
}

//==============================================================================

void WaniKaniWidget::setPushButtonColor(QPushButton *pPushButton,
                                        const QRgb &pColor)
{
    // Set the background of the given push button to the given colour

    mColors.insert(pPushButton, pColor);

    pPushButton->setStyleSheet(QString("QPushButton#%1 {"
                                       "    border: 1px solid gray;"
                                       "    background-color: rgba(%2, %3, %4, %5);"
                                       "}").arg(pPushButton->objectName())
                                           .arg(qRed(pColor))
                                           .arg(qGreen(pColor))
                                           .arg(qBlue(pColor))
                                           .arg(qAlpha(pColor)));
}

//==============================================================================
// End of file
//==============================================================================
