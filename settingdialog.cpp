#include "settingdialog.h"
#include "ui_settingdialog.h"
#include "mynote.h"
#include "mydef.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDomDocument>
#include <QTextStream>
#include <QSettings>
#include <QAudioDeviceInfo>
#include <QDebug>

SettingDialog::SettingDialog(MyNote *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);

    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setText(QString::fromLocal8Bit("确定"));
    QPushButton *cancelButton = ui->buttonBox->button(QDialogButtonBox::Cancel);
    cancelButton->setText(QString::fromLocal8Bit("取消"));

    //获取主窗体指针
    m_p_parent = parent;

    iniSettings();

    //检测音频输入设备能否使用
    if(QAudioDeviceInfo::availableDevices(QAudio::AudioInput).isEmpty())
    {
        ui->checkBoxVoiceCommand->setChecked(false);
        ui->checkBoxVoiceCommand->setEnabled(false);
        printLog(__FILE__, __LINE__, QString::fromLocal8Bit("没有可用的音频输入设备"));
        exportMsg(QString::fromLocal8Bit("没有可用的音频输入设备"));
    }

    //链接信号槽
    connect(ui->pushButtonBrowser, &QPushButton::clicked, this, &SettingDialog::onPushButtonBrowserClicked);
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &SettingDialog::onDialogButtonClicked);
    connect(ui->checkBoxAutoRun, &QCheckBox::clicked, this, &SettingDialog::onCheckBoxAutoRunClicked);
    connect(ui->checkBoxVoiceCommand, &QCheckBox::clicked, this, &SettingDialog::onCheckBoxVoiceCommandClicked);
    connect(ui->lineEditSavePath, &QLineEdit::textChanged, this, &SettingDialog::onSavePathLineEditChanged);
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::setSavePath(QString savePath)
{
    ui->lineEditSavePath->setText(savePath);
}

void SettingDialog::onPushButtonBrowserClicked()
{
    QString oldSavePath = m_p_parent->savePath();
    QDir dir(oldSavePath);
    if(!dir.exists())
    {
        if(!dir.mkdir(oldSavePath))
        {
            QString err = QString::fromLocal8Bit("无法创建目录") + oldSavePath;
            //调试
            printLog(__FILE__, __LINE__, err);
            exportMsg(err);
        }
    }
    QString destinationDirPath = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("请选择一个目录"), oldSavePath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!destinationDirPath.isEmpty())
    {
        setSavePath(destinationDirPath);
    }
}

void SettingDialog::onDialogButtonClicked(QAbstractButton *button)
{
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        saveSettings();
    }
    else if(button == ui->buttonBox->button(QDialogButtonBox::Cancel))
    {
        setSavePath(m_p_parent->savePath());
        close();
    }
}

void SettingDialog::onCheckBoxAutoRunClicked(bool clicked)
{
    //获取应用名称
    QString applicationName = QApplication::applicationName();
    QSettings setting(QString("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"), QSettings::NativeFormat);

    //当选中时设置开机自启
    if(clicked)
    {
        //获取应用路径
        QString applicationFilePath = QApplication::applicationFilePath();
        //写入注册表
        setting.setValue(applicationName, applicationFilePath.replace("/", "\\"));
    }
    //否则关闭开机自启
    else
        setting.remove(applicationName);
}

void SettingDialog::onCheckBoxVoiceCommandClicked(bool clicked)
{
    //当选中时设置开启语音命令
    if(clicked)
    {
        //检测是否有可用的音频输入设备
        if(QAudioDeviceInfo::availableDevices(QAudio::AudioInput).isEmpty())
        {
            ui->checkBoxVoiceCommand->setChecked(false);
            ui->checkBoxVoiceCommand->setEnabled(false);
            QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("没有可用的音频输入设备"));
            exportMsg(QString::fromLocal8Bit("没有可用的音频输入设备"));
            //关闭语音命令
            return;
        }
        //开启语音命令
    }
    else
    {
        //关闭语音命令
    }
}

void SettingDialog::onSavePathLineEditChanged(QString path)
{
    m_temp_save_path = path;
}

void SettingDialog::saveSettings()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QFile configFile(appPath + QString::fromLocal8Bit("/config.xml"));
    QDomDocument doc;
    QDomElement root;
    //如果文件无法打开，错误处理
    if(!configFile.open(QFile::ReadOnly))
    {
        QString err = QCoreApplication::applicationDirPath() + QString::fromLocal8Bit("/config.xml文件无法打开");
        //调试
        printLog(__FILE__, __LINE__, err);
        exportMsg(err);
        return;
    }
    QString *errorMsg = nullptr;
    int *errorLine = nullptr;
    int *errorColumn = nullptr;
    //如果文件读取失败，错误处理
    if(!doc.setContent(&configFile, false, errorMsg, errorLine, errorColumn))
    {
        if(errorMsg != nullptr && errorLine != nullptr && errorColumn != nullptr)
        {
            QString err = QString::fromLocal8Bit("配置文件") + appPath + QString::fromLocal8Bit("/config.xml(Line ") + QString::number(*errorLine) + QString::fromLocal8Bit(", Column ") + QString::number(*errorColumn) + QString::fromLocal8Bit("): ") + (*errorMsg);
            //调试
            printLog(__FILE__, __LINE__, err);
            exportMsg(err);
        }
        else if(errorMsg != nullptr)
        {
            QString err = QString::fromLocal8Bit("配置文件") + appPath + QString::fromLocal8Bit("/config.xml出现错误：") + (*errorMsg);
            //调试
            printLog(__FILE__, __LINE__, err);
            exportMsg(err);
        }
        else
        {
            QString err = QString::fromLocal8Bit("配置文件") + appPath + QString::fromLocal8Bit("/config.xml出现未知错误");
            //调试
            printLog(__FILE__, __LINE__, err);
            exportMsg(err);
        }
        configFile.close();
        return;
    }
    configFile.close();
    root = doc.documentElement();

    //<SavePath>
    QDomNode savePathNode = root.namedItem(QString::fromLocal8Bit("SavePath"));
    savePathNode.firstChild().setNodeValue(m_temp_save_path);

    //<AutoRun>
    QDomNode autoRunNode = root.namedItem(QString::fromLocal8Bit("AutoRun"));
    if(ui->checkBoxAutoRun->isChecked())
        autoRunNode.firstChild().setNodeValue(QString::number(1));
    else
        autoRunNode.firstChild().setNodeValue(QString::number(0));

    //<VoiceCommand>
    QDomNode voiceCommandNode = root.namedItem(QString::fromLocal8Bit("VoiceCommand"));
    if(ui->checkBoxVoiceCommand->isChecked())
        voiceCommandNode.firstChild().setNodeValue(QString::number(1));
    else
        voiceCommandNode.firstChild().setNodeValue(QString::number(0));

    //保存到文件
    if(!configFile.open(QFile::WriteOnly))
    {
        QString err = appPath + QString::fromLocal8Bit("/config.xml文件无法打开");
        //调试
        printLog(__FILE__, __LINE__, err);
        exportMsg(err);
        return;
    }
    QTextStream stream(&configFile);
    doc.save(stream, 4);
    configFile.close();

    moveSavingDir();
}

void SettingDialog::moveSavingDir()
{
    QString oldDirPath = m_p_parent->savePath();
    m_p_parent->setSavePath(m_temp_save_path);
    QDir oldDir(oldDirPath);
    oldDir.setNameFilters(QStringList() << QString::fromLocal8Bit("*.json"));
    bool hasErrorWhenMove = false;
    if(oldDir.count() > 0)
    {
        for(uint i = 0;i < oldDir.count();i++)
        {
            QString scheduleFileName = oldDir[i];
            QRegExp reg = QRegExp(QString::fromLocal8Bit("[0-9]{4}-[0-9]{2}-[0-9]{2}.json"));
            if(!reg.exactMatch(scheduleFileName))
            {
                //调试
                printLog(__FILE__, __LINE__, QString::fromLocal8Bit("文件名不匹配：") + scheduleFileName);
                continue;
            }
            if((oldDirPath + QString::fromLocal8Bit("/") + scheduleFileName)!=(m_temp_save_path + QString::fromLocal8Bit("/") + scheduleFileName)){
                QFile scheduleFile(oldDirPath + QString::fromLocal8Bit("/") + scheduleFileName);
                if(!scheduleFile.rename(m_temp_save_path + QString::fromLocal8Bit("/") + scheduleFileName))
                {
                    QString err = QString::fromLocal8Bit("文件") + scheduleFileName + QString::fromLocal8Bit("移动失败");
                    //调试
                    printLog(__FILE__, __LINE__, err);
                    exportMsg(err);
                    hasErrorWhenMove = true;
                    continue;
                }
            }
        }
    }
    if(hasErrorWhenMove)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("部分日程文件移动失败，可在原路径中找到。由于未知错误，建议直接删除，同时这意味着记录的日程也会删除。"));
    }
}

void SettingDialog::iniSettings()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QFile configFile(appPath + QString::fromLocal8Bit("/config.xml"));
    QDomDocument doc;
    QDomElement root;
    //如果配置文件存在
    if(configFile.exists())
    {
        //如果配置文件无法打开，错误处理
        if(!configFile.open(QFile::ReadOnly))
        {
            QString err = QString::fromLocal8Bit("设置文件") + appPath + QString::fromLocal8Bit("/config.xml文件无法打开");
            //调试
            printLog(__FILE__, __LINE__, err);
            exportMsg(err);
            return;
        }
        QString *errorMsg = nullptr;
        int *errorLine = nullptr;
        int *errorColumn = nullptr;
        //如果文档读取失败，错误处理
        if(!doc.setContent(&configFile, false, errorMsg, errorLine, errorColumn))
        {
            if(errorMsg != nullptr && errorLine != nullptr && errorColumn != nullptr)
            {
                QString err = configFile.fileName() + QString::fromLocal8Bit("(Line ") + QString::number(*errorLine) + QString::fromLocal8Bit(", Column ") + QString::number(*errorColumn) + QString::fromLocal8Bit("): ") + (*errorMsg);
                //调试
                printLog(__FILE__, __LINE__, err);
                exportMsg(err);
            }
            else if(errorMsg != nullptr)
            {
                QString err = configFile.fileName() + QString::fromLocal8Bit(": ") + (*errorMsg);
                //调试
                printLog(__FILE__, __LINE__, err);
                exportMsg(err);
            }
            else
            {
                QString err = configFile.fileName() + QString::fromLocal8Bit(": 未知错误");
                //调试
                printLog(__FILE__, __LINE__, err);
                exportMsg(err);
            }
            configFile.close();
            return;
        }
        configFile.close();
        root = doc.documentElement();

        QDomNode savePathNode = root.namedItem(QString::fromLocal8Bit("SavePath"));
        QString savePath = savePathNode.firstChild().nodeValue();
        m_p_parent->setSavePath(savePath);
        ui->lineEditSavePath->setText(savePath);
        m_temp_save_path = savePath;

        QDomNode autoRunNode = root.namedItem(QString::fromLocal8Bit("AutoRun"));
        bool isAutoRun = autoRunNode.firstChild().nodeValue().toInt();
        ui->checkBoxAutoRun->setChecked(isAutoRun);

        QDomNode voiceCommandNode = root.namedItem(QString::fromLocal8Bit("VoiceCommand"));
        bool isVoiceCommand = voiceCommandNode.firstChild().nodeValue().toInt();
        ui->checkBoxVoiceCommand->setChecked(isVoiceCommand);
    }
    //当配置文件不存在时，创建
    else
    {
        //如果文件创建失败，错误处理
        if(!configFile.open(QFile::WriteOnly))
        {
            QString err = appPath + QString::fromLocal8Bit("/config.xml文件无法创建");
            //调试
            printLog(__FILE__, __LINE__, err);
            exportMsg(err);
            return;
        }
        //添加处理命令
        QDomProcessingInstruction instruction = doc.createProcessingInstruction(QString::fromLocal8Bit("xml"), QString::fromLocal8Bit("version=\"1.0\" encoding=\"UTF-8\""));
        doc.appendChild(instruction);
        root = doc.createElement(QString::fromLocal8Bit("Setting"));
        doc.appendChild(root);

        //<SavePath>
        QDomElement savePathElement = doc.createElement(QString::fromLocal8Bit("SavePath"));
        QDomText savePathText = doc.createTextNode(appPath + QString::fromLocal8Bit("/Schedule"));
        savePathElement.appendChild(savePathText);
        root.appendChild(savePathElement);

        //<AutoRun>
        QDomElement autoRunElement = doc.createElement(QString::fromLocal8Bit("AutoRun"));
        QDomText autoRunText = doc.createTextNode(QString::number(0));
        autoRunElement.appendChild(autoRunText);
        root.appendChild(autoRunElement);

        //<VoiceCommand>
        QDomElement voiceCommandElement = doc.createElement(QString::fromLocal8Bit("VoiceCommand"));
        QDomText voiceCommandText = doc.createTextNode(QString::number(0));
        voiceCommandElement.appendChild(voiceCommandText);
        root.appendChild(voiceCommandElement);

        m_temp_save_path = appPath + QString::fromLocal8Bit("/Schedule");
        m_p_parent->setSavePath(m_temp_save_path);

        //保存到文件
        QTextStream stream(&configFile);
        doc.save(stream, 4);
        configFile.close();
    }
}
