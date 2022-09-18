#include "speechrecognizer.h"
#include "mydef.h"

#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QCoreApplication>

SpeechRecognizer::SpeechRecognizer(QString url_server, QString token_client, QObject *parent) : QObject(parent)
{
    url = url_server;
    token = token_client;
    recorder[0].setName(QString("recsound0"));
    recorder[1].setName(QString("recsound1"));
    _audioRecorder = recorder[0];

    timer = new QTimer;
    timer->setInterval(6000);
    connect(timer, &QTimer::timeout, this, &SpeechRecognizer::Timer_Tick);

    qnam = new QNetworkAccessManager(this);
    connect(qnam, &QNetworkAccessManager::finished, this, &SpeechRecognizer::httpFinished);

    connect(this, &SpeechRecognizer::DeleteWavFile, this, &SpeechRecognizer::DelWavFile);
}

SpeechRecognizer::~SpeechRecognizer()
{
    delete []recorder;
    delete timer;
    delete qnam;
}

bool SpeechRecognizer::isRecognizing()
{
    return _is_recognizing;
}

void SpeechRecognizer::Start()
{
    timer->start();

    _is_recognizing = true;
    _audioRecorder.Record();
}

void SpeechRecognizer::Stop()
{
    timer->stop();
    _audioRecorder.Stop();
    QString filename = QString("speechfile_end.wav");
    _audioRecorder.Save(filename);
    _audioRecorder.Close();

    SpeechRecognize(filename);

    emit DeleteWavFile(filename);

    _is_recognizing = false;
}

void SpeechRecognizer::Timer_Tick()
{
    _audioRecorder_old = _audioRecorder;

    //停止后切换对象立即继续接力录音
    i_audioRecorder++;
    _audioRecorder = recorder[i_audioRecorder % 2];

    _audioRecorder.Record();
    _audioRecorder_old.Stop();

    recorder[(i_audioRecorder + 1) % 2] = WavRecorder("recsound" + QString::number(i_audioRecorder + 1));

    //保存文件
    QString filename = QString("speechfile") + QString::number(i_audioRecorder) + QString(".wav");
    _audioRecorder_old.Save(filename);
    _audioRecorder_old.Close();

    SpeechRecognize(filename);

    emit DeleteWavFile(filename);
}

void SpeechRecognizer::httpFinished(QNetworkReply *reply)
{
    if(reply->error())
    {
        QString err = QString("Error:\n%1.").arg(reply->errorString());
        MyDef::printLog(__FILE__, __LINE__, err);
        MyDef::exportErrorMsg(err);
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    if(!reply->isReadable())
    {
        QString err = QString::fromLocal8Bit("无法读取网络响应");
        MyDef::printLog(__FILE__, __LINE__, err);
        MyDef::exportErrorMsg(err);
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    QByteArray content = reply->readAll();
    QString text = QString(content);

    emit ReceiveText(text);

    reply->deleteLater();
    reply = nullptr;
}

void SpeechRecognizer::DelWavFile(QString filename)
{
    QFile file(filename);
    if(file.exists())
        file.remove();
}

void SpeechRecognizer::SpeechRecognize(QString filename)
{
    //将wav文件post到服务器进行语音识别
    //将识别回来的文本写入文本框
    wav wave = WaveAccess(filename);
    if(wave.length == 0)
    {
        QString err = QString::fromLocal8Bit("处理wav文件出错");
        if(wave.wavs)
            delete []wave.wavs;
        MyDef::printLog(__FILE__, __LINE__, err);
        MyDef::exportErrorMsg(err);
        return;
    }
    qint16 *wavs = wave.wavs;

    int fs = wave.fs;
    QString wave_str;
    for(int i = 0; i < wave.length; i++)
    {
        wave_str += QString("&wavs=") + QString::number(wavs[i]);
    }

    Post(url, token, QString::number(fs), wave_str);
    if(wavs)
        delete []wavs;
}

SpeechRecognizer::wav SpeechRecognizer::WaveAccess(QString filename)
{
    int position = 0;
    wav wave;
    QFile file(filename);
    QFileInfo fileinfo(file);
    if(!file.open(QIODevice::ReadOnly))
    {
        QString err = QString::fromLocal8Bit("录音文件") + filename + QString::fromLocal8Bit("无法打开");
        MyDef::printLog(__FILE__, __LINE__, err);
        MyDef::exportErrorMsg(err);
        return wave;
    }

    QDataStream bread(&file);

    char *riff = new char[4];
    bread.readRawData(riff, 4);
    position += 4;
    bytesReserve(riff, 4);
    if(ToUInt32(riff) != 0x52494646)
    {
        QString err = QString::fromLocal8Bit("录音文件") + filename + QString::fromLocal8Bit("不是wav文件");
        MyDef::printLog(__FILE__, __LINE__, err);
        MyDef::exportErrorMsg(err);
        delete []riff;
        return wave;
    }
    delete []riff;

    char *riffSize = new char[4];
    bread.readRawData(riffSize, 4);
    position += 4;
    if(ToUInt32(riffSize) != (file.size() - position))
    {
        QString err = QString::fromLocal8Bit("录音文件") + filename + QString("损坏，文件长度与标记不符");
        MyDef::printLog(__FILE__, __LINE__, err);
        MyDef::exportErrorMsg(err);
        delete []riffSize;
        return wave;
    }
    delete []riffSize;

    char *waveID = new char[4];
    bread.readRawData(waveID, 4);
    position += 4;
    bytesReserve(waveID, 4);
    if(ToUInt32(waveID) != 0x57415645)
    {
        QString err = QString::fromLocal8Bit("录音文件") + filename + QString::fromLocal8Bit("不是wav文件");
        MyDef::printLog(__FILE__, __LINE__, err);
        MyDef::exportErrorMsg(err);
        delete []waveID;
        return wave;
    }
    delete []waveID;

    char *fmtID = nullptr;
    char *tmp = new char[4];
    bread.readRawData(tmp, 4);
    position += 4;
    bytesReserve(tmp, 4);
    if(ToUInt32(tmp) == 0x4a554e4b)
    {
        //包含junk标记的wav
        char *junklength = new char[4];
        bread.readRawData(junklength, 4);
        position += 4;
        uint junklen = ToUInt32(junklength);
        //将不要的junk部分读出
        char *temp = new char[junklen];
        bread.readRawData(temp, junklen);
        position += junklen;
        delete []temp;
        delete []junklength;

        fmtID = new char[4];
        bread.readRawData(fmtID, 4);
        bytesReserve(fmtID, 4);
        position += 4;
        delete []tmp;
    }
    else if(ToUInt32(tmp) == 0x666d7420)
        fmtID = tmp;
    else
    {
        QString err = QString::fromLocal8Bit("无法找到录音文件") + filename + QString::fromLocal8Bit("的junk和fmt标记");
        MyDef::printLog(__FILE__, __LINE__, err);
        MyDef::exportErrorMsg(err);
        delete []tmp;
        return wave;
    }

    if(ToUInt32(fmtID) != 0x666d7420)
    {
        QString err = QString::fromLocal8Bit("无法找到录音文件") + filename + QString::fromLocal8Bit("的fmt标记");
        MyDef::printLog(__FILE__, __LINE__, err);
        MyDef::exportErrorMsg(err);
        delete []fmtID;
        return wave;
    }
    delete []fmtID;

    char *cksize = new char[4];
    bread.readRawData(cksize, 4);
    position += 4;
    delete []cksize;

    char *waveTypeChars = new char[2];
    bread.readRawData(waveTypeChars, 2);
    position += 2;
    uint waveType = ToUInt16(waveTypeChars);
    delete []waveTypeChars;
    if(waveType != 1)
    {
        QString err = QString::fromLocal8Bit("录音文件") + filename + QString::fromLocal8Bit("不是pcm格式，暂时不支持");
        MyDef::printLog(__FILE__, __LINE__, err);
        MyDef::exportErrorMsg(err);
        return wave;
    }

    //声道数
    char *channel = new char[2];
    bread.readRawData(channel, 2);
    position += 2;
    delete []channel;

    //采样频率
    char *sample_rate = new char[4];
    bread.readRawData(sample_rate, 4);
    position += 4;
    int fs = ToUInt32(sample_rate);
    delete []sample_rate;

    //每秒字节数
    char *bytespersec = new char[4];
    bread.readRawData(bytespersec, 4);
    position += 4;
    delete []bytespersec;

    //每次采样的字节大小，2为单声道，4为立体声道
    char *blocklen_sample = new char[2];
    bread.readRawData(blocklen_sample, 2);
    position += 2;
    delete []blocklen_sample;

    //每个声道的采样精度，默认16bit
    char *bitNum = new char[2];
    bread.readRawData(bitNum, 2);
    position += 2;
    delete []bitNum;

    tmp = new char[2];
    bread.readRawData(tmp, 2);
    position += 2;
    //寻找data标记
    bytesReserve(tmp, 2);
    while(ToUInt16(tmp) != 0x6461)
    {
        bread.readRawData(tmp, 2);
        position += 2;
        bytesReserve(tmp, 2);
    }
    bread.readRawData(tmp, 2);
    position += 2;

    bytesReserve(tmp, 2);
    if(ToUInt16(tmp) != 0x7461)
    {
        QString err = QString::fromLocal8Bit("无法找到录音文件") + filename + QString::fromLocal8Bit("的data标记");
        MyDef::printLog(__FILE__, __LINE__, err);
        MyDef::exportErrorMsg(err);
        delete []tmp;
        return wave;
    }
    delete []tmp;

    //wav数据byte长度
    char *DataSizeChars = new char[4];
    bread.readRawData(DataSizeChars, 4);
    position += 4;
    uint DataSize = ToUInt32(DataSizeChars);
    delete []DataSizeChars;
    //计算样本数
    long NumSamples = DataSize / 2;

    if(NumSamples == 0)
    {
        NumSamples = (file.size() - position) / 2;
    }

    qint16 *data = new qint16[NumSamples];
    char *dataChars = new char[2];

    for(int i = 0; i < NumSamples; i++)
    {
        //读入2字节有符号整数
        bread.readRawData(dataChars, 2);
        position += 2;
        data[i] = dataChars[0] + dataChars[1] * 256;
    }
    delete []dataChars;

    file.close();
    wave.wavs = data;
    wave.fs = fs;
    wave.length = NumSamples;
    return wave;
}

void SpeechRecognizer::bytesReserve(char *nbytes, int length)
{
    char *sbytes = new char[length];
    for(int i = 0; i < length; i++)
        sbytes[i] = nbytes[i];
    for(int i = 0; i < length; i++)
        nbytes[i] = sbytes[length - i - 1];
    delete []sbytes;
}

quint32 SpeechRecognizer::ToUInt32(char *nbytes)
{
    unsigned char sbytes[4];
    for(int i = 0; i < 4; i++)
        sbytes[i] = nbytes[i];
    quint32 num = sbytes[0] + sbytes[1] * 256 + sbytes[2] * 256 * 256 + sbytes[3] * 256 * 256 * 256;
    return num;
}

quint16 SpeechRecognizer::ToUInt16(char *nbytes)
{
    unsigned char sbytes[2];
    sbytes[0] = nbytes[0];
    sbytes[1] = nbytes[1];
    quint16 num = sbytes[0] + sbytes[1] * 256;
    return num;
}

void SpeechRecognizer::Post(QString url, QString token, QString fs, QString wavs)
{
    QUrl newUrl = QUrl::fromUserInput(url);
    if(!newUrl.isValid())
    {
        QString err = QString("URL = ") + url + QString::fromLocal8Bit("链接不可用");
        MyDef::printLog(__FILE__, __LINE__, err);
        MyDef::exportErrorMsg(err);
        return;
    }

    QByteArray postData = (QString("token=") + token + QString("&fs=") + fs + wavs).toUtf8();
    QNetworkRequest request(newUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(QString("application/x-www-form-urlencoded")));
    qnam->post(request, postData);
}
