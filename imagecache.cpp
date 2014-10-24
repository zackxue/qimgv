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

bool ImageCache::isFull() {
    if(cacheSize()>maxCacheSize) {
        return true;
    }
    else return false;
}

bool ImageCache::pushImage(Image* image)
{
    float imageMBytes = (float) image->ramSize();
    shrinkTo(maxCacheSize - imageMBytes);
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

void ImageCache::readSettings() {
    maxCacheSize = globalSettings->s.value("cacheSize").toInt();
    if(maxCacheSize < 64) {
        maxCacheSize = 64;
        globalSettings->s.setValue("cacheSize","64");
    }
    shrinkTo(maxCacheSize);
}

// delete images until cache size is less than MB
void ImageCache::shrinkTo(int MB) {
    while (cacheSize() > MB && cacheSize() != 0)
    {
        if(!cachedImages.last()->isInUse()) {
            qDebug() << "CACHE: deleting " <<
                        cachedImages.last()->getName() <<
                        ";  new size:" <<
                        cacheSize();
            delete cachedImages.last();
            cachedImages.removeLast();
        }
        else {
            break;
        }
    }

    while (cacheSize() > MB && cacheSize() != 0)
    {
        if(!cachedImages.first()->isInUse()) {
            qDebug() << "CACHE: deleting " <<
                        cachedImages.first()->getName() <<
                        ";  new size:" <<
                        cacheSize();
            delete cachedImages.first();
            cachedImages.removeFirst();
        }
        else {
            break;
        }
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
