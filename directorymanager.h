#ifndef DIRECTORYMANAGER_H
#define DIRECTORYMANAGER_H

#include <QDir>
#include <QFile>
#include <QString>
#include <QSize>
#include <QDebug>
#include <QFileInfo>

enum fileType { NONE, STATIC, GIF };

class DirectoryManager
{
public:
    DirectoryManager();
    bool setFile(QString _path);
    void setFileDimensions(QSize);
    void changeCurrentDir(QString);
    void clearFileInfo();
    void next();
    void prev();
    QDir currentDir;
    QFileInfo fileInfo;
    QStringList fileList;
    QSize fileDimensions;
    fileType type;
    QString getInfo();
    double aspectRatio;
    int currentPosition; // -1 = default (no file open)

};

#endif // DIRECTORYMANAGER_H
