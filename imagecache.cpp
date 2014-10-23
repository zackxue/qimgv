#include "imagecache.h"

ImageCache::ImageCache() {
    maxCacheSize = 256; // MB
}

Image* ImageCache::findImagePointer(Image* image)
{
    for (int i = 0; i < cachedImages.size(); i++)
        if (cachedImages.at(i)->compare(image))
            return cachedImages.at(i);
    return NULL;
}

bool ImageCache::imageIsCached(Image* image)
{
    for (int i = 0; i < cachedImages.size(); i++)
        if (cachedImages.at(i)->compare(image)) {
            return true;
        }
    return false;
}

bool ImageCache::pushImage(Image* image)
{
    float imageMBytes = (float) image->ramSize();
    while (cacheSize() > maxCacheSize - imageMBytes && cacheSize() != 0)
    {
        qDebug() << "DEBUG: FILE- " << cachedImages.last()->getName() << " IN USE: " << cachedImages.last()->isInUse();
        if(!cachedImages.last()->isInUse()) {
            qDebug() << "CACHE: deleting " << cachedImages.last()->getName() << " from cache";
            delete cachedImages.last();
            cachedImages.removeLast();
        }
        else {
            break;
        }
    }
    if(cacheSize() <= maxCacheSize - imageMBytes || cachedImages.count() == 0) {
        cachedImages.push_front(image);
        qDebug() << "CACHE: image loaded - " << cachedImages.first()->getName();
        qDebug() << "CACHE: " << cacheSize() << "/" << maxCacheSize << " MB";
        return true;
    }
    else {
        qDebug() << "CACHE: image too big - " << cachedImages.first()->getName();
        return false;
    }
}

ImageCache::~ImageCache()
{

}

qint64 ImageCache::cacheSize() const
{
    qint64 size = 0;
    for (int i = 0; i < cachedImages.size(); i++)
        size += cachedImages.at(i)->ramSize();
    return size;
}
