#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include "image.h"
#include "settings.h"
#include <QVector>
#include <QMutex>

class ImageCache
{
public:
    ImageCache();
    ~ImageCache();
    Image* findImagePointer(Image* image);
    bool imageIsCached(Image*);
    bool pushImage(Image*, bool);
    qint64 cacheSize() const;
    void applySettings();
    bool isFull();
    bool cacheImage(Image *image);
    bool cacheImageForced(Image *image);
    void lock();
    void unlock();
private:
    void shrinkTo(int);
    QVector<Image*> cachedImages;
    uint maxCacheSize;
    QMutex mutex;
    void readSettings();
};

#endif // IMAGECACHE_H
