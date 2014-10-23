#include "core.h"
#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
//#include "image.h"
#include "settings.h"

//#include <iostream>

void saveSettings() {
    delete globalSettings;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("greenpepper software");
    QCoreApplication::setOrganizationDomain("nosite.com");
    QCoreApplication::setApplicationName("qimgv");
    QCoreApplication::setApplicationVersion("0.24dev");

    globalSettings = Settings::getInstance();
    atexit(saveSettings);

    Core *c = new Core();
    MainWindow *mw = new MainWindow();

    c->connectGui(mw);
    mw->show();

    if(a.arguments().length()>1) {
        mw->slotTriggerFullscreen();
        c->open(a.arguments().at(1));
    }

    return a.exec();
}
