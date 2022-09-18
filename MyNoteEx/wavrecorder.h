#ifndef WAVRECORDER_H
#define WAVRECORDER_H

#include <QObject>
#include <Windows.h>

typedef MCIERROR (*FUNC)(LPCTSTR lpszCommand, LPTSTR lpszReturnString, UINT cchReturn, HANDLE hwndCallback);

class WavRecorder : public QObject
{
    Q_OBJECT
public:
    explicit WavRecorder(QObject *parent = nullptr);
    WavRecorder(QString dName, QObject *parent = nullptr);
    ~WavRecorder();
    void Record();
    void Stop();
    void Save(QString filename = QString("1.wav"));
    void Close();
    void Play();
    void setName(QString dName);
    WavRecorder &operator=(const WavRecorder& wav);

private:
    QString _dName;
    //每个样本的比特数，单位：位/比特/bit
    int bitspersample = 16;
    //声道数
    int channels = 1;
    //采样频率，单位：Hz
    int samplespersec = 16000;
    //从winmm.dll文件中取得的mciSendString句柄
    FUNC mci_send_string;

};

#endif // WAVRECORDER_H
