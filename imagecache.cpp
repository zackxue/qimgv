#include "imagecache.h"

ImageCache::ImageCache() {
    maxCacheSize = 256; // MB
}

Image* ImageCache::findImagePointer(Image* image)
{
    lock();
    for (int i = 0; i < cachedImages.size(); i++)
        if (cachedImages.at(i)->compare(image)) {
            Image* tmp = cachedImages.at(i);
            unlock();
            return tmp;
        }
    unlock();
    return NULL;
}

bool ImageCache::imageIsCached(Image* image)
{
    lock();
    for (int i = 0; i < cachedImages.size(); i++)
        if (cachedImages.at(i)->compare(image)) {
            unlock();
            return true;
        }
    unlock();
    return false;
}

bool ImageCache::isFull() {
    if(cacheSize()>maxCacheSize) {
        return true;
    }
    else return false;
}

bool ImageCache::cacheImage(Image* image) {
    return pushImage(image, false);
}

bool ImageCache::cacheImageForced(Image* image) {
    return pushImage(image, true);
}

bool ImageCache::pushImage(Image* image, bool forced) {
    float imageMBytes = (float) image->ramSize();
    shrinkTo(maxCacheSize - imageMBytes);
    lock();
    if(forced || (!forced && cacheSize() <= maxCacheSize - imageMBytes || cachedImages.count() == 0)) {
        cachedImages.push_front(image);
        unlock();
        //qDebug() << "CACHE: image cached - " << image->getName();
        return true;
    }
    else {
        unlock();
        //qDebug() << "CACHE: image too big - " << image->getName();
        return false;
    }
}

void ImageCache::readSettings() {
    maxCacheSize = globalSettings->s.value("cacheSize").toInt();
    if(maxCacheSize < 32) {
        maxCacheSize = 32;
        globalSettings->s.setValue("cacheSize","64");
    }
    shrinkTo(maxCacheSize);
}

// delete images until cache size is less than MB
void ImageCache::shrinkTo(int MB) {
    lock();
    while (cacheSize() > MB && cachedImages.length() > 1)
    {
        if(!cachedImages.last()->isInUse()) {
            delete cachedImages.last();
            cachedImages.removeLast();
        }
        else {
            break;
        }
    }

    while (cacheSize() > MB && cachedImages.length() > 1)
    {
        if(!cachedImages.first()->isInUse()) {
            delete cachedImages.first();
            cachedImages.removeFirst();
        }
        else {
            break;
        }
    }
    unlock();
}

void ImageCache::lock() {
    mutex.lock();
}

void ImageCache::unlock() {
    mutex.unlock();
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
