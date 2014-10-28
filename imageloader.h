#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include "imagecache.h"
#include "settings.h"
#include "image.h"
#include "directorymanager.h"
#include <QtConcurrent>
#include <time.h>
#include <QMutex>

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
    void preload_thread(FileInfo* path);
    bool writingCache;
    QMutex mutex;
    void lock();
    void unlock();

signals:

private slots:
    void preload(Image*);

};

#endif // IMAGELOADER_H
