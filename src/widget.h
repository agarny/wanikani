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
#include <QLabel>
#include <QMap>
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

class Widget;

//==============================================================================

class LabelWidget : public QLabel
{
    Q_OBJECT

public:
    explicit LabelWidget(QWidget *pParent);

protected:
    void mouseMoveEvent(QMouseEvent *pEvent) override;
};

//==============================================================================

class ProgressBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressBarWidget(QWidget *pParent);

    void setValue(double pValue);
    void setColor(const QColor &pColor);

protected:
    void mouseMoveEvent(QMouseEvent *pEvent) override;
    void paintEvent(QPaintEvent *pEvent) override;

private:
    double mValue;

    QColor mColor;
};

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

    void update(int pRange);

    void setRadicalsColor(const QColor &pRadicalsColor);
    void setKanjiColor(const QColor &pKanjiColor);
    void setVocabularyColor(const QColor &pVocabularyColor);

protected:
    void mouseMoveEvent(QMouseEvent *pEvent) override;
    void paintEvent(QPaintEvent *pEvent) override;

private:
    Widget *mWidget;

    int mRange;

    QColor mRadicalsColor;
    QColor mKanjiColor;
    QColor mVocabularyColor;

    QList<ReviewsTimeLineData> mData;
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

protected:
    bool event(QEvent *pEvent) override;
#ifdef Q_OS_MAC
    void changeEvent(QEvent *pEvent) override;
    void closeEvent(QCloseEvent *pEvent) override;
#endif
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    void keyPressEvent(QKeyEvent *pEvent) override;
#endif

private:
    Ui::Widget *mGui;

    bool mInitializing;

    WaniKani mWaniKani;

    QString mFileName;

    QTimer mWaniKaniTimer;

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
    qint64 mLevelStartTime;

    QList<qint64> mRadicalGuruTimes;
    QList<qint64> mKanjiGuruTimes;

    void retrieveSettings(bool pResetSettings = false);

    void setPushButtonColor(QPushButton *pPushButton, QRgb pColor);

    QColor color(int pRow, int pColumn) const;

    void updateInterval(int pInterval);

    QString iconDataUri(const QString &pIcon, int pWidth = -1, int pHeight = -1,
                        QIcon::Mode pMode = QIcon::Normal);

    void updateSrsDistributionPalettes();
    void updateSrsDistributionInformation(QLabel *pLabel, const QString &pIcon,
                                          const SrsDistributionInformation &pInformation);

    void updateWallpaper(bool pForceUpdate = false);

    void setWallpaper();

    void determineReviews(const Reviews &pCurrentReviews,
                          const Reviews &pAllReviews, QDateTime &pNextDateTime,
                          qint64 &pDiff, int *pNbOfReviews);

    qint64 guruTime(int pSrsLevel = 0, qint64 pNextReview = 0);

    void resetInternals(bool pVisible = true);

private slots:
    void on_apiKeyValue_returnPressed();
    void on_apiTokenValue_returnPressed();

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

    void updateTimeRelatedInformation();

    void updatePushButtonColor();

    void checkWallpaper();
};

//==============================================================================
// End of file
//==============================================================================
