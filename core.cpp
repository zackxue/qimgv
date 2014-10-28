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

}

void Core::initSettings() {
    imageLoader->readSettings();
}

// do not call before connecting gui
// because muh architecture
void Core::reconfigure() {
    imageLoader->readSettings();
   // mainWindow->readSettings();
}

void Core::setInfoString() {
    QString infoString = "";
    infoString.append(currentImage->getInfo()->getName() + "  ");
    infoString.append("(" +
                      QString::number(currentImage->getInfo()->getWidth()) +
                      "x" +
                      QString::number(currentImage->getInfo()->getHeight()) +
                      ")  ");
    infoString.append("[ " +
                      QString::number(dirManager->currentPos+1) +
                      "/" +
                      QString::number(dirManager->fileList.length()) +
                      " ]");
    emit infoStringChanged(infoString);
}

void Core::setCurrentDir(QString path) {
    dirManager->setCurrentDir(path);
}

void Core::slotNextImage() {
    emit signalUnsetImage();
    currentImage = imageLoader->loadNext();
    emit signalSetImage(currentImage);
    setInfoString();
}

void Core::slotPrevImage() {
    emit signalUnsetImage();
    currentImage = imageLoader->loadPrev();
    emit signalSetImage(currentImage);
    setInfoString();
}

void Core::loadImage(QString filePath) {
    emit signalUnsetImage();
    currentImage = imageLoader->load(filePath);
    emit signalSetImage(currentImage);
    setInfoString();
}
