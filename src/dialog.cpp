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

#include "dialog.h"

//==============================================================================

#include <QApplication>
#include <QCloseEvent>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPainter>
#include <QSettings>
#include <QTimer>
#include <QVBoxLayout>

//==============================================================================

#include "ui_dialog.h"

//==============================================================================

#if defined(Q_OS_WIN)
    #include <Windows.h>
#elif defined(Q_OS_MAC)
    #include "macos.h"
#else
    #include <QProcess>
#endif

//==============================================================================

#include <math.h>

//==============================================================================

static const auto SettingsApiKey = QStringLiteral("ApiKey");

//==============================================================================

Dialog::Dialog() :
    mGui(new Ui::Dialog)
{
    // Set up our GUI

    mGui->setupUi(this);

    QSettings settings;

    mApiKey = settings.value(SettingsApiKey).toString();

    mGui->apiKeyValue->setText(mApiKey);

    // Create some actions

    mWaniKaniAction = new QAction(tr("WaniKani"), this);
    mQuitAction = new QAction(tr("Quit"), this);

    connect(mWaniKaniAction, SIGNAL(triggered(bool)),
            this, SLOT(showDialog()));
    connect(mQuitAction, SIGNAL(triggered(bool)),
            qApp, SLOT(quit()));

    // Create our system tray icon menu

    mTrayIconMenu = new QMenu(this);

    mTrayIconMenu->addAction(mWaniKaniAction);
    mTrayIconMenu->addSeparator();
    mTrayIconMenu->addAction(mQuitAction);

    // Create and show our system tray icon

    mTrayIcon = new QSystemTrayIcon(this);

    mTrayIcon->setContextMenu(mTrayIconMenu);
    mTrayIcon->setIcon(QIcon(":/icon.png"));

    connect(mTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(const QSystemTrayIcon::ActivationReason &)));

    mTrayIcon->show();

    // Create a timer to generate and set our wallpaper

    mTimer = new QTimer(this);

    connect(mTimer, SIGNAL(timeout()),
            this, SLOT(updateWallpaper()));

    mTimer->start(60000);

    QTimer::singleShot(0, this, SLOT(updateWallpaper()));
}

//==============================================================================

Dialog::~Dialog()
{
    // Keep track of some settings

    QSettings settings;

    settings.setValue(SettingsApiKey, mGui->apiKeyValue->text());
}

//==============================================================================

void Dialog::closeEvent(QCloseEvent *pEvent)
{
    // Hide ourselves rather than closing ourselves

#ifdef Q_OS_OSX
    if (!pEvent->spontaneous() || !isVisible())
        return;
#endif

    if (mTrayIcon->isVisible()) {
        hide();

        pEvent->ignore();
    }
}

//==============================================================================

void Dialog::trayIconActivated(const QSystemTrayIcon::ActivationReason &pReason)
{
    // Show ourselves or our menu, depending on the platofmr on which we are

    if (pReason == QSystemTrayIcon::Trigger) {
#ifdef Q_OS_WIN
        showDialog();
#else
        mTrayIcon->show();
#endif
    }
}

//==============================================================================

void Dialog::showDialog()
{
    // Show ourselves

    show();

    raise();
    activateWindow();
}

//==============================================================================

void Dialog::on_buttonBox_clicked(QAbstractButton *pButton)
{
    // Apply the changes made by the user, if requested

    QDialogButtonBox::StandardButton standardButton = mGui->buttonBox->standardButton(pButton);

    if (   (standardButton == QDialogButtonBox::Ok)
        || (standardButton == QDialogButtonBox::Apply)) {
        // Keep track of the API key and update our wallpaper

        mApiKey = mGui->apiKeyValue->text();

        updateWallpaper();
    }

    if (standardButton == QDialogButtonBox::Ok)
        accept();
    else if (standardButton == QDialogButtonBox::Cancel)
        reject();
}

//==============================================================================

void Dialog::updateWallpaper()
{
    // Full list of Kanjis

    static const QString kanjis =
    "一二三四五六七八九十口日月田目古吾冒朋明唱晶品呂昌早旭世胃旦胆亘凹凸旧自白百中千舌升昇丸寸"
    "専博占上下卓朝只貝貞員見児元頁頑凡負万句肌旬勺的首乙乱直具真工左右有賄貢項刀刃切召昭則副別"
    "丁町可頂子孔了女好如母貫兄克小少大多夕汐外名石肖硝砕砂削光太器臭妙省厚奇川州順水氷永泉原願"
    "泳沼沖江汁潮源活消況河泊湖測土吐圧埼垣圭封涯寺時均火炎煩淡灯畑災灰点照魚漁里黒墨鯉量厘埋同"
    "洞胴向尚字守完宣宵安宴寄富貯木林森桂柏枠梢棚杏桐植枯朴村相机本札暦案燥未末沫味妹朱株若草苦"
    "寛薄葉模漠墓暮膜苗兆桃眺犬状黙然荻狩猫牛特告先洗介界茶合塔王玉宝珠現狂皇呈全栓理主注柱金銑"
    "鉢銅釣針銘鎮道導辻迅造迫逃辺巡車連軌輸前各格略客額夏処条落冗軍輝運冠夢坑高享塾熟亭京涼景鯨"
    "舎周週士吉壮荘売学覚栄書津牧攻敗枚故敬言警計獄訂討訓詔詰話詠詩語読調談諾諭式試弐域賊栽載茂"
    "成城誠威滅減桟銭浅止歩渉頻肯企歴武賦正証政定錠走超赴越是題堤建延誕礎婿衣裁装裏壊哀遠猿初布"
    "帆幅帽幕幌錦市姉肺帯滞刺制製転芸雨雲曇雷霜冬天橋嬌立泣章競帝童瞳鐘商嫡適滴敵匕北背比昆皆混"
    "渇謁褐喝旨脂壱毎敏梅海乞乾腹複欠吹炊歌軟次茨資姿諮賠培剖音暗韻識鏡境亡盲妄荒望方妨坊芳肪訪"
    "放激脱説鋭曽増贈東棟凍妊廷染燃賓歳県栃地池虫蛍蛇虹蝶独蚕風己起妃改記包胞砲泡亀電竜滝豚逐遂"
    "家嫁豪腸場湯羊美洋詳鮮達羨差着唯焦礁集准進雑雌準奮奪確午許歓権観羽習翌曜濯曰困固国団因姻園"
    "回壇店庫庭庁床麻磨心忘忍認忌志誌忠串患思恩応意想息憩恵恐惑感憂寡忙悦恒悼悟怖慌悔憎慣愉惰慎"
    "憾憶慕添必泌手看摩我義議犠抹抱搭抄抗批招拓拍打拘捨拐摘挑指持括揮推揚提損拾担拠描操接掲掛研"
    "戒械鼻刑型才財材存在乃携及吸扱丈史吏更硬又双桑隻護獲奴怒友抜投没設撃殻支技枝肢茎怪軽叔督寂"
    "淑反坂板返販爪妥乳浮将奨採菜受授愛払広拡鉱弁雄台怠治始胎窓去法会至室到致互棄育撤充銃硫流允"
    "唆出山拙岩炭岐峠崩密蜜嵐崎入込分貧頒公松翁訟谷浴容溶欲裕鉛沿賞党堂常裳掌皮波婆披破被残殉殊"
    "殖列裂烈死葬瞬耳取趣最撮恥職聖敢聴懐慢漫買置罰寧濁環還夫扶渓規替賛潜失鉄迭臣姫蔵臓賢堅臨覧"
    "巨拒力男労募劣功勧努励加賀架脇脅協行律復得従徒待往征径彼役徳徹徴懲微街衡稿稼程税稚和移秒秋"
    "愁私秩秘称利梨穫穂稲香季委秀透誘穀菌米粉粘粒粧迷粋糧菊奥数楼類漆様求球救竹笑笠笹筋箱筆筒等"
    "算答策簿築人佐但住位仲体悠件仕他伏伝仏休仮伯俗信佳依例個健側侍停値倣倒偵僧億儀償仙催仁侮使"
    "便倍優伐宿傷保褒傑付符府任賃代袋貸化花貨傾何荷俊傍久畝囚内丙柄肉腐座卒傘匁以似併瓦瓶宮営善"
    "年夜液塚幣弊喚換融施旋遊旅勿物易賜尿尼尻泥塀履屋握屈掘堀居据層局遅漏刷尺尽沢訳択昼戸肩房扇"
    "炉戻涙雇顧啓示礼祥祝福祉社視奈尉慰款禁襟宗崇祭察擦由抽油袖宙届笛軸甲押岬挿申伸神捜果菓課裸"
    "斤析所祈近折哲逝誓暫漸断質斥訴昨詐作雪録尋急穏侵浸寝婦掃当争浄事唐糖康逮伊君群耐需儒端両満"
    "画歯曲曹遭漕槽斗料科図用庸備昔錯借惜措散廿庶遮席度渡奔噴墳憤焼暁半伴畔判券巻圏勝藤謄片版之"
    "乏芝不否杯矢矯族知智矛柔務霧班帰弓引弔弘強弱沸費第弟巧号朽誇汚与写身射謝老考孝教拷者煮著署"
    "暑諸猪渚賭峡狭挟追師帥官棺管父交効較校足促距路露跳躍践踏骨滑髄禍渦過阪阿際障随陪陽陳防附院"
    "陣隊墜降階陛隣隔隠堕陥穴空控突究窒窃窪搾窯窮探深丘岳兵浜糸織繕縮繁縦線締維羅練緒続絵統絞給"
    "絡結終級紀紅納紡紛紹経紳約細累索総綿絹繰継緑縁網緊紫縛縄幼後幽幾機玄畜蓄弦擁滋慈磁系係孫懸"
    "却脚卸御服命令零齢冷領鈴勇通踊疑擬凝範犯厄危宛腕苑怨柳卵留貿印興酉酒酌酵酷酬酪酢酔配酸猶尊"
    "豆頭短豊鼓喜樹皿血盆盟盗温監濫鑑猛盛塩銀恨根即爵節退限眼良朗浪娘食飯飲飢餓飾館養飽既概慨平"
    "呼坪評刈希凶胸離殺純鈍辛辞梓宰壁避新薪親幸執報叫糾収卑碑陸睦勢熱菱陵亥核刻該劾述術寒醸譲壌"
    "嬢毒素麦青精請情晴清静責績積債漬表俵潔契喫害轄割憲生星姓性牲産隆峰縫拝寿鋳籍春椿泰奏実奉俸"
    "棒謹勤漢嘆難華垂睡錘乗剰今含吟念琴陰予序預野兼嫌鎌謙廉西価要腰票漂標栗遷覆煙南楠献門問閲閥"
    "間簡開閉閣閑聞潤欄闘倉創非俳排悲罪輩扉侯候決快偉違緯衛韓干肝刊汗軒岸幹芋宇余除徐叙途斜塗束"
    "頼瀬勅疎速整剣険検倹重動勲働種衝薫病痴痘症疾痢疲疫痛癖匿匠医匹区枢殴欧抑仰迎登澄発廃僚寮療"
    "彫形影杉彩彰彦顔須膨参惨修珍診文対紋蚊斉剤済斎粛塁楽薬率渋摂央英映赤赦変跡蛮恋湾黄横把色絶"
    "艶肥甘紺某謀媒欺棋旗期碁基甚勘堪貴遺遣舞無組粗租祖阻査助宜畳並普譜湿顕繊霊業撲僕共供異翼洪"
    "港暴爆恭選殿井囲耕亜悪円角触解再講購構溝論倫輪偏遍編冊典氏紙婚低抵底民眠捕浦蒲舗補邸郭郡郊"
    "部都郵邦郷響郎廊盾循派脈衆逓段鍛后幻司伺詞飼嗣舟舶航般盤搬船艦艇瓜弧孤繭益暇敷来気汽飛沈妻"
    "衰衷面革靴覇声呉娯誤蒸承函極牙芽邪雅釈番審翻藩毛耗尾宅託為偽長張帳脹髪展喪巣単戦禅弾桜獣脳"
    "悩厳鎖挙誉猟鳥鳴鶴烏蔦鳩鶏島暖媛援緩属嘱偶遇愚隅逆塑岡鋼綱剛缶陶揺謡就懇墾免逸晩勉象像馬駒"
    "験騎駐駆駅騒駄驚篤騰虎虜膚虚戯虞慮劇虐鹿薦慶麗熊能態寅演辰辱震振娠唇農濃送関咲鬼醜魂魔魅塊"
    "襲嚇朕雰箇錬遵罷屯且藻隷癒丹潟丑卯巳謎椅翔贅芯酎俺闇枕綺鍋醤丼賂伎斐墟蜂拳遜狙噌誰呪也頃叱"
    "斬鍵巾爽阜庄瞭崖箸淀堰鰐隙貼蟹鬱々";

    QMap<QString, QString> oldKanjiState = QMap<QString, QString>();

    // Retrieve the list of Kanjis (and their state) the user has already
    // studied

    QString url = "https://www.wanikani.com/api/v1/user/"+mApiKey+"/kanji";
    QNetworkAccessManager networkAccessManager;
    QNetworkReply *networkReply = networkAccessManager.get(QNetworkRequest(url));
    QEventLoop eventLoop;

    QObject::connect(networkReply, SIGNAL(finished()),
                     &eventLoop, SLOT(quit()));

    eventLoop.exec();

    QByteArray response = QByteArray();

    if (networkReply->error() == QNetworkReply::NoError)
        response = networkReply->readAll();

    networkReply->deleteLater();

    if (response.isEmpty())
        return;

    QJsonDocument json = QJsonDocument::fromJson(response);

    if (json.isNull())
        return;

    bool error = json.object().contains("error");
    QMap<QString, QString> kanjiState = QMap<QString, QString>();
    QVariantMap requestedInformationMap;

    if (!error) {
        foreach (const QVariant &requestedInformation,
                 json.object().toVariantMap()["requested_information"].toList()) {
            requestedInformationMap = requestedInformation.toMap();

            kanjiState.insert(requestedInformationMap["character"].toString(),
                              requestedInformationMap["stats"].toMap()["srs"].toString());
        }
    }

    // Generate and set the wallpaper, if needed

    if (error || (kanjiState != oldKanjiState)) {
        // Default wallpaper

        QString wallpaperFileName = QDir::toNativeSeparators(qApp->applicationDirPath()+"/WaniKani.jpg");

        QPixmap pixmap;

        pixmap.load(":/wallpaper.jpg");

        if (!error) {
            // Keep track of our Kanji/state map

            oldKanjiState = kanjiState;

            // Generate the wallpaper

            static const int xStart = 1240;
            static const int shift = 32;
            static const int smallShift = 1;

            int areaWidth = pixmap.width()-xStart-2*shift;
            int areaHeight = pixmap.height()-2*shift;

            QFont font = QFont("MS Mincho");
            int fontPixelSize = 1;
            int charWidth = 0;
            int charHeight = 0;
            int nbOfRows = 0;
            int nbOfCols = 0;
            int descent = 0;

            forever {
                font.setPixelSize(fontPixelSize);

                QFontMetrics fontMetrics(font);
                int crtCharWidth = fontMetrics.width(kanjis.at(0));
                int crtCharHeight = fontMetrics.height();
                int crtNbOfCols = areaWidth/(crtCharWidth+smallShift);
                int crtNbOfRows =  floor(kanjiState.size()/crtNbOfCols)
                                  +((kanjiState.size() % crtNbOfCols)?1:0);

                if (crtNbOfRows*crtCharHeight+(crtNbOfRows-1)*smallShift+fontMetrics.descent() <= areaHeight) {
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

            int x = 0;
            int y = shift+((areaHeight-nbOfRows*charHeight-(nbOfRows-1)*smallShift) >> 1)-descent;
            int radius = ceil(0.75*(qMax(charWidth, charHeight) >> 3));

            for (int i = 0, j = 0, iMax = kanjis.size(); i < iMax; ++i) {
                if (kanjiState.keys().contains(kanjis.at(i))) {
                    if (!(j % nbOfCols)) {
                        x = xStart+shift;
                        y += charHeight+(j?smallShift:0);
                    }

                    QString state = kanjiState.value(kanjis.at(i));

                    if (!state.compare("apprentice"))
                        painter.setPen("#dd0093");
                    else if (!state.compare("guru"))
                        painter.setPen("#882d9e");
                    else if (!state.compare("master"))
                        painter.setPen("#294ddb");
                    else if (!state.compare("enlighten"))
                        painter.setPen("#0093dd");
                    else if (!state.compare("burned"))
                        painter.setPen("#fbc042");
                    else
                        painter.setPen("#808080");

                    QPainterPath path;

                    path.addRoundedRect(QRectF(x, y-charHeight+descent, charWidth, charHeight),
                                        radius, radius);

                    painter.fillPath(path, state.compare("burned")?
                                               QColor("#40434343"):
                                               QColor("#80434343"));
                    painter.drawText(x, y, kanjis.at(i));

                    x += charWidth+smallShift;

                    ++j;
                }
            }
        }

        // Save and set our wallpaper

        pixmap.save(wallpaperFileName);

        setWallpaper(wallpaperFileName);
    }
}

//==============================================================================

void Dialog::setWallpaper(const QString &pWallpaperFileName)
{
    // Set the given wallpaper

#if defined(Q_OS_WIN)
    SystemParametersInfo(SPI_SETDESKWALLPAPER, 0,
                         (PVOID) pWallpaperFileName.utf16(), SPIF_UPDATEINIFILE);
#elif defined(Q_OS_MAC)
    setMacosWallpaper(qPrintable(pWallpaperFileName));
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
                                << QUrl::fromLocalFile(pWallpaperFileName).toString());
    process.waitForFinished();
#endif
}

//==============================================================================
// End of file
//==============================================================================
