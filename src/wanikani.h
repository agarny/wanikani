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
// WaniKani
//==============================================================================

#pragma once

//==============================================================================

#include <QMap>
#include <QObject>
#include <QSystemTrayIcon>

//==============================================================================

class WaniKaniWidget;

//==============================================================================

class QtSingleApplication;

//==============================================================================

class WaniKani : public QObject
{
    Q_OBJECT

public:
    WaniKani(int pArgC, char *pArgV[]);
    ~WaniKani();

    int exec();

    void close();

    void updateInterval(const int &pInterval);

    void updateWallpaper(const bool &pForceUpdate = false);

private:
    QtSingleApplication *mApplication;

    QSystemTrayIcon *mTrayIcon;
    QMenu *mTrayIconMenu;

    bool mNeedToCheckWallpaper;

    WaniKaniWidget *mWaniKaniWidget;

    QTimer *mTimer;

    bool mKanjisError;
    QMap<QString, QString> mKanjisState;
    QMap<QString, QString> mOldKanjisState;

    QJsonDocument waniKaniRequest(const QString &pUrl);

    void setWallpaper();

public slots:
    void updateUserInformation();
    void updateKanjis(const bool &pForceUpdate = false);

private slots:
    void trayIconActivated(const QSystemTrayIcon::ActivationReason &pReason);

    void checkWallpaper();
};

//==============================================================================
// End of file
//==============================================================================
