#include "core.h"

Core::Core() :
    QObject(),
    mainWindow(NULL),
    imageViewer(NULL),
    imgLoader(NULL),
    openDialog(NULL),
    dirManager(NULL)
{
    initVariables();
    connectSlots();
    initSettings();
}

void Core::initVariables() {
    imageViewer = new ImageViewer();
    dirManager = new DirectoryManager();
    imgLoader = new ImageLoader(dirManager);
    openDialog = new OpenDialog();
    settingsDialog = new SettingsDialog();
}

// misc connections not related to gui
void Core::connectSlots() {
    connect(imageViewer, SIGNAL(imageChanged()), this, SLOT(setInfoString()));
    connect(settingsDialog, SIGNAL(settingsChanged()), this, SLOT(reconfigure()));
}

void Core::initSettings() {
    imgLoader->readSettings();
}

// do not call before connecting gui
// because muh architecture
void Core::reconfigure() {
    imgLoader->readSettings();
    mainWindow->readSettings();
}

void Core::connectGui(MainWindow *mw) {
    mainWindow = mw;
    mainWindow->setCentralWidget(imageViewer);
    openDialog->setParent(mainWindow);
    imageViewer->setParent(mainWindow);
    connect(mainWindow, SIGNAL(signalOpenDialog()), this, SLOT(showOpenDialog()));
    connect(mainWindow, SIGNAL(signalSettingsDialog()), this, SLOT(showSettingsDialog()));
    connect(mainWindow, SIGNAL(signalNextImage()), this, SLOT(slotNextImage()));
    connect(mainWindow, SIGNAL(signalPrevImage()), this, SLOT(slotPrevImage()));
    connect(mainWindow, SIGNAL(signalFitAll()), imageViewer, SLOT(slotFitAll()));
    connect(mainWindow, SIGNAL(signalFitWidth()), imageViewer, SLOT(slotFitWidth()));
    connect(mainWindow, SIGNAL(signalFitNormal()), imageViewer, SLOT(slotFitNormal()));
    connect(mainWindow, SIGNAL(signalZoomIn()), imageViewer, SLOT(slotZoomIn()));
    connect(mainWindow, SIGNAL(signalZoomOut()), imageViewer, SLOT(slotZoomOut()));
    connect(imageViewer, SIGNAL(sendDoubleClick()), mainWindow, SLOT(slotTriggerFullscreen()));
    connect(mainWindow, SIGNAL(signalFullscreenEnabled(bool)), imageViewer, SLOT(slotShowControls(bool)));
    connect(mainWindow, SIGNAL(signalFullscreenEnabled(bool)), imageViewer, SLOT(slotShowInfo(bool)));
    connect(imageViewer->getControls(), SIGNAL(exitClicked()), mainWindow, SLOT(close()));
    connect(imageViewer->getControls(), SIGNAL(exitFullscreenClicked()), mainWindow, SLOT(slotTriggerFullscreen()));
    connect(imageViewer->getControls(), SIGNAL(minimizeClicked()), mainWindow, SLOT(slotMinimize()));
    connect(imageViewer->getControls(), SIGNAL(minimizeClicked()), mainWindow, SLOT(slotMinimize()));
    mainWindow->readSettings();
}

void Core::setInfoString() {
    Image *i = imageViewer->getImage();
    QString infoString = "";
    infoString.append(i->getName() + "  ");
    infoString.append("(" +
                      QString::number(i->width()) +
                      "x" +
                      QString::number(i->height()) +
                      ")  ");
    infoString.append("[ " +
                      QString::number(i->getInfo().getCurrentPos()) +
                      "/" +
                      QString::number(i->getInfo().getMaxPos()) +
                      " ]");
    imageViewer->slotSetInfoString(infoString);
    if(mainWindow != NULL) {
        infoString.append(" - ");
        infoString.append(QCoreApplication::applicationName());
        mainWindow->setWindowTitle(infoString);
    }
}

void Core::setCurrentDir(QString path) {
    dirManager->setCurrentDir(path);
}

void Core::showOpenDialog() {
    QString str = openDialog->getOpenFileName();
    if(!str.isEmpty()) {
        open(str);
    }
}

void Core::showSettingsDialog() {
    settingsDialog->show();
}

void Core::slotNextImage() {
    imageViewer->unsetImage();
    imageViewer->setImage(imgLoader->loadNext());
    globalSettings->s.setValue("lastPosition", dirManager->currentPosition);
}

void Core::slotPrevImage() {
    imageViewer->unsetImage();
    imageViewer->setImage(imgLoader->loadPrev());
    globalSettings->s.setValue("lastPosition", dirManager->currentPosition);
}

void Core::open(QString filePath) {
    imageViewer->unsetImage();
    imageViewer->setImage(imgLoader->load(filePath));
    globalSettings->s.setValue("lastPosition", dirManager->currentPosition);
}
