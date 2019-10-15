#include "Application.h"
#include <QDir>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDateTime>
#include <iostream>
#include "ApplicationSettings.h"

static const QString LogFile = "log.txt";

void DebugOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);

    static QMap<QtMsgType, QChar> msgTypes =
    {
        {QtMsgType::QtInfoMsg,     'I'},
        {QtMsgType::QtDebugMsg,    'D'},
        {QtMsgType::QtWarningMsg,  'W'},
        {QtMsgType::QtCriticalMsg, 'C'},
        {QtMsgType::QtFatalMsg,    'F'},
    };

    QFile logFile(LogFile);
    if(!logFile.open(QIODevice::Append | QIODevice::Text))
    {
        return;
    }

#ifdef QT_DEBUG
    std::cout << QString("%1 - [%2] - %3:%4: %5 \n")
                 .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz"))
                 .arg(msgTypes.value(type))
                 .arg(QString(context.function))
                 .arg(context.line)
                 .arg(msg).toStdString() << std::endl;
#endif

    QTextStream textStream(&logFile);
    textStream << QString("%1 [%2] %3 \n")
                  .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz"))
                  .arg(msgTypes.value(type))
                  .arg(msg);

    textStream.flush();
    logFile.flush();
    logFile.close();

    if(type == QtFatalMsg)
    {
        qWarning("ATTENTION!!! CRASH!!!");
        abort();
    }
}


Application::Application(int &argc, char **argv):
    QApplication(argc, argv)
{
    QFile::remove(LogFile);

    QDir::setCurrent(qApp->applicationDirPath());
    qDebug() << "Current dir" << QDir::currentPath();

    QCommandLineParser parser;
    parser.addHelpOption();

    QCommandLineOption writeLogOption("log", QString("Write program log to %1").arg(LogFile));
    parser.addOption(writeLogOption);

    QCommandLineOption langOption("l", "Application language", "language", "");
    parser.addOption(langOption);

    parser.process(*this);

    if (parser.isSet(writeLogOption))
    {
       qInstallMessageHandler(DebugOutput);
    }
    else
    {
       qSetMessagePattern("[%{time mm:ss.zzz}:%{function}:%{line}] %{message}");
    }

    QString language = ApplicationSettings::instance().language;

    if(parser.isSet(langOption))
    {
        language = parser.value(langOption).toLower().left(2);
        ApplicationSettings::instance().language = language;
    }

    loadTranslation(language);
}

Application::~Application()
{
    qDebug() << "Application dtor";
}

void Application::loadTranslation(const QString& lang)
{
    QMap<QString, QString> LangMap = {{"en", "localization_en.qm"},
                                      {"ru", "localization_ru.qm"}};

    QString language;
    if(LangMap.contains(lang))
    {
        language = lang;
    }
    else
    {
        language = "ru";
    }

    auto translation = LangMap.value(language);

    qInfo() << "Loading translations" << language << translation;

    bool loaded = translator.load(translation);
    if(!loaded || !installTranslator(&translator))
    {
        qWarning() << "Translation was not loaded!";
    }
}

