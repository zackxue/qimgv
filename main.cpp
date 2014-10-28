#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "settings.h"

void saveSettings() {
    delete globalSettings;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("greenpepper software");
    QCoreApplication::setOrganizationDomain("nosite.com");
    QCoreApplication::setApplicationName("qimgv");
    QCoreApplication::setApplicationVersion("0.26dev");

    globalSettings = Settings::getInstance();
    atexit(saveSettings);

    MainWindow mw;

    if(a.arguments().length()>1) {
        QString fileName = a.arguments().at(1);
        fileName.replace("\\\\","/");
        fileName.replace("\\","/");
        mw.slotTriggerFullscreen();
//        c->loadImage(fileName);
    }
    mw.show();

    return a.exec();
}
