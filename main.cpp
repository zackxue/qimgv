#include "core.h"
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
    QCoreApplication::setApplicationVersion("0.25dev");

    globalSettings = Settings::getInstance();
    atexit(saveSettings);

    Core *c = new Core();
    MainWindow *mw = new MainWindow();
    c->connectGui(mw);
    mw->show();

    if(a.arguments().length()>1) {
        QString fileName = a.arguments().at(1); // "/mnt/main/Pictures/things/2b450b34e6cd92ea52c9097874f69f3583213307.jpg";
        fileName.replace("\\\\","/");
        fileName.replace("\\","/");
        mw->slotTriggerFullscreen();
        c->open(fileName);
    }

    return a.exec();
}
