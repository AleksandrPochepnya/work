#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QDebug>
#include <QTranslator>

class Application: public QApplication
{
    Q_OBJECT
public:
      Application(int& argc, char** argv);
      ~Application();

private:
      void loadTranslation(const QString &lang);
      QTranslator translator;
};

#endif // APPLICATION_H
