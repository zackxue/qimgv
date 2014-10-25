#include "imageviewer.h"

ImageViewer::ImageViewer(): QWidget(),
    shrinkSize(),
    currentScale(1.0),
    resizePolicy(NORMAL),
    img(NULL),
    isDisplayingFlag(false),
    lock(0)
{
    initOverlays();
    image.load(":/images/res/logo.png");
    drawingRect = image.rect();
    fitDefault();
}

ImageViewer::ImageViewer(QWidget* parent): QWidget(parent),
    shrinkSize(),
    currentScale(1.0),
    resizePolicy(NORMAL),
    img(NULL),
    isDisplayingFlag(false),
    lock(0)
{
    initOverlays();
    image.load(":/images/res/logo.png");
    drawingRect = image.rect();
}

ImageViewer::~ImageViewer() {
    delete img;
}

void ImageViewer::initOverlays() {
    infoOverlay = new InfoOverlay(this);
    mapOverlay = new MapOverlay(this);
    controlsOverlay = new ControlsOverlay(this);
    infoOverlay->hide();
    controlsOverlay->hide();
}

bool ImageViewer::scaled() const {
    return scale() != 1.0;
}

void ImageViewer::stopAnimation() {
    if(img->getType()==GIF) {
        img->getMovie()->stop();
        disconnect(img->getMovie(), SIGNAL(frameChanged(int)), this, SLOT(onAnimation()));
    }
}

void ImageViewer::startAnimation() {
    connect(img->getMovie(), SIGNAL(frameChanged(int)), this, SLOT(onAnimation()));
    img->getMovie()->start();
}

void ImageViewer::setImage(Image* i) {
    if (img!=NULL) {
        stopAnimation();
        //mark previous image as unused
        img->setInUse(false);
    }

    if(i->getType()==NONE) {
        //empty or corrupted image
        image.load(":/images/res/error.png");
        isDisplayingFlag = false;
    }
    else {
        isDisplayingFlag = true;
        img = i;
        if(img->getType() == STATIC) {
            image = *img->getImage();

        }
        else if (img->getType() == GIF) {
            img->getMovie()->jumpToFrame(0);
            image = img->getMovie()->currentImage();
            maxScale = defaultMaxSale;
            startAnimation();
            qDebug() << "startAnimation";
        }
        calculateMaxScale();
        emit imageChanged();
    }
    drawingRect = image.rect();
    currentScale = 1.0;
    if(resizePolicy == FREE)
        resizePolicy = NORMAL;
    fitDefault();
}

void ImageViewer::calculateMaxScale() {
    float newMaxScaleX = (float)width()/image.width();
    float newMaxScaleY = (float)height()/image.height();
    if(newMaxScaleX < newMaxScaleY) {
        maxScale = newMaxScaleX;
    }
    else {
        maxScale = newMaxScaleY;
    }
    if(maxScale > defaultMaxSale)
        maxScale = defaultMaxSale;
}

float ImageViewer::scale() const {
    return currentScale;
}

void ImageViewer::setScale(float scale) {
    if(currentScale!=scale) {
        if (scale >= minScale) {
            currentScale = minScale;
        }
        else if(scale <= maxScale) {
            currentScale = maxScale;
            //resizePolicy = ALL;
        }
        else {
            currentScale = scale;
        }
        QSize sz = image.size();
        sz = sz.scaled(sz * scale, Qt::KeepAspectRatio);
        drawingRect.setSize(sz);
    }
}

void ImageViewer::scaleImage() {
    update();
}

void ImageViewer::onAnimation() {
    image = img->getMovie()->currentImage();
    update();
}

// ##############################################
void ImageViewer::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.fillRect(rect(), QBrush(QColor(0, 0, 0)));
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    //int time = clock();
    painter.drawImage(drawingRect, image);
    //qDebug() << "VIEWER: draw time: " << clock() - time;
}
// ##############################################

void ImageViewer::mousePressEvent(QMouseEvent* event) {
    moveStartPos = event->pos();
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
    }
    if (event->button() == Qt::RightButton) {
        this->setCursor(QCursor(Qt::SizeVerCursor));
        zoomPoint = event->pos();
    }
}

void ImageViewer::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        if(drawingRect.size().width() > this->width() ||
           drawingRect.size().height() > this->height())
        {
            moveStartPos -= event->pos();
            int left = drawingRect.x() - moveStartPos.x();
            int top = drawingRect.y() - moveStartPos.y();
            int right = left + drawingRect.width();
            int bottom = top + drawingRect.height();
            if (left <= 0 && right > size().width())
                drawingRect.moveLeft(left);
            if (top <= 0 && bottom > size().height())
                drawingRect.moveTop(top);
            moveStartPos = event->pos();
            update();
            updateMap();
        }
    }
    if (event->buttons() & Qt::RightButton && isDisplayingFlag) {
        float step = (maxScale - minScale) / -300.0;
        qDebug() << maxScale;
        int currentPos = event->pos().y();
        int moveDistance = moveStartPos.y() - currentPos;
        float newScale = currentScale + step*(moveDistance);
        if(moveDistance > 0 && newScale > minScale) {
                newScale = minScale;
        }
        if(moveDistance < 0 && newScale < maxScale-FLT_EPSILON) {
                newScale = maxScale;
        }
        if(newScale == currentScale) {
            return;
        }
        resizePolicy = FREE;
        scaleAround(zoomPoint, newScale);
        moveStartPos = event->pos();
    }
}

void ImageViewer::mouseReleaseEvent(QMouseEvent* event) {
    moveStartPos = event->pos();
    zoomPoint = event->pos();
    this->setCursor(QCursor(Qt::ArrowCursor));
}

void ImageViewer::fitWidth() {
    if(isDisplayingFlag) {
        float scale = (float) width() / image.width();
        setScale(scale);
        imageAlign();
        if(drawingRect.height()>height())
            drawingRect.moveTop(0);
        update();
    }
    else {
        centerImage();
    }
}

void ImageViewer::fitAll() {
    if(isDisplayingFlag) {
        bool h = image.height() <= this->height();
        bool w = image.width() <= this->width();
        if(h && w) {
            fitNormal();
        }
        else {
            float widgetAspect = (float) height() / width();
            float drawingAspect = (float) drawingRect.height() /
                                          drawingRect.width();
            if(widgetAspect > drawingAspect) {
                float scale = (float) width() / image.width();
                setScale(scale);
            }
            else {
                float scale = (float) height() / image.height();
                setScale(scale);
            }
            if(scaled()) {
                drawingRect.moveCenter(rect().center());
                update();
            }
        }
    }
    else {
        centerImage();
    }
}

void ImageViewer::fitNormal() {
   setScale(1.0);
   centerImage();
   update();
}

void ImageViewer::fitDefault() {
    switch(resizePolicy) {
        case NORMAL: fitNormal(); break;
        case WIDTH: fitWidth(); break;
        case ALL: fitAll(); break;
        default: fitNormal(); break;
    }
    updateMap();
}

void ImageViewer::updateMap() {
    mapOverlay->updateMap(size(), drawingRect);
}

void ImageViewer::centerImage() {
    drawingRect.moveCenter(rect().center());
    imageAlign();
}

void ImageViewer::slotFitNormal() {
    resizePolicy = NORMAL;
    fitDefault();
}

void ImageViewer::slotFitWidth() {
    resizePolicy = WIDTH;
    fitDefault();
}

void ImageViewer::slotFitAll() {
    resizePolicy = ALL;
    fitDefault();
}

void ImageViewer::resizeEvent(QResizeEvent* event) {
    resize(event->size());
    calculateMaxScale();
    if(resizePolicy == FREE || resizePolicy == NORMAL) {
        imageAlign();
    }
    else {
        fitDefault();
    }
    mapOverlay->updateMap(size(),drawingRect);
    controlsOverlay->updateSize();
    mapOverlay->updatePosition();
}

void ImageViewer::mouseDoubleClickEvent(QMouseEvent *event) {
    if(event->button() == Qt::RightButton) {
        slotFitNormal();
    }
    else {
        emit sendDoubleClick();
    }
}

// #####################################
void ImageViewer::imageAlign() {
    if(drawingRect.height() <= height()) {
        drawingRect.moveTop((height()-drawingRect.height())/2);
    }
    else {
        fixAlignVertical();
    }
    if(drawingRect.width() <= width()) {
        drawingRect.moveLeft((width()-drawingRect.width())/2);
    }
    else {
        fixAlignHorizontal();
    }
}

void ImageViewer::fixAlignHorizontal() {
    if(drawingRect.x()>0 && drawingRect.right()>width()) {
        drawingRect.moveLeft(0);
    }
    if(width()-drawingRect.x()>drawingRect.width()) {
        drawingRect.moveRight(width());
    }
}

void ImageViewer::fixAlignVertical() {
    if(drawingRect.y()>0 && drawingRect.bottom()>height()) {
        drawingRect.moveTop(0);
    }
    if(height()-drawingRect.y()>drawingRect.height()) {
        drawingRect.moveBottom(height());
    }
}

void ImageViewer::scaleAround(QPointF p, float newScale) {
    float xPos = (float)(p.x()-drawingRect.x())/drawingRect.width();
    float oldPx = (float)xPos*drawingRect.width();
    float oldX = drawingRect.x();
    float yPos = (float)(p.y()-drawingRect.y())/drawingRect.height();
    float oldPy = (float)yPos*drawingRect.height();
    float oldY = drawingRect.y();
    setScale(newScale);
    float newPx = (float)xPos*drawingRect.width();
    drawingRect.moveLeft(oldX - (newPx-oldPx));
    float newPy = (float)yPos*drawingRect.height();
    drawingRect.moveTop(oldY - (newPy-oldPy));
    imageAlign();
    update();
    updateMap();
}
// #####################################

void ImageViewer::slotZoomIn() {
    if(isDisplayingFlag) {
        float newScale = scale() + zoomStep;
        if(newScale > minScale ||
                newScale == currentScale) //skip if minScale
            return;
        resizePolicy = FREE;
        zoomPoint = rect().center();
        scaleAround(zoomPoint, newScale);
    }
}

void ImageViewer::slotZoomOut() {
    if(isDisplayingFlag) {
        float newScale = scale() - zoomStep;
        if(newScale < maxScale-FLT_EPSILON ||
                newScale == currentScale) //skip if maxScale
            return;
        resizePolicy = FREE;
        zoomPoint = rect().center();
        scaleAround(zoomPoint, newScale);
    }
}

Image* ImageViewer::getImage() const {
    return img;
}

void ImageViewer::slotSetInfoString(QString info) {
    infoOverlay->setText(info);
}

void ImageViewer::slotShowInfo(bool x) {
    x?infoOverlay->show():infoOverlay->hide();
}

void ImageViewer::slotShowControls(bool x) {
    x?controlsOverlay->show():controlsOverlay->hide();
}

ControlsOverlay* ImageViewer::getControls() {
    return controlsOverlay;
}

bool ImageViewer::isDisplaying() {
    return isDisplayingFlag;
}
