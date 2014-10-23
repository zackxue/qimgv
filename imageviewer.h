#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QImageReader>
#include <QColor>
#include <QPalette>
#include <QTimer>
#include <QThread>
#include <QtConcurrent>
#include <QFuture>
#include <QDebug>
#include <QErrorMessage>
#include <vector>
#include "image.h"
#include "mapoverlay.h"
#include "infooverlay.h"
#include "controlsoverlay.h"
#include "settings.h"
#include <time.h>
#include "sleep.cpp"

#include <qgraphicseffect.h>
#define AVG(a,b)  ( ((((a)^(b)) & 0xfefefefeUL) >> 1) + ((a)&(b)) )

enum WindowResizePolicy
{
    NORMAL,
    WIDTH,
    ALL,
    FREE
};

class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    ImageViewer();
    ImageViewer(QWidget* parent);
    ~ImageViewer();
    void setImage(Image* image);
    Image* getImage() const;
    ControlsOverlay* getControls();
    bool isDisplaying();

    void centerVertical();
    void centerHorizontal();

    void scaleImage();
    float scale() const;
    bool scaled() const;
    void smoothScale();
    QImage halfSized(const QImage &source);

    Image* img;
    QTimer animationTimer;
    QImageReader imageReader;
    QImage image, imageScaled;
    QPoint cursorMovedDistance;
    QRect drawingRect;
    QSize shrinkSize;
    MapOverlay *mapOverlay;
    InfoOverlay *infoOverlay;
    ControlsOverlay *controlsOverlay;
    QFuture<void> scalerThread;
    ImageViewer* q;
    QMutex mutex;
    uint lock;

    WindowResizePolicy resizePolicy;

    int freeSpaceLeft;
    int freeSpaceBottom;
    int freeSpaceRight;
    int freeSpaceTop;

    bool isDisplayingFlag;

    static const float maxScale = 0.10; //fix this crap.
    static const float minScale = 3.0;
    static const float zoomStep = 0.1;

signals:
    void sendDoubleClick();
    void imageChanged();
    void scalingFinished();

public slots:
    void slotFitNormal();
    void slotFitWidth();
    void slotFitAll();
    void slotZoomIn();
    void slotZoomOut();
    void slotSetInfoString(QString);
    void slotShowInfo(bool);
    void slotShowControls(bool);

private slots:
    void onAnimation();
    void updateAfterScaling();

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);


private:
    void fitDefault();
    void fitHorizontal();
    void fitVertical();
    void fitOriginal();
    void fitWidth();
    void fitAll();
    void setScale(float scale);
    void centerImage();
    float currentScale;
};

#endif // IMAGEVIEWER_H
