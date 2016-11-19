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
#include <QMenu>
#include <QSettings>

//==============================================================================

#include "ui_settings.h"

//==============================================================================

static const auto SettingsApiKey      = QStringLiteral("ApiKey");
static const auto SettingsFontName    = QStringLiteral("FontName");
static const auto SettingsBoldFont    = QStringLiteral("BoldFont");
static const auto SettingsItalicsFont = QStringLiteral("ItalicsFont");

//==============================================================================

Settings::Settings(WaniKani *pWaniKani) :
    mGui(new Ui::Settings),
    mInitializing(true),
    mWaniKani(pWaniKani)
{
    // Set up our GUI

    mGui->setupUi(this);

    QSettings settings;

    mGui->apiKeyValue->setText(settings.value(SettingsApiKey).toString());
    mGui->fontComboBox->setCurrentText(settings.value(SettingsFontName).toString());
    mGui->boldFontCheckBox->setChecked(settings.value(SettingsBoldFont).toBool());
    mGui->italicsFontCheckBox->setChecked(settings.value(SettingsItalicsFont).toBool());

    if (mGui->fontComboBox->currentText().isEmpty())
        mGui->fontComboBox->setCurrentIndex(0);

    // Create some actions

    mWaniKaniAction = new QAction(tr("WaniKani"), this);
    mQuitAction = new QAction(tr("Quit"), this);

    connect(mWaniKaniAction, SIGNAL(triggered(bool)),
            this, SLOT(showSettings()));
    connect(mQuitAction, SIGNAL(triggered(bool)),
            qApp, SLOT(quit()));

    // Create our system tray icon menu

    mTrayIconMenu = new QMenu(this);

    mTrayIconMenu->addAction(mWaniKaniAction);
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

    settings.setValue(SettingsApiKey, mGui->apiKeyValue->text());
    settings.setValue(SettingsFontName, mGui->fontComboBox->currentText());
    settings.setValue(SettingsBoldFont, mGui->boldFontCheckBox->isChecked());
    settings.setValue(SettingsItalicsFont, mGui->italicsFontCheckBox->isChecked());
}

//==============================================================================

QString Settings::apiKey() const
{
    // Return our API key

    return mGui->apiKeyValue->text();
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

void Settings::closeEvent(QCloseEvent *pEvent)
{
    // Hide ourselves rather than closing ourselves

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

void Settings::on_updateButton_clicked()
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
        mWaniKani->updateWallpaper(true);
}

//==============================================================================

void Settings::on_boldFontCheckBox_clicked()
{
    // Force the update our wallpaper

    if (!mInitializing)
        mWaniKani->updateWallpaper(true);
}

//==============================================================================

void Settings::on_italicsFontCheckBox_clicked()
{
    // Force the update our wallpaper

    if (!mInitializing)
        mWaniKani->updateWallpaper(true);
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
