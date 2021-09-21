#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class SettingDialog;
}

class MyNote;
class QAbstractButton;

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(MyNote *parent = nullptr);
    ~SettingDialog();
    //设置保存路径
    void setSavePath(QString savePath);

private slots:
    //“浏览”按钮按下时
    void onPushButtonBrowserClicked();
    //对话框按钮按下时
    void onDialogButtonClicked(QAbstractButton *button);
    //开机自启选中时
    void onCheckBoxAutoRunClicked(bool clicked);
    //语音命令选中时
    void onCheckBoxVoiceCommandClicked(bool clicked);
    //当保存路径编辑框改变时
    void onSavePathLineEditChanged(QString path);

private:
    Ui::SettingDialog *ui;
    //主窗体指针
    MyNote* m_p_parent;
    //“保存路径”临时字符串
    QString m_temp_save_path;

    //保存设置
    void saveSettings();
    //移动日程保存目录
    void moveSavingDir();
    //初始化设置
    void iniSettings();
};

#endif // SETTINGDIALOG_H
