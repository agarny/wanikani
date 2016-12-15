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

#include <QJsonDocument>
#include <QObject>
#include <QPixmap>
#include <QList>
#include <QString>

//==============================================================================

class SrsDistributionInformation
{
    friend class WaniKani;

public:
    explicit SrsDistributionInformation();

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
    explicit SrsDistribution();

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

class KanjiUserSpecific
{
    friend class WaniKani;

public:
    explicit KanjiUserSpecific();

    QString srs() const;
    int srsNumeric() const;
    int unlockedDate() const;
    int availableDate() const;
    bool burned() const;
    int burnedDate() const;
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
    QString readingNote() const;

private:
    QString mSrs;
    int mSrsNumeric;
    int mUnlockedDate;
    int mAvailableDate;
    bool mBurned;
    int mBurnedDate;
    int mMeaningCorrect;
    int mMeaningIncorrect;
    int mMeaningMaxStreak;
    int mMeaningCurrentStreak;
    int mReadingCorrect;
    int mReadingIncorrect;
    int mReadingMaxStreak;
    int mReadingCurrentStreak;
    QString mMeaningNote;
    QString mUserSynonyms;
    QString mReadingNote;
};

//==============================================================================

class Kanji
{
    friend class WaniKani;

public:
    explicit Kanji();

    QChar character() const;
    QString meaning() const;
    QString onyomi() const;
    QString kunyomi() const;
    QString nanori() const;
    QString imporantReading() const;
    int level() const;
    KanjiUserSpecific userSpecific() const;

private:
    QChar mCharacter;
    QString mMeaning;
    QString mOnyomi;
    QString mKunyomi;
    QString mNanori;
    QString mImportantReading;
    int mLevel;
    KanjiUserSpecific mUserSpecific;
};

//==============================================================================

typedef QList<Kanji> KanjiList;

//==============================================================================

class WaniKani : public QObject
{
    Q_OBJECT

public:
    explicit WaniKani();

    void setApiKey(const QString &pApiKey);

    QString userName() const;
    QPixmap gravatar() const;
    int level() const;
    QString title() const;
    QString about() const;
    QString website() const;
    QString twitter() const;
    int topicsCount() const;
    int postsCount() const;
    int creationDate() const;
    int vacationDate() const;

    SrsDistribution srsDistribution() const;
    KanjiList kanjiList() const;

private:
    QString mApiKey;

    QString mUserName;
    QPixmap mGravatar;
    int mLevel;
    QString mTitle;
    QString mAbout;
    QString mWebsite;
    QString mTwitter;
    int mTopicsCount;
    int mPostsCount;
    int mCreationDate;
    int mVacationDate;

    SrsDistribution mSrsDistribution;
    KanjiList mKanjiList;

    QJsonDocument waniKaniRequest(const QString &pRequest);

    void updateSrsDistribution(const QString &pName,
                               const QVariantMap &pVariantMap,
                               SrsDistributionInformation &pSrsDistributionInformation);

signals:
    void updated();
    void error();

public slots:
    void update();
};

//==============================================================================
// End of file
//==============================================================================
