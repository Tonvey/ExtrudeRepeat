#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->statusLabel = new QLabel(this);
    this->statusLabel->setText("Ready");
    this->ui->statusbar->addWidget(statusLabel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_image_browse_clicked()
{
    QString imageFileName = QFileDialog::getOpenFileName(this,"Choose Atlas","",tr("Images (*.png *.xpm *.jpg)"));
    QFileInfo imageFileInfo(imageFileName);
    QDir dir = imageFileInfo.dir();
    this->setImageFilePath(imageFileName);

    if(this->ui->lineEdit_json->text().length()==0)
    {
        QString imageBaseName = imageFileInfo.baseName();
        QString jsonFile= dir.filePath(imageBaseName+".json");
        QFileInfo jsonFileInfo(jsonFile);
        if(jsonFileInfo.exists())
        {
            this->setJsonFilePath(jsonFile);
        }
    }
}

void MainWindow::on_pushButton_json_browse_clicked()
{
    QString jsonFileName = QFileDialog::getOpenFileName(this,"Choose Atlas","",tr("Json (*.json)"));
    QFileInfo jsonFileInfo(jsonFileName);
    QDir dir = jsonFileInfo.dir();
    this->setJsonFilePath(jsonFileName);

    if(this->ui->lineEdit_image->text().length()==0)
    {
        QString jsonBaseName = jsonFileInfo.baseName();
        QString imageFile= dir.filePath(jsonBaseName+".png");
        QFileInfo imageFileInfo(imageFile);
        if(imageFileInfo.exists())
        {
            this->setImageFilePath(imageFile);
        }
    }
}

void MainWindow::setImageFilePath(const QString &path)
{
    this->ui->lineEdit_image->setText(path);
}

void MainWindow::setJsonFilePath(const QString &path)
{
    this->ui->lineEdit_json->setText(path);
}

static void fillImage(QImage &image , QPoint refOffset , QPoint offset,int stepX,int stepY)
{
    if(image.format()==QImage::Format_Indexed8)
    {
        int idx = image.pixelIndex(
                    stepX + refOffset.x(),
                    stepY + refOffset.y());
        image.setPixel(
                    stepX + offset.x(),
                    stepY + offset.y(),
                    idx);
    }
    else
    {
        QColor color = image.pixelColor(
                    stepX + refOffset.x(),
                    stepY + refOffset.y());
        image.setPixelColor(
                    stepX + offset.x(),
                    stepY + offset.y(),
                    color);
    }
}

void MainWindow::replaceExtrude(QImage &image,QPoint topLeft,
                                QPoint topRight,QPoint bottomLeft,
                                QPoint bottomRight,int extrude)
{

    int w = topRight.x() - topLeft.x();
    int h = bottomRight.y() - topRight.y();
    //Base on the 5th zone (numpad) to extrude repeat
    //  | 7 | 8 | 9 |
    //  | 4 | 5 | 6 |
    //  | 1 | 2 | 3 |
    QPoint offset,refOffset;
    //zone2
    offset = QPoint(bottomLeft.x(),bottomLeft.y());
    refOffset = QPoint(topLeft.x(),topLeft.y());
    for(int stepX = 0 ; stepX < w;++stepX)
    {
        for(int stepY = 0 ; stepY < extrude ; ++stepY)
        {
            fillImage(image,offset,refOffset,stepX,stepY);
        }
    }
    //zone4
    offset = QPoint(topLeft.x(),topLeft.y());
    refOffset = QPoint(topRight.x(),topLeft.y());
    for(int stepX = -1 ; stepX >= -extrude;--stepX)
    {
        for(int stepY = 0 ; stepY < h ; ++stepY)
        {
            fillImage(image,offset,refOffset,stepX,stepY);
        }
    }
    //zone6
    offset = QPoint(topRight.x(),topRight.y());
    refOffset = QPoint(topLeft.x(),topRight.y());
    for(int stepX = 0 ; stepX < extrude;++stepX)
    {
        for(int stepY = 0 ; stepY < h ; ++stepY)
        {
            fillImage(image,offset,refOffset,stepX,stepY);
        }
    }
    //zone8
    offset = QPoint(topLeft.x(),topLeft.y());
    refOffset = QPoint(bottomLeft.x(),bottomLeft.y());
    for(int stepX = 0 ; stepX < w;++stepX)
    {
        for(int stepY = -1 ; stepY >= -extrude ; --stepY)
        {
            fillImage(image,offset,refOffset,stepX,stepY);
        }
    }
    //zone1
    offset = QPoint(bottomLeft.x(),bottomLeft.y());
    refOffset = QPoint(topRight.x(),topRight.y());
    for(int stepX = -1 ; stepX >= -extrude;--stepX)
    {
        for(int stepY = 0 ; stepY < extrude ; ++stepY)
        {
            fillImage(image,offset,refOffset,stepX,stepY);
        }
    }
    //zone3
    offset = QPoint(bottomRight.x(),bottomRight.y());
    refOffset = QPoint(topLeft.x(),topLeft.y());
    for(int stepX = 0 ; stepX < extrude;++stepX)
    {
        for(int stepY = 0 ; stepY < extrude ; ++stepY)
        {
            fillImage(image,offset,refOffset,stepX,stepY);
        }
    }
    //zone7
    offset = QPoint(topLeft.x(),topLeft.y());
    refOffset = QPoint(bottomRight.x(),bottomRight.y());
    for(int stepX = -1 ; stepX >= -extrude;--stepX)
    {
        for(int stepY = -1 ; stepY >= -extrude ; --stepY)
        {
            fillImage(image,offset,refOffset,stepX,stepY);
        }

    }
    //zone9
    offset = QPoint(topRight.x(),topRight.y());
    refOffset = QPoint(bottomLeft.x(),bottomLeft.y());
    for(int stepX = 0 ; stepX < extrude;++stepX)
    {
        for(int stepY = -1 ; stepY >= -extrude ; --stepY)
        {
            fillImage(image,offset,refOffset,stepX,stepY);
        }
    }
}

int MainWindow::convert(QString imageFileName, QString jsonFileName,
                         QString imageOutputFileName,QString jsonOutputFileName,int extrude)
{
    this->statusLabel->setText("Converting...");
    qDebug().noquote()<<"=======================================";
    qDebug().noquote()<<"Begin convert:";
    qDebug().noquote()<<"Input image:"<<imageFileName;
    qDebug().noquote()<<"Input json:"<<jsonFileName;
    qDebug().noquote()<<"Output image:"<<imageOutputFileName;
    qDebug().noquote()<<"Output json:"<<jsonOutputFileName;
    qDebug().noquote()<<"exturde:"<<extrude;
    qDebug().noquote()<<"=======================================";

    QFileInfo imageFileInfo(imageFileName);
    QFileInfo jsonFileInfo(jsonFileName);
    if(!imageFileInfo.isFile())
    {
        qCritical()<<"Input image:"<<imageFileName << "is not a file";
        return -1;
    }
    if(!jsonFileInfo.isFile())
    {
        qCritical()<<"Input json:"<<jsonFileName << "is not a file";
        return -1;
    }

    QFile jsonFile(jsonFileName);
    jsonFile.open(QFile::ReadOnly);

    QByteArray jsonContent = jsonFile.readAll();
    jsonFile.close();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(jsonContent,&err);
    if(err.error!=QJsonParseError::NoError)
    {
        qCritical()<<"Can not parse json , error : " << err.errorString();
        if(!this->isHidden())
        {
            QMessageBox::critical(this,"Parse Error!",
                  "Can not parse json , error : " + err.errorString());
        }
        return -1;
    }

    QJsonObject obj = doc.object();

    QJsonArray framesArray = obj.value("frames").toArray();
    QJsonObject meta = obj.value("meta").toObject();
    QImage image(imageFileName);
    this->ui->progressBar->setMaximum(framesArray.size());
    for(int i = 0 ; i< framesArray.size() ; ++i)
    {
        auto frameObj = framesArray[i].toObject();
        QString fileName = frameObj.value("filename").toString();
        qDebug().noquote()<<fileName;
        int x , y, w , h;
        auto rect = frameObj.value("frame").toObject();
        x = rect.value("x").toInt();
        y = rect.value("y").toInt();
        w = rect.value("w").toInt();
        h = rect.value("h").toInt();
        QPoint topLeft(x,y);
        QPoint topRight(x+w,y);
        QPoint bottomLeft(x,y+h);
        QPoint bottomRight(x+w,y+h);
        this->replaceExtrude(image,topLeft,topRight,bottomLeft,bottomRight,extrude);
        this->ui->progressBar->setValue(i+1);
    }

    // Save output
    {
        // TODO: Recorde the extrude value , because the meta not do it default
        this->statusLabel->setText("Saving...");
        if(!image.save(imageOutputFileName))
        {
            if(!this->isHidden())
                QMessageBox::critical(this,"Error","Can not save image!");
            qCritical()<<"Can not save image!";
            this->statusLabel->setText("Fail");
            return -1;
        }
        meta.insert("extrude",extrude);
        obj.insert("meta",meta);

        QFile outJsonFile(jsonOutputFileName);
        outJsonFile.open(QFile::WriteOnly|QFile::Truncate);
        QJsonDocument docOut(obj);
        outJsonFile.write(docOut.toJson());
        outJsonFile.close();
        this->statusLabel->setText("Success!");
    }
    return 0;
}

void MainWindow::on_pushButton_convert_clicked()
{
    QString imageFileName = this->ui->lineEdit_image->text();
    QString jsonFileName = this->ui->lineEdit_json->text();
    QString strExtrude = this->ui->lineEdit_extrude->text();
    bool canConvertExtrude;
    int extrude = strExtrude.toInt(&canConvertExtrude);
    if(!canConvertExtrude)
    {
        QMessageBox::critical(this,"Error","Can not convert extrude string to uint : "+strExtrude);
        return;
    }

    if(imageFileName.trimmed().isEmpty()||jsonFileName.trimmed().isEmpty())
    {
        QMessageBox::critical(this,"Error","Please choose image file and json file!");
        return;
    }
    this->convert(imageFileName,jsonFileName,imageFileName,jsonFileName,extrude);
}
