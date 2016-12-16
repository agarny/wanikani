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
// Widget
//==============================================================================

#pragma once

//==============================================================================

#include "wanikani.h"

//==============================================================================

#include <QMap>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>

//==============================================================================

namespace Ui {
    class Widget;
}

//==============================================================================

class QLabel;
class QPushButton;

//==============================================================================

class ProgressBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressBarWidget(QWidget *pParent);

    void setValue(const double &pValue);
    void setColor(const QRgb &pColor);

protected:
    virtual void paintEvent(QPaintEvent *pEvent);

private:
    double mValue;

    QRgb mColor;
};

//==============================================================================

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget();

#ifdef Q_OS_MAC
protected:
    virtual void keyPressEvent(QKeyEvent *pEvent);
#endif

private:
    Ui::Widget *mGui;

    bool mInitializing;

    WaniKani mWaniKani;

    QString mFileName;

    QTimer mTimer;

    QSystemTrayIcon mTrayIcon;

    QMap<QPushButton *, QRgb> mColors;

    QMap<QChar, QString> mCurrentKanjiState;
    QMap<QChar, QString> mAllKanjiState;
    QMap<QChar, QString> mOldKanjiState;

    bool mNeedToCheckWallpaper;

    ProgressBarWidget *mCurrentRadicalsValue;
    ProgressBarWidget *mCurrentKanjiValue;

    void retrieveSettings(const bool &pResetSettings = false);

    void setPushButtonColor(QPushButton *pPushButton, const QRgb &pColor);

    QColor color(const int &pRow, const int &pColumn) const;

    void updateInterval(const int &pInterval);

    QJsonDocument waniKaniRequest(const QString &pRequest);

    QString iconDataUri(const QString &pIcon, const int &pWidth = -1,
                        const int &pHeight = -1,
                        const QIcon::Mode &pMode = QIcon::Normal);

    void updateGravatar(const QPixmap &pGravatar);
    void updateSrsDistributionPalettes();
    void updateSrsDistributionInformation(QLabel *pLabel, const QString &pIcon,
                                          const SrsDistributionInformation &pInformation);

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

    void on_resetAllPushButton_clicked();
    void on_closeToolButton_clicked();

    void waniKaniUpdated();
    void waniKaniError();

    void trayIconActivated();

    void updateLevels();

    void updatePushButtonColor();

    void checkWallpaper();
};

//==============================================================================
// End of file
//==============================================================================
