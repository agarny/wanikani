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
    mLevel(QString()),
    mTitle(QString()),
    mSrsDistribution(SrsDistribution())
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
    QNetworkReply *networkReply = networkAccessManager.get(QNetworkRequest(QString("https://www.wanikani.com/api/v1/user/%1/%2").arg(mApiKey, pRequest)));
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

    QJsonDocument jsonDocument = waniKaniRequest("srs-distribution");

    if (!jsonDocument.isNull()) {
        // Retrieve the user's gravatar

        QVariantMap userInformationMap = jsonDocument.object().toVariantMap()["user_information"].toMap();
        QNetworkAccessManager networkAccessManager;
        QNetworkReply *networkReply = networkAccessManager.get(QNetworkRequest("https://www.gravatar.com/avatar/"+userInformationMap["gravatar"].toString()));
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

        mUserName = userInformationMap["username"].toString();
        mLevel = userInformationMap["level"].toString();
        mTitle = userInformationMap["title"].toString();

        // Retrieve the user's SRS distribution

        QVariantMap srsDistributionMap = jsonDocument.object().toVariantMap()["requested_information"].toMap();

        updateSrsDistribution("Apprentice", srsDistributionMap["apprentice"].toMap(), mSrsDistribution.mApprentice);
        updateSrsDistribution("Guru", srsDistributionMap["guru"].toMap(), mSrsDistribution.mGuru);
        updateSrsDistribution("Master", srsDistributionMap["master"].toMap(), mSrsDistribution.mMaster);
        updateSrsDistribution("Enlightened", srsDistributionMap["enlighten"].toMap(), mSrsDistribution.mEnlightened);
        updateSrsDistribution("Burned", srsDistributionMap["burned"].toMap(), mSrsDistribution.mBurned);

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

QPixmap WaniKani::gravatar() const
{
    // Return our gravatar

    return mGravatar;
}

//==============================================================================

QString WaniKani::level() const
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

SrsDistribution WaniKani::srsDistribution() const
{
    // Return our SRS distribution

    return mSrsDistribution;
}

//==============================================================================
// End of file
//==============================================================================
