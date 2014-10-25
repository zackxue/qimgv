#include "directorymanager.h"

DirectoryManager::DirectoryManager() :
    currentPosition(-1),
    fileInfo(new FileInfo())
{
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp";
    QString startDir = globalSettings->s.value("lastDir", currentDir.homePath()).toString();
    setCurrentDir(startDir);
    currentDir.setNameFilters(filters);
}

void DirectoryManager::setCurrentDir(QString path) {
    if(currentDir.exists()) {
        if(currentDir.path() != path) {
            changePath(path);
        }
    }
    else changePath(path);
}

void DirectoryManager::changePath(QString path) {
    currentDir.setPath(path);
    if(currentDir.isReadable())
        globalSettings->s.setValue("lastDir", path);
    currentDir.setNameFilters(filters);
    currentPosition = -1;
    fileList = currentDir.entryList();
    emit directoryChanged(path);
}

void DirectoryManager::next() {
    if(fileList.length()) {
        if(++currentPosition>=fileList.length()) {
            currentPosition=0;
        }
        QString fileName = currentDir.path()
                        +"/"
                        +fileList.at(currentPosition);
        loadFileInfo(fileName);
        setFilePositions();
    }
}

void DirectoryManager::prev() {
    if(fileList.length()) {
        if(--currentPosition<0) {
            currentPosition=fileList.length()-1;
        }
        QString fileName = currentDir.path()
                        +"/"
                        +fileList.at(currentPosition);
        loadFileInfo(fileName);
        setFilePositions();
    }
}

void DirectoryManager::setFilePositions() {
    if(fileInfo) {
        fileInfo->setPositions(currentPosition+1, fileList.length());
    }
}

FileInfo DirectoryManager::peekPrev() {
    FileInfo tmp;
    prev();
    tmp=getFile();
    next();
    return tmp;
}

FileInfo DirectoryManager::peekNext() {
    FileInfo tmp;
    next();
    tmp=getFile();
    prev();
    return tmp;
}

void DirectoryManager::loadFileInfo(QString path) {
    if(fileInfo != NULL && !fileInfo->inUse) {
        delete fileInfo;
    }
    fileInfo = NULL;
    fileInfo = new FileInfo(&path);
}

FileInfo DirectoryManager::setFile(QString path) {
    loadFileInfo(path);
    setCurrentDir(fileInfo->getDirPath());
    currentPosition = fileList.indexOf(fileInfo->getName());
    setFilePositions();
    return getFile();
}

FileInfo DirectoryManager::getFile() {
    return *fileInfo;
}
