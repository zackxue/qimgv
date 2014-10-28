#include "imageloader.h"

ImageLoader::ImageLoader(DirectoryManager *dm) {
    cache = new ImageCache();
    dirManager = dm;
    writingCache = false;
}

Image* ImageLoader::loadNext() {
    Image *img = new Image(dirManager->next());
    loadImage(img);
    preload_thread(dirManager->peekNext()); // move to thread later.. maybe
    return img;
}

Image* ImageLoader::loadPrev() {
    Image *img = new Image(dirManager->prev());
    loadImage(img);
    preload_thread(dirManager->peekPrev()); // move to thread
    //qDebug() << "#########################";
    return img;
}

Image* ImageLoader::load(QString path) {
    lock();
    Image *img = new Image(dirManager->setFile(path));
    loadImage(img);
    unlock();
    preload_thread(dirManager->peekNext()); // move to thread
    preload_thread(dirManager->peekPrev()); // move to thread
    //qDebug() << "#########################";
    return img;
}

void ImageLoader::preload_thread(FileInfo *file) {
    Image* img = new Image(file);
    QtConcurrent::run(this, &ImageLoader::preload, img);
}

void ImageLoader::lock()
{
    mutex.lock();
}

void ImageLoader::unlock()
{
    mutex.unlock();
}

void ImageLoader::preload(Image* img) {
    lock();
    cache->lock();
    if (!cache->imageIsCached(img))
    {
        //qDebug() << "LOADER: preloading file - " << img->getName();
        img->loadImage();
        if(!cache->cacheImageForced(img)) {
            delete img;
            img = NULL;
        }
        img->moveToThread(this->thread()); // important
    }
    cache->unlock();
    unlock();
}

void ImageLoader::loadImage(Image*& image)
{
    cache->lock();
    Image* found = cache->findImagePointer(image);
    if(!found) {
        image->loadImage();
        if(!cache->cacheImageForced(image)) {
        }
    }
    else {
        delete image;
        image = found;
    }
    image->setInUse(true);
    cache->unlock();
}

void ImageLoader::readSettings() {
    cache->applySettings();
}
