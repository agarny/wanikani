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
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>

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

static const auto SettingsFileName     = QStringLiteral("FileName");
static const auto SettingsApiKey       = QStringLiteral("ApiKey");
static const auto SettingsCurrentKanji = QStringLiteral("CurrentKanji");
static const auto SettingsInterval     = QStringLiteral("Interval");
static const auto SettingsFontName     = QStringLiteral("FontName");
static const auto SettingsBoldFont     = QStringLiteral("BoldFont");
static const auto SettingsItalicsFont  = QStringLiteral("ItalicsFont");
static const auto SettingsColor        = QStringLiteral("Color%1%2");

//==============================================================================

static const auto LinkStyle = " style=\"color: rgb(103, 103, 103); outline: 0px; text-decoration: none;\"";

//==============================================================================

Widget::Widget() :
    mGui(new Ui::Widget),
    mInitializing(true),
    mFileName(QString()),
    mColors(QMap<QPushButton *, QRgb>()),
    mKanjiError(false),
    mKanjiState(QMap<QString, QString>()),
    mOldKanjiState(QMap<QString, QString>()),
    mNeedToCheckWallpaper(true)
{
    // Set up our GUI

    mGui->setupUi(this);

    setMinimumSize(QSize(1024, 768));

#ifdef Q_OS_MAC
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

    // Some about information

    QFile versionFile(":/version");

    versionFile.open(QIODevice::ReadOnly);

    QTextStream stream(&versionFile);
    QString version = stream.readAll();

    versionFile.close();

    int currentYear = QDate::currentDate().year();

    mGui->aboutValue->setText("<span style=\"font-size: 19px;\"><strong><a href=\"https://github.com/agarny/wanikani\""+QString(LinkStyle)+">WaniKani</a> "+version+"</strong></span><br/>"
                              "© 2016"+((currentYear > 2016)?QString("-%1").arg(currentYear):QString())+" <a href=\"https://github.com/agarny\""+QString(LinkStyle)+">Alan Garny</a>");

    // Handle signals from our WaniKani object

    connect(&mWaniKani, SIGNAL(updated()),
            this, SLOT(waniKaniUpdated()));
    connect(&mWaniKani, SIGNAL(error()),
            this, SLOT(waniKaniError()));

    // Retrieve our settings and handle a click on our foreground/background
    // push buttons

    on_resetAllPushButton_clicked(true);

    // Retrieve some initial information about the user's kanji

    updateKanji();

    // Use our timer to update our WaniKani object, as well as retrieve the
    // kanji and set our wallpaper

    connect(&mTimer, SIGNAL(timeout()),
            &mWaniKani, SLOT(update()));
    connect(&mTimer, SIGNAL(timeout()),
            this, SLOT(updateKanji()));

    updateInterval(mGui->intervalSpinBox->value());

    // Create and show our system tray icon

    mTrayIcon.setIcon(QIcon(":/icon"));
    mTrayIcon.setToolTip("WaniKani");

    connect(&mTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(const QSystemTrayIcon::ActivationReason &)));

    mTrayIcon.show();

    mInitializing = false;
}

//==============================================================================

#ifdef Q_OS_MAC
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

void Widget::updateInterval(const int &pInterval)
{
    // Update our timer's interval

    mTimer.start(60000*pInterval);
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

void Widget::updateGravatar(const QPixmap &pGravatar)
{
    // Update our gravatar

    mGui->gravatarValue->setPixmap(pGravatar.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
                       "    <tr>\n"
                       "        <td>Radicals:</td>\n"
                       "        <td align=right>"+pInformation.radicals()+"</td>\n"
                       "    </tr>\n"
                       "    <tr>\n"
                       "        <td>Kanji:</td>\n"
                       "        <td align=right>"+pInformation.kanji()+"</td>\n"
                       "    </tr>\n"
                       "    <tr>\n"
                       "        <td>Vocabulary:</td>\n"
                       "        <td align=right>"+pInformation.vocabulary()+"</td>\n"
                       "    </tr>\n"
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

static const QString Kanji =
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

void Widget::updateKanji(const bool &pForceUpdate)
{
    // Reset some internal properties

    mKanjiError = true;
    mKanjiState = QMap<QString, QString>();

    if (pForceUpdate)
        mOldKanjiState = QMap<QString, QString>();

    // Retrieve the list of Kanji (and their state) the user has already studied

    QString request = "kanji";

    if (!mGui->currentKanjiRadioButton->isChecked()) {
        request += "/1";

        for (int i = 2; i <= 60; ++i)
            request += ","+QString::number(i);
    }

    QJsonDocument json = waniKaniRequest(request);

    if (!json.isNull()) {
        mKanjiError = json.object().contains("error");

        QVariantMap requestedInformationMap;

        if (!mKanjiError) {
            foreach (const QVariant &requestedInformation,
                     json.object().toVariantMap()["requested_information"].toList()) {
                requestedInformationMap = requestedInformation.toMap();

                mKanjiState.insert(requestedInformationMap["character"].toString(),
                                   requestedInformationMap["stats"].toMap()["srs"].toString());
            }
        }
    }

    // Update our wallpaper

    updateWallpaper();
}

//==============================================================================

void Widget::updateWallpaper(const bool &pForceUpdate)
{
    // Generate and set the wallpaper, if needed

    if (pForceUpdate || mKanjiError || (mKanjiState != mOldKanjiState)) {
        // Default wallpaper

        QPixmap pixmap;

        pixmap.load(":/wallpaper");

        if (!mKanjiError) {
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
                int crtCharWidth = fontMetrics.width(Kanji.at(0));
                int crtCharHeight = fontMetrics.height();
                int crtNbOfCols = areaWidth/(crtCharWidth+SmallShift);
                int crtNbOfRows =  floor(mKanjiState.size()/crtNbOfCols)
                                  +((mKanjiState.size() % crtNbOfCols)?1:0);

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

            for (int i = 0, j = 0, iMax = Kanji.size(); i < iMax; ++i) {
                if (mKanjiState.keys().contains(Kanji.at(i))) {
                    if (!(j % nbOfCols)) {
                        x = xStart;
                        y += charHeight+(j?SmallShift:0);
                    }

                    QString state = mKanjiState.value(Kanji.at(i));
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
                    painter.drawText(x, y, Kanji.at(i));

                    x += charWidth+SmallShift;

                    ++j;
                }
            }
        }

        // Delete our old wallpaper and save our new one before setting it

        if (!mFileName.isEmpty())
            QFile(mFileName).remove();

        mFileName = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)+QDir::separator()+QString("WaniKani%1.jpg").arg(QDateTime::currentMSecsSinceEpoch()));

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
    // Update our user's information and Kanji (and therefore our wallpaper)

    mWaniKani.setApiKey(mGui->apiKeyValue->text());

    updateKanji(true);
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
    // Update our Kanji (and therefore our wallpaper)

    updateKanji(true);
}

//==============================================================================

void Widget::on_fontComboBox_currentTextChanged(const QString &pFontName)
{
    Q_UNUSED(pFontName);

    // Force the update our wallpaper

    if (!mInitializing)
        updateWallpaper(true);
}

//==============================================================================

void Widget::on_boldFontCheckBox_clicked()
{
    // Force the update our wallpaper

    if (!mInitializing)
        updateWallpaper(true);
}

//==============================================================================

void Widget::on_italicsFontCheckBox_clicked()
{
    // Force the update our wallpaper

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

void Widget::on_resetAllPushButton_clicked(const bool &pRetrieveSettingsOnly)
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

    if (!pRetrieveSettingsOnly) {
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

    if (!pRetrieveSettingsOnly) {
        mInitializing = false;

        updateSrsDistributionPalettes();

        updateKanji(true);
    }
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

    for (int i = 1; i <= 6; ++i) {
        for (int j = 1; j <= 2; ++j)
            settings.setValue(SettingsColor.arg(i).arg(j), mColors.value(qobject_cast<QPushButton *>(qobject_cast<QGridLayout *>(mGui->colorsLayout)->itemAtPosition(i, j)->widget())));
    }

    // Close ourselves

    qApp->quit();
}

//==============================================================================

void Widget::waniKaniUpdated()
{
    // Update the GUI based on our WaniKani information

    updateGravatar(mWaniKani.gravatar());
    updateSrsDistributionPalettes();

    mGui->userInformationValue->setText("<center>"
                                        "    <span style=\"font-size: 15px;\"><strong><a href=\"https://www.wanikani.com/community/people/"+mWaniKani.userName()+"\""+QString(LinkStyle)+">"+mWaniKani.userName()+"</a></strong> of Sect <strong>"+mWaniKani.title()+"</strong></span><br/>"
                                        "    <span style=\"font-size: 11px;\"><strong>Level "+mWaniKani.level()+"</strong></span>"
                                        "</center>");

    updateSrsDistributionInformation(mGui->apprenticeValue, ":/apprentice", mWaniKani.srsDistribution().apprentice());
    updateSrsDistributionInformation(mGui->guruValue, ":/guru", mWaniKani.srsDistribution().guru());
    updateSrsDistributionInformation(mGui->masterValue, ":/master", mWaniKani.srsDistribution().master());
    updateSrsDistributionInformation(mGui->enlightenedValue, ":/enlightened", mWaniKani.srsDistribution().enlightened());
    updateSrsDistributionInformation(mGui->burnedValue, ":/burned", mWaniKani.srsDistribution().burned());

    mGui->userInformationValue->show();
    mGui->apprenticeValue->show();
    mGui->guruValue->show();
    mGui->masterValue->show();
    mGui->enlightenedValue->show();
    mGui->burnedValue->show();
}

//==============================================================================

void Widget::waniKaniError()
{
    // Something went wrong, so hide a few things

    updateGravatar(QPixmap(":/warning"));

    mGui->userInformationValue->hide();
    mGui->apprenticeValue->hide();
    mGui->guruValue->hide();
    mGui->masterValue->hide();
    mGui->enlightenedValue->hide();
    mGui->burnedValue->hide();
}

//==============================================================================

void Widget::trayIconActivated(const QSystemTrayIcon::ActivationReason &pReason)
{
    Q_UNUSED(pReason);

    // Show ourselves in all cases by first making sure that we are in the
    // centre of the screen

    QDesktopWidget desktopWidget;
    QRect availableGeometry = desktopWidget.availableGeometry();

    move(availableGeometry.center()-QPoint(width() >> 1, height() >> 1));

    // Show ourselves

    show();

    // Note: to show ourselves, one would normally use activateWindow() (and
    //       possibly raise()), but depending on the operating system it may or
    //       not bring our widget to the foreground, so instead we do what
    //       follows, depending on the operating system...

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
        updateKanji(true);
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

    mColors.insert(pPushButton, pColor);

    pPushButton->setStyleSheet(QString("QPushButton#%1 {"
                                       "    border: 1px solid gray;"
                                       "    background-color: rgba(%2, %3, %4, %5);"
                                       "}").arg(pPushButton->objectName())
                                           .arg(qRed(pColor))
                                           .arg(qGreen(pColor))
                                           .arg(qBlue(pColor))
                                           .arg(qAlpha(pColor)));
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
