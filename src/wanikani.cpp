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

SrsDistributionInformation::SrsDistributionInformation() :
    mName(QString()),
    mRadicals(QString()),
    mKanji(QString()),
    mVocabulary(QString()),
    mTotal(QString())
{
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

SrsDistribution::SrsDistribution() :
    mApprentice(SrsDistributionInformation()),
    mGuru(SrsDistributionInformation()),
    mMaster(SrsDistributionInformation()),
    mEnlightened(SrsDistributionInformation()),
    mBurned(SrsDistributionInformation())
{
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

Item::Item() :
    mCharacter(QChar()),
    mMeaning(QString()),
    mLevel(0)
{
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

UserSpecific::UserSpecific() :
    mSrs(QString()),
    mSrsNumeric(0),
    mUnlockedDate(0),
    mAvailableDate(0),
    mBurned(false),
    mBurnedDate(0),
    mMeaningCorrect(0),
    mMeaningIncorrect(0),
    mMeaningMaxStreak(0),
    mMeaningCurrentStreak(0),
    mReadingCorrect(0),
    mReadingIncorrect(0),
    mReadingMaxStreak(0),
    mReadingCurrentStreak(0),
    mMeaningNote(QString()),
    mUserSynonyms(QString())
{
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

int UserSpecific::unlockedDate() const
{
    // Return our unlocked date

    return mUnlockedDate;
}

//==============================================================================

int UserSpecific::availableDate() const
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

int UserSpecific::burnedDate() const
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

Radical::Radical() :
    Item(),
    mImage(QString()),
    mUserSpecific(UserSpecific())
{
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

KanjiUserSpecific::KanjiUserSpecific() :
    UserSpecific(),
    mReadingNote(QString())
{
}

//==============================================================================

QString KanjiUserSpecific::readingNote() const
{
    // Return our reading note

    return mReadingNote;
}

//==============================================================================

Kanji::Kanji() :
    Item(),
    mOnyomi(QString()),
    mKunyomi(QString()),
    mNanori(QString()),
    mImportantReading(QString()),
    mUserSpecific(KanjiUserSpecific())
{
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

KanjiUserSpecific Kanji::userSpecific() const
{
    // Return our user specific information

    return mUserSpecific;
}

//==============================================================================

WaniKani::WaniKani() :
    mApiKey(QString()),
    mUserName(QString()),
    mGravatar(QString()),
    mLevel(0),
    mTitle(QString()),
    mAbout(QString()),
    mWebsite(QString()),
    mTwitter(QString()),
    mTopicsCount(0),
    mPostsCount(0),
    mCreationDate(0),
    mVacationDate(0),
    mSrsDistribution(SrsDistribution()),
    mKanjis(Kanjis())
{
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

    if (mApiKey.isEmpty())
        return QJsonDocument();

    // Send a request to WaniKani and convert its response to a JSON document,
    // if possible

    QNetworkAccessManager networkAccessManager;
    QNetworkReply *networkReply = networkAccessManager.get(QNetworkRequest(QString("https://www.wanikani.com/api/v1.4/user/%1/%2").arg(mApiKey, pRequest)));
    QEventLoop eventLoop;

    QObject::connect(networkReply, SIGNAL(finished()),
                     &eventLoop, SLOT(quit()));

    eventLoop.exec();

    QByteArray response = QByteArray();

    if (networkReply->error() == QNetworkReply::NoError)
        response = networkReply->readAll();

    networkReply->deleteLater();

    if (response.isEmpty()) {
        return QJsonDocument();
    } else {
        QJsonDocument res = QJsonDocument::fromJson(response);

        return res.object().toVariantMap()["error"].toMap().count()?QJsonDocument():res;
    }
}

//==============================================================================

void WaniKani::update()
{
    // Retrieve the user's information and his/her SRS distribution

    QJsonDocument srsDistributionResponse = waniKaniRequest("srs-distribution");

    if (   !srsDistributionResponse.isNull()
        && !srsDistributionResponse.object().contains("error")) {
        // Retrieve the user's list of Kanji (and their information)

        QJsonDocument kanjiResponse = waniKaniRequest("kanji/1,2,3,4,5,6,7,8,9,0,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60");

        if (   !kanjiResponse.isNull()
            && !kanjiResponse.object().contains("error")) {
            // Retrieve some of the user's information

            QVariantMap userInformationMap = srsDistributionResponse.object().toVariantMap()["user_information"].toMap();

            mUserName = userInformationMap["username"].toString();
            mGravatar = userInformationMap["gravatar"].toString();
            mLevel = userInformationMap["level"].toInt();
            mTitle = userInformationMap["title"].toString();
            mAbout = userInformationMap["about"].toString();
            mWebsite = userInformationMap["website"].toString();
            mTwitter = userInformationMap["twitter"].toString();
            mTopicsCount = userInformationMap["topics_count"].toInt();
            mPostsCount = userInformationMap["posts_count"].toInt();
            mCreationDate = userInformationMap["creation_date"].toInt();
            mVacationDate = userInformationMap["vacation_date"].toInt();

            // Retrieve the user's SRS distribution

            QVariantMap srsDistributionMap = srsDistributionResponse.object().toVariantMap()["requested_information"].toMap();

            updateSrsDistribution("Apprentice", srsDistributionMap["apprentice"].toMap(), mSrsDistribution.mApprentice);
            updateSrsDistribution("Guru", srsDistributionMap["guru"].toMap(), mSrsDistribution.mGuru);
            updateSrsDistribution("Master", srsDistributionMap["master"].toMap(), mSrsDistribution.mMaster);
            updateSrsDistribution("Enlightened", srsDistributionMap["enlighten"].toMap(), mSrsDistribution.mEnlightened);
            updateSrsDistribution("Burned", srsDistributionMap["burned"].toMap(), mSrsDistribution.mBurned);

            // Retrieve the Kanji and their information

            foreach (const QVariant &kanjiInformation,
                     kanjiResponse.object().toVariantMap()["requested_information"].toList()) {
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
                kanji.mUserSpecific.mUnlockedDate = kanjiUserSpecificInformationMap["unlocked_date"].toInt();
                kanji.mUserSpecific.mAvailableDate = kanjiUserSpecificInformationMap["available_date"].toInt();
                kanji.mUserSpecific.mBurned = kanjiUserSpecificInformationMap["burned"].toBool();
                kanji.mUserSpecific.mBurnedDate = kanjiUserSpecificInformationMap["burned_date"].toInt();
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

            // Let people know that we have been updated

            emit updated();
        } else {
            // Let people know that something went wrong

            emit error();
        }
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

int WaniKani::creationDate() const
{
    // Return our creation date

    return mCreationDate;
}

//==============================================================================

int WaniKani::vacationDate() const
{
    // Return our vacation date

    return mVacationDate;
}

//==============================================================================

SrsDistribution WaniKani::srsDistribution() const
{
    // Return our SRS distribution

    return mSrsDistribution;
}

//==============================================================================

Kanjis WaniKani::kanjis() const
{
    // Return our list of Kanji

    return mKanjis;
}

//==============================================================================
// End of file
//==============================================================================
