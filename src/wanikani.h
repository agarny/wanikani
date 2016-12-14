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

class WaniKani : public QObject
{
    Q_OBJECT

public:
    explicit WaniKani();

    void setApiKey(const QString &pApiKey);

    QString userName() const;
    QPixmap gravatar() const;
    QString level() const;
    QString title() const;

    SrsDistribution srsDistribution() const;

private:
    QString mApiKey;

    QString mUserName;
    QPixmap mGravatar;
    QString mLevel;
    QString mTitle;

    SrsDistribution mSrsDistribution;

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
