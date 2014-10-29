#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include "directorymanager.h"
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
    explicit ImageLoader(DirectoryManager *);
    void load(QString path);
    void load(FileInfo* file);
    void loadNext();
    void loadPrev();
    void readSettings();
    void preload(FileInfo* path);

private:
    DirectoryManager *dm;
    ImageCache *cache;
    Image* load_thread(Image*& image);
    Image* currentImg;

    QMutex mutex, mutex2;
    void lock();
    void unlock();

    Image *getCurrentImg() const;
    void setCurrentImg(Image *value);

    bool isCurrent(Image *img);
signals:
    void loadFinished(Image*);
    void startPreload();

private slots:
    void preloadNearest();
    void preload_thread(Image*);
};

#endif // IMAGELOADER_H
