#include "imageviewer.h"

ImageViewer::ImageViewer(): QWidget(),
    shrinkSize(),
    currentScale(1.0),
    resizePolicy(NORMAL),
    img(NULL),
    isDisplayingFlag(false),
    lock(0)
{
    infoOverlay = new InfoOverlay(this);
    mapOverlay = new MapOverlay(this);
    controlsOverlay = new ControlsOverlay(this);
    infoOverlay->hide();
    controlsOverlay->hide();
    connect(this, SIGNAL(scalingFinished()), this, SLOT(updateAfterScaling()));

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
    infoOverlay = new InfoOverlay(this);
    mapOverlay = new MapOverlay(this);
    controlsOverlay = new ControlsOverlay(this);
    infoOverlay->hide();
    controlsOverlay->hide();
    connect(this, SIGNAL(scalingFinished()), this, SLOT(updateAfterScaling()));

    image.load(":/images/res/logo.png");
    drawingRect = image.rect();
}

ImageViewer::~ImageViewer()
{
    delete img;
}

bool ImageViewer::scaled() const
{
    return scale() != 1.0;
}

void ImageViewer::setImage(Image* i) {
    if (img!=NULL) {
        if(img->getType()==GIF) {
            img->getMovie()->stop();
            disconnect(img->getMovie(), SIGNAL(frameChanged(int)), this, SLOT(onAnimation()));
        }
        img->setInUse(false); //bye-bye
        //delete img;
        //img = NULL;
    }

    if(i->getType()==NONE) {
        //empty or corrupted image
        image.load(":/images/res/error.png");
        isDisplayingFlag = false;
    }
    else {
        //ok, proceeding
        isDisplayingFlag = true;
        img = i;
        if(img->getType() == STATIC) {
            image = *img->getImage();
        }
        else if (img->getType() == GIF) {
            img->getMovie()->jumpToFrame(0);
            image = img->getMovie()->currentImage();
            connect(img->getMovie(), SIGNAL(frameChanged(int)), this, SLOT(onAnimation()));
            img->getMovie()->start();
        }
        emit imageChanged();
    }
    drawingRect = image.rect();
    if(resizePolicy == FREE)
        resizePolicy = NORMAL;
    fitDefault();
}

float ImageViewer::scale() const
{
    return currentScale;
}

void ImageViewer::setScale(float scale)
{
    currentScale = scale;
    QSize sz;

    sz = image.size().scaled(image.size() * scale, Qt::KeepAspectRatio); // for qt's
    drawingRect.setSize(sz);
}

void ImageViewer::smoothScale() {
    lock++;

    //Sleeper::msleep(100); //unicorn magic. prevents some bad things
    if(lock == 1) {
        imageScaled = image.scaled(drawingRect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation); //SLOW
        //update(); // nope
        emit scalingFinished();

    }
    lock--;
}

void ImageViewer::updateAfterScaling() {
    //qDebug() << "VIEWER: update after scale";
    update();
}

/*QImage ImageViewer::halfSized(const QImage &source)
{
    QImage dest(source.size() * 0.5, QImage::Format_ARGB32_Premultiplied);

    const quint32 *src = reinterpret_cast<const quint32*>(source.bits());
    int sx = source.bytesPerLine() >> 2;
    int sx2 = sx << 1;

    quint32 *dst = reinterpret_cast<quint32*>(dest.bits());
    int dx = dest.bytesPerLine() >> 2;
    int ww = dest.width();
    int hh = dest.height();

    for (int y = hh; y; --y, dst += dx, src += sx2) {
        const quint32 *p1 = src;
        const quint32 *p2 = src + sx;
        quint32 *q = dst;
        for (int x = ww; x; --x, q++, p1 += 2, p2 += 2)
            * q = AVG(AVG(p1[0], p1[1]), AVG(p2[0], p2[1]));
    }
    return dest;
}
*/

void ImageViewer::scaleImage()
{
    if(scaled() && scale() <= 1.5) {
        imageScaled = image.scaled(drawingRect.size(), Qt::IgnoreAspectRatio);
        //smoothing + gif = lags
        if(isDisplayingFlag && img->getType() == STATIC)
            QFuture<void> t1 = QtConcurrent::run(this, &ImageViewer::smoothScale);
    }
    update();
}

void ImageViewer::centerHorizontal()
{
    QPoint point((width() - drawingRect.width()) / 2, drawingRect.y());
    drawingRect.moveTo(point);
}

void ImageViewer::centerVertical()
{
    QPoint point(drawingRect.x(), (height() - drawingRect.height()) / 2);
    drawingRect.moveTo(point);
}

void ImageViewer::onAnimation()
{
    image = img->getMovie()->currentImage();
    scaleImage();
    update();
}

void ImageViewer::paintEvent(QPaintEvent* event)
{
    QColor bgColor = QColor(0,0,0,255);
    QPainter painter(this);
    painter.setPen(bgColor);
    painter.setBrush(Qt::SolidPattern);
    painter.drawRect(QRect(0,0,this->width(),this->height()));

    int time = clock();
    if(scaled() && scale() <= 1.5) {
        //qDebug() << drawingRect << " <_> " << imageScaled.size();
        painter.drawImage(drawingRect, imageScaled);
    }
    else {
        painter.drawImage(drawingRect, image);
    }
    //qDebug() << "draw time: " << clock() - time;
}

void ImageViewer::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
        cursorMovedDistance = event->pos();
    }
}

void ImageViewer::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        if(drawingRect.size().width() > this->width() ||
           drawingRect.size().height() > this->height()) {
           // qDebug() << drawingRect;
           // qDebug() << this->size();
            cursorMovedDistance -= event->pos();
            int left = drawingRect.x() - cursorMovedDistance.x();
            int top = drawingRect.y() - cursorMovedDistance.y();
            int right = left + drawingRect.width();
            int bottom = top + drawingRect.height();

            if (left < 0 && right > size().width())
                drawingRect.moveLeft(left);

            if (top < 0 && bottom > size().height())
                drawingRect.moveTop(top);

            cursorMovedDistance = event->pos();
            update();
            mapOverlay->updateMap(size(),drawingRect);
        }
    }
}

void ImageViewer::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ArrowCursor));
        cursorMovedDistance = event->pos();
    }
}

void ImageViewer::fitWidth()
{
    float scale = (float) width() / image.width();
    drawingRect.setX(0);
    setScale(scale);
    if(drawingRect.height()<=height()) {
        QPoint point(0, (height() - drawingRect.height()) / 2);
        drawingRect.moveTo(point);
    }
    else
        drawingRect.moveTop(0);
    scaleImage();
}

void ImageViewer::fitHorizontal()
{
    float scale = (float) width() / image.width();
    drawingRect.setX(0);
    setScale(scale);
    centerVertical();
}

void ImageViewer::fitVertical()
{
    float scale = (float) height() / image.height();
    drawingRect.setY(0);
    setScale(scale);
    centerHorizontal();
}

void ImageViewer::fitAll()
{
    bool h = image.height() <= this->height();
    bool w = image.width() <= this->width();
    if(h && w) {
        fitOriginal();
    }
    else {
        QSize oldSize = drawingRect.size();
        float widgetAspect = (float) height() / width();
        float drawingAspect = (float) drawingRect.height() /
                drawingRect.width();
        if(widgetAspect < drawingAspect)
            fitVertical();
        else
            fitHorizontal();
        if(scaled() && oldSize != drawingRect.size()) {
            scaleImage();
        }
    }
}

void ImageViewer::fitOriginal()
{
   setScale(1.0);
   centerImage();
   update();
}

void ImageViewer::fitDefault() {
    switch(resizePolicy) {
        case NORMAL: fitOriginal(); break;
        case WIDTH: fitWidth(); break;
        case ALL: fitAll(); break;
        default: centerImage(); break;
    }
    mapOverlay->updateMap(size(),drawingRect);
}

void ImageViewer::centerImage() {
    int left = drawingRect.left()<0?0:drawingRect.left();
    int right = drawingRect.right()<0?0:drawingRect.right();
    int top = drawingRect.top()<0?0:drawingRect.top();
    int bottom = drawingRect.bottom()<0?0:drawingRect.bottom();
    int spaceLeft = left - rect().left();
    int spaceTop = top - rect().top();
    int spaceRight = rect().right() - right;
    int spaceBottom = rect().bottom() - bottom;

    if (spaceLeft < 0 && spaceRight > 0)
        drawingRect.translate(spaceRight, 0);
    else if (spaceLeft > 0 || spaceRight > 0)
        centerHorizontal();

    if (spaceTop < 0 && spaceBottom > 0)
        drawingRect.translate(0, spaceBottom);
    else if (spaceTop > 0 || spaceBottom > 0)
        centerVertical();
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

void ImageViewer::resizeEvent(QResizeEvent* event)
{
    resize(event->size());
    fitDefault();
    mapOverlay->updateMap(size(),drawingRect);
    controlsOverlay->updateSize();
    mapOverlay->updatePosition();
}

void ImageViewer::mouseDoubleClickEvent(QMouseEvent *event) {
    emit sendDoubleClick();
}

void ImageViewer::slotZoomIn() {
    resizePolicy = FREE;
    if(isDisplayingFlag) {
        if(scale() == minScale)
            return;
        float possibleScale = scale() + zoomStep;
        if (possibleScale <= minScale) {
            setScale(possibleScale);
        }
        else {
            setScale(minScale);
        }
        centerHorizontal();
        centerVertical();
        scaleImage();
        mapOverlay->updateMap(size(), drawingRect);
    }
}

void ImageViewer::slotZoomOut() {
    resizePolicy = FREE;
    if(isDisplayingFlag) {
        if(scale() == maxScale)
            return;
        float possibleScale = scale() - zoomStep;
        if (possibleScale >= maxScale) {
            setScale(possibleScale);
        }
        else {
            setScale(maxScale);
        }
        centerHorizontal();
        centerVertical();
        scaleImage();
        mapOverlay->updateMap(size(), drawingRect);
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
