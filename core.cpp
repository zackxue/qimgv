#include "core.h"

Core::Core() :
    QObject(),
    imageLoader(NULL),
    dirManager(NULL)
{
    initVariables();
    connectSlots();
    initSettings();
}

void Core::initVariables() {
    dirManager = new DirectoryManager();
    imageLoader = new ImageLoader(dirManager);
}

// misc connections not related to gui
void Core::connectSlots() {
    connect(imageLoader, SIGNAL(loadFinished(Image*)),
            this, SLOT(onLoadFinished(Image*)));
}

void Core::initSettings() {
}

// do not call before connecting gui
// because muh architecture
void Core::reconfigure() {
    imageLoader->readSettings();
   // mainWindow->readSettings();
}

void Core::setInfoString() {
    QString infoString = "";
    infoString.append(" [ " +
                      QString::number(dirManager->currentPos+1) +
                      "/" +
                      QString::number(dirManager->fileList.length()) +
                      " ]   ");
    if(currentImage) {
        infoString.append(currentImage->getInfo()->getName() + "  ");
        infoString.append("(" +
                          QString::number(currentImage->getInfo()->getWidth()) +
                          "x" +
                          QString::number(currentImage->getInfo()->getHeight()) +
                          ")  ");
    }
    else {
        infoString.append("Loading...");
    }

    emit infoStringChanged(infoString);
}

void Core::setCurrentDir(QString path) {
    dirManager->setCurrentDir(path);
}

void Core::slotNextImage() {
    currentImage = NULL;
    FileInfo* f = dirManager->next();
    setInfoString();
    imageLoader->load(f);
}

void Core::slotPrevImage() {
    currentImage = NULL;
    FileInfo* f = dirManager->prev();
    setInfoString();
    imageLoader->load(f);
}

void Core::loadImage(QString path) {
    currentImage = NULL;
    FileInfo* f = dirManager->next();
    setInfoString();
    imageLoader->load(path);
}

void Core::onLoadFinished(Image* img) {
    emit signalUnsetImage();
    currentImage = img;
    emit signalSetImage(currentImage);
    setInfoString();
}
