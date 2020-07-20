#include <QLabel>
#include <QDebug>
#include <QMouseEvent>
#include <QScrollBar>
#include "imagearea.h"

ImageArea::ImageArea(QWidget* parent, bool isMain)
    : QScrollArea(parent)
    , m_label(nullptr)
    , m_percent(1.0f)
    , m_resizePercent(1.0f)
    , m_isMain(isMain)
{
    m_label = new QLabel(this);
    m_label->setScaledContents(true);

    setWidget(m_label);

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
}

void ImageArea::percentChanged(float p)
{
    m_percent = p;
    float f = p * m_resizePercent;
    int newWidth = m_image.width()*f;
    int newHeight = m_image.height()*f;

    int offsetX = (m_label->width() - newWidth) / 2;
    int offsetY = (m_label->height() - newHeight) / 2;

    m_label->setGeometry(m_label->x() + offsetX, m_label->y() + offsetY, newWidth, newHeight);
}

QImage ImageArea::getClippedImage() const
{
    QPoint lefttop = m_label->mapFromParent(QPoint(0, 0));
    float f = m_percent * m_resizePercent;
    lefttop = QPoint(lefttop.x()/f, lefttop.y()/f);

    if (m_isMain) {
        if (lefttop.y() + 800/m_percent > m_image.height()
            || lefttop.x() + 800/m_percent > m_image.width()) {
           return QImage();
        }
    }
    else {
        if (lefttop.x() + 730/m_percent > m_image.width()
            || lefttop.y() + height()/(m_percent*m_resizePercent) > m_image.height()) {
            return QImage();
        }
    }

    QImage image;
    if (m_isMain) {
        image = m_image.copy(lefttop.x(), lefttop.y(), 800/m_percent, 800/m_percent);
        image = image.scaled(800, 800);
    }
    else {
        int h = height() / m_resizePercent;
        image = m_image.copy(lefttop.x(), lefttop.y(), 730/m_percent, h/m_percent);
        image = image.scaled(730, h);
    }
    return image;
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
}

void ImageArea::hideEvent(QHideEvent *)
{
    m_label->setPixmap(QPixmap());
    m_label->setGeometry(0, 0, 0, 0);
}

void ImageArea::mousePressEvent(QMouseEvent *e)
{
    m_start = e->pos();
}

void ImageArea::mouseMoveEvent(QMouseEvent *e)
{
//    int offX = e->x() - m_start.x();
//    int offY = e->y() - m_start.y();
    m_start = e->pos();
}
