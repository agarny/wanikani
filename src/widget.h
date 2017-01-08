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

#include <QDateTime>
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

class Widget;

//==============================================================================

struct ReviewsTimeLineData
{
    QString date;

    double xStart;
    double xEnd;

    double yStart;
    double yEnd;

    int currentRadicals;
    int allRadicals;

    int currentKanji;
    int allKanji;

    int currentVocabulary;
    int allVocabulary;
};

//==============================================================================

class ReviewsTimeLineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReviewsTimeLineWidget(QWidget *pParent);

    void update(const int &pRange);

    void setRadicalsColor(const QColor &pRadicalsColor);
    void setKanjiColor(const QColor &pKanjiColor);
    void setVocabularyColor(const QColor &pVocabularyColor);

protected:
    virtual void mouseMoveEvent(QMouseEvent *pEvent);
    virtual void paintEvent(QPaintEvent *pEvent);

private:
    Widget *mWidget;

    int mRange;

    QColor mRadicalsColor;
    QColor mKanjiColor;
    QColor mVocabularyColor;

    QList<ReviewsTimeLineData> mData;
};

//==============================================================================

class ProgressBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressBarWidget(QWidget *pParent);

    void setValue(const double &pValue);
    void setColor(const QColor &pColor);

protected:
    virtual void paintEvent(QPaintEvent *pEvent);

private:
    double mValue;

    QColor mColor;
};

//==============================================================================

typedef QMap<QDateTime, int> Reviews;

//==============================================================================

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget();

    QDateTime now() const;

    Reviews currentRadicalsReviews() const;
    Reviews allRadicalsReviews() const;

    Reviews currentKanjiReviews() const;
    Reviews allKanjiReviews() const;

    Reviews currentVocabularyReviews() const;
    Reviews allVocabularyReviews() const;

#ifdef Q_OS_MAC
protected:
    virtual void closeEvent(QCloseEvent *pEvent);
    virtual void keyPressEvent(QKeyEvent *pEvent);
#endif

private:
    Ui::Widget *mGui;

    bool mInitializing;

    WaniKani mWaniKani;

    QString mFileName;

    QTimer mWaniKaniTimer;
    QTimer mReviewsTimeLineTimer;

    QSystemTrayIcon mTrayIcon;

    QMap<QPushButton *, QRgb> mColors;

    QMap<QChar, QString> mCurrentKanjiState;
    QMap<QChar, QString> mAllKanjiState;
    QMap<QChar, QString> mOldKanjiState;

    bool mNeedToCheckWallpaper;

    Reviews mCurrentRadicalsReviews;
    Reviews mAllRadicalsReviews;

    Reviews mCurrentKanjiReviews;
    Reviews mAllKanjiReviews;

    Reviews mCurrentVocabularyReviews;
    Reviews mAllVocabularyReviews;

    QDateTime mNow;
    uint mLevelStartTime;

    QList<int> mRadicalGuruTimes;
    QList<int> mKanjiGuruTimes;

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

    void determineReviews(const Reviews &pCurrentReviews,
                          const Reviews &pAllReviews, QDateTime &pNextDateTime,
                          int &pDiff, int *pNbOfReviews);

    int guruTime(const int &pSrsLevel, const int &pNextReview);

    void resetInternals(const bool &pVisible = true);

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

    void updateTimeRelatedInformation(const int &pRange = -1);

    void updatePushButtonColor();

    void checkWallpaper();
};

//==============================================================================
// End of file
//==============================================================================
