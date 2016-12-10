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
#include <QWidget>

//==============================================================================

namespace Ui {
    class WaniKaniWidget;
}

//==============================================================================

class WaniKani;

//==============================================================================

class QMenu;
class QPushButton;

//==============================================================================

class WaniKaniWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WaniKaniWidget(WaniKani *pWaniKani);
    ~WaniKaniWidget();

    QString fileName() const;
    void setFileName(const QString &pFileName);

    QString apiKey() const;

    int interval() const;

    bool currentKanjis() const;

    QString fontName() const;
    bool boldFont() const;
    bool italicsFont() const;

    QColor color(const int &pRow, const int &pColumn) const;

protected:
    virtual void closeEvent(QCloseEvent *pEvent);

private:
    Ui::WaniKaniWidget *mGui;

    bool mInitializing;

    QString mFileName;

    WaniKani *mWaniKani;

    QString mVersion;

    QMenu *mPopupMenu;

    QAction *mAboutAction;
    QAction *mQuitAction;

    QMap<QPushButton *, QRgb> mColors;

    void setPushButtonColor(QPushButton *pPushButton, const QRgb &pColor);

private slots:
    void on_apiKeyValue_returnPressed();

    void on_intervalSpinBox_valueChanged(int pInterval);
    void on_forceUpdateButton_clicked();

    void on_fontComboBox_currentTextChanged(const QString &pFontName);
    void on_boldFontCheckBox_clicked();
    void on_italicsFontCheckBox_clicked();

    void on_swapPushButton_clicked();

    void on_resetAllPushButton_clicked(const bool &pRetrieveSettingsOnly = false);

    void updateLevels();

    void updatePushButtonColor();

    void about();
};

//==============================================================================
// End of file
//==============================================================================
