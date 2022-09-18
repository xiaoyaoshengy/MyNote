#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QWebEngineView>
#include <QMainWindow>
#include <QProcess>
#include <QPushButton>
#include <QEvent>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <QtNetwork>
#include <QString>
#include "speechrecognizer.h"

class MyNote;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(MyNote *parent);
    ~MainWindow();
    QWebEngineView* m_webView;
    QString live2d_path;
    int TerminateProcess(QString ProcessName, QString PathName = "");
    //    void mouseDoubleClickEvent(QMouseEvent*) override{
    //        qDebug("sss");
    //    };
    //设置语音输入按钮是否显示
    void setVoiceCommandBtns(bool shown);

private:
    Ui::MainWindow *ui;
    QProcess *cmd;
    //日程模块
    MyNote *m_p_parent;
    //语音识别模块
    SpeechRecognizer *asr;
    //语音命令枚举类
    enum Commands
    {
        NewSchedule,
        OpenSchedule,
        OpenSetting,
        InvaildCommand
    };

    Commands convertToCommand(QString str);
    void transformCommands(Commands cmd);

private slots:
    void SpeechRecognizer_OnReceiveText(QString text);
    void onbtnStartClicked();
    void onbtnStopClicked();
    void on_readerror();
};
#endif // MAINWINDOW_H
