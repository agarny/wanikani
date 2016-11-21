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

#include "settings.h"
#include "wanikani.h"

//==============================================================================

#include <QCloseEvent>
#include <QColorDialog>
#include <QMenu>
#include <QSettings>

//==============================================================================

#include "ui_settings.h"

//==============================================================================

static const auto SettingsPosition      = QStringLiteral("Position");
static const auto SettingsFileName      = QStringLiteral("FileName");
static const auto SettingsApiKey        = QStringLiteral("ApiKey");
static const auto SettingsCurrentKanjis = QStringLiteral("CurrentKanjis");
static const auto SettingsInterval      = QStringLiteral("Interval");
static const auto SettingsFontName      = QStringLiteral("FontName");
static const auto SettingsBoldFont      = QStringLiteral("BoldFont");
static const auto SettingsItalicsFont   = QStringLiteral("ItalicsFont");
static const auto SettingsColor         = QStringLiteral("Color%1%2");

//==============================================================================

Settings::Settings(WaniKani *pWaniKani) :
    mGui(new Ui::Settings),
    mInitializing(true),
    mFileName(QString()),
    mWaniKani(pWaniKani),
    mColors(QMap<QPushButton *, QRgb>())
{
    // Set up our GUI

    mGui->setupUi(this);

    connect(mGui->currentKanjisRadioButton, SIGNAL(clicked()),
            this, SLOT(updateLevels()));
    connect(mGui->allKanjisRadioButton, SIGNAL(clicked()),
            this, SLOT(updateLevels()));

    for (int i = 1; i <= 6; ++i) {
        for (int j = 1; j <= 2; ++j) {
            connect(qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsGroupBox->layout())->itemAtPosition(i, j)->widget()), SIGNAL(clicked()),
                    this, SLOT(updatePushButtonColor()));
        }
    }

    // Retrieve our settings and handle a click on our foreground/background
    // push buttons

    on_resetAllPushButton_clicked();

    // Create some actions

    mSettingsAction = new QAction(tr("Settings"), this);
    mQuitAction = new QAction(tr("Quit"), this);

    connect(mSettingsAction, SIGNAL(triggered(bool)),
            this, SLOT(showSettings()));
    connect(mQuitAction, SIGNAL(triggered(bool)),
            qApp, SLOT(quit()));

    // Create our system tray icon menu

    mTrayIconMenu = new QMenu(this);

    mTrayIconMenu->addAction(mSettingsAction);
    mTrayIconMenu->addSeparator();
    mTrayIconMenu->addAction(mQuitAction);

    // Create and show our system tray icon

    mTrayIcon = new QSystemTrayIcon(this);

    mTrayIcon->setContextMenu(mTrayIconMenu);
    mTrayIcon->setIcon(QIcon(":/icon"));

    connect(mTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(const QSystemTrayIcon::ActivationReason &)));

    mTrayIcon->show();

    mInitializing = false;
}

//==============================================================================

Settings::~Settings()
{
    // Keep track of some settings

    QSettings settings;

    settings.setValue(SettingsPosition, pos());
    settings.setValue(SettingsFileName, mFileName);
    settings.setValue(SettingsApiKey, mGui->apiKeyValue->text());
    settings.setValue(SettingsCurrentKanjis, mGui->currentKanjisRadioButton->isChecked());
    settings.setValue(SettingsInterval, mGui->intervalSpinBox->value());
    settings.setValue(SettingsFontName, mGui->fontComboBox->currentText());
    settings.setValue(SettingsBoldFont, mGui->boldFontCheckBox->isChecked());
    settings.setValue(SettingsItalicsFont, mGui->italicsFontCheckBox->isChecked());

    for (int i = 1; i <= 6; ++i) {
        for (int j = 1; j <= 2; ++j) {
            settings.setValue(SettingsColor.arg(i).arg(j), mColors.value(qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsGroupBox->layout())->itemAtPosition(i, j)->widget())));
        }
    }
}

//==============================================================================

QString Settings::fileName() const
{
    // Return our file name

    return mFileName;
}

//==============================================================================

void Settings::setFileName(const QString &pFileName)
{
    // Set our file name

    mFileName = pFileName;
}

//==============================================================================

QString Settings::apiKey() const
{
    // Return our API key

    return mGui->apiKeyValue->text();
}

//==============================================================================

int Settings::interval() const
{
    // Return our interval

    return mGui->intervalSpinBox->value();
}

//==============================================================================

bool Settings::currentKanjis() const
{
    // Return whether we are to display only our current levels

    return mGui->currentKanjisRadioButton->isChecked();
}

//==============================================================================

QString Settings::fontName() const
{
    // Return our font name

    return mGui->fontComboBox->currentText();
}

//==============================================================================

bool Settings::boldFont() const
{
    // Return whether our font is to be bold

    return mGui->boldFontCheckBox->isChecked();
}

//==============================================================================

bool Settings::italicsFont() const
{
    // Return whether our font is to be in italics

    return mGui->italicsFontCheckBox->isChecked();
}

//==============================================================================

QColor Settings::color(const int &pRow, const int &pColumn) const
{
    // Return whether our font is to be in italics

    QRgb rgba = mColors.value(qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsGroupBox->layout())->itemAtPosition(pRow, pColumn)->widget()));

    return QColor(qRed(rgba), qGreen(rgba), qBlue(rgba), qAlpha(rgba));
}

//==============================================================================

void Settings::closeEvent(QCloseEvent *pEvent)
{
    // Hide ourselves rather than close ourselves

#ifdef Q_OS_OSX
    if (!pEvent->spontaneous() || !isVisible())
        return;
#endif

    if (mTrayIcon->isVisible()) {
        hide();

        pEvent->ignore();
    }
}

//==============================================================================

void Settings::on_apiKeyValue_returnPressed()
{
    // Update our Kanjis (and therefore our wallpaper)

    mWaniKani->updateKanjis();
}

//==============================================================================

void Settings::on_intervalSpinBox_valueChanged(int pInterval)
{
    // Update our timer's interval

    if (!mInitializing)
        mWaniKani->updateInterval(pInterval);
}

//==============================================================================

void Settings::on_forceUpdateButton_clicked()
{
    // Update our Kanjis (and therefore our wallpaper)

    mWaniKani->updateKanjis();
}

//==============================================================================

void Settings::on_fontComboBox_currentTextChanged(const QString &pFontName)
{
    Q_UNUSED(pFontName);

    // Force the update our wallpaper

    if (!mInitializing)
        mWaniKani->updateWallpaper();
}

//==============================================================================

void Settings::on_boldFontCheckBox_clicked()
{
    // Force the update our wallpaper

    if (!mInitializing)
        mWaniKani->updateWallpaper();
}

//==============================================================================

void Settings::on_italicsFontCheckBox_clicked()
{
    // Force the update our wallpaper

    if (!mInitializing)
        mWaniKani->updateWallpaper();
}

//==============================================================================

void Settings::on_swapPushButton_clicked()
{
    // Swap the foreground and background colours, but leaving the alpha values
    // untouched

    for (int i = 1; i <= 6; ++i) {
        QPushButton *fgPushButton = qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsGroupBox->layout())->itemAtPosition(i, 1)->widget());
        QPushButton *bgPushButton = qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsGroupBox->layout())->itemAtPosition(i, 2)->widget());
        QRgb fgColor = mColors.value(fgPushButton);
        QRgb bgColor = mColors.value(bgPushButton);

        setPushButtonColor(fgPushButton, qRgba(qRed(bgColor), qGreen(bgColor), qBlue(bgColor), qAlpha(fgColor)));
        setPushButtonColor(bgPushButton, qRgba(qRed(fgColor), qGreen(fgColor), qBlue(fgColor), qAlpha(bgColor)));
    }

    mWaniKani->updateWallpaper();
}

//==============================================================================

void Settings::on_resetAllPushButton_clicked(const bool &pRetrieveSettingsOnly)
{
    // Retrieve all of our settings after having reset some of them, if
    // requested

    QSettings settings;

    if (mInitializing) {
        QPoint position = settings.value(SettingsPosition).toPoint();

        if (!position.isNull())
            move(position);

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
            QPushButton *pushButton = qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsGroupBox->layout())->itemAtPosition(i, j)->widget());
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

        mWaniKani->updateKanjis();
    }
}

//==============================================================================

void Settings::updateLevels()
{
    // Update the levels to display

    if (!mInitializing)
        mWaniKani->updateKanjis();
}

//==============================================================================

void Settings::updatePushButtonColor()
{
    // Update the background colour of the given push button

    QPushButton *pushButton = qobject_cast<QPushButton *>(sender());
    QColorDialog  colorDialog;

    colorDialog.setCurrentColor(pushButton->palette().color(QPalette::Button));
    colorDialog.setOption(QColorDialog::ShowAlphaChannel);

    if (colorDialog.exec() == QDialog::Accepted) {
        setPushButtonColor(pushButton, colorDialog.currentColor().rgba());

        mWaniKani->updateWallpaper();
    }
}

//==============================================================================

void Settings::setPushButtonColor(QPushButton *pPushButton, const QRgb &pColor)
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

void Settings::trayIconActivated(const QSystemTrayIcon::ActivationReason &pReason)
{
    // Show ourselves or our menu, depending on the platofmr on which we are

    if (pReason == QSystemTrayIcon::Trigger) {
#ifdef Q_OS_WIN
        showSettings();
#else
        mTrayIcon->show();
#endif
    }
}

//==============================================================================

void Settings::showSettings()
{
    // Show ourselves

    show();

    raise();
    activateWindow();
}

//==============================================================================
// End of file
//==============================================================================
