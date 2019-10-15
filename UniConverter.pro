QT += core gui sql

CONFIG += c++14
CONFIG -= app_bundle

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UniConverter
TEMPLATE = app

# Run qmake before build to versions match
VERSION = $$cat($$PWD/Build/version.txt)

message(VERSION = $$VERSION)

DEFINES += APPNAME=\\\"$$TARGET\\\"

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    Application.cpp \
    Views/Controls/WaitingSpinnerWidget.cpp \
    Views/MainWindow.cpp \
    Models/SourceBaseStationsTableModel.cpp \
    Types/BaseStation.cpp \
    Database/AbstractDbStorage.cpp \
    Database/AbstractBaseStationsSource.cpp \
    Converting/AbstractBaseStationsConverter.cpp \
    Database/Excel/ExcelBaseStationsSource.cpp \
    Database/SqliteDbStorage.cpp \
    Utils.cpp \
    ApplicationSettings.cpp \
    Database/Excel/XlsBaseStationsSource.cpp \
    Database/Excel/XlsxBaseStationsSource.cpp \
    Database/BaseStationsSourceTemplate.cpp \
    Views/BaseStationsSourceTemplateWidget.cpp \
    BasicExcel/BasicExcel.cpp \
    Views/ConvertingErrorsDialog.cpp

HEADERS += \
    Application.h \
    Views/Controls/WaitingSpinnerWidget.hpp \
    Views/MainWindow.h \
    Models/SourceBaseStationsTableModel.h \
    Types/BaseStation.h \
    Database/AbstractDbStorage.h \
    Database/AbstractBaseStationsSource.h \
    Converting/AbstractBaseStationsConverter.h \
    Database/Excel/ExcelBaseStationsSource.h \
    Database/SqliteDbStorage.h \
    Utils.h \
    ApplicationSettings.h \
    Database/Excel/XlsBaseStationsSource.h \
    Database/Excel/XlsxBaseStationsSource.h \
    Database/BaseStationsSourceTemplate.h \
    Views/BaseStationsSourceTemplateWidget.h \
    BasicExcel/BasicExcel.hpp \
    Views/ConvertingErrorsDialog.h

# Include QtXlsx library
    include($$PWD/QtXlsx/src/xlsx/qtxlsx.pri)

# Translations sources
TRANSLATIONS    +=  Localization/localization_ru.ts
TRANSLATIONS    +=  Localization/localization_en.ts

RC_ICONS = Resources/app.ico

unix:!macx {
    CONFIG(debug, debug|release) {
        DESTDIR = Debug
    }
    CONFIG(release, debug|release) {
        DESTDIR = Release
    }

    OBJECTS_DIR = $$DESTDIR/obj
    MOC_DIR = $$DESTDIR/moc
    RCC_DIR = $$DESTDIR/rcc

    CONFIG(release, debug|release) {
        QMAKE_PRE_LINK += "chmod +x $$PWD/Build/update_version.sh && $$PWD/Build/update_version.sh"
        QMAKE_POST_LINK += "sed -i 's/^Version=.*$//Version=\"$$VERSION\"/g' $$PWD/Installs/MakeRelease-linux.sh && chmod +x $$PWD/Installs/MakeRelease-linux.sh"
    }
}

win32:CONFIG(release, debug|release) {
# Set requireAdministrator for app
    CONFIG += embed_manifest_exe
    QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:level=\'requireAdministrator\'

    QMAKE_PRE_LINK += "$$PWD/Build/increase_version.bat"

    QMAKE_POST_LINK += echo ISCC.exe $$PWD/Installs/MakeInstaller-win32.iss /DVersion=$$VERSION /DReleaseDir=$$OUT_PWD/release /DAppName=$$TARGET /DAppExeName=\"$$TARGET\".exe > $$PWD/Installs/MakeRelease-win32.bat$$escape_expand(\n\t) \
                       echo pause >> $$PWD/Installs/MakeRelease-win32.bat$$escape_expand(\n\t)\
}
