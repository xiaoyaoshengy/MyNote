#ifndef SPEECHRECOGNIZER_H
#define SPEECHRECOGNIZER_H

#include "wavrecorder.h"

#include <QObject>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class SpeechRecognizer : public QObject
{
    Q_OBJECT
public:
    SpeechRecognizer(QString url_server = QString("https://api.ailemon.net/asrt/v1/"), QString token_client = QString("qwertasd"), QObject *parent = nullptr);
    ~SpeechRecognizer();
    bool isRecognizing();
    void Start();
    void Stop();

signals:
    void ReceiveText(QString text);
    void DeleteWavFile(QString filename);

private slots:
    void Timer_Tick();
    void httpFinished(QNetworkReply *reply);
    void DelWavFile(QString filename);

private:
    WavRecorder *recorder = new WavRecorder[2];

    QString url = QString("https://api.ailemon.net/asrt/v1/");
    QString token = QString("qwertasd");

    int i_audioRecorder = 0;
    WavRecorder _audioRecorder;
    WavRecorder _audioRecorder_old;
    //定义计时器
    QTimer *timer;
    bool _is_recognizing = false;
    struct wav
    {
    public:
        qint16 *wavs = nullptr;
        int fs = 0;
        long length = 0;
    };

    QNetworkAccessManager *qnam;

    void SpeechRecognize(QString filename);
    wav WaveAccess(QString filename);
    void bytesReserve(char *nbytes, int length);
    quint32 ToUInt32(char *nbytes);
    quint16 ToUInt16(char *nbytes);
    void Post(QString url, QString token, QString fs, QString wavs);

};

#endif // SPEECHRECOGNIZER_H
