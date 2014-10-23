#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include "image.h"
#include "settings.h"
#include <QVector>

class ImageCache
{
public:
    ImageCache();
    ~ImageCache();
    Image* findImagePointer(Image* image);
    bool imageIsCached(Image*);
    bool pushImage(Image* image);
    qint64 cacheSize() const;
    void readSettings();
    bool isFull();
private:
    void shrinkTo(int);
    QVector<Image*> cachedImages;
    uint maxCacheSize;
};

#endif // IMAGECACHE_H
