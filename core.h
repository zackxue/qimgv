#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QWidget>
#include <QFileDialog>
#include "mainwindow.h"
#include "imageviewer.h"
#include "directorymanager.h"
#include "opendialog.h"
#include "settingsdialog.h"
#include "imageloader.h"
#include "settings.h"

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core();
    ImageViewer *imageViewer;
    DirectoryManager *dirManager;
    void connectGui(MainWindow*);
    void open(QString);

private:
    void initVariables();
    void connectSlots();
    MainWindow *mainWindow;
    OpenDialog *openDialog;
    SettingsDialog *settingsDialog;
    ImageLoader *imgLoader;

private slots:
    void setInfoString();

signals:

public slots:
    void initSettings();
    void slotNextImage();
    void slotPrevImage();
    void setCurrentDir(QString);
    //from gui
    void showOpenDialog();
    void showSettingsDialog();
    void reconfigure();
};

#endif // CORE_H

