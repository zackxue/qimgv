#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include "imagecache.h"
#include "settings.h"
#include "image.h"
#include <QtConcurrent>
#include <time.h>
#include <QMutex>

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader();
    Image* load(FileInfo* file);
    void preload(FileInfo* path);
    void readSettings();

private:
    ImageCache *cache;
    Image* load_thread(Image*& image);

    QMutex mutex;
    void lock();
    void unlock();

signals:

private slots:
    void preload_thread(Image*);

};

#endif // IMAGELOADER_H
