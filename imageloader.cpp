#include "imageloader.h"

ImageLoader::ImageLoader(DirectoryManager *dm) {
    cache = new ImageCache();
    dirManager = dm;
    notCached = NULL;
    preloading = 0;
}

Image* ImageLoader::loadNext() {
    dirManager->next();
    Image *img = new Image(dirManager->getFile());
    loadImage(img);
    preload_thread(dirManager->peekNext()); // move to thread later.. maybe
    return img;
}

Image* ImageLoader::loadPrev() {
    dirManager->prev();
    Image *img = new Image(dirManager->getFile());
    loadImage(img);
    preload_thread(dirManager->peekPrev()); // move to thread
    return img;
}

Image* ImageLoader::load(QString file) {
    while(preloading>0) { qDebug() << "LOADER: load - waiting"; }
    dirManager->setFile(file);
    Image *img = new Image(dirManager->getFile());
    loadImage(img);
    preload_thread(dirManager->peekNext()); // move to thread
    preload_thread(dirManager->peekPrev()); // move to thread
    return img;
}

void ImageLoader::preload_thread(FileInfo file) {
    QtConcurrent::run(this, &ImageLoader::preload, file);
}

void ImageLoader::preload(FileInfo info) {
    while(preloading>0) { qDebug() << "LOADER: preload - waiting"; }

    Image *img = new Image(info);
    if (!cache->imageIsCached(img))
    {
        preloading++;
        qDebug() << "LOADER: preloading file - " << info.getName();
        img->loadImage();
        if(!cache->pushImage(img)) {
            delete img;
            img = NULL;
        }
        preloading--;
    }

}

void ImageLoader::loadImage(Image*& image)
{
    qDebug() << "LOADER: opening " << image->getName();
    Image* found = cache->findImagePointer(image);
    if(!found) {
        image->loadImage();
        if(!cache->pushImage(image)) {
            deleteLastImage();
            notCached = image;
            qDebug() << "LOADER: image not found, loading";
        }
    }
    else {
        delete image;
        image = found;
        qDebug() << "LOADER: already cached - " << image->getName();
    }
    image->setInUse(true);
}

void ImageLoader::deleteLastImage() {
    if(notCached && !notCached->isInUse()) {
        delete notCached;
        notCached = NULL;
    }
}

void ImageLoader::readSettings() {
    cache->readSettings();
}
