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

//==============================================================================
// Main window
//==============================================================================

#pragma once

//==============================================================================

#include <QDialog>
#include <QMap>
#include <QSystemTrayIcon>

//==============================================================================

namespace Ui {
    class Settings;
}

//==============================================================================

class WaniKani;

//==============================================================================

class QAbstractButton;

//==============================================================================

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(WaniKani *pWaniKani);
    ~Settings();

    QString apiKey() const;

    int interval() const;

    QString fontName() const;
    bool boldFont() const;
    bool italicsFont() const;

    QColor color(const int &pRow, const int &pColumn) const;

protected:
    virtual void closeEvent(QCloseEvent *pEvent);

private:
    Ui::Settings *mGui;

    bool mInitializing;

    WaniKani *mWaniKani;

    QSystemTrayIcon *mTrayIcon;
    QMenu *mTrayIconMenu;

    QAction *mWaniKaniAction;
    QAction *mQuitAction;

    QMap<QPushButton *, QRgb> mColors;

    void setPushButtonColor(QPushButton *pPushButton, const QRgb &pColor);

private slots:
    void on_intervalSpinBox_valueChanged(int pInterval);
    void on_forceUpdateButton_clicked();

    void on_fontComboBox_currentTextChanged(const QString &pFontName);
    void on_boldFontCheckBox_clicked();
    void on_italicsFontCheckBox_clicked();

    void on_swapPushButton_clicked();

    void on_resetAllPushButton_clicked(const bool &pRetrieveSettingsOnly = true);

    void updatePushButtonColor();

    void trayIconActivated(const QSystemTrayIcon::ActivationReason &pReason);

    void showSettings();
};

//==============================================================================
// End of file
//==============================================================================
