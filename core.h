#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QWidget>
#include <QFileDialog>
#include "directorymanager.h"
#include "imageloader.h"
#include "settings.h"

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core();
    DirectoryManager *dirManager;
    void open(QString);

private:
    void initVariables();
    void connectSlots();
    ImageLoader *imageLoader;
    Image* currentImage;

private slots:
    void setInfoString();

signals:
    void signalUnsetImage();
    void signalSetImage(Image*);
    void infoStringChanged(QString);

public slots:
    void loadImage(QString);
    void initSettings();
    void slotNextImage();
    void slotPrevImage();
    void setCurrentDir(QString);
    //from gui
    void reconfigure();
};

#endif // CORE_H

