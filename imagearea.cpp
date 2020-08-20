#include <QLabel>
#include <QDebug>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include "imagearea.h"

ImageArea::ImageArea(QWidget* parent, bool isMain)
    : QWidget(parent)
    , m_label(nullptr)
    , m_percent(1.0f)
    , m_resizePercent(1.0f)
    , m_isMain(isMain)
{
    m_label = new QLabel(this);
    m_label->setScaledContents(true);

//    setWidget(m_label);

    QPalette palette;
    palette.setBrush(QPalette::Background, Qt::red);
    setAutoFillBackground(true);
    setPalette(palette);
}

QSize ImageArea::imageSize() const
{
    return m_image.size();
}

void ImageArea::loadImage(const QString &path)
{
    m_image = QImage(path);
    m_label->setPixmap(QPixmap::fromImage(m_image));
    m_label->resize(m_image.size());
    calcMinimumPercent();
}

void ImageArea::percentChanged(float p)
{
    m_percent = p;
    float f = p * m_resizePercent;
    int newWidth = qRound(m_image.width()*f);
    int newHeight = qRound(m_image.height()*f);

    int lap1 = newWidth - width();
    int lap2 = newHeight - height();
    if (lap1 < 0 || lap2 < 0) {
        if (lap1 < lap2) {
            int lap = qAbs(lap1);
            newHeight += ((float)m_image.height()/(float)m_image.width()) * lap;
            newWidth += lap;
        }
        else {
            int lap = qAbs(lap2);
            newWidth += ((float)m_image.width()/(float)m_image.height()) * lap;
            newHeight += lap;
        }
    }

    int offsetX = (m_label->width() - newWidth) / 2;
    int offsetY = (m_label->height() - newHeight) / 2;

    int x = m_label->x() + offsetX;
    int y = m_label->y() + offsetY;

    if (x > 0) {
        x = 0;
    }
    if (y > 0) {
        y = 0;
    }

    if (x + newWidth < width()) {
        x = width() - newWidth;
    }
    if (y + newHeight < height()) {
        y = height() - newHeight;
    }

    m_label->setGeometry(x, y, newWidth, newHeight);
}

QImage ImageArea::getClippedImage() const
{
    QPoint lefttop = m_label->mapFromParent(QPoint(0, 0));
    float f = m_percent * m_resizePercent;
    lefttop = QPoint(lefttop.x()/f, lefttop.y()/f);

//    if (m_isMain) {
//        if (lefttop.y() + 800/m_percent > m_image.height()
//            || lefttop.x() + 800/m_percent > m_image.width()) {
//           return QImage();
//        }
//    }
//    else {
//        if (lefttop.x() + 730/m_percent > m_image.width()
//            || lefttop.y() + height()/(m_percent*m_resizePercent) > m_image.height()) {
//            return QImage();
//        }
//    }

    QImage image;
    if (m_isMain) {
        image = m_image.copy(lefttop.x(), lefttop.y(), 800/m_percent, 800/m_percent);
        image = image.scaled(800, 800, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    else {
        int h = height() / m_resizePercent;
        image = m_image.copy(lefttop.x(), lefttop.y(), 730/m_percent, h/m_percent);
        image = image.scaled(730, h, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    return image;
}

void ImageArea::reset()
{
    m_label->setPixmap(QPixmap());
    m_label->setGeometry(0, 0, 0, 0);
}

void ImageArea::resizeEvent(QResizeEvent *)
{
    if (m_isMain) {
        m_resizePercent = float(width()) / 800.0f;
    }
    else {
        m_resizePercent = float(width()) / 730.0f;
    }
    percentChanged(m_percent);
    calcMinimumPercent();
}

void ImageArea::mousePressEvent(QMouseEvent *e)
{
    m_start = e->pos();
}

void ImageArea::mouseMoveEvent(QMouseEvent *e)
{
    int offX = e->x() - m_start.x();
    int offY = e->y() - m_start.y();
    m_start = e->pos();
    int x = m_label->x() + offX;
    int y = m_label->y() + offY;
    if (x > 0) {
        x = 0;
    }
    if (y > 0) {
        y = 0;
    }

    if (x + m_label->width() < width()) {
        x = width() - m_label->width();
    }
    if (y + m_label->height() < height()) {
        y = height() - m_label->height();
    }
    m_label->move(x, y);
}

void ImageArea::wheelEvent(QWheelEvent *e)
{
    if (e->delta() < 0) {
        emit decValue();
    }
    else {
        emit incValue();
    }
}

void ImageArea::calcMinimumPercent()
{
    float minF = 0.0f;
    if (m_image.width() > m_image.height()) {
        minF = (float)height()/(float)m_image.height()/m_resizePercent;
    }
    else {
        minF = (float)width()/(float)m_image.width()/m_resizePercent;
    }
    emit notifyMinimumPercent(minF);
}
