#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_pushButton_image_browse_clicked();
    void on_pushButton_json_browse_clicked();
    void setImageFilePath(const QString &path);
    void setJsonFilePath(const QString &path);
    void on_pushButton_convert_clicked();

private:
    Ui::MainWindow *ui;
    QString imageFileName;
    QString jsonFileName;
    int exturde = 10;

private:
    void convert(QString imageFileName,QString jsonFileName);


};
#endif // MAINWINDOW_H
