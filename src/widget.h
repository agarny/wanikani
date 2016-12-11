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

#include <QMap>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>

//==============================================================================

namespace Ui {
    class Widget;
}

//==============================================================================

class QPushButton;

//==============================================================================

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget();

protected:
    virtual void closeEvent(QCloseEvent *pEvent);

private:
    Ui::Widget *mGui;

    bool mInitializing;

    QString mFileName;

    QTimer mTimer;

    QSystemTrayIcon mTrayIcon;
    QMenu mTrayIconMenu;

    QPoint mPosition;

    QMap<QPushButton *, QRgb> mColors;

    bool mKanjisError;
    QMap<QString, QString> mKanjisState;
    QMap<QString, QString> mOldKanjisState;

    bool mNeedToCheckWallpaper;

    void setPushButtonColor(QPushButton *pPushButton, const QRgb &pColor);

    QColor color(const int &pRow, const int &pColumn) const;

    void updateInterval(const int &pInterval);

    QJsonDocument waniKaniRequest(const QString &pRequest);

    void updateSrsDistributionPalettes();
    void updateUserInformation();

    void updateWallpaper(const bool &pForceUpdate = false);

    void setWallpaper();

private slots:
    void on_apiKeyValue_returnPressed();

    void on_intervalSpinBox_valueChanged(int pInterval);
    void on_forceUpdateButton_clicked();

    void on_fontComboBox_currentTextChanged(const QString &pFontName);
    void on_boldFontCheckBox_clicked();
    void on_italicsFontCheckBox_clicked();

    void on_swapPushButton_clicked();

    void on_resetAllPushButton_clicked(const bool &pRetrieveSettingsOnly = false);
    void on_closeToolButton_clicked();

    void trayIconActivated(const QSystemTrayIcon::ActivationReason &pReason);

    void updateLevels();

    void updatePushButtonColor();

    void updateKanjis(const bool &pForceUpdate = false);

    void checkWallpaper();
};

//==============================================================================
// End of file
//==============================================================================
