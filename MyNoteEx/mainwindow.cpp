#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWebEngineView>
#include <QStackedLayout>
#include <QDialog>
#include <QProcess>
#include <QMessageBox>
#include <QKeyEvent>
#include <iostream>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QCoreApplication>
#include <QThread>
#include <QApplication>
#include <QSysInfo>
#include <cstdlib>
#include <iostream>
using std::cout;
#include <windows.h>
#include <tlhelp32.h>
#include "psapi.h"
#pragma comment(lib,"psapi.lib")
#include <QtCore/QCoreApplication>
#include <QSystemTrayIcon>
#include <QtGui>

#include "lobal.h"
#include <QAudioDeviceInfo>
#include "mydef.h"
#include "mynote.h"
#include <QMouseEvent>
#include <QScreen>
#include "mydef.h"

MainWindow::MainWindow(MyNote *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    ///////////////////Windows控制台部分/////////////////////////
    cmd=new QProcess(this);
    connect(cmd , SIGNAL(readyReadStandardError()) , this , SLOT(on_readerror()));
    cmd->start("cmd");
    cmd->waitForStarted();
    //打包用代码
//    QString currentDirPath = QCoreApplication::applicationDirPath();
//    cmd->write((QString(currentDirPath.at(0)) + QString(":\r\n")).toStdString().c_str());
//    cmd->write((QString("cd ") + currentDirPath + QString("\r\n")).toStdString().c_str());
    //在编辑器中运行需要注释上面三行代码
    cmd->write("server.exe\r\n");
    ///////////////////////////////////////////////////////////
    m_webView = new QWebEngineView(this);
    m_webView->setGeometry(0,50,1000,400);
    m_webView->load(QUrl("http://127.0.0.1:8081/public/Samples/TypeScript/Demo/index.html"));
    flag = 1;
    QString strCurrentApplicationDirPath=QCoreApplication::applicationDirPath();
    qDebug()<<strCurrentApplicationDirPath;


    m_p_parent = parent;

    if(m_p_parent->isVoiceCommandEnable())
    {
        ui->btnStart->show();
        ui->btnStart->setEnabled(true);
        ui->btnStop->show();
        ui->btnStop->setEnabled(false);
    }
    else
    {
        ui->btnStart->hide();
        ui->btnStop->hide();
    }

    m_webView->reload();
    connect(ui->btnStart, &QPushButton::clicked, this, &MainWindow::onbtnStartClicked);
    connect(ui->btnStop,&QPushButton::clicked, this, &MainWindow::onbtnStopClicked);
    m_webView->show();
    m_webView->page()->setBackgroundColor(Qt::transparent);
    m_webView->setAttribute(Qt::WA_TranslucentBackground);
    m_webView->setStyleSheet("background:transparent");
    m_webView->page()->setBackgroundColor(Qt::transparent);

    //设置窗体属性
    setWindowFlags(Qt::Tool | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_QuitOnClose);

    QScreen* screen = QGuiApplication::primaryScreen();
    setGeometry(screen->availableGeometry().width() - 350, screen->availableGeometry().height() - 500, 350, 500);

    asr = new SpeechRecognizer;
    connect(asr, &SpeechRecognizer::ReceiveText, this, &MainWindow::SpeechRecognizer_OnReceiveText);
}

MainWindow::~MainWindow()
{
    if(cmd){
        cmd->execute("taskkill", QStringList() << "-PID" << QString("%1").arg(cmd->processId()) << "-F" <<"-T");
        cmd->close();
        delete cmd;

    }
    flag = 0;
    delete asr;
    delete ui;
}

void MainWindow::setVoiceCommandBtns(bool shown)
{
    if(shown)
    {
        ui->btnStart->show();
        ui->btnStop->show();
        ui->btnStop->setEnabled(false);
    }
    else
    {
        ui->btnStart->hide();
        ui->btnStop->hide();
    }
}

MainWindow::Commands MainWindow::convertToCommand(QString str)
{
    MyDef::printLog(__FILE__, __LINE__, str);
    Commands commands = MainWindow::InvaildCommand;
    int length = str.length();
    QString str_cha = QString::fromLocal8Bit("叉扱扠杈臿差挿偛馇插揷嗏喳锸碴銟艖疀嚓鍤餷叉茬茶垞查査秅猹嵖搽靫詧楂槎察碴檫叉衩蹅镲鑔叉奼汊杈岔衩刹侘诧剎姹差咤紁詫");
        QString str_cheng = QString::fromLocal8Bit("阷泟爯柽棦称秤浾琤偁铛碀蛏晿牚赪靗憆稱摚撑撐噌樘緽頳赬瞠橕檉竀罉蟶穪鏿鐣饓丞成朾呈郕诚枨承荿宬娍峸洆城乗乘埕挰珹珵窚掁脭铖盛程揨堘裎絾筬惩椉棖誠塍塖溗酲鋮畻澄澂憕橙檙瀓懲騬侱徎逞骋悜庱睈裎騁称秤牚鯎");
        QString str_jian = QString::fromLocal8Bit("戋尖幵奸间歼坚浅戔冿玪肩艰姦姧兼监笺豜菅菺猏惤渐堅缄溅湔葌間雃犍牋瑊椷椾睷碊蒹搛缣煎蔪箋鲣緘鹣樫蕑蕳熞鳽熸縑鋻餰馢鞬艱鞯鳒瀐鵑鵳殱礛瀸鰔鰜鶼殲櫼譼韀礷鰹囏虃韉鑯暕囝枧拣俭茧柬笕倹捡挸湕帴减剪检梘趼詃検锏睑硷裥堿減揃揀谫弿瑐絸筧简戬戩碱儉翦篯撿藆襇襉檢謇蹇謭簡繭礆瞼鬋鹸鰎蠒鐧鐗瀽鹻譾籛襺鹼见件间見建饯贱洊荐剑牮珔监舰剣健俴涧栫渐剱谏徤袸毽臶腱锏践跈寋溅楗旔賎鉴键監槛榗漸僭劎劍澗箭糋諓賤踺踐趝餞諫鍵薦劒劔鍳瞷磵礀繝濺檻擶瀳譛鏩覵覸艦轞鐱鑑鑒鑬鑳豣彅墹麉");
        QString str_kan = QString::fromLocal8Bit("刊看栞龛勘嵁堪戡龕凵冚坎侃砍莰偘惂欿歁槛輡顑轗竷看衎崁墈阚磡瞰闞鬫矙");
        QString str_ri = QString::fromLocal8Bit("日驲囸衵釰釼鈤馹乤乬乫乮乭乶乽乼乺挘艝鑓虄");
        QString str_she = QString::fromLocal8Bit("奢猞赊畬畲畭輋賖賒檨舌佘折虵蛇阇揲蛥舍捨叶厍设社舍舎厙拾涉射赦設渉涻弽滠慑摄摂蔎慴歙蠂韘騇麝懾灄攝欇");
        QString str_xin = QString::fromLocal8Bit("忄心邤辛芯妡炘杺欣昕盺莘俽惞訢鈊锌歆新廞鋅噷嬜薪馨鑫馫枔镡襑鐔伈阠伩囟孞芯信軐訫脪衅焮馸顖舋釁");
        QString str_zhi = QString::fromLocal8Bit("之氏支只卮汁芝汦汥吱巵知肢泜织枝栀祗胝胑秓衼疷隻脂祬栺倁梔椥臸綕禔稙搘馶蜘榰憄鳷鴲織鼅蘵执侄妷直姪值値聀釞职淔埴執殖戠植犆禃跖絷瓡馽摭潪嬂漐慹踯膱樴縶蹢蹠職蟙軄躑夂止只阯劧旨帋芷扺沚纸址坁坧抧茋祉秖恉轵枳砋衹咫指洔淽茝紙疻訨趾黹軹酯徴墌徵藢襧芖至豸扻伿识忮志炙质治垁织迣郅厔制帜帙挃庢庤陟峙洷栉柣祑挚桎晊歭秩贽轾徏娡狾致袟紩畤窒痔痓铚鸷貭翐觗袠猘徝掷乿剬偫秷梽智滞崻傂骘痣彘蛭置锧跱輊筫雉寘搱廌滍稚滯墆銍製覟誌瘈疐觯踬質鋕稺熫膣摯摨幟擳隲璏旘瀄鴙駤緻懥螲櫛穉儨劕擿擲懫贄觶騭鯯櫍瓆豑礩騺驇鷙躓鑕豒俧潌");
        QString str_command;
        for(int i = 0; i < length; i++)
        {
            if(str_cha.contains(str.at(i)))
                str_command += QString("cha");
            else if(str_cheng.contains(str.at(i)))
                str_command += QString("cheng");
            else if(str_jian.contains(str.at(i)))
                str_command += QString("jian");
            else if(str_kan.contains(str.at(i)))
                str_command += QString("kan");
            else if(str_ri.contains(str.at(i)))
                str_command += QString("ri");
            else if(str_she.contains(str.at(i)))
                str_command += QString("she");
            else if(str_xin.contains(str.at(i)))
                str_command += QString("xin");
            else if(str_zhi.contains(str.at(i)))
                str_command += QString("zhi");
        }
        if(str_command == QString("richeng") || str_command == QString("chakanricheng"))
            commands = OpenSchedule;
        else if(str_command == QString("xinjianricheng") || str_command == QString("xinjian"))
            commands = NewSchedule;
        else if(str_command == QString("shezhi") || str_command == QString("chakanshezhi"))
            commands = OpenSetting;
        else
            commands = InvaildCommand;
        return commands;
}

void MainWindow::transformCommands(MainWindow::Commands cmd)
{
    switch (cmd) {
    case NewSchedule:
        m_p_parent->onActionNewScheduleTriggered();
        break;
    case OpenSchedule:
        m_p_parent->onActionShowScheduleTriggered();
        break;
    case OpenSetting:
        m_p_parent->onActionSettingTriggered();
        break;
    default:
        MyDef::printLog(__FILE__, __LINE__, QString::fromLocal8Bit("没有听懂"));
        QMessageBox::information(0, QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("抱歉没有听懂"));
        break;
    }
}

void MainWindow::SpeechRecognizer_OnReceiveText(QString text)
{
    //识别语音命令
    transformCommands(convertToCommand(text));
}

void MainWindow::onbtnStartClicked()
{
    //检测是否有可用的音频输入设备
    if(QAudioDeviceInfo::availableDevices(QAudio::AudioInput).isEmpty())
    {
        ui->btnStart->setEnabled(false);
        ui->btnStop->setEnabled(false);
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("没有可用的音频输入设备"));
        MyDef::exportErrorMsg(QString::fromLocal8Bit("没有可用的音频输入设备"));
        //关闭语音命令
        ui->btnStart->hide();
        ui->btnStop->hide();
        return;
    }
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);
    asr->Start();
}

void MainWindow::onbtnStopClicked()
{
    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
    asr->Stop();
}

void MainWindow::on_readerror()
{
    MyDef::exportErrorMsg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss: ") + QString::fromLocal8Bit(cmd->readAllStandardError()));
    MyDef::exportErrorMsg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss: ") + QString::fromLocal8Bit(cmd->readAllStandardOutput()));
    QMessageBox::information(0, "Error", "8081 is already in use");
}


