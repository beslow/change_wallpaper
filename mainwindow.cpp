#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include "windows.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QTextCodec>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString bingUrl = "https://cn.bing.com/HPImageArchive.aspx?format=js&idx=0&n=10&mkt=zh-CN";

    QNetworkRequest request;
    QNetworkAccessManager* naManager = new QNetworkAccessManager(this);
    QMetaObject::Connection connRet = QObject::connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
    Q_ASSERT(connRet);

    request.setUrl(QUrl(bingUrl));
    naManager->get(request);
//    QNetworkReply* reply =

    //壁纸注册表表
    QSettings wallPaper("HKEY_CURRENT_USER\\Control Panel\\Desktop",
                       QSettings::NativeFormat);

    //新的桌面图片路径
    QString path("C:\\Users\\云\\Pictures\\161849709793.jpg");

    //给壁纸注册表设置新的值（新的图片路径）
    wallPaper.setValue("Wallpaper",path);
    QByteArray byte = path.toLocal8Bit();

    //调用windowsAPI
    SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, byte.data(), SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
}

void MainWindow::requestFinished(QNetworkReply* reply) {
    // 获取http状态码
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(statusCode.isValid())
        qDebug() << "status code=" << statusCode.toInt();

    QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    if(reason.isValid())
        qDebug() << "reason=" << reason.toString();

    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        qDebug() << "Failed: " << reply->errorString();
    }
    else {
        // 获取返回内容
        QTextCodec *codec = QTextCodec::codecForName("UTF8");
        QString response = codec->toUnicode(reply->readAll());
        qDebug() << response;

        QJsonParseError err_rpt;
        QJsonDocument root_Doc = QJsonDocument::fromJson(response.toUtf8(), &err_rpt);
        if(err_rpt.error != QJsonParseError::NoError) {
            qDebug() << "JSON格式错误";
            return;
        } else {
            QJsonObject root_Obj = root_Doc.object();
            QJsonValue imagesValue = root_Obj.value("images");
            QJsonArray imageArray = imagesValue.toArray();
            for(int i=0; i<imageArray.size(); i++) {
                QJsonObject image = imageArray.at(i).toObject();
                QString url = image.value("url").toString();
                qDebug() << url;

                downloadFileFromUrl("https://www.bing.com" + url, QCoreApplication::applicationDirPath() + "/data/" + QString::number(i) + ".jpg");
            }
        }
    }
}

//get
void MainWindow::downloadFileFromUrl(QString url, QString path)
{

    //构造请求
    QNetworkAccessManager manager;
    QNetworkRequest request;
    request.setUrl(url);
    // 发送请求
    QNetworkReply *reply = manager.get(request);
    //开启一个局部的事件循环，等待响应结束，退出
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    //判断是否出错,出错则结束
    if (reply->error() != QNetworkReply::NoError)
    {
        return;
    }
    //保存文件
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
        return;
    file.write(reply->readAll());
    file.close();
    delete reply;

    qDebug() << "sent request";
}
