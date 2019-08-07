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

#include "wanikani.h"

//==============================================================================

#include <QEventLoop>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

//==============================================================================

#include "zlib.h"

//==============================================================================

int StudyQueue::lessonsAvailable() const
{
    // Return our number of lessons available

    return mLessonsAvailable;
}

//==============================================================================

int StudyQueue::reviewsAvailable() const
{
    // Return our number of reviews available

    return mReviewsAvailable;
}

//==============================================================================

uint StudyQueue::nextReviewDate() const
{
    // Return our next review date

    return mNextReviewDate;
}

//==============================================================================

int StudyQueue::reviewsAvailableNextHour() const
{
    // Return our number of reviews available within the next hour

    return mReviewsAvailableNextHour;
}

//==============================================================================

int StudyQueue::reviewsAvailableNextDay() const
{
    // Return our number of reviews available within the next day

    return mReviewsAvailableNextDay;
}

//==============================================================================

int LevelProgression::radicalsProgress() const
{
    // Return our radicals progress

    return mRadicalsProgress;
}

//==============================================================================

int LevelProgression::radicalsTotal() const
{
    // Return our total number of radicals

    return mRadicalsTotal;
}

//==============================================================================

int LevelProgression::kanjiProgress() const
{
    // Return our Kanji progress

    return mKanjiProgress;
}

//==============================================================================

int LevelProgression::kanjiTotal() const
{
    // Return our total number of Kanji

    return mKanjiTotal;
}

//==============================================================================

QString SrsDistributionInformation::name() const
{
    // Return our name

    return mName;
}

//==============================================================================

QString SrsDistributionInformation::radicals() const
{
    // Return our number of radicals

    return mRadicals;
}

//==============================================================================

QString SrsDistributionInformation::kanji() const
{
    // Return our number of Kanji

    return mKanji;
}

//==============================================================================

QString SrsDistributionInformation::vocabulary() const
{
    // Return our number of vocabulary

    return mVocabulary;
}

//==============================================================================

QString SrsDistributionInformation::total() const
{
    // Return our total

    return mTotal;
}

//==============================================================================

SrsDistributionInformation SrsDistribution::apprentice() const
{
    // Return our apprentice SRS distribution

    return mApprentice;
}

//==============================================================================

SrsDistributionInformation SrsDistribution::guru() const
{
    // Return our guru SRS distribution

    return mGuru;
}

//==============================================================================

SrsDistributionInformation SrsDistribution::master() const
{
    // Return our master SRS distribution

    return mMaster;
}

//==============================================================================

SrsDistributionInformation SrsDistribution::enlightened() const
{
    // Return our enlightened SRS distribution

    return mEnlightened;
}

//==============================================================================

SrsDistributionInformation SrsDistribution::burned() const
{
    // Return our burned SRS distribution

    return mBurned;
}

//==============================================================================

QChar Item::character() const
{
    // Return our character

    return mCharacter;
}

//==============================================================================

QString Item::meaning() const
{
    // Return our meaning

    return mMeaning;
}

//==============================================================================

int Item::level() const
{
    // Return our level

    return mLevel;
}

//==============================================================================

QString UserSpecific::srs() const
{
    // Return our SRS

    return mSrs;
}

//==============================================================================

int UserSpecific::srsNumeric() const
{
    // Return our SRS numeric

    return mSrsNumeric;
}

//==============================================================================

uint UserSpecific::unlockedDate() const
{
    // Return our unlocked date

    return mUnlockedDate;
}

//==============================================================================

uint UserSpecific::availableDate() const
{
    // Return our available date

    return mAvailableDate;
}

//==============================================================================

bool UserSpecific::burned() const
{
    // Return whether we are burned

    return mBurned;
}

//==============================================================================

uint UserSpecific::burnedDate() const
{
    // Return our burned date

    return mBurnedDate;
}

//==============================================================================

int UserSpecific::meaningCorrect() const
{
    // Return our number of correct meanings

    return mMeaningCorrect;
}

//==============================================================================

int UserSpecific::meaningIncorrect() const
{
    // Return our number of incorrect meanings

    return mMeaningIncorrect;
}

//==============================================================================

int UserSpecific::meaningMaxStreak() const
{
    // Return our maximum streak for the meaning

    return mMeaningMaxStreak;
}

//==============================================================================

int UserSpecific::meaningCurrentStreak() const
{
    // Return our current streak for the meaning

    return mMeaningCurrentStreak;
}

//==============================================================================

int UserSpecific::readingCorrect() const
{
    // Return our number of correct readings

    return mReadingCorrect;
}

//==============================================================================

int UserSpecific::readingIncorrect() const
{
    // Return our number of incorrect readings

    return mReadingCorrect;
}

//==============================================================================

int UserSpecific::readingMaxStreak() const
{
    // Return our maximum streak for the reading

    return mReadingMaxStreak;
}

//==============================================================================

int UserSpecific::readingCurrentStreak() const
{
    // Return our current streak for the reading

    return mReadingCurrentStreak;
}

//==============================================================================

QString UserSpecific::meaningNote() const
{
    // Return our meaning note

    return mMeaningNote;
}

//==============================================================================

QString UserSpecific::userSynonyms() const
{
    // Return our user synonyms

    return mUserSynonyms;
}

//==============================================================================

QString Radical::image() const
{
    // Return our image

    return mImage;
}

//==============================================================================

UserSpecific Radical::userSpecific() const
{
    // Return our user specific information

    return mUserSpecific;
}

//==============================================================================

QString ExtraUserSpecific::readingNote() const
{
    // Return our reading note

    return mReadingNote;
}

//==============================================================================

QString Kanji::onyomi() const
{
    // Return our Onyomi reading

    return mOnyomi;
}

//==============================================================================

QString Kanji::kunyomi() const
{
    // Return our Kunyomi reading

    return mKunyomi;
}

//==============================================================================

QString Kanji::nanori() const
{
    // Return our Nanori reading

    return mNanori;
}

//==============================================================================

QString Kanji::imporantReading() const
{
    // Return our important reading

    return mImportantReading;
}

//==============================================================================

ExtraUserSpecific Kanji::userSpecific() const
{
    // Return our user specific information

    return mUserSpecific;
}

//==============================================================================

QString Vocabulary::kana() const
{
    // Return our Kana reading

    return mKana;
}

//==============================================================================

ExtraUserSpecific Vocabulary::userSpecific() const
{
    // Return our user specific information

    return mUserSpecific;
}

//==============================================================================

void WaniKani::setApiKey(const QString &pApiKey)
{
    // Set our API key and update our information

    mApiKey = pApiKey;

    update();
}

//==============================================================================

QJsonDocument WaniKani::waniKaniRequest(const QString &pRequest)
{
    // Make sure that we have an API key

    if (mApiKey.isEmpty()) {
        return QJsonDocument();
    }

    // Send a request to WaniKani, asking for its response to be compressed, and
    // then convert its response to a JSON document, if possible and after
    // having uncompressed it

    QNetworkAccessManager networkAccessManager;
    QNetworkRequest networkRequest(QString("https://www.wanikani.com/api/v1.4/user/%1/%2").arg(mApiKey, pRequest));

    networkRequest.setRawHeader("Accept-Encoding", "gzip");

    QNetworkReply *networkReply = networkAccessManager.get(networkRequest);
    QEventLoop eventLoop;

    QObject::connect(networkReply, &QNetworkReply::finished,
                     &eventLoop, &QEventLoop::quit);

    eventLoop.exec();

    QByteArray response = QByteArray();

    if (networkReply->error() == QNetworkReply::NoError) {
        response = networkReply->readAll();
    }

    networkReply->deleteLater();

    if (response.isEmpty()) {
        return QJsonDocument();
    } else {
        // Uncompress the response

        z_stream stream;
        QByteArray json = QByteArray();

        memset(&stream, 0, sizeof(z_stream));

        if (inflateInit2_(&stream, MAX_WBITS+16, ZLIB_VERSION, sizeof(z_stream)) == Z_OK) {
            enum {
                BufferSize = 32768
            };

            Bytef buffer[BufferSize];

            stream.next_in = reinterpret_cast<Bytef *>(response.data());
            stream.avail_in = uint(response.size());

            do {
                stream.next_out = buffer;
                stream.avail_out = BufferSize;

                inflate(&stream, Z_NO_FLUSH);

                if (!stream.msg) {
                    json += QByteArray::fromRawData(reinterpret_cast<char *>(buffer), BufferSize-int(stream.avail_out));
                } else {
                    json = QByteArray();
                }
            } while (!stream.avail_out);

            inflateEnd(&stream);
        } else {
            return QJsonDocument();
        }

        // Convert the response to a JSON document

        QJsonDocument res = QJsonDocument::fromJson(json);

        return res.object().toVariantMap()["error"].toMap().count()?QJsonDocument():res;
    }
}

//==============================================================================

void WaniKani::update()
{
    // Retrieve
    //  - the user's information and study queue
    //  - the user's SRS distribution
    //  - the user's list of radicals (and their information)
    //  - the user's list of Kanji (and their information)
    //  - the user's list of vocabulary (and their information)

    QJsonDocument studyQueueResponse = waniKaniRequest("study-queue");
    QJsonDocument levelProgressionResponse = (   studyQueueResponse.isNull()
                                              || studyQueueResponse.object().contains("error"))?
                                                     QJsonDocument():
                                                     waniKaniRequest("level-progression");
    QJsonDocument srsDistributionResponse = (   levelProgressionResponse.isNull()
                                             || levelProgressionResponse.object().contains("error"))?
                                                    QJsonDocument():
                                                    waniKaniRequest("srs-distribution");
    QJsonDocument radicalsResponse = (   srsDistributionResponse.isNull()
                                      || srsDistributionResponse.object().contains("error"))?
                                             QJsonDocument():
                                             waniKaniRequest("radicals/1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60");
    QJsonDocument kanjiResponse = (   radicalsResponse.isNull()
                                   || radicalsResponse.object().contains("error"))?
                                          QJsonDocument():
                                          waniKaniRequest("kanji/1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60");
    QJsonDocument vocabularyResponse = (   kanjiResponse.isNull()
                                        || kanjiResponse.object().contains("error"))?
                                               QJsonDocument():
                                               waniKaniRequest("vocabulary/1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60");

    if (   !vocabularyResponse.isNull()
        && !vocabularyResponse.object().contains("error")) {
        // Retrieve some of the user's information

        QVariantMap userInformationMap = studyQueueResponse.object().toVariantMap()["user_information"].toMap();

        mUserName = userInformationMap["username"].toString();
        mGravatar = userInformationMap["gravatar"].toString();
        mLevel = userInformationMap["level"].toInt();
        mTitle = userInformationMap["title"].toString();
        mAbout = userInformationMap["about"].toString();
        mWebsite = userInformationMap["website"].toString();
        mTwitter = userInformationMap["twitter"].toString();
        mTopicsCount = userInformationMap["topics_count"].toInt();
        mPostsCount = userInformationMap["posts_count"].toInt();
        mCreationDate = userInformationMap["creation_date"].toUInt();
        mVacationDate = userInformationMap["vacation_date"].toUInt();

        // Retrieve the user's study queue

        QVariantMap studyQueueMap = studyQueueResponse.object().toVariantMap()["requested_information"].toMap();

        mStudyQueue.mLessonsAvailable = studyQueueMap["lessons_available"].toInt();
        mStudyQueue.mReviewsAvailable = studyQueueMap["reviews_available"].toInt();
        mStudyQueue.mNextReviewDate = studyQueueMap["next_review_date"].toUInt();
        mStudyQueue.mReviewsAvailableNextHour = studyQueueMap["reviews_available_next_hour"].toInt();
        mStudyQueue.mReviewsAvailableNextDay = studyQueueMap["reviews_available_next_day"].toInt();

        // Retrieve the user's level progression

        QVariantMap levelProgressionResponseMap = levelProgressionResponse.object().toVariantMap()["requested_information"].toMap();

        mLevelProgression.mRadicalsProgress = levelProgressionResponseMap["radicals_progress"].toInt();
        mLevelProgression.mRadicalsTotal = levelProgressionResponseMap["radicals_total"].toInt();
        mLevelProgression.mKanjiProgress = levelProgressionResponseMap["kanji_progress"].toInt();
        mLevelProgression.mKanjiTotal = levelProgressionResponseMap["kanji_total"].toInt();

        // Retrieve the user's SRS distribution

        QVariantMap srsDistributionMap = srsDistributionResponse.object().toVariantMap()["requested_information"].toMap();

        updateSrsDistribution("Apprentice", srsDistributionMap["apprentice"].toMap(), mSrsDistribution.mApprentice);
        updateSrsDistribution("Guru", srsDistributionMap["guru"].toMap(), mSrsDistribution.mGuru);
        updateSrsDistribution("Master", srsDistributionMap["master"].toMap(), mSrsDistribution.mMaster);
        updateSrsDistribution("Enlightened", srsDistributionMap["enlighten"].toMap(), mSrsDistribution.mEnlightened);
        updateSrsDistribution("Burned", srsDistributionMap["burned"].toMap(), mSrsDistribution.mBurned);

        // Retrieve the radicals and their information

        mRadicals = Radicals();

        for (const auto &radicalInformation : radicalsResponse.object().toVariantMap()["requested_information"].toList()) {
            QVariantMap radicalInformationMap = radicalInformation.toMap();
            Radical radical;

            radical.mCharacter = radicalInformationMap["character"].toString()[0];
            radical.mMeaning = radicalInformationMap["meaning"].toString();
            radical.mImage = radicalInformationMap["image"].toString();
            radical.mLevel = radicalInformationMap["level"].toInt();

            QVariantMap radicalUserSpecificInformationMap = radicalInformationMap["user_specific"].toMap();

            radical.mUserSpecific.mSrs = radicalUserSpecificInformationMap["srs"].toString();
            radical.mUserSpecific.mSrsNumeric = radicalUserSpecificInformationMap["srs_numeric"].toInt();
            radical.mUserSpecific.mUnlockedDate = radicalUserSpecificInformationMap["unlocked_date"].toUInt();
            radical.mUserSpecific.mAvailableDate = radicalUserSpecificInformationMap["burned"].toBool()?0:radicalUserSpecificInformationMap["available_date"].toUInt();
            radical.mUserSpecific.mBurned = radicalUserSpecificInformationMap["burned"].toBool();
            radical.mUserSpecific.mBurnedDate = radicalUserSpecificInformationMap["burned_date"].toUInt();
            radical.mUserSpecific.mMeaningCorrect = radicalUserSpecificInformationMap["meaning_correct"].toInt();
            radical.mUserSpecific.mMeaningIncorrect = radicalUserSpecificInformationMap["meaning_incorrect"].toInt();
            radical.mUserSpecific.mMeaningMaxStreak = radicalUserSpecificInformationMap["meaning_max_streak"].toInt();
            radical.mUserSpecific.mMeaningCurrentStreak = radicalUserSpecificInformationMap["meaning_current_streak"].toInt();
            radical.mUserSpecific.mReadingCorrect = radicalUserSpecificInformationMap["reading_correct"].toInt();
            radical.mUserSpecific.mReadingIncorrect = radicalUserSpecificInformationMap["reading_incorrect"].toInt();
            radical.mUserSpecific.mReadingMaxStreak = radicalUserSpecificInformationMap["reading_max_streak"].toInt();
            radical.mUserSpecific.mReadingCurrentStreak = radicalUserSpecificInformationMap["reading_current_streak"].toInt();
            radical.mUserSpecific.mMeaningNote = radicalUserSpecificInformationMap["meaning_note"].toString();
            radical.mUserSpecific.mUserSynonyms = radicalUserSpecificInformationMap["user_synonyms"].toString();

            mRadicals << radical;
        }

        // Retrieve the Kanji and their information

        mKanjis = Kanjis();

        for (const auto &kanjiInformation : kanjiResponse.object().toVariantMap()["requested_information"].toList()) {
            QVariantMap kanjiInformationMap = kanjiInformation.toMap();
            Kanji kanji;

            kanji.mCharacter = kanjiInformationMap["character"].toString()[0];
            kanji.mMeaning = kanjiInformationMap["meaning"].toString();
            kanji.mOnyomi = kanjiInformationMap["onyomi"].toString();
            kanji.mKunyomi = kanjiInformationMap["kunyomi"].toString();
            kanji.mNanori = kanjiInformationMap["nanori"].toString();
            kanji.mImportantReading = kanjiInformationMap["important_reading"].toString();
            kanji.mLevel = kanjiInformationMap["level"].toInt();

            QVariantMap kanjiUserSpecificInformationMap = kanjiInformationMap["user_specific"].toMap();

            kanji.mUserSpecific.mSrs = kanjiUserSpecificInformationMap["srs"].toString();
            kanji.mUserSpecific.mSrsNumeric = kanjiUserSpecificInformationMap["srs_numeric"].toInt();
            kanji.mUserSpecific.mUnlockedDate = kanjiUserSpecificInformationMap["unlocked_date"].toUInt();
            kanji.mUserSpecific.mAvailableDate = kanjiUserSpecificInformationMap["burned"].toBool()?0:kanjiUserSpecificInformationMap["available_date"].toUInt();
            kanji.mUserSpecific.mBurned = kanjiUserSpecificInformationMap["burned"].toBool();
            kanji.mUserSpecific.mBurnedDate = kanjiUserSpecificInformationMap["burned_date"].toUInt();
            kanji.mUserSpecific.mMeaningCorrect = kanjiUserSpecificInformationMap["meaning_correct"].toInt();
            kanji.mUserSpecific.mMeaningIncorrect = kanjiUserSpecificInformationMap["meaning_incorrect"].toInt();
            kanji.mUserSpecific.mMeaningMaxStreak = kanjiUserSpecificInformationMap["meaning_max_streak"].toInt();
            kanji.mUserSpecific.mMeaningCurrentStreak = kanjiUserSpecificInformationMap["meaning_current_streak"].toInt();
            kanji.mUserSpecific.mReadingCorrect = kanjiUserSpecificInformationMap["reading_correct"].toInt();
            kanji.mUserSpecific.mReadingIncorrect = kanjiUserSpecificInformationMap["reading_incorrect"].toInt();
            kanji.mUserSpecific.mReadingMaxStreak = kanjiUserSpecificInformationMap["reading_max_streak"].toInt();
            kanji.mUserSpecific.mReadingCurrentStreak = kanjiUserSpecificInformationMap["reading_current_streak"].toInt();
            kanji.mUserSpecific.mMeaningNote = kanjiUserSpecificInformationMap["meaning_note"].toString();
            kanji.mUserSpecific.mUserSynonyms = kanjiUserSpecificInformationMap["user_synonyms"].toString();
            kanji.mUserSpecific.mReadingNote = kanjiUserSpecificInformationMap["reading_note"].toString();

            mKanjis << kanji;
        }

        // Retrieve the vocabularies and their information

        mVocabularies = Vocabularies();

        for (const auto &vocabularyInformation : vocabularyResponse.object().toVariantMap()["requested_information"].toList()) {
            QVariantMap vocabularyInformationMap = vocabularyInformation.toMap();
            Vocabulary vocabulary;

            vocabulary.mCharacter = vocabularyInformationMap["character"].toString()[0];
            vocabulary.mKana = vocabularyInformationMap["kana"].toString();
            vocabulary.mMeaning = vocabularyInformationMap["meaning"].toString();
            vocabulary.mLevel = vocabularyInformationMap["level"].toInt();

            QVariantMap vocabularyUserSpecificInformationMap = vocabularyInformationMap["user_specific"].toMap();

            vocabulary.mUserSpecific.mSrs = vocabularyUserSpecificInformationMap["srs"].toString();
            vocabulary.mUserSpecific.mSrsNumeric = vocabularyUserSpecificInformationMap["srs_numeric"].toInt();
            vocabulary.mUserSpecific.mUnlockedDate = vocabularyUserSpecificInformationMap["unlocked_date"].toUInt();
            vocabulary.mUserSpecific.mAvailableDate = vocabularyUserSpecificInformationMap["burned"].toBool()?0:vocabularyUserSpecificInformationMap["available_date"].toUInt();
            vocabulary.mUserSpecific.mBurned = vocabularyUserSpecificInformationMap["burned"].toBool();
            vocabulary.mUserSpecific.mBurnedDate = vocabularyUserSpecificInformationMap["burned_date"].toUInt();
            vocabulary.mUserSpecific.mMeaningCorrect = vocabularyUserSpecificInformationMap["meaning_correct"].toInt();
            vocabulary.mUserSpecific.mMeaningIncorrect = vocabularyUserSpecificInformationMap["meaning_incorrect"].toInt();
            vocabulary.mUserSpecific.mMeaningMaxStreak = vocabularyUserSpecificInformationMap["meaning_max_streak"].toInt();
            vocabulary.mUserSpecific.mMeaningCurrentStreak = vocabularyUserSpecificInformationMap["meaning_current_streak"].toInt();
            vocabulary.mUserSpecific.mReadingCorrect = vocabularyUserSpecificInformationMap["reading_correct"].toInt();
            vocabulary.mUserSpecific.mReadingIncorrect = vocabularyUserSpecificInformationMap["reading_incorrect"].toInt();
            vocabulary.mUserSpecific.mReadingMaxStreak = vocabularyUserSpecificInformationMap["reading_max_streak"].toInt();
            vocabulary.mUserSpecific.mReadingCurrentStreak = vocabularyUserSpecificInformationMap["reading_current_streak"].toInt();
            vocabulary.mUserSpecific.mMeaningNote = vocabularyUserSpecificInformationMap["meaning_note"].toString();
            vocabulary.mUserSpecific.mUserSynonyms = vocabularyUserSpecificInformationMap["user_synonyms"].toString();
            vocabulary.mUserSpecific.mReadingNote = vocabularyUserSpecificInformationMap["reading_note"].toString();

            mVocabularies << vocabulary;
        }

        // Let people know that we have been updated

        emit updated();
    } else {
        // Let people know that something went wrong

        emit error();
    }
}

//==============================================================================

void WaniKani::updateSrsDistribution(const QString &pName,
                                     const QVariantMap &pVariantMap,
                                     SrsDistributionInformation &pSrsDistributionInformation)
{
    // Update the given SRS distribution information using the given variant map

    pSrsDistributionInformation.mName = pName;
    pSrsDistributionInformation.mRadicals = pVariantMap["radicals"].toString();
    pSrsDistributionInformation.mKanji = pVariantMap["kanji"].toString();
    pSrsDistributionInformation.mVocabulary = pVariantMap["vocabulary"].toString();
    pSrsDistributionInformation.mTotal = pVariantMap["total"].toString();
}

//==============================================================================

QString WaniKani::userName() const
{
    // Return our user name

    return mUserName;
}

//==============================================================================

QString WaniKani::gravatar() const
{
    // Return our gravatar

    return mGravatar;
}

//==============================================================================

int WaniKani::level() const
{
    // Return our level

    return mLevel;
}

//==============================================================================

QString WaniKani::title() const
{
    // Return our title

    return mTitle;
}

//==============================================================================

QString WaniKani::about() const
{
    // Return our about information

    return mAbout;
}

//==============================================================================

QString WaniKani::website() const
{
    // Return our website

    return mWebsite;
}

//==============================================================================

QString WaniKani::twitter() const
{
    // Return our Twitter account

    return mTwitter;
}

//==============================================================================

int WaniKani::topicsCount() const
{
    // Return the number of topics we have created

    return mTopicsCount;
}

//==============================================================================

int WaniKani::postsCount() const
{
    // Return the number of posts we have made

    return mPostsCount;
}

//==============================================================================

uint WaniKani::creationDate() const
{
    // Return our creation date

    return mCreationDate;
}

//==============================================================================

uint WaniKani::vacationDate() const
{
    // Return our vacation date

    return mVacationDate;
}

//==============================================================================

StudyQueue WaniKani::studyQueue() const
{
    // Return our study queue

    return mStudyQueue;
}

//==============================================================================

LevelProgression WaniKani::levelProgression() const
{
    // Return our level progression

    return mLevelProgression;
}

//==============================================================================

SrsDistribution WaniKani::srsDistribution() const
{
    // Return our SRS distribution

    return mSrsDistribution;
}

//==============================================================================

Radicals WaniKani::radicals() const
{
    // Return our list of radicals

    return mRadicals;
}

//==============================================================================

Kanjis WaniKani::kanjis() const
{
    // Return our list of Kanji

    return mKanjis;
}

//==============================================================================

Vocabularies WaniKani::vocabularies() const
{
    // Return our list of vocabulary

    return mVocabularies;
}

//==============================================================================
// End of file
//==============================================================================
