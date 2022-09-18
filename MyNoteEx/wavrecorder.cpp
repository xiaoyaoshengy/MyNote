#include "wavrecorder.h"
#include "mydef.h"

#include <QLibrary>

WavRecorder::WavRecorder(QObject *parent) : QObject(parent)
{
    _dName = QString("");
    mci_send_string = nullptr;
    //加载dll文件，获取mciSendString方法
    QLibrary lib("winmm.dll");
    if(!lib.load())
    {
        QString err = QString::fromLocal8Bit("winmm.dll加载失败");
        MyDef::printLog(__FILE__, __LINE__, err);
        MyDef::exportErrorMsg(err);
    }
    else
    {
        mci_send_string = (FUNC)lib.resolve("mciSendStringW");
        if(!mci_send_string)
        {
            QString err = QString::fromLocal8Bit("mciSendString方法获取失败");
            MyDef::printLog(__FILE__, __LINE__, err);
            MyDef::exportErrorMsg(err);
        }
    }
}

WavRecorder::WavRecorder(QString dName, QObject *parent)
{
    _dName = dName;
    mci_send_string = nullptr;
    //加载dll文件，获取mciSendString方法
    QLibrary lib("winmm.dll");
    if(!lib.load())
    {
        QString err = QString::fromLocal8Bit("winmm.dll加载失败");
        MyDef::printLog(__FILE__, __LINE__, err);
        MyDef::exportErrorMsg(err);
    }
    else
    {
        mci_send_string = (FUNC)lib.resolve("mciSendString");
        if(!mci_send_string)
        {
            QString err = QString::fromLocal8Bit("mciSendString方法获取失败");
            MyDef::printLog(__FILE__, __LINE__, err);
            MyDef::exportErrorMsg(err);
        }
    }
}

WavRecorder::~WavRecorder()
{

}

void WavRecorder::Record()
{
    if(!mci_send_string)
        return;

    mci_send_string((QString("open new Type waveaudio Alias ") + _dName).toStdWString().c_str(), LPTSTR(), 0, 0);
    mci_send_string((QString("stop ") + _dName).toStdWString().c_str(), LPTSTR(), 0, 0);

    //每秒的字节数
    int bytespersec = bitspersample * channels * samplespersec / 8;
    //每个时刻样本的字节数
    int alignment = bitspersample * channels / 8;

    QString command = QString("set ") + _dName + QString(" time format ms");
    command += QString(" bitspersample ") + QString::number(bitspersample);
    command += QString(" channels ") + QString::number(channels);
    command += QString(" samplespersec ") + QString::number(samplespersec);
    command += QString(" bytespersec ") + QString::number(bytespersec);
    command += QString(" alignment ") + QString::number(alignment);

    mci_send_string(command.toStdWString().c_str(), LPTSTR(), 0, 0);
    mci_send_string((QString("record ") + _dName).toStdWString().c_str(), LPTSTR(), 0, 0);
}

void WavRecorder::Stop()
{
    if(!mci_send_string)
        return;

    mci_send_string((QString("stop ") + _dName).toStdWString().c_str(), LPTSTR(), 0, 0);
}

void WavRecorder::Save(QString filename)
{
    if(!mci_send_string)
        return;

    mci_send_string((QString("save ") + _dName + QString(" ") + filename).toStdWString().c_str(), LPTSTR(), 0, 0);
}

void WavRecorder::Close()
{
    if(!mci_send_string)
        return;

    mci_send_string((QString("close ") + _dName).toStdWString().c_str(), LPTSTR(), 0, 0);
}

void WavRecorder::Play()
{
    if(!mci_send_string)
        return;

    mci_send_string((QString("seek ") + _dName + QString(" to start")).toStdWString().c_str(), LPTSTR(), 0, 0);
    mci_send_string((QString("play ") + _dName).toStdWString().c_str(), LPTSTR(), 0, 0);
}

void WavRecorder::setName(QString dName)
{
    _dName = dName;
}

WavRecorder &WavRecorder::operator=(const WavRecorder &wav)
{
    _dName = wav._dName;
    mci_send_string = wav.mci_send_string;
    return *this;
}
