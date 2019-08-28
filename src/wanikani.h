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

#include <QDateTime>
#include <QJsonDocument>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QString>

//==============================================================================

class Common
{
public:
    void reset();

protected:
    bool mHasData = false;
};

//==============================================================================

class User : public Common
{
    friend class WaniKani;

public:
    QDateTime currentVacationStartedAt() const;
    int level() const;
    QString profileUrl() const;
    QString userName() const;

private:
    QDateTime mCurrentVacationStartedAt;
    int mLevel = 0;
    QString mProfileUrl;
    QString mUserName;
};

//==============================================================================

class StudyQueue
{
    friend class WaniKani;

public:
    int lessonsAvailable() const;
    int reviewsAvailable() const;
    uint nextReviewDate() const;
    int reviewsAvailableNextHour() const;
    int reviewsAvailableNextDay() const;

private:
    int mLessonsAvailable = 0;
    int mReviewsAvailable = 0;
    uint mNextReviewDate = 0;
    int mReviewsAvailableNextHour = 0;
    int mReviewsAvailableNextDay = 0;
};

//==============================================================================

class LevelProgression
{
    friend class WaniKani;

public:
    int radicalsProgress() const;
    int radicalsTotal() const;
    int kanjiProgress() const;
    int kanjiTotal() const;

private:
    int mRadicalsProgress = 0;
    int mRadicalsTotal = 0;
    int mKanjiProgress = 0;
    int mKanjiTotal = 0;
};

//==============================================================================

class SrsDistributionInformation
{
    friend class WaniKani;

public:
    QString name() const;
    QString radicals() const;
    QString kanji() const;
    QString vocabulary() const;
    QString total() const;

private:
    QString mName;
    QString mRadicals;
    QString mKanji;
    QString mVocabulary;
    QString mTotal;
};

//==============================================================================

class SrsDistribution
{
    friend class WaniKani;

public:
    SrsDistributionInformation apprentice() const;
    SrsDistributionInformation guru() const;
    SrsDistributionInformation master() const;
    SrsDistributionInformation enlightened() const;
    SrsDistributionInformation burned() const;

private:
    SrsDistributionInformation mApprentice;
    SrsDistributionInformation mGuru;
    SrsDistributionInformation mMaster;
    SrsDistributionInformation mEnlightened;
    SrsDistributionInformation mBurned;
};

//==============================================================================

class Item
{
    friend class WaniKani;

public:
    QChar character() const;
    QString meaning() const;
    int level() const;

private:
    QChar mCharacter;
    QString mMeaning;
    int mLevel = 0;
};

//==============================================================================

class UserSpecific
{
    friend class WaniKani;

public:
    QString srs() const;
    int srsNumeric() const;
    uint unlockedDate() const;
    uint availableDate() const;
    bool burned() const;
    uint burnedDate() const;
    int meaningCorrect() const;
    int meaningIncorrect() const;
    int meaningMaxStreak() const;
    int meaningCurrentStreak() const;
    int readingCorrect() const;
    int readingIncorrect() const;
    int readingMaxStreak() const;
    int readingCurrentStreak() const;
    QString meaningNote() const;
    QString userSynonyms() const;

private:
    QString mSrs;
    int mSrsNumeric = 0;
    uint mUnlockedDate = 0;
    uint mAvailableDate = 0;
    bool mBurned = false;
    uint mBurnedDate = 0;
    int mMeaningCorrect = 0;
    int mMeaningIncorrect = 0;
    int mMeaningMaxStreak = 0;
    int mMeaningCurrentStreak = 0;
    int mReadingCorrect = 0;
    int mReadingIncorrect = 0;
    int mReadingMaxStreak = 0;
    int mReadingCurrentStreak = 0;
    QString mMeaningNote;
    QString mUserSynonyms;
};

//==============================================================================

class Radical : public Item
{
    friend class WaniKani;

public:
    QString image() const;
    UserSpecific userSpecific() const;

private:
    QString mImage;
    UserSpecific mUserSpecific;
};

//==============================================================================

typedef QList<Radical> Radicals;

//==============================================================================

class ExtraUserSpecific : public UserSpecific
{
    friend class WaniKani;

public:
    QString readingNote() const;

private:
    QString mReadingNote;
};

//==============================================================================

class Kanji : public Item
{
    friend class WaniKani;

public:
    QString onyomi() const;
    QString kunyomi() const;
    QString nanori() const;
    QString imporantReading() const;
    ExtraUserSpecific userSpecific() const;

private:
    QString mOnyomi;
    QString mKunyomi;
    QString mNanori;
    QString mImportantReading;
    ExtraUserSpecific mUserSpecific;
};

//==============================================================================

typedef QList<Kanji> Kanjis;

//==============================================================================

class Vocabulary : public Item
{
    friend class WaniKani;

public:
    QString kana() const;
    ExtraUserSpecific userSpecific() const;

private:
    QString mKana;
    ExtraUserSpecific mUserSpecific;
};

//==============================================================================

typedef QList<Vocabulary> Vocabularies;

//==============================================================================

class QNetworkAccessManager;
class QNetworkReply;

//==============================================================================

static const int GravatarSize = 80;

//==============================================================================

class WaniKani : public QObject
{
    Q_OBJECT

public:
    explicit WaniKani();
    ~WaniKani();

    void setApiKey(const QString &pApiKey);
    void setApiToken(const QString &pApiToken);

    User user() const;
    StudyQueue studyQueue() const;
    LevelProgression levelProgression() const;
    SrsDistribution srsDistribution() const;
    Radicals radicals() const;
    Kanjis kanjis() const;
    Vocabularies vocabularies() const;

    void forceUpdate();

private:
    QString mApiKey;
    QString mApiToken;

    User mUser;
    StudyQueue mStudyQueue;
    LevelProgression mLevelProgression;
    SrsDistribution mSrsDistribution;
    Radicals mRadicals;
    Kanjis mKanjis;
    Vocabularies mVocabularies;

    QNetworkAccessManager *mNetworkAccessManager;

    QJsonDocument mUserResponse;
    QJsonDocument mStudyQueueResponse;
    QJsonDocument mLevelProgressionResponse;
    QJsonDocument mSrsDistributionResponse;
    QJsonDocument mRadicalsResponse;
    QJsonDocument mKanjiResponse;
    QJsonDocument mVocabularyResponse;

    int mNbOfReplies = 0;
    int mNbOfNeededReplies = 7;

    QNetworkReply * waniKaniNetworkReply(const QString &pRequest);
    QNetworkReply * waniKaniV2NetworkReply(const QString &pRequest);
    QJsonDocument waniKaniJsonResponse(QNetworkReply *pNetworkReply);

    bool validJsonDocument(const QJsonDocument &pJsonDocument);

    void checkNbOfReplies();

    void doUpdate(bool pForce = false);

    void updateSrsDistribution(const QString &pName,
                               const QVariantMap &pVariantMap,
                               SrsDistributionInformation &pSrsDistributionInformation);

signals:
    void updated();
    void error();

public slots:
    void update();

private slots:
    void userReply();

    void studyQueueReply();
    void levelProgressionReply();
    void srsDistributionReply();
    void radicalsReply();
    void kanjiReply();
    void vocabularyReply();
};

//==============================================================================
// End of file
//==============================================================================
