#include "imagecache.h"

ImageCache::ImageCache() {
    applySettings();
}

Image* ImageCache::findImagePointer(Image* image)
{
    for (int i = 0; i < cachedImages.size(); i++)
        if (cachedImages.at(i)->compare(image)) {
            Image* tmp = cachedImages.at(i);
            return tmp;
        }
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
    if(forced || (!forced && cacheSize() <= maxCacheSize - imageMBytes ||
                  cachedImages.count() == 0)) {
        cachedImages.push_front(image);
        //qDebug() << "CACHE: image cached - " << image->getName();
        return true;
    }
    else {
        //qDebug() << "CACHE: image too big - " << image->getName();
        return false;
    }
}

void ImageCache::readSettings() {
    maxCacheSize = globalSettings->s.value("cacheSize").toInt();
    if(maxCacheSize < 32) {
        maxCacheSize = 32;
        globalSettings->s.setValue("cacheSize","32");
    }
}

void ImageCache::applySettings() {
    readSettings();
    shrinkTo(maxCacheSize);
}

// delete images until cache size is less than MB
void ImageCache::shrinkTo(int MB) {
    while (cacheSize() > MB && cachedImages.length() > 1) // wipes previous
    //while (cacheSize() > MB && cachedImages.length() > 2) // leaves previous
    {
        if(!cachedImages.last()->isInUse()) {
            delete cachedImages.last();
            cachedImages.removeLast();
        }
        else {
            break;
        }
    }

    while (cacheSize() > MB && cachedImages.length() > 1) // wipes previous
    //while (cacheSize() > MB && cachedImages.length() > 2) // leaves previous
    {
        if(!cachedImages.first()->isInUse()) {
            delete cachedImages.first();
            cachedImages.removeFirst();
        }
        else {
            break;
        }
    }
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
