#include "imageloader.h"

ImageLoader::ImageLoader() {
    cache = new ImageCache();
}

//thread-safe
Image* ImageLoader::load(FileInfo* file) {
    Image *img = new Image(file);
    QFuture<Image*> future = QtConcurrent::run(this,
                                               &ImageLoader::load_thread,
                                               img);
    return future.result();
}

Image* ImageLoader::load_thread(Image*& img)
{
    lock();
    Image* found = cache->findImage(img);
    if(!found) {
        img->loadImage();
        cache->cacheImageForced(img);
    }
    else {
        delete img;
        img = found;
    }
    img->setInUse(true);
    img->moveToThread(this->thread()); // important
    unlock();
    return img;
}

//thread-safe
void ImageLoader::preload(FileInfo *file) {
    Image* img = new Image(file);
    QtConcurrent::run(this, &ImageLoader::preload_thread, img);
}

void ImageLoader::preload_thread(Image* img) {
    lock();
    if (!cache->findImage(img))
    {
        img->loadImage();
        if(!cache->cacheImageForced(img)) {
            delete img;
            img = NULL;
        }
        else {
            img->moveToThread(this->thread()); // important
        }
    }
    unlock();
}

void ImageLoader::readSettings() {
    cache->applySettings();
}

void ImageLoader::lock()
{
    mutex.lock();
}

void ImageLoader::unlock()
{
    mutex.unlock();
}
