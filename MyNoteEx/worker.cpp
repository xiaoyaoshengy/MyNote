#include "worker.h"
#include "mynote.h"
#include <cstdlib>
#include <iostream>
#include <QObject>
#include<QDebug>
#include<QThread>
#include <QApplication>
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
#include<QCoreApplication>
#include <QThread>
#include "controller.h"
#include "lobal.h"
#include <QSysInfo>
#include<QThread>
#include <cstdlib>
#include <iostream>
#include<QString>
#include"mainwindow.h"
#include<QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
using std::cout;
void Worker::onWriteText(){
    QFile file_h(QCoreApplication::applicationDirPath()+"/../public/Samples/TypeScript/Demo/hitokoto.json");
    if(!file_h.open(QIODevice::WriteOnly)) {
           qDebug() << "File open error";
       } else {
           qDebug() <<"File open!";
       }
   // 使用QJsonObject对象插入键值对。
       QJsonObject jsonObject;
       //jsonObject.remove()
       Result="您还没有日程哦。";
       jsonObject.insert("hitokoto",Result);
   // 使用QJsonDocument设置该json对象
       QJsonDocument jsonDoc;
       jsonDoc.setObject(jsonObject);

   // 将json以文本形式写入文件并关闭文件。
       file_h.write(jsonDoc.toJson());
       file_h.close();

}
void Worker::onWriteMessage(QString Result){
    ////////////////////////////////////////////////////////////////////////////////////


      QFile f(QCoreApplication::applicationDirPath()+"/../public/Samples/TypeScript/Demo/live2d/message01.json");
      if(!f.open(QIODevice::WriteOnly)){
          qDebug()<<"Message File loads failed";
      }
      qDebug()<<"Message File loads successfully";
//      QJsonParseError jsonParserError_f;
//      QJsonDocument jsonDocument_M = QJsonDocument::fromJson(f.readAll(),&jsonParserError_f);

      QJsonObject rootObj;

      QJsonObject obj;
      obj["selector"] = "#landlord #live2d";
      obj["text"] = Result;


      QJsonArray classArray;
      classArray.append(obj);

      rootObj["mouseover"] = classArray;

      QJsonDocument jdoc(rootObj);
      f.write(jdoc.toJson());
      f.close();
}

void Worker::setSavePath(QString a)
{
    SavePath=a;
}
void Worker::doWork(int parameter){
    qDebug()<<"receive the execute signal---------------------------------";
    qDebug()<<"     current thread ID:"<<QThread::currentThreadId();
    while(1){

           //*////////////////////////////////////////////////////////////////// //延时模块
           QDateTime n2_f=QDateTime::currentDateTime();
           QDateTime now_f;
           do{
           now_f=QDateTime::currentDateTime();
           if(flag==0){break;}
           } while (n2_f.secsTo(now_f)<=1);//1为需要延时的秒数,每1s执行一次while循环里的代码
             /////////////////////////////////////////////////////////////////////////
       if(flag==0){break;}
       qDebug()<<SavePath;

       QDir *dir=new QDir(SavePath);
       qDebug() << dir;
       QStringList filter;
       filter<<"*.json";
       dir->setNameFilters(filter); //过滤文件类型
       QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
       qDebug()<<"File Number:";
       int count=fileInfo->count();  //json文件个数
       qDebug()<<count<<"\n";
       int R=0;
       R=count;
       int judge=1;
       while(judge){
           QDateTime n2_f=QDateTime::currentDateTime();
           QDateTime now_f;
           do{
           now_f=QDateTime::currentDateTime();
           if(flag==0){break;}
           } while (n2_f.secsTo(now_f)<=1);//1为需要延时的秒数,每1s执行一次while循环里的代码
           if(flag==0){break;}
       if(count>0){
          if(count>1){
              qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
              R=qrand()%(count);//生成0到文件个数的随机数
           }
          if(count==1){
              R=0;
          }
          QString Dir;
          if(flag==0){break;}
          qDebug()<<"the listnumber is"<<R<<"\n";
          qDebug()<<SavePath;
          QRegularExpression re("\\d{4}\-\\d{2}\-\\d{2}");
          QRegularExpressionMatch match = re.match(fileInfo->at(R).fileName());
          QString Date=fileInfo->at(R).fileName();
          Date=Date.remove(10,14);
          qDebug()<<Date;

          if(match.hasMatch()){
              judge=0;
             //Dir=m_p_parent->savePath();
              Dir=SavePath+"/"+fileInfo->at(R).fileName();

            qDebug()<<Dir;
            QFile file(Dir);
            if ( file.open( QIODevice::ReadWrite ) ) {

            qDebug()<< "File loads successfully";
            QJsonParseError jsonParserError;
            QJsonDocument   jsonDocument = QJsonDocument::fromJson( file.readAll(), &jsonParserError );

            if (!jsonDocument.isNull() && jsonParserError.error == QJsonParseError::NoError ) {
                  qDebug()<< "File analyses successfully";


            if ( jsonDocument.isArray()){

                 QJsonArray jsonArray = jsonDocument.array();

                 for(int i =jsonArray.size()-1;i>=0;i--){
                 if(jsonArray[i].isObject()){
                     QJsonObject jsonObject=jsonArray[i].toObject();
                     if(jsonObject.contains("Address")&&jsonObject.contains("Time")&&
                        jsonObject.value("Address").isString()&&jsonObject.value("Time").isString()){
                         qDebug()<<"load Address and Time";
                         Address= jsonObject.value( "Address" ).toString();               
                         Time=jsonObject.value("Time").toString();
                         qDebug()<<Time;
                         if(Address.isEmpty()){
                             qDebug()<<"Address is Null";
                             Result="您在"+Date+"-"+Time+"有事情要做哦。";
                         }else{
                         Result="记得在"+Date+"-"+Time+"去"+Address;
                         }
                         cout << "Address is "<< jsonObject.value( "Address" ).toString().toStdString()<< "\n";
                         cout << "Time is "<< jsonObject.value( "Time" ).toString().toStdString()<< "\n";
                         cout<<"-------\n";

                         QFile file_h(QCoreApplication::applicationDirPath()+"/../public/Samples/TypeScript/Demo/hitokoto.json");
                         if(!file_h.open(QIODevice::WriteOnly)) {
                               qDebug() << "File open error";

                             } else {
                               qDebug() <<"File open!";
                             }
                               // 使用QJsonObject对象插入键值对。
                         QJsonObject jsonObject;
                                   //jsonObject.remove()
                         jsonObject.insert("hitokoto", Result);

                               // 使用QJsonDocument设置该json对象
                         QJsonDocument jsonDoc;
                        jsonDoc.setObject(jsonObject);

                               // 将json以文本形式写入文件并关闭文件。
                        file_h.write(jsonDoc.toJson());
                        file_h.close();
                        onWriteMessage(Result);
                        qDebug() << "Write to file";


                                //cout<<Address.toStdString()<<"--"<<Time.toStdString()<<"\n";
                            }
                        }

                    }

                }

            }else{
              onWriteMessage("您还没有日程哦。");
            }
          }
            file.close();

         }
          else {
              if(count==1){
                onWriteMessage("您还没有日程哦。");
              }
              else{
                  judge=1;
              }

          }

       }

       else{
          onWriteText();
          onWriteMessage("您还没有日程哦。");
          break;
       }
       if(flag==0){break;}
      }


       ////////////////////////////////////////////////////////////////////////////////////

    ++parameter;
    delete dir;
    if(flag==0){break;}
}
   // 发送结束信号
   qDebug()<<"      finish the work and sent the resultReady signal\n";

   emit resultReady(parameter);
}
