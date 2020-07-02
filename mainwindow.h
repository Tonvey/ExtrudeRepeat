#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int convert(QString imageFileName,QString jsonFileName,
         QString imageOutputFileName,QString jsonOutputFileName,int extrude);

private slots:
    void on_pushButton_image_browse_clicked();
    void on_pushButton_json_browse_clicked();
    void setImageFilePath(const QString &path);
    void setJsonFilePath(const QString &path);
    void on_pushButton_convert_clicked();

private:
    Ui::MainWindow *ui;
    QLabel *statusLabel;

private:
    void replaceExtrude(QImage &image,QPoint topLeft,QPoint topRight,QPoint bottomLeft,QPoint bottomRight,int extrude);

};
#endif // MAINWINDOW_H
