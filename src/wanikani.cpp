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

WaniKani::WaniKani() :
    mApiKey(QString()),
    mUserName(QString()),
    mGravatar(QPixmap()),
    mLevel(0),
    mTitle(QString()),
    mSrsDistribution(SrsDistribution()),
    mAbout(QString()),
    mWebsite(QString()),
    mTwitter(QString()),
    mTopicsCount(0),
    mPostsCount(0),
    mCreationDate(0),
    mVacationDate(0)
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
            // Retrieve the user's gravatar

            QVariantMap userMap = srsDistributionResponse.object().toVariantMap()["user_information"].toMap();
            QNetworkAccessManager networkAccessManager;
            QNetworkReply *networkReply = networkAccessManager.get(QNetworkRequest("https://www.gravatar.com/avatar/"+userMap["gravatar"].toString()));
            QEventLoop eventLoop;

            QObject::connect(networkReply, SIGNAL(finished()),
                             &eventLoop, SLOT(quit()));

            eventLoop.exec();

            QByteArray gravatarData = QByteArray();

            if (networkReply->error() == QNetworkReply::NoError)
                gravatarData = networkReply->readAll();

            networkReply->deleteLater();

            if (gravatarData.isEmpty())
                mGravatar = QPixmap(":/face");
            else
                mGravatar.loadFromData(gravatarData);

            // Retrieve some of the user's information

            mUserName = userMap["username"].toString();
            mLevel = userMap["level"].toInt();
            mTitle = userMap["title"].toString();
            mAbout = userMap["about"].toString();
            mWebsite = userMap["website"].toString();
            mTwitter = userMap["twitter"].toString();
            mTopicsCount = userMap["topics_count"].toInt();
            mPostsCount = userMap["posts_count"].toInt();
            mCreationDate = userMap["creation_date"].toInt();
            mVacationDate = userMap["vacation_date"].toInt();

            // Retrieve the user's SRS distribution

            QVariantMap srsDistributionMap = srsDistributionResponse.object().toVariantMap()["requested_information"].toMap();

            updateSrsDistribution("Apprentice", srsDistributionMap["apprentice"].toMap(), mSrsDistribution.mApprentice);
            updateSrsDistribution("Guru", srsDistributionMap["guru"].toMap(), mSrsDistribution.mGuru);
            updateSrsDistribution("Master", srsDistributionMap["master"].toMap(), mSrsDistribution.mMaster);
            updateSrsDistribution("Enlightened", srsDistributionMap["enlighten"].toMap(), mSrsDistribution.mEnlightened);
            updateSrsDistribution("Burned", srsDistributionMap["burned"].toMap(), mSrsDistribution.mBurned);

            // Retrieve the Kanji and their information

            foreach (const QVariant &kanji,
                     kanjiResponse.object().toVariantMap()["requested_information"].toList()) {
                QVariantMap kanjiMap = kanji.toMap();

//---GRY--- TO BE DONE...
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

QPixmap WaniKani::gravatar() const
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
// End of file
//==============================================================================
