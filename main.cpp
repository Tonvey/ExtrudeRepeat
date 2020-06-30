#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>


struct ParserArgs
{
    QString inputAtlas;
    QString inputConfig;
    QString outputAtlas;
    QString outputConfig;
    int extrude;
};

bool ParseCMDLine(QApplication &app,ParserArgs &retArgs)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(QGuiApplication::translate("main", "Qt"));  // Set Application description
    parser.addHelpOption();  							// ("-h" or "-help")
    parser.addVersionOption(); 					 		// ("-v" or "--version")
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    QCommandLineOption atlasInOption(QStringList() <<"i" << "input-atlas",
                   QGuiApplication::translate("main", "Specify the atlas file input."),
                   QGuiApplication::translate("main", "xxx.png"), "");
    QCommandLineOption configInOption(QStringList() <<"c" << "input-config",
                   QGuiApplication::translate("main", "Specify the input json array file of the atlas."),
                   QGuiApplication::translate("main", "xxx.json"), "");
    QCommandLineOption atlasOutOption(QStringList() <<"o" << "output-atlas",
                   QGuiApplication::translate("main", "Specify the atlas file output."),
                   QGuiApplication::translate("main", "xxx.png"), "");
    QCommandLineOption configOutOption(QStringList() <<"f" << "output-config",
                   QGuiApplication::translate("main", "Specify the output json array file of the atlas."),
                   QGuiApplication::translate("main", "xxx.json"), "");
    QCommandLineOption extrudeOption(QStringList() << "e" << "extrude",
                     QGuiApplication::translate("main", "Specify the extrude pixel num per tile of the atlas , default is 32."),
                     QGuiApplication::translate("main", "n"), "32");
    parser.addOption(atlasInOption);
    parser.addOption(atlasOutOption);
    parser.addOption(configInOption);
    parser.addOption(configOutOption);
    parser.addOption(extrudeOption);
    parser.process(app);

    if(!parser.isSet(atlasInOption))
    {
        qCritical()<<"Error: input-atlas must be specified";
        return false;
    }
    if(!parser.isSet(configInOption))
    {
        qCritical()<<"Error: input-config must be specified";
        return false;
    }

    QString atlasInFile = parser.value(atlasInOption);
    QString configInFile = parser.value(configInOption);
    QString atlasOutFile;
    QString configOutFile;

    if(!parser.isSet(atlasOutOption))
    {
        atlasOutFile = atlasInFile;
        qDebug()<<"output-atlas not be specifed , use " << atlasOutFile;
    }
    else
    {
        atlasOutFile = parser.value(atlasOutOption);
    }
    if(!parser.isSet(configOutOption))
    {
        configOutFile = configInFile;
        qDebug()<<"output-atlas not be specifed , use " << atlasOutFile;
    }
    else
    {
        configOutFile = parser.value(configOutOption);
    }

    int extrude = parser.value(extrudeOption).toInt();

    retArgs.inputAtlas = atlasInFile;
    retArgs.inputConfig = configInFile;
    retArgs.outputAtlas = atlasOutFile;
    retArgs.outputConfig = configOutFile;
    retArgs.extrude = extrude;

    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    if(argc>1)
    {
        ParserArgs retArgs;
        if(ParseCMDLine(a,retArgs))
        {
            return w.convert(retArgs.inputAtlas,
                      retArgs.inputConfig,
                      retArgs.outputAtlas,
                      retArgs.outputConfig,
                      retArgs.extrude);
        }
    }
    else
    {
        w.show();
        return a.exec();
    }
    return 0;
}
