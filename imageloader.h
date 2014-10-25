#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include "imagecache.h"
#include "settings.h"
#include "image.h"
#include "directorymanager.h"
#include <QtConcurrent>
// #include "scrollarea.h"

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(DirectoryManager*);
    Image* loadNext();
    Image* loadPrev();
    Image* load(QString file);

    DirectoryManager *dirManager;
    void readSettings();

private:
    ImageCache *cache;
    void loadImage(Image*& image);
    Image* notCached;
    int preloading;

    void preload_thread(FileInfo path);
signals:

private slots:
    void preload(FileInfo);

public slots:
    void deleteLastImage();

};

#endif // IMAGELOADER_H
