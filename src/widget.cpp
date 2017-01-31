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

#include "widget.h"

//==============================================================================

#include <QBuffer>
#include <QColorDialog>
#include <QDate>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#ifdef Q_OS_LINUX
#include <QMenu>
#endif
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>
#include <QToolTip>

//==============================================================================

#if defined(Q_OS_WIN)
    #include <Windows.h>
#elif defined(Q_OS_MAC)
    #include "macos.h"

    #include <math.h>
#else
    #include <QProcess>
#endif

//==============================================================================

#include "ui_widget.h"

//==============================================================================

QString timeToString(const int &pSeconds)
{
    // Return the given number of seconds as a formatted string

    if (pSeconds < 60) {
        return "less than 1 minute";
    } else {
        QString res = QString();
        int days = pSeconds/86400;
        int hours = (pSeconds/3600)%24;
        int minutes = (pSeconds/60)%60;

        if (days)
            res += (days == 1)?"1 day":QString("%1 days").arg(days);

        if (hours) {
            if (!res.isEmpty())
                res += ", ";

            res += (hours == 1)?"1 hour":QString("%1 hours").arg(hours);
        }

        if (minutes) {
            if (!res.isEmpty())
                res += ", ";

            res += (minutes == 1)?"1 minute":QString("%1 minutes").arg(minutes);
        }

        int lastCommaPosition = res.lastIndexOf(',');

        if (lastCommaPosition == -1)
            return res;
        else
            return res.left(lastCommaPosition)+" and"+res.right(res.length()-lastCommaPosition-1);
    }
}

//==============================================================================

LabelWidget::LabelWidget(QWidget *pParent) :
    QLabel(pParent)
{
}

//==============================================================================

void LabelWidget::mouseMoveEvent(QMouseEvent *pEvent)
{
    // Default handling of the event

    QLabel::mouseMoveEvent(pEvent);

    // (Immediately) show the tool tip of the label

    QPoint globalPos = pEvent->globalPos();

    QToolTip::showText(globalPos, QString().fill(' ', globalPos.x()*globalPos.y())+toolTip());
}

//==============================================================================

ProgressBarWidget::ProgressBarWidget(QWidget *pParent) :
    QWidget(pParent),
    mValue(0.0),
    mColor(QPalette().highlight().color())
{
    // Minimum and maximum sizes for our progress bar

    setMinimumSize(QSize(0, 6));
    setMaximumSize(QSize(16777215, 6));
}

//==============================================================================

void ProgressBarWidget::mouseMoveEvent(QMouseEvent *pEvent)
{
    // Default handling of the event

    QWidget::mouseMoveEvent(pEvent);

    // (Immediately) show the tool tip of the progress bar

    QPoint globalPos = pEvent->globalPos();

    QToolTip::showText(globalPos, QString().fill(' ', globalPos.x()*globalPos.y())+toolTip());
}

//==============================================================================

void ProgressBarWidget::paintEvent(QPaintEvent *pEvent)
{
    // Paint ourselves (incl. our 90% threshold)

    QPainter painter(this);

    int value = mValue*(width()-2);

    painter.setPen(QPalette().mid().color());
    painter.drawRect(0, 0, width()-1, height()-1);
    painter.drawLine(0.9*(width()-1), 0, 0.9*(width()-1), height()-1);

    if (value)
        painter.fillRect(1, 1, value, height()-2, mColor);

    // Accept the event

    pEvent->accept();
}

//==============================================================================

void ProgressBarWidget::setValue(const double &pValue)
{
    // Update both our value and ourselves, if needed

    double value = qMin(1.0, qMax(pValue, 0.0));

    if (value != mValue) {
        bool needUpdate = int(mValue*width()) != int(value*width());

        mValue = value;

        if (needUpdate)
            update();
    }
}

//==============================================================================

void ProgressBarWidget::setColor(const QColor &pColor)
{
    // Update our color, if needed

    if (pColor != mColor) {
        mColor = pColor;

        update();
    }
}

//==============================================================================

ReviewsTimeLineWidget::ReviewsTimeLineWidget(QWidget *pParent) :
    QWidget(pParent),
    mWidget(qobject_cast<Widget *>(pParent)),
    mRange(6),
    mRadicalsColor(QColor()),
    mKanjiColor(QColor()),
    mVocabularyColor(QColor()),
    mData(QList<ReviewsTimeLineData>())
{
    // Minimum and maximum sizes for our progress bar

    setMinimumSize(QSize(0, 150));
    setMaximumSize(QSize(16777215, 150));
}

//==============================================================================

void ReviewsTimeLineWidget::update(const int &pRange)
{
    // Set our new range and update ourselves

    mRange = pRange;

    QWidget::update();
}

//==============================================================================

void ReviewsTimeLineWidget::setRadicalsColor(const QColor &pRadicalsColor)
{
    // Set the colour for our radicals

    mRadicalsColor = pRadicalsColor;

    QWidget::update();
}

//==============================================================================

void ReviewsTimeLineWidget::setKanjiColor(const QColor &pKanjiColor)
{
    // Set the colour for our Kanji

    mKanjiColor = pKanjiColor;

    QWidget::update();
}

//==============================================================================

void ReviewsTimeLineWidget::setVocabularyColor(const QColor &pVocabularyColor)
{
    // Set the colour for our vocabulary

    mVocabularyColor = pVocabularyColor;

    QWidget::update();
}

//==============================================================================

void ReviewsTimeLineWidget::mouseMoveEvent(QMouseEvent *pEvent)
{
    // Default handling of the event

    QWidget::mouseMoveEvent(pEvent);

    // Show our tool tip based on where our mouse pointer is
    // Note: the leading spaces are just a trick to ensure that the tool tip
    //       gets moved as the user moves the mouse...

    static const QString ReviewsToolTip = "%1\n"
                                          "<table>\n"
                                          "    <thead>\n"
                                          "        <tr>\n"
                                          "            <td colspan=\"5\" align=center><span style=\"font-weight: bold;\">%2 (%3) reviews</span><br/>%4</td>\n"
                                          "        </tr>\n"
                                          "    </thead>\n"
                                          "    <tbody>\n"
                                          "        <tr>\n"
                                          "            <td>Radicals:</td>\n"
                                          "            <td style=\"width: 4px;\"></td>\n"
                                          "            <td align=center>%5</td>\n"
                                          "            <td style=\"width: 4px;\"></td>\n"
                                          "            <td align=center>(%6)</td>\n"
                                          "        </tr>\n"
                                          "        <tr>\n"
                                          "            <td>Kanji:</td>\n"
                                          "            <td style=\"width: 4px;\"></td>\n"
                                          "            <td align=center>%7</td>\n"
                                          "            <td style=\"width: 4px;\"></td>\n"
                                          "            <td align=center>(%8)</td>\n"
                                          "        </tr>\n"
                                          "        <tr>\n"
                                          "            <td>Vocabulary:</td>\n"
                                          "            <td style=\"width: 4px;\"></td>\n"
                                          "            <td align=center>%9</td>\n"
                                          "            <td style=\"width: 4px;\"></td>\n"
                                          "            <td align=center>(%10)</td>\n"
                                          "        </tr>\n"
                                          "    </tbody>\n"
                                          "</table>\n";

    bool toolTipSet = false;
    int x = pEvent->pos().x();
    int y = pEvent->pos().y();

    foreach (const ReviewsTimeLineData &data, mData) {
        if (   (x >= data.xStart) && (x <= data.xEnd)
            && (y >= data.yStart) && (y <= data.yEnd)) {
            QToolTip::showText(pEvent->globalPos(),
                               ReviewsToolTip.arg(QString().fill(' ', x*y))
                                             .arg(data.allRadicals+data.allKanji+data.allVocabulary)
                                             .arg(data.currentRadicals+data.currentKanji+data.currentVocabulary)
                                             .arg(data.date)
                                             .arg(data.allRadicals)
                                             .arg(data.currentRadicals)
                                             .arg(data.allKanji)
                                             .arg(data.currentKanji)
                                             .arg(data.allVocabulary)
                                             .arg(data.currentVocabulary));

            toolTipSet = true;

            break;
        }
    }

    if (!toolTipSet)
        QToolTip::hideText();
}

//==============================================================================

void ReviewsTimeLineWidget::paintEvent(QPaintEvent *pEvent)
{
    // Determine the number of reviews for a given time slot

    QList<QDateTime> dateTimes = QList<QDateTime>() << mWidget->allRadicalsReviews().keys()
                                                    << mWidget->allKanjiReviews().keys()
                                                    << mWidget->allVocabularyReviews().keys();

    std::sort(dateTimes.begin(), dateTimes.end());
    dateTimes.erase(std::unique(dateTimes.begin(), dateTimes.end()), dateTimes.end());

    QMap<QDateTime, int> currentRadicalsReviews = QMap<QDateTime, int>();
    QMap<QDateTime, int> allRadicalsReviews = QMap<QDateTime, int>();
    QMap<QDateTime, int> currentKanjiReviews = QMap<QDateTime, int>();
    QMap<QDateTime, int> allKanjiReviews = QMap<QDateTime, int>();
    QMap<QDateTime, int> currentVocabularyReviews = QMap<QDateTime, int>();
    QMap<QDateTime, int> allVocabularyReviews = QMap<QDateTime, int>();
    QDateTime startTime = QDateTime(mWidget->now().date(), QTime(mWidget->now().time().hour(),
                                                                 (mWidget->now().time().minute() < 15)?
                                                                     0:
                                                                     (mWidget->now().time().minute() < 30)?
                                                                         15:
                                                                         (mWidget->now().time().minute() < 45)?
                                                                             30:
                                                                             45));
    QDateTime endTime = startTime.addSecs(3600*mRange);
    int currentRadicalsReviewsBeforeStartTime = 0;
    int allRadicalsReviewsBeforeStartTime = 0;
    int currentKanjiReviewsBeforeStartTime = 0;
    int allKanjiReviewsBeforeStartTime = 0;
    int currentVocabularyReviewsBeforeStartTime = 0;
    int allVocabularyReviewsBeforeStartTime = 0;

    foreach (const QDateTime &dateTime, dateTimes) {
        if (dateTime < startTime) {
            currentRadicalsReviewsBeforeStartTime += mWidget->currentRadicalsReviews().value(dateTime);
            allRadicalsReviewsBeforeStartTime += mWidget->allRadicalsReviews().value(dateTime);

            currentKanjiReviewsBeforeStartTime += mWidget->currentKanjiReviews().value(dateTime);
            allKanjiReviewsBeforeStartTime += mWidget->allKanjiReviews().value(dateTime);

            currentVocabularyReviewsBeforeStartTime += mWidget->currentVocabularyReviews().value(dateTime);
            allVocabularyReviewsBeforeStartTime += mWidget->allVocabularyReviews().value(dateTime);
        } else if (dateTime < endTime) {
            currentRadicalsReviews.insert(dateTime, mWidget->currentRadicalsReviews().value(dateTime));
            allRadicalsReviews.insert(dateTime, mWidget->allRadicalsReviews().value(dateTime));

            currentKanjiReviews.insert(dateTime, mWidget->currentKanjiReviews().value(dateTime));
            allKanjiReviews.insert(dateTime, mWidget->allKanjiReviews().value(dateTime));

            currentVocabularyReviews.insert(dateTime, mWidget->currentVocabularyReviews().value(dateTime));
            allVocabularyReviews.insert(dateTime, mWidget->allVocabularyReviews().value(dateTime));
        }
    }

    if (   allRadicalsReviewsBeforeStartTime
        || allKanjiReviewsBeforeStartTime
        || allVocabularyReviewsBeforeStartTime) {
        currentRadicalsReviews.insert(startTime, currentRadicalsReviewsBeforeStartTime+currentRadicalsReviews.value(startTime));
        allRadicalsReviews.insert(startTime, allRadicalsReviewsBeforeStartTime+allRadicalsReviews.value(startTime));

        currentKanjiReviews.insert(startTime, currentKanjiReviewsBeforeStartTime+currentKanjiReviews.value(startTime));
        allKanjiReviews.insert(startTime, allKanjiReviewsBeforeStartTime+allKanjiReviews.value(startTime));

        currentVocabularyReviews.insert(startTime, currentVocabularyReviewsBeforeStartTime+currentVocabularyReviews.value(startTime));
        allVocabularyReviews.insert(startTime, allVocabularyReviewsBeforeStartTime+allVocabularyReviews.value(startTime));
    }

    int maxReviews = 0;

    foreach (const QDateTime &dateTime, allRadicalsReviews.keys()) {
        int crtReviews = allRadicalsReviews.value(dateTime)+allKanjiReviews.value(dateTime)+allVocabularyReviews.value(dateTime);

        if (crtReviews > maxReviews)
            maxReviews = crtReviews;
    }

    int reviewsRange = 10*(ceil(0.1*maxReviews));
    int reviewsStep = (reviewsRange > 10)?
                          (reviewsRange > 100)?
                              (reviewsRange > 200)?
                                  50:
                                  20:
                              10:
                          2;

    // Determine where to start painting things, as well as the time and reviews
    // major/minor lines

    static const int Space = 4;

    QPainter painter(this);
    QFont font = painter.font();

    font.setPixelSize(11);

    painter.setFont(font);

    QFontMetrics fontMetrics = painter.fontMetrics();
    int xShift = fontMetrics.width(QString::number(reviewsRange))+Space;
    int yShift = fontMetrics.height();
    int canvasWidth = width()-xShift;
    int canvasHeight = height()-yShift-Space;
    double canvasWidthOverRange = double(canvasWidth-1)/mRange;
    int timeMajorStep = 1;

    while (timeMajorStep*canvasWidthOverRange < 72.0) {
        if (timeMajorStep == 1)
            timeMajorStep = 3;
        else
            timeMajorStep *= 2;
    }

    double timeMinorStep = (timeMajorStep == 1)?
                               0.25:
                               (timeMajorStep == 12)?
                                   3.0:
                                   1.0;

    // Paint our background

    painter.fillRect(0, 0, width(), height(), QPalette().button());

    // Paint the minor time lines

    painter.translate(xShift, yShift);

    QPen pen = painter.pen();

    pen.setColor(Qt::lightGray);
    pen.setStyle(Qt::DotLine);

    painter.setPen(pen);

    int startTimeHour = startTime.time().hour();
    double startTimeHourAndMinutes = startTimeHour+startTime.time().minute()/60.0;
    double xDayShift = -startTimeHourAndMinutes/mRange*(canvasWidth-1);

    for (double i = 0.0, iMax = mRange+startTimeHourAndMinutes; i <= iMax; i += timeMinorStep) {
        double x = xDayShift+i*canvasWidthOverRange;

        if (x >= 0)
            painter.drawLine(QPointF(x, -yShift), QPointF(x, canvasHeight-1.0));
    }

    // Paint the reviews lines

    double canvasHeightOverRange = double(canvasHeight-1)/reviewsRange;

    pen.setStyle(Qt::DotLine);

    QTextOption textOption = QTextOption();

    textOption.setAlignment(Qt::AlignRight);

    for (double j = 0.0; j <= reviewsRange; j += reviewsStep) {
        double y = canvasHeight-j*canvasHeightOverRange-1.0;

        pen.setColor(Qt::lightGray);

        painter.setPen(pen);

        painter.drawLine(QPointF(0.0, y), QPointF(canvasWidth-1.0, y));

        pen.setColor(Qt::black);

        painter.setPen(pen);

        painter.drawText(QRectF(QPointF(-xShift-Space, y-0.6*yShift), QSizeF(xShift, yShift)),
                         QString::number(j), textOption);
    }

    // Paint the major time lines

    pen.setStyle(Qt::SolidLine);

    for (double i = 0.0, iMax = mRange+startTimeHour; i <= iMax; i += timeMajorStep) {
        double x = xDayShift+i*canvasWidthOverRange;

        if (x >= 0) {
            int dayHour = fmod(i, 24.0);

            pen.setColor(dayHour?Qt::lightGray:Qt::red);

            painter.setPen(pen);

            painter.drawLine(QPointF(x, -yShift), QPointF(x, canvasHeight-1.0));

            pen.setColor(dayHour?Qt::black:Qt::red);

            painter.setPen(pen);

            painter.drawText(QPointF(x+Space, -Space),
                             dayHour?
                                 QTime(dayHour, 0).toString("hap"):
                                 startTime.addDays(i?i/24:0).toString("ddd"));
        }
    }

    // Paint the various reviews for the different time slots

    double timeMultiplier = canvasWidthOverRange*mRange/(endTime.toTime_t()-startTime.toTime_t());

    mData = QList<ReviewsTimeLineData>();

    canvasHeightOverRange = double(canvasHeight)/reviewsRange;
    // Note: slightly different value from the one above since this time we are
    //       using it with QPainter::fillRect()...

    foreach (const QDateTime &dateTime, allRadicalsReviews.keys()) {
        uint timeDiff = dateTime.toTime_t()-mWidget->now().toTime_t();
        double x = (dateTime.toTime_t()-startTime.toTime_t())*timeMultiplier;
        double xWidth = 900.0*timeMultiplier;

        ReviewsTimeLineData data;

        if (timeDiff <= 0) {
            data.date = "now";
        } else {
            QString day = dateTime.toString("dddd");

            data.date = QString("%1 at %2<br/>i.e. in %3").arg(mWidget->now().toString("dddd").compare(day)?
                                                                   day:
                                                                   (timeDiff < 86400)?
                                                                       "Today":
                                                                       QString("Next %1").arg(day))
                                                          .arg(dateTime.toString("h:mmap"))
                                                          .arg(timeToString(timeDiff));
        }

        data.xStart = x+xShift;
        data.xEnd = data.xStart+xWidth;

        data.yStart = height()-canvasHeight-Space;
        data.yEnd = data.yStart+canvasHeight;

        data.currentRadicals = currentRadicalsReviews.value(dateTime);
        data.allRadicals = allRadicalsReviews.value(dateTime);

        data.currentKanji = currentKanjiReviews.value(dateTime);
        data.allKanji = allKanjiReviews.value(dateTime);

        data.currentVocabulary = currentVocabularyReviews.value(dateTime);
        data.allVocabulary = allVocabularyReviews.value(dateTime);

        mData << data;

        double radicalsReviewsHeight = data.allRadicals*canvasHeightOverRange;
        double kanjiReviewsHeight = data.allKanji*canvasHeightOverRange;
        double vocabularyReviewsHeight = data.allVocabulary*canvasHeightOverRange;

        if (data.currentRadicals || data.currentKanji || data.currentVocabulary) {
            painter.fillRect(QRectF(x, 0,
                                    xWidth, canvasHeight-radicalsReviewsHeight-kanjiReviewsHeight-vocabularyReviewsHeight),
                             Qt::white);
        }

        painter.fillRect(QRectF(x, canvasHeight-radicalsReviewsHeight-kanjiReviewsHeight-vocabularyReviewsHeight,
                                xWidth, radicalsReviewsHeight+kanjiReviewsHeight+vocabularyReviewsHeight),
                         QPalette().button());

        painter.fillRect(QRectF(x, canvasHeight-radicalsReviewsHeight,
                                xWidth, radicalsReviewsHeight),
                         mRadicalsColor);
        painter.fillRect(QRectF(x, canvasHeight-radicalsReviewsHeight-kanjiReviewsHeight,
                                xWidth, kanjiReviewsHeight),
                         mKanjiColor);
        painter.fillRect(QRectF(x, canvasHeight-radicalsReviewsHeight-kanjiReviewsHeight-vocabularyReviewsHeight,
                                xWidth, vocabularyReviewsHeight),
                         mVocabularyColor);
    }

    // Paint our border

    pen.setColor(Qt::lightGray);

    painter.setPen(pen);

    painter.drawRect(0, 0, canvasWidth-1, canvasHeight-1);

    // Accept the event

    pEvent->accept();
}

//==============================================================================

static const auto SettingsFileName        = QStringLiteral("FileName");
static const auto SettingsApiKey          = QStringLiteral("ApiKey");
static const auto SettingsCurrentKanji    = QStringLiteral("CurrentKanji");
static const auto SettingsInterval        = QStringLiteral("Interval");
static const auto SettingsFontName        = QStringLiteral("FontName");
static const auto SettingsBoldFont        = QStringLiteral("BoldFont");
static const auto SettingsItalicsFont     = QStringLiteral("ItalicsFont");
static const auto SettingsColor           = QStringLiteral("Color%1%2");
static const auto SettingsReviewsTimeLine = QStringLiteral("ReviewsTimeLine");

//==============================================================================

static const auto LinkStyle = " style=\"color: rgb(103, 103, 103); outline: 0px; text-decoration: none;\"";

//==============================================================================

Widget::Widget() :
    mGui(new Ui::Widget),
    mInitializing(true),
    mFileName(QString()),
    mColors(QMap<QPushButton *, QRgb>()),
    mCurrentKanjiState(QMap<QChar, QString>()),
    mAllKanjiState(QMap<QChar, QString>()),
    mOldKanjiState(QMap<QChar, QString>()),
    mNeedToCheckWallpaper(true),
    mCurrentRadicalsReviews(Reviews()),
    mAllRadicalsReviews(Reviews()),
    mCurrentKanjiReviews(Reviews()),
    mAllKanjiReviews(Reviews()),
    mCurrentVocabularyReviews(Reviews()),
    mAllVocabularyReviews(Reviews()),
    mNow(QDateTime::currentDateTime()),
    mLevelStartTime(0),
    mRadicalGuruTimes(QList<int>()),
    mKanjiGuruTimes(QList<int>())
{
    // Set up our GUI

    mGui->setupUi(this);

    setMinimumSize(1440, 900);

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    setWindowFlags(Qt::FramelessWindowHint);
#else
    setWindowFlags(Qt::Popup);
#endif

    connect(mGui->currentKanjiRadioButton, SIGNAL(clicked()),
            this, SLOT(updateLevels()));
    connect(mGui->allKanjiRadioButton, SIGNAL(clicked()),
            this, SLOT(updateLevels()));

    for (int i = 1; i <= 6; ++i) {
        for (int j = 1; j <= 2; ++j) {
            connect(qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsLayout)->itemAtPosition(i, j)->widget()), SIGNAL(clicked()),
                    this, SLOT(updatePushButtonColor()));
        }
    }

    connect(mGui->reviewsTimeLineSlider, SIGNAL(valueChanged(int)),
            this, SLOT(updateTimeRelatedInformation(const int &)));

#ifdef Q_OS_MAC
    mGui->apiKeyValue->setAttribute(Qt::WA_MacShowFocusRect, false);
    mGui->intervalSpinBox->setAttribute(Qt::WA_MacShowFocusRect, false);
#endif

    // Some about information

    static const QString About = "<span style=\"font-weight: bold; font-size: 19px;\"><a href=\"https://github.com/agarny/wanikani\""+QString(LinkStyle)+">WaniKani</a> %1</span><br/>"
                                 "© 2016-%2 <a href=\"https://github.com/agarny\""+QString(LinkStyle)+">Alan Garny</a>";

    QFile versionFile(":/version");

    versionFile.open(QIODevice::ReadOnly);

    QTextStream stream(&versionFile);
    QString version = stream.readAll();

    versionFile.close();

    mGui->aboutValue->setText(About.arg(version).arg(QDate::currentDate().year()));

    // Handle signals from our WaniKani object

    connect(&mWaniKani, SIGNAL(updated()),
            this, SLOT(waniKaniUpdated()));
    connect(&mWaniKani, SIGNAL(updated()),
            this, SLOT(updateTimeRelatedInformation()));

    connect(&mWaniKani, SIGNAL(error()),
            this, SLOT(waniKaniError()));
    connect(&mWaniKani, SIGNAL(error()),
            this, SLOT(updateTimeRelatedInformation()));

    // Retrieve our settings

    retrieveSettings();

    // Use our timer to update our WaniKani object

    connect(&mWaniKaniTimer, SIGNAL(timeout()),
            &mWaniKani, SLOT(update()));

    updateInterval(mGui->intervalSpinBox->value());

    // User our other timer to update our reviews time line

    connect(&mReviewsTimeLineTimer, SIGNAL(timeout()),
            this, SLOT(updateTimeRelatedInformation()));

    mReviewsTimeLineTimer.start(1000);

    // Create and show our system tray icon
    // Note: activation of the tray icon doesn't (currently) work on Linux, so
    //       we achieve the same result through a 'fake' context menu...

    mTrayIcon.setIcon(QIcon(":/icon"));
    mTrayIcon.setToolTip("WaniKani");

#ifdef Q_OS_LINUX
    mTrayIcon.setContextMenu(new QMenu());

    connect(mTrayIcon.contextMenu(), SIGNAL(aboutToShow()),
            this, SLOT(trayIconActivated()));
#else
    connect(&mTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated()));
#endif

    mTrayIcon.show();

    mInitializing = false;
}

//==============================================================================

bool Widget::event(QEvent *pEvent)
{
    if (pEvent->type() == QEvent::WindowDeactivate) {
        // We are not active anymore, so handle the even and then hide ourselves

        QWidget::event(pEvent);

        hide();

        return true;
    }

    // Default handling of the event

    return QWidget::event(pEvent);
}

//==============================================================================

#ifdef Q_OS_MAC
void Widget::closeEvent(QCloseEvent *pEvent)
{
    // Prevent ourselves from closing when pressing Cmd+Q

    pEvent->ignore();
}
#endif

//==============================================================================

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
void Widget::keyPressEvent(QKeyEvent *pEvent)
{
    // Hide ourselves if the user presses the escape key

    if (pEvent->key() == Qt::Key_Escape)
        hide();
    else
        QWidget::keyPressEvent(pEvent);
}
#endif

//==============================================================================

QDateTime Widget::now() const
{
    // Return our current date/time

    return mNow;
}

//==============================================================================

Reviews Widget::currentRadicalsReviews() const
{
    // Return our current radicals reviews

    return mCurrentRadicalsReviews;
}

//==============================================================================

Reviews Widget::allRadicalsReviews() const
{
    // Return all our radicals reviews

    return mAllRadicalsReviews;
}

//==============================================================================

Reviews Widget::currentKanjiReviews() const
{
    // Return our current Kanji reviews

    return mCurrentKanjiReviews;
}

//==============================================================================

Reviews Widget::allKanjiReviews() const
{
    // Return all our Kanji reviews

    return mAllKanjiReviews;
}

//==============================================================================

Reviews Widget::currentVocabularyReviews() const
{
    // Return our current vocabulary reviews

    return mCurrentVocabularyReviews;
}

//==============================================================================

Reviews Widget::allVocabularyReviews() const
{
    // Return all our vocabulary reviews

    return mAllVocabularyReviews;
}

//==============================================================================

void Widget::retrieveSettings(const bool &pResetSettings)
{
    // Retrieve all of our settings after having reset some of them, if
    // requested

    QSettings settings;
    bool setWaniKaniApiKey = false;

    if (mInitializing) {
        mFileName = settings.value(SettingsFileName).toString();

        mGui->apiKeyValue->setText(settings.value(SettingsApiKey).toString());

        setWaniKaniApiKey = true;
    }

    if (pResetSettings) {
        mInitializing = true;

        settings.clear();
    }

    if (settings.value(SettingsCurrentKanji, true).toBool())
        mGui->currentKanjiRadioButton->setChecked(true);
    else
        mGui->allKanjiRadioButton->setChecked(true);

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
    mGui->reviewsTimeLineSlider->setValue(settings.value(SettingsReviewsTimeLine, 6).toInt());

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

    if (setWaniKaniApiKey)
        mWaniKani.setApiKey(mGui->apiKeyValue->text());

    if (pResetSettings) {
        mInitializing = false;

        updateSrsDistributionPalettes();

        updateWallpaper(true);
    }
}

//==============================================================================

void Widget::updateInterval(const int &pInterval)
{
    // Update our timer's interval

    mWaniKaniTimer.start(60000*pInterval);
}

//==============================================================================

QString Widget::iconDataUri(const QString &pIcon, const int &pWidth,
                            const int &pHeight, const QIcon::Mode &pMode)
{
    // Convert an icon, which resource name is given, to a data URI, after
    // having resized it, if requested

    QIcon icon(pIcon);

    if (icon.isNull())
        return QString();

    QByteArray data;
    QBuffer buffer(&data);
    QSize iconSize = icon.availableSizes().first();

    buffer.open(QIODevice::WriteOnly);
    icon.pixmap((pWidth == -1)?iconSize.width():pWidth,
                (pHeight == -1)?iconSize.height():pHeight,
                pMode).save(&buffer, "PNG");

    return QString("data:image/png;base64,%1").arg(QString(data.toBase64()));
}

//==============================================================================

static const int GravatarSize = 80;

//==============================================================================

void Widget::updateGravatar(const QPixmap &pGravatar)
{
    // Update our gravatar

    mGui->gravatarValue->setPixmap(pGravatar.scaled(GravatarSize, GravatarSize,
                                                    Qt::KeepAspectRatio,
                                                    Qt::SmoothTransformation));
}

//==============================================================================

void Widget::updateSrsDistributionPalettes()
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

void Widget::updateSrsDistributionInformation(QLabel *pLabel,
                                              const QString &pIcon,
                                              const SrsDistributionInformation &pInformation)
{
    // Update the given SRS distribution information

    pLabel->setText("<img src=\""+iconDataUri(pIcon, 32, 32)+"\"><br/>"+pInformation.total());
    pLabel->setToolTip("<table>\n"
                       "    <thead>\n"
                       "        <tr>\n"
                       "            <td colspan=\"3\" align=center style=\"font-weight: bold;\">"+pInformation.name()+"</td>\n"
                       "        </tr>\n"
                       "    </thead>\n"
                       "    <tbody>\n"
                       "        <tr>\n"
                       "            <td>Radicals:</td>\n"
                       "            <td style=\"width: 4px;\"></td>\n"
                       "            <td align=center>"+pInformation.radicals()+"</td>\n"
                       "        </tr>\n"
                       "        <tr>\n"
                       "            <td>Kanji:</td>\n"
                       "            <td style=\"width: 4px;\"></td>\n"
                       "            <td align=center>"+pInformation.kanji()+"</td>\n"
                       "        </tr>\n"
                       "        <tr>\n"
                       "            <td>Vocabulary:</td>\n"
                       "            <td style=\"width: 4px;\"></td>\n"
                       "            <td align=center>"+pInformation.vocabulary()+"</td>\n"
                       "        </tr>\n"
                       "    </tbody>\n"
                       "</table>\n");
}

//==============================================================================

QJsonDocument Widget::waniKaniRequest(const QString &pRequest)
{
    // Make sure that we have an API key

    if (mGui->apiKeyValue->text().isEmpty())
        return QJsonDocument();

    // Send a request to WaniKani and convert its response to a JSON document,
    // if possible

    QNetworkAccessManager networkAccessManager;
    QNetworkReply *networkReply = networkAccessManager.get(QNetworkRequest(QString("https://www.wanikani.com/api/v1/user/%1/%2").arg(mGui->apiKeyValue->text(), pRequest)));
    QEventLoop eventLoop;

    QObject::connect(networkReply, SIGNAL(finished()),
                     &eventLoop, SLOT(quit()));

    eventLoop.exec();

    QByteArray response = QByteArray();

    if (networkReply->error() == QNetworkReply::NoError)
        response = networkReply->readAll();

    networkReply->deleteLater();

    if (response.isEmpty())
        return QJsonDocument();
    else
        return QJsonDocument::fromJson(response);
}

//==============================================================================

static const QString KanjiTable =
"一二三四五六七八九十口日月田目古吾冒朋明唱晶品呂昌早旭世胃旦胆亘凹凸旧自白百中千舌升昇丸寸専博"
"占上下卓朝只貝貞員見児元頁頑凡負万句肌旬勺的首乙乱直具真工左右有賄貢項刀刃切召昭則副別丁町可頂"
"子孔了女好如母貫兄克小少大多夕汐外名石肖硝砕砂削光太器臭妙省厚奇川州順水氷永泉原願泳沼沖江汁潮"
"源活消況河泊湖測土吐圧埼垣圭封涯寺時均火炎煩淡灯畑災灰点照魚漁里黒墨鯉量厘埋同洞胴向尚字守完宣"
"宵安宴寄富貯木林森桂柏枠梢棚杏桐植枯朴村相机本札暦案燥未末沫味妹朱株若草苦寛薄葉模漠墓暮膜苗兆"
"桃眺犬状黙然荻狩猫牛特告先洗介界茶合塔王玉宝珠現狂皇呈全栓理主注柱金銑鉢銅釣針銘鎮道導辻迅造迫"
"逃辺巡車連軌輸前各格略客額夏処条落冗軍輝運冠夢坑高享塾熟亭京涼景鯨舎周週士吉壮荘売学覚栄書津牧"
"攻敗枚故敬言警計獄訂討訓詔詰話詠詩語読調談諾諭式試弐域賊栽載茂成城誠威滅減桟銭浅止歩渉頻肯企歴"
"武賦正証政定錠走超赴越是題堤建延誕礎婿衣裁装裏壊哀遠猿初布帆幅帽幕幌錦市姉肺帯滞刺制製転芸雨雲"
"曇雷霜冬天橋嬌立泣章競帝童瞳鐘商嫡適滴敵匕北背比昆皆混渇謁褐喝旨脂壱毎敏梅海乞乾腹複欠吹炊歌軟"
"次茨資姿諮賠培剖音暗韻識鏡境亡盲妄荒望方妨坊芳肪訪放激脱説鋭曽増贈東棟凍妊廷染燃賓歳県栃地池虫"
"蛍蛇虹蝶独蚕風己起妃改記包胞砲泡亀電竜滝豚逐遂家嫁豪腸場湯羊美洋詳鮮達羨差着唯焦礁集准進雑雌準"
"奮奪確午許歓権観羽習翌曜濯曰困固国団因姻園回壇店庫庭庁床麻磨心忘忍認忌志誌忠串患思恩応意想息憩"
"恵恐惑感憂寡忙悦恒悼悟怖慌悔憎慣愉惰慎憾憶慕添必泌手看摩我義議犠抹抱搭抄抗批招拓拍打拘捨拐摘挑"
"指持括揮推揚提損拾担拠描操接掲掛研戒械鼻刑型才財材存在乃携及吸扱丈史吏更硬又双桑隻護獲奴怒友抜"
"投没設撃殻支技枝肢茎怪軽叔督寂淑反坂板返販爪妥乳浮将奨採菜受授愛払広拡鉱弁雄台怠治始胎窓去法会"
"至室到致互棄育撤充銃硫流允唆出山拙岩炭岐峠崩密蜜嵐崎入込分貧頒公松翁訟谷浴容溶欲裕鉛沿賞党堂常"
"裳掌皮波婆披破被残殉殊殖列裂烈死葬瞬耳取趣最撮恥職聖敢聴懐慢漫買置罰寧濁環還夫扶渓規替賛潜失鉄"
"迭臣姫蔵臓賢堅臨覧巨拒力男労募劣功勧努励加賀架脇脅協行律復得従徒待往征径彼役徳徹徴懲微街衡稿稼"
"程税稚和移秒秋愁私秩秘称利梨穫穂稲香季委秀透誘穀菌米粉粘粒粧迷粋糧菊奥数楼類漆様求球救竹笑笠笹"
"筋箱筆筒等算答策簿築人佐但住位仲体悠件仕他伏伝仏休仮伯俗信佳依例個健側侍停値倣倒偵僧億儀償仙催"
"仁侮使便倍優伐宿傷保褒傑付符府任賃代袋貸化花貨傾何荷俊傍久畝囚内丙柄肉腐座卒傘匁以似併瓦瓶宮営"
"善年夜液塚幣弊喚換融施旋遊旅勿物易賜尿尼尻泥塀履屋握屈掘堀居据層局遅漏刷尺尽沢訳択昼戸肩房扇炉"
"戻涙雇顧啓示礼祥祝福祉社視奈尉慰款禁襟宗崇祭察擦由抽油袖宙届笛軸甲押岬挿申伸神捜果菓課裸斤析所"
"祈近折哲逝誓暫漸断質斥訴昨詐作雪録尋急穏侵浸寝婦掃当争浄事唐糖康逮伊君群耐需儒端両満画歯曲曹遭"
"漕槽斗料科図用庸備昔錯借惜措散廿庶遮席度渡奔噴墳憤焼暁半伴畔判券巻圏勝藤謄片版之乏芝不否杯矢矯"
"族知智矛柔務霧班帰弓引弔弘強弱沸費第弟巧号朽誇汚与写身射謝老考孝教拷者煮著署暑諸猪渚賭峡狭挟追"
"師帥官棺管父交効較校足促距路露跳躍践踏骨滑髄禍渦過阪阿際障随陪陽陳防附院陣隊墜降階陛隣隔隠堕陥"
"穴空控突究窒窃窪搾窯窮探深丘岳兵浜糸織繕縮繁縦線締維羅練緒続絵統絞給絡結終級紀紅納紡紛紹経紳約"
"細累索総綿絹繰継緑縁網緊紫縛縄幼後幽幾機玄畜蓄弦擁滋慈磁系係孫懸却脚卸御服命令零齢冷領鈴勇通踊"
"疑擬凝範犯厄危宛腕苑怨柳卵留貿印興酉酒酌酵酷酬酪酢酔配酸猶尊豆頭短豊鼓喜樹皿血盆盟盗温監濫鑑猛"
"盛塩銀恨根即爵節退限眼良朗浪娘食飯飲飢餓飾館養飽既概慨平呼坪評刈希凶胸離殺純鈍辛辞梓宰壁避新薪"
"親幸執報叫糾収卑碑陸睦勢熱菱陵亥核刻該劾述術寒醸譲壌嬢毒素麦青精請情晴清静責績積債漬表俵潔契喫"
"害轄割憲生星姓性牲産隆峰縫拝寿鋳籍春椿泰奏実奉俸棒謹勤漢嘆難華垂睡錘乗剰今含吟念琴陰予序預野兼"
"嫌鎌謙廉西価要腰票漂標栗遷覆煙南楠献門問閲閥間簡開閉閣閑聞潤欄闘倉創非俳排悲罪輩扉侯候決快偉違"
"緯衛韓干肝刊汗軒岸幹芋宇余除徐叙途斜塗束頼瀬勅疎速整剣険検倹重動勲働種衝薫病痴痘症疾痢疲疫痛癖"
"匿匠医匹区枢殴欧抑仰迎登澄発廃僚寮療彫形影杉彩彰彦顔須膨参惨修珍診文対紋蚊斉剤済斎粛塁楽薬率渋"
"摂央英映赤赦変跡蛮恋湾黄横把色絶艶肥甘紺某謀媒欺棋旗期碁基甚勘堪貴遺遣舞無組粗租祖阻査助宜畳並"
"普譜湿顕繊霊業撲僕共供異翼洪港暴爆恭選殿井囲耕亜悪円角触解再講購構溝論倫輪偏遍編冊典氏紙婚低抵"
"底民眠捕浦蒲舗補邸郭郡郊部都郵邦郷響郎廊盾循派脈衆逓段鍛后幻司伺詞飼嗣舟舶航般盤搬船艦艇瓜弧孤"
"繭益暇敷来気汽飛沈妻衰衷面革靴覇声呉娯誤蒸承函極牙芽邪雅釈番審翻藩毛耗尾宅託為偽長張帳脹髪展喪"
"巣単戦禅弾桜獣脳悩厳鎖挙誉猟鳥鳴鶴烏蔦鳩鶏島暖媛援緩属嘱偶遇愚隅逆塑岡鋼綱剛缶陶揺謡就懇墾免逸"
"晩勉象像馬駒験騎駐駆駅騒駄驚篤騰虎虜膚虚戯虞慮劇虐鹿薦慶麗熊能態寅演辰辱震振娠唇農濃送関咲鬼醜"
"魂魔魅塊襲嚇朕雰箇錬遵罷屯且藻隷癒丹潟丑卯巳謎椅翔贅芯酎俺闇枕綺鍋醤丼賂伎斐墟蜂拳遜狙噌誰呪也"
"頃叱斬鍵巾爽阜庄瞭崖箸淀堰鰐隙貼蟹鬱々";

//==============================================================================

void Widget::updateWallpaper(const bool &pForceUpdate)
{
    // Generate and set the wallpaper, if needed

    QMap<QChar, QString> kanjiState = mGui->currentKanjiRadioButton->isChecked()?mCurrentKanjiState:mAllKanjiState;

    if (   !kanjiState.isEmpty()
        &&  (pForceUpdate || (kanjiState != mOldKanjiState))) {
        // Keep track our needed Kanji

        mOldKanjiState = kanjiState;

        // Default wallpaper

        QPixmap pixmap;

        pixmap.load(":/wallpaper");

        // Generate the wallpaper

        static const int LeftBorder = 1240;
        static const int Shift = 32;
        static const int SmallShift = 1;

        QDesktopWidget desktopWidget;
        QRect availableGeometry = desktopWidget.availableGeometry();
        QRect geometry = desktopWidget.geometry();

        int areaWidth = pixmap.width()-LeftBorder-2*Shift;
        int areaHeight = double(availableGeometry.height())/geometry.height()*pixmap.height()-2*Shift;

        QFont font = QFont(mGui->fontComboBox->currentText());

        font.setBold(mGui->boldFontCheckBox->isChecked());
        font.setItalic(mGui->italicsFontCheckBox->isChecked());

        int fontPixelSize = 1;
        int charWidth = 0;
        int charHeight = 0;
        int nbOfRows = 0;
        int nbOfCols = 0;
        int descent = 0;

        forever {
            font.setPixelSize(fontPixelSize);

            QFontMetrics fontMetrics(font);
            int crtCharWidth = fontMetrics.width(KanjiTable.at(0));
            int crtCharHeight = fontMetrics.height();
            int crtNbOfCols = areaWidth/(crtCharWidth+SmallShift);
            int crtNbOfRows =  floor(kanjiState.size()/crtNbOfCols)
                              +((kanjiState.size() % crtNbOfCols)?1:0);

            if (crtNbOfRows*crtCharHeight+(crtNbOfRows-1)*SmallShift+fontMetrics.descent() <= areaHeight) {
                charWidth = crtCharWidth;
                charHeight = crtCharHeight;

                nbOfRows = crtNbOfRows;
                nbOfCols = crtNbOfCols;

                descent = fontMetrics.descent();

                ++fontPixelSize;
            } else {
                font.setPixelSize(fontPixelSize-1);

                break;
            }
        }

        QPainter painter(&pixmap);

        painter.setFont(font);

        int xStart = LeftBorder+Shift+((areaWidth-nbOfCols*charWidth-(nbOfCols-1)*SmallShift) >> 1);
        int x = 0;
        int y =  double(availableGeometry.top())/geometry.height()*pixmap.height()
                +Shift+((areaHeight-nbOfRows*charHeight-(nbOfRows-1)*SmallShift) >> 1)-descent;
        int radius = ceil(0.75*(qMax(charWidth, charHeight) >> 3));

        for (int i = 0, j = 0, iMax = KanjiTable.size(); i < iMax; ++i) {
            if (kanjiState.keys().contains(KanjiTable.at(i))) {
                if (!(j % nbOfCols)) {
                    x = xStart;
                    y += charHeight+(j?SmallShift:0);
                }

                QString state = kanjiState.value(KanjiTable.at(i));
                QColor foregroundColor;
                QColor backgroundColor;

                if (!state.compare("apprentice")) {
                    foregroundColor = color(2, 1);
                    backgroundColor = color(2, 2);
                } else if (!state.compare("guru")) {
                    foregroundColor = color(3, 1);
                    backgroundColor = color(3, 2);
                } else if (!state.compare("master")) {
                    foregroundColor = color(4, 1);
                    backgroundColor = color(4, 2);
                } else if (!state.compare("enlighten")) {
                    foregroundColor = color(5, 1);
                    backgroundColor = color(5, 2);
                } else if (!state.compare("burned")) {
                    foregroundColor = color(6, 1);
                    backgroundColor = color(6, 2);
                } else {
                    foregroundColor = color(1, 1);
                    backgroundColor = color(1, 2);
                }

                painter.setPen(foregroundColor);

                QPainterPath path;

                path.addRoundedRect(QRectF(x, y-charHeight+descent, charWidth, charHeight),
                                    radius, radius);

                painter.fillPath(path, QColor(backgroundColor));
                painter.drawText(x, y, KanjiTable.at(i));

                x += charWidth+SmallShift;

                ++j;
            }
        }

        // Delete any old wallpaper and save our new one before setting it

        QString picturesPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)+QDir::separator();
        QDir picturesDir = QDir(picturesPath);

        foreach (const QString &fileName, picturesDir.entryList(QStringList() << "WaniKani*.jpg", QDir::Files|QDir::NoSymLinks))
            QFile(picturesPath+fileName).remove();

        mFileName = QDir::toNativeSeparators(picturesPath+QString("WaniKani%1.jpg").arg(QDateTime::currentMSecsSinceEpoch()));

        pixmap.save(mFileName);

        setWallpaper();
    }

    // Ask for a wallpaper to be checked in about one second, if necessary

    if (mNeedToCheckWallpaper) {
        mNeedToCheckWallpaper = false;

        QTimer::singleShot(1000, this, SLOT(checkWallpaper()));
    }
}

//==============================================================================

void Widget::setWallpaper()
{
    // Set the new wallpaper

#if defined(Q_OS_WIN)
    SystemParametersInfo(SPI_SETDESKWALLPAPER, 0,
                         PVOID(mFileName.utf16()), SPIF_UPDATEINIFILE);
#elif defined(Q_OS_MAC)
    setMacosWallpaper(qPrintable(mFileName));
#else
    QProcess process;

    process.start("gsettings",
                  QStringList() << "set"
                                << "org.gnome.desktop.background"
                                << "picture-options"
                                << "stretched");
    process.waitForFinished();

    process.start("gsettings",
                  QStringList() << "set"
                                << "org.gnome.desktop.background"
                                << "picture-uri"
                                << QUrl::fromLocalFile(mFileName).toString());
    process.waitForFinished();
#endif
}

//==============================================================================

QColor Widget::color(const int &pRow, const int &pColumn) const
{
    // Return whether our font is to be in italics

    QRgb rgba = mColors.value(qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsLayout)->itemAtPosition(pRow, pColumn)->widget()));

    return QColor(qRed(rgba), qGreen(rgba), qBlue(rgba), qAlpha(rgba));
}

//==============================================================================

void Widget::on_apiKeyValue_returnPressed()
{
    // Set our WaniKani API key

    mWaniKani.setApiKey(mGui->apiKeyValue->text());
}

//==============================================================================

void Widget::on_intervalSpinBox_valueChanged(int pInterval)
{
    // Update our timer's interval

    if (!mInitializing)
        updateInterval(pInterval);
}

//==============================================================================

void Widget::on_forceUpdateButton_clicked()
{
    // Update our WaniKani object

    mWaniKani.update();
}

//==============================================================================

void Widget::on_fontComboBox_currentTextChanged(const QString &pFontName)
{
    Q_UNUSED(pFontName);

    // Force the update of our wallpaper

    if (!mInitializing)
        updateWallpaper(true);
}

//==============================================================================

void Widget::on_boldFontCheckBox_clicked()
{
    // Force the update of our wallpaper

    if (!mInitializing)
        updateWallpaper(true);
}

//==============================================================================

void Widget::on_italicsFontCheckBox_clicked()
{
    // Force the update of our wallpaper

    if (!mInitializing)
        updateWallpaper(true);
}

//==============================================================================

void Widget::on_swapPushButton_clicked()
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

    updateWallpaper(true);
}

//==============================================================================

void Widget::on_resetAllPushButton_clicked()
{
    // Retrieve all of our settings after having reset some of them

    retrieveSettings(true);
}

//==============================================================================

void Widget::on_closeToolButton_clicked()
{
    // Keep track of our settings

    QSettings settings;

    settings.setValue(SettingsFileName, mFileName);
    settings.setValue(SettingsApiKey, mGui->apiKeyValue->text());
    settings.setValue(SettingsCurrentKanji, mGui->currentKanjiRadioButton->isChecked());
    settings.setValue(SettingsInterval, mGui->intervalSpinBox->value());
    settings.setValue(SettingsFontName, mGui->fontComboBox->currentText());
    settings.setValue(SettingsBoldFont, mGui->boldFontCheckBox->isChecked());
    settings.setValue(SettingsItalicsFont, mGui->italicsFontCheckBox->isChecked());
    settings.setValue(SettingsReviewsTimeLine, mGui->reviewsTimeLineSlider->value());

    for (int i = 1; i <= 6; ++i) {
        for (int j = 1; j <= 2; ++j)
            settings.setValue(SettingsColor.arg(i).arg(j), mColors.value(qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsLayout)->itemAtPosition(i, j)->widget())));
    }

    // Close ourselves

    qApp->quit();
}

//==============================================================================

void Widget::determineReviews(const Reviews &pCurrentReviews,
                              const Reviews &pAllReviews,
                              QDateTime &pNextDateTime, int &pDiff,
                              int *pNbOfReviews)
{
    // Determine all the given reviews

    foreach (const QDateTime &dateTime, pAllReviews.keys()) {
        int localDiff = mNow.secsTo(dateTime);

        if (localDiff < pDiff) {
            pDiff = localDiff;

            pNextDateTime = dateTime;
        }

        int currentReviews = pCurrentReviews.value(dateTime);
        int allReviews = pAllReviews.value(dateTime);

        if (localDiff <= 0) {
            pNbOfReviews[0] += currentReviews;
            pNbOfReviews[1] += allReviews;
        }

        if (localDiff < 3600) {
            pNbOfReviews[2] += currentReviews;
            pNbOfReviews[3] += allReviews;
        }

        if (localDiff < 86400) {
            pNbOfReviews[4] += currentReviews;
            pNbOfReviews[5] += allReviews;
        }
    }
}

//==============================================================================

int Widget::guruTime(const int &pSrsLevel, const int &pNextReview)
{
    // Make sure that we are not yet at the Guru level

    if (pSrsLevel >= 5)
        return 0;

    // Compute and return the Guru time for the item which SRS level and next
    // review time are given

    static const int SrsIntervals[2][4] = { { 2, 4, 8, 23 },
                                            { 4, 8, 23, 47 } };

    int res = pSrsLevel?pNextReview:0;

    for (int i = pSrsLevel; i < 4; ++i)
        res += (pSrsLevel <= i)*SrsIntervals[mWaniKani.level() > 2][i]*3600;

    return res;
}

//==============================================================================

void Widget::resetInternals(const bool &pVisible)
{
    // Reset some of our internals

    mGui->userInformationValue->setVisible(pVisible);
    mGui->apprenticeValue->setVisible(pVisible);
    mGui->guruValue->setVisible(pVisible);
    mGui->masterValue->setVisible(pVisible);
    mGui->enlightenedValue->setVisible(pVisible);
    mGui->burnedValue->setVisible(pVisible);
    mGui->currentRadicalsProgress->setVisible(pVisible);
    mGui->currentKanjiProgress->setVisible(pVisible);
    mGui->levelStatisticsValue->setVisible(pVisible);

    mGui->topSeparator->setVisible(pVisible);
    mGui->nextLessonsValue->setVisible(pVisible);
    mGui->nextReviewsValue->setVisible(pVisible);
    mGui->nextHourReviewsValue->setVisible(pVisible);
    mGui->nextDayReviewsValue->setVisible(pVisible);
    mGui->reviewsTimeLine->setVisible(pVisible);

    mNow = QDateTime::currentDateTime();

    mCurrentRadicalsReviews = Reviews();
    mAllRadicalsReviews = Reviews();

    mCurrentKanjiState = QMap<QChar, QString>();
    mAllKanjiState = QMap<QChar, QString>();

    mCurrentKanjiReviews = Reviews();
    mAllKanjiReviews = Reviews();

    mCurrentVocabularyReviews = Reviews();
    mAllVocabularyReviews = Reviews();
}

//==============================================================================

void Widget::waniKaniUpdated()
{
    // Retrieve the user's gravatar

    QNetworkAccessManager networkAccessManager;
    QNetworkReply *networkReply = networkAccessManager.get(QNetworkRequest(QString("https://www.gravatar.com/avatar/%1?s=%2&d=404").arg(mWaniKani.gravatar())
                                                                                                                                   .arg(GravatarSize)));
    QEventLoop eventLoop;

    QObject::connect(networkReply, SIGNAL(finished()),
                     &eventLoop, SLOT(quit()));

    eventLoop.exec();

    QByteArray gravatarData = QByteArray();

    if (networkReply->error() == QNetworkReply::NoError)
        gravatarData = networkReply->readAll();

    networkReply->deleteLater();

    QPixmap gravatar;

    if (gravatarData.isEmpty())
        gravatar = QPixmap(":/face");
    else
        gravatar.loadFromData(gravatarData);

    // Update the GUI based on our WaniKani information

    updateGravatar(gravatar);
    updateSrsDistributionPalettes();

    mGui->userInformationValue->setText("<center>\n"
                                        "    <span style=\"font-size: 15px;\"><span style=\"font-weight: bold;\"><a href=\"https://www.wanikani.com/community/people/"+mWaniKani.userName()+"\""+QString(LinkStyle)+">"+mWaniKani.userName()+"</a></span> of Sect <span style=\"font-weight: bold;\">"+mWaniKani.title()+"</span></span><br/>\n"
                                        "    <span style=\"font-weight: bold; font-size: 11px;\">Level "+QString::number(mWaniKani.level())+"</span>\n"
                                        "</center>\n");

    updateSrsDistributionInformation(mGui->apprenticeValue, ":/apprentice", mWaniKani.srsDistribution().apprentice());
    updateSrsDistributionInformation(mGui->guruValue, ":/guru", mWaniKani.srsDistribution().guru());
    updateSrsDistributionInformation(mGui->masterValue, ":/master", mWaniKani.srsDistribution().master());
    updateSrsDistributionInformation(mGui->enlightenedValue, ":/enlightened", mWaniKani.srsDistribution().enlightened());
    updateSrsDistributionInformation(mGui->burnedValue, ":/burned", mWaniKani.srsDistribution().burned());

    // Reset some of our internals

    resetInternals();

    // Retrieve various information about our radicals

    int radicalsProgress = 0;
    int radicalsTotal = 0;
    uint nowTime = mNow.toTime_t();

    mLevelStartTime = 0;
    mRadicalGuruTimes = QList<int>();

    foreach (const Radical &radical, mWaniKani.radicals()) {
        if (radical.level() == mWaniKani.level()) {
            // A radical from our current level, so determine how soon it can
            // reach Guru level

            mRadicalGuruTimes << guruTime(radical.userSpecific().srsNumeric(),
                                          radical.userSpecific().availableDate()-nowTime);

            // Keep track of our radical progress

            if (radical.userSpecific().srsNumeric() == 5)
                ++radicalsProgress;

            ++radicalsTotal;

            // Retrieve, if needed, when we started our current level

            if (   !mLevelStartTime
                ||  (   radical.userSpecific().unlockedDate()
                     && (radical.userSpecific().unlockedDate() < mLevelStartTime))) {
                mLevelStartTime = radical.userSpecific().unlockedDate();
            }
        }

        if (radical.userSpecific().availableDate()) {
            QDateTime dateTime = QDateTime::fromTime_t(radical.userSpecific().availableDate());

            if (radical.level() == mWaniKani.level())
                mCurrentRadicalsReviews.insert(dateTime, mCurrentRadicalsReviews.value(dateTime)+1);

            mAllRadicalsReviews.insert(dateTime, mAllRadicalsReviews.value(dateTime)+1);
        }
    }

    // Retrieve various information about our Kanji

    int kanjiProgress = 0;
    int kanjiTotal = 0;

    mKanjiGuruTimes = QList<int>();

    foreach (const Kanji &kanji, mWaniKani.kanjis()) {
        if (kanji.level() == mWaniKani.level()) {
            // A Kanji from our current level, so determine how soon it can
            // reach Guru level

            mKanjiGuruTimes << guruTime(kanji.userSpecific().srsNumeric(),
                                        kanji.userSpecific().availableDate()-nowTime);

            // Keep track of our Kanji progress

            if (kanji.userSpecific().srsNumeric() == 5)
                ++kanjiProgress;

            ++kanjiTotal;
        }

        if (kanji.level() <= mWaniKani.level())
            mCurrentKanjiState.insert(kanji.character(), kanji.userSpecific().srs());

        mAllKanjiState.insert(kanji.character(), kanji.userSpecific().srs());

        if (kanji.userSpecific().availableDate()) {
            QDateTime dateTime = QDateTime::fromTime_t(kanji.userSpecific().availableDate());

            if (kanji.level() == mWaniKani.level())
                mCurrentKanjiReviews.insert(dateTime, mCurrentKanjiReviews.value(dateTime)+1);

            mAllKanjiReviews.insert(dateTime, mAllKanjiReviews.value(dateTime)+1);
        }
    }

    std::sort(mRadicalGuruTimes.begin(), mRadicalGuruTimes.end());
    std::sort(mKanjiGuruTimes.begin(), mKanjiGuruTimes.end());

    // Retrieve various information about our vocabulary

    foreach (const Vocabulary &vocabulary, mWaniKani.vocabularies()) {
        if (vocabulary.userSpecific().availableDate()) {
            QDateTime dateTime = QDateTime::fromTime_t(vocabulary.userSpecific().availableDate());

            if (vocabulary.level() == mWaniKani.level())
                mCurrentVocabularyReviews.insert(dateTime, mCurrentVocabularyReviews.value(dateTime)+1);

            mAllVocabularyReviews.insert(dateTime, mAllVocabularyReviews.value(dateTime)+1);
        }
    }

    // Determine our radicals and Kanji progressions

    static const QString ProgressToolTip = "<table>\n"
                                           "    <thead>\n"
                                           "        <tr>\n"
                                           "            <td align=center style=\"font-weight: bold;\">%1</td>\n"
                                           "        </tr>\n"
                                           "    </thead>\n"
                                           "    <tbody>\n"
                                           "        <tr>\n"
                                           "            <td align=center>%2/%3 (%4%)</td>\n"
                                           "        </tr>\n"
                                           "    </tbody>\n"
                                           "</table>\n";

    double currentRadicalsValue = double(radicalsProgress)/radicalsTotal;
    double currentKanjiValue = double(kanjiProgress)/kanjiTotal;

    mGui->currentRadicalsProgress->setValue(currentRadicalsValue);
    mGui->currentRadicalsProgress->setToolTip(ProgressToolTip.arg("Radicals Progress")
                                                             .arg(radicalsProgress)
                                                             .arg(radicalsTotal)
                                                             .arg(int(100*currentRadicalsValue)));

    mGui->currentKanjiProgress->setValue(currentKanjiValue);
    mGui->currentKanjiProgress->setToolTip(ProgressToolTip.arg("Kanji Progression")
                                                          .arg(kanjiProgress)
                                                          .arg(kanjiTotal)
                                                          .arg(int(100*currentKanjiValue)));

    // Determine the next, next hour and next day reviews

    QDateTime nextDateTime = mNow;
    int diff = INT_MAX;
    int nbOfRadicalsReviews[6] = {0, 0, 0, 0, 0, 0};
    int nbOfKanjiReviews[6] = {0, 0, 0, 0, 0, 0};
    int nbOfVocabularyReviews[6] = {0, 0, 0, 0, 0, 0};

    determineReviews(mCurrentRadicalsReviews, mAllRadicalsReviews, nextDateTime,
                     diff, nbOfRadicalsReviews);
    determineReviews(mCurrentKanjiReviews, mAllKanjiReviews, nextDateTime, diff,
                     nbOfKanjiReviews);
    determineReviews(mCurrentVocabularyReviews, mAllVocabularyReviews,
                     nextDateTime, diff, nbOfVocabularyReviews);

    if (!nbOfRadicalsReviews[1] && !nbOfKanjiReviews[1] && !nbOfVocabularyReviews[1]) {
        nbOfRadicalsReviews[0] = mCurrentRadicalsReviews.value(nextDateTime);
        nbOfRadicalsReviews[1] = mAllRadicalsReviews.value(nextDateTime);

        nbOfKanjiReviews[0] = mCurrentKanjiReviews.value(nextDateTime);
        nbOfKanjiReviews[1] = mAllKanjiReviews.value(nextDateTime);

        nbOfVocabularyReviews[0] = mCurrentVocabularyReviews.value(nextDateTime);
        nbOfVocabularyReviews[1] = mAllVocabularyReviews.value(nextDateTime);
    }

    static const QString LessonsText = "<center>\n"
                                       "    <span style=\"font-weight: bold; font-size: 15px;\">%1</span><br/>\n"
                                       "</center>\n";
    static const QString ReviewsText = "<center>\n"
                                       "    <span style=\"font-weight: bold; font-size: 15px;\">%1</span><br/>\n"
                                       "    <span style=\"font-size: 11px;\">%2</span>\n"
                                       "</center>\n";
    static const QString ReviewsToolTip = "<table>\n"
                                          "    <tbody>\n"
                                          "        <tr>\n"
                                          "            <td>Radicals:</td>\n"
                                          "            <td style=\"width: 4px;\"></td>\n"
                                          "            <td align=center>%1</td>\n"
                                          "            <td style=\"width: 4px;\"></td>\n"
                                          "            <td align=center>(%2)</td>\n"
                                          "        </tr>\n"
                                          "        <tr>\n"
                                          "            <td>Kanji:</td>\n"
                                          "            <td style=\"width: 4px;\"></td>\n"
                                          "            <td align=center>%3</td>\n"
                                          "            <td style=\"width: 4px;\"></td>\n"
                                          "            <td align=center>(%4)</td>\n"
                                          "        </tr>\n"
                                          "        <tr>\n"
                                          "            <td>Vocabulary:</td>\n"
                                          "            <td style=\"width: 4px;\"></td>\n"
                                          "            <td align=center>%5</td>\n"
                                          "            <td style=\"width: 4px;\"></td>\n"
                                          "            <td align=center>(%6)</td>\n"
                                          "        </tr>\n"
                                          "    </tbody>\n"
                                          "</table>\n";
    static const QString ReviewsLink = "<a href=\"https://www.wanikani.com/review/session\""+QString(LinkStyle)+">%1</a>";
    static const QString Reviews = "%1 (%2) reviews";
    static const QString NoReviews = "No reviews";

    mGui->nextLessonsValue->setText(LessonsText.arg(mWaniKani.studyQueue().lessonsAvailable()?
                                                        QString("<a href=\"https://www.wanikani.com/lesson/session\""+QString(LinkStyle)+">%1 lessons</a>").arg(mWaniKani.studyQueue().lessonsAvailable()):
                                                        "No lessons"));

    int nbOfReviews = nbOfRadicalsReviews[1]+nbOfKanjiReviews[1]+nbOfVocabularyReviews[1];
    int nbOfCurrentReviews = nbOfRadicalsReviews[0]+nbOfKanjiReviews[0]+nbOfVocabularyReviews[0];

    mGui->nextReviewsValue->setText(ReviewsText.arg(nbOfReviews?
                                                        (diff <= 0)?
                                                            QString(ReviewsLink.arg(Reviews)).arg(nbOfReviews).arg(nbOfCurrentReviews):
                                                            Reviews.arg(nbOfReviews).arg(nbOfCurrentReviews):
                                                        NoReviews)
                                               .arg(mWaniKani.vacationDate()?
                                                        QString():
                                                        (diff <= 0)?
                                                            ReviewsLink.arg("now"):
                                                            "in "+timeToString(diff)));
    mGui->nextReviewsValue->setToolTip(ReviewsToolTip.arg(nbOfRadicalsReviews[1]).arg(nbOfRadicalsReviews[0])
                                                     .arg(nbOfKanjiReviews[1]).arg(nbOfKanjiReviews[0])
                                                     .arg(nbOfVocabularyReviews[1]).arg(nbOfVocabularyReviews[0]));

    nbOfReviews = nbOfRadicalsReviews[3]+nbOfKanjiReviews[3]+nbOfVocabularyReviews[3];
    nbOfCurrentReviews = nbOfRadicalsReviews[2]+nbOfKanjiReviews[2]+nbOfVocabularyReviews[2];

    mGui->nextHourReviewsValue->setText(ReviewsText.arg(nbOfReviews?
                                                            Reviews.arg(nbOfReviews).arg(nbOfCurrentReviews):
                                                            NoReviews)
                                                   .arg("within the next hour"));
    mGui->nextHourReviewsValue->setToolTip(ReviewsToolTip.arg(nbOfRadicalsReviews[3]).arg(nbOfRadicalsReviews[2])
                                                         .arg(nbOfKanjiReviews[3]).arg(nbOfKanjiReviews[2])
                                                         .arg(nbOfVocabularyReviews[3]).arg(nbOfVocabularyReviews[2]));

    nbOfReviews = nbOfRadicalsReviews[5]+nbOfKanjiReviews[5]+nbOfVocabularyReviews[5];
    nbOfCurrentReviews = nbOfRadicalsReviews[4]+nbOfKanjiReviews[4]+nbOfVocabularyReviews[4];

    mGui->nextDayReviewsValue->setText(ReviewsText.arg(nbOfReviews?
                                                           Reviews.arg(nbOfReviews).arg(nbOfCurrentReviews):
                                                           NoReviews)
                                                  .arg("within the next day"));
    mGui->nextDayReviewsValue->setToolTip(ReviewsToolTip.arg(nbOfRadicalsReviews[5]).arg(nbOfRadicalsReviews[4])
                                                        .arg(nbOfKanjiReviews[5]).arg(nbOfKanjiReviews[4])
                                                        .arg(nbOfVocabularyReviews[5]).arg(nbOfVocabularyReviews[4]));

    // Update our wallpaper

    updateWallpaper();
}

//==============================================================================

void Widget::waniKaniError()
{
    // Something went wrong, so hide a few things

    updateGravatar(QPixmap(":/warning"));

    resetInternals(false);
}

//==============================================================================

void Widget::trayIconActivated()
{
    // Make sure that we are in the centre of the screen

    QDesktopWidget desktopWidget;
    QRect availableGeometry = desktopWidget.availableGeometry();

    move(availableGeometry.center()-QPoint(width() >> 1, height() >> 1));

    // Show ourselves
    // Note: to show ourselves, one would normally use activateWindow() (and
    //       possibly raise()), but depending on the operating system it may or
    //       not bring our widget to the foreground, so instead we do what
    //       follows, depending on the operating system...

    show();

#if defined(Q_OS_WIN)
    // Show ourselves the Windows way

    // Retrieve our window Id

    HWND mainWinId = reinterpret_cast<HWND>(winId());

    // Bring us to the foreground

    DWORD foregroundThreadProcId = GetWindowThreadProcessId(GetForegroundWindow(), 0);
    DWORD mainThreadProcId = GetWindowThreadProcessId(mainWinId, 0);

    if (foregroundThreadProcId != mainThreadProcId) {
        // Our thread process Id is not that of the foreground window, so attach
        // the foreground thread to ourselves, set ourselves to the foreground,
        // and detach the foreground thread from ourselves

        AttachThreadInput(foregroundThreadProcId, mainThreadProcId, true);

        SetForegroundWindow(mainWinId);
        SetFocus(mainWinId);

        AttachThreadInput(foregroundThreadProcId, mainThreadProcId, false);
    } else {
        // Our thread process Id is that of the foreground window, so just set
        // ourselves to the foreground

        SetForegroundWindow(mainWinId);
    }
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    // We are on Linux or macOS, so we can simply activate the window and raise
    // ourselves

    activateWindow();
    raise();
#else
    #error Unsupported platform
#endif
}

//==============================================================================

void Widget::updateLevels()
{
    // Update the levels to display

    if (!mInitializing)
        updateWallpaper(true);
}

//==============================================================================

void Widget::updateTimeRelatedInformation(const int &pRange)
{
    // Update our level statistics

    mNow = QDateTime::currentDateTime();

    uint nowTime = mNow.toTime_t();

    static const QString LevelStatisticsText = "<center>\n"
                                               "    <table style=\"font-size: 11px;\">\n"
                                               "        <tbody>\n"
                                               "            <tr>\n"
                                               "                <td align=right style=\"font-weight: bold;\">Start:</td>\n"
                                               "                <td style=\"width: 4px;\"></td>\n"
                                               "                <td>%1</td>\n"
                                               "            </tr>\n"
                                               "            <tr>\n"
                                               "                <td align=right style=\"font-weight: bold;\">Finish:</td>\n"
                                               "                <td style=\"width: 4px;\"></td>\n"
                                               "                <td>%2</td>\n"
                                               "            </tr>\n"
                                               "            <tr>\n"
                                               "                <td align=right style=\"font-weight: bold;\">Total:</td>\n"
                                               "                <td style=\"width: 4px;\"></td>\n"
                                               "                <td>%3</td>\n"
                                               "            </tr>\n"
                                               "        </tbody>\n"
                                               "    </table>\n"
                                               "</center>";

    int start = nowTime-mLevelStartTime;
    int finish =  (mRadicalGuruTimes.isEmpty()?
                       guruTime():
                       mRadicalGuruTimes[ceil(0.9*mRadicalGuruTimes.count())-1])
                 +(mKanjiGuruTimes.isEmpty()?
                       guruTime():
                       mKanjiGuruTimes[ceil(0.9*mKanjiGuruTimes.count())-1]);

    mGui->levelStatisticsValue->setText(LevelStatisticsText.arg(mLevelStartTime?timeToString(start):"now",
                                                                timeToString(finish),
                                                                mLevelStartTime?timeToString(start+finish):timeToString(finish)));

    // Update our reviews time line

    int nbOfHours = 6*((pRange == -1)?mGui->reviewsTimeLineSlider->value():pRange);

    mGui->reviewsTimeLine->update(nbOfHours);

    static const QString ReviewsTimeLineText = "<center>\n"
                                               "    <span style=\"font-weight: bold; font-size: 11px;\">%1</span><br/>\n"
                                               "    <span style=\"font-size: 11px;\">within the next %2</span>\n"
                                               "</center>";

    int nbOfReviews = 0;
    int nbOfCurrentReviews = 0;
    QDateTime endTime = mNow.addSecs(3600*nbOfHours);
    QList<QDateTime> dateTimes = QList<QDateTime>() << mAllRadicalsReviews.keys()
                                                    << mAllKanjiReviews.keys()
                                                    << mAllVocabularyReviews.keys();

    std::sort(dateTimes.begin(), dateTimes.end());
    dateTimes.erase(std::unique(dateTimes.begin(), dateTimes.end()), dateTimes.end());

    foreach (const QDateTime &dateTime, dateTimes) {
        if (dateTime < endTime) {
            nbOfCurrentReviews +=  mCurrentRadicalsReviews.value(dateTime)
                                  +mCurrentKanjiReviews.value(dateTime)
                                  +mCurrentVocabularyReviews.value(dateTime);
            nbOfReviews +=  mAllRadicalsReviews.value(dateTime)
                           +mAllKanjiReviews.value(dateTime)
                           +mAllVocabularyReviews.value(dateTime);
        }
    }

    mGui->reviewsTimeLineLabel->setText(ReviewsTimeLineText.arg(QString((nbOfReviews == 1)?
                                                                            "%1 (%2) review":
                                                                            "%1 (%2) reviews").arg(nbOfReviews)
                                                                                              .arg(nbOfCurrentReviews))
                                                           .arg((nbOfHours < 24)?
                                                                    QString("%1 hours").arg(nbOfHours):
                                                                    (nbOfHours == 24)?
                                                                        "day":
                                                                        QString("%1 days").arg(nbOfHours/24.0)));
}

//==============================================================================

void Widget::updatePushButtonColor()
{
    // Update the background colour of the given push button

    QPushButton *pushButton = qobject_cast<QPushButton *>(sender());
    QColorDialog  colorDialog;

    colorDialog.setCurrentColor(pushButton->palette().color(QPalette::Button));
    colorDialog.setOption(QColorDialog::ShowAlphaChannel);

    if (colorDialog.exec() == QDialog::Accepted) {
        setPushButtonColor(pushButton, colorDialog.currentColor().rgba());

        updateSrsDistributionPalettes();

        updateWallpaper(true);
    }
}

//==============================================================================

void Widget::setPushButtonColor(QPushButton *pPushButton, const QRgb &pColor)
{
    // Set the background of the given push button to the given colour

    static const QString PushButtonStyle = "QPushButton#%1 {"
                                           "    border: 1px solid gray;"
                                           "    background-color: rgba(%2, %3, %4, %5);"
                                           "}";

    mColors.insert(pPushButton, pColor);

    pPushButton->setStyleSheet(PushButtonStyle.arg(pPushButton->objectName())
                                              .arg(qRed(pColor))
                                              .arg(qGreen(pColor))
                                              .arg(qBlue(pColor))
                                              .arg(qAlpha(pColor)));

    QColor color;

    color.setRgba(pColor);

    if (pPushButton == mGui->enlightenedBackgroundPushButton) {
        mGui->currentRadicalsProgress->setColor(color);

        mGui->reviewsTimeLine->setRadicalsColor(color);
    } else if (pPushButton == mGui->apprenticeBackgroundPushButton) {
        mGui->currentKanjiProgress->setColor(color);

        mGui->reviewsTimeLine->setKanjiColor(color);
    } else if (pPushButton == mGui->guruBackgroundPushButton) {
        mGui->currentKanjiProgress->setColor(color);

        mGui->reviewsTimeLine->setVocabularyColor(color);
    }
}

//==============================================================================

void Widget::checkWallpaper()
{
    // Retrieve the file name of the current wallpaper

#if defined(Q_OS_WIN)
    WCHAR tempFileName[MAX_PATH];

    SystemParametersInfo(SPI_GETDESKWALLPAPER, sizeof(tempFileName),
                         tempFileName, 0);

    QString wallpaperFileName = QString::fromUtf16(reinterpret_cast<const ushort *>(tempFileName));
#elif defined(Q_OS_MAC)
    QString wallpaperFileName = QUrl(macosWallpaper()).toLocalFile();
#else
    QProcess process;

    process.start("gsettings",
                  QStringList() << "get"
                                << "org.gnome.desktop.background"
                                << "picture-uri");
    process.waitForFinished();

    QString tempFileName = QString(process.readAll()).trimmed();
    QString wallpaperFileName = QUrl(tempFileName.mid(1, tempFileName.length()-2)).toLocalFile();
#endif

    // Update our wallpaper, if the current wallpaper file name is not the same
    // as the one in our settings (which might happen if we switch virtual
    // desktops, for example)

    if (wallpaperFileName.compare(mFileName))
        setWallpaper();

    // Check again in about one second

    QTimer::singleShot(1000, this, SLOT(checkWallpaper()));
}

//==============================================================================
// End of file
//==============================================================================
