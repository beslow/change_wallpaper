#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void requestFinished(QNetworkReply* reply);

    void downloadFileFromUrl(QString strUrl, QString path);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
