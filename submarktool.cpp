#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QColorDialog>
#include <QPainter>
#include "submarktool.h"

static QSize s_markSize(130, 96);

SubMarkTool::SubMarkTool(QWidget *parent)
    : QWidget(parent)
    , m_colorBtn(nullptr)
    , m_colorLabel(nullptr)
    , m_instruction(nullptr)
    , m_finishBtn(nullptr)
    , m_prevBtn(nullptr)
    , m_nextBtn(nullptr)
    , m_current(-1)
    , m_scaled(1.0f)
{
    m_markColor = Qt::black;
    m_colorBtn = new QPushButton(this);
    m_colorBtn->setText("选择水印颜色");
    connect(m_colorBtn, SIGNAL(clicked()), this, SLOT(chooseColor()));

    QFont ft = m_colorBtn->font();
    ft.setPointSize(14);
    m_colorBtn->setFont(ft);

    m_colorLabel = new QLabel(this);
    m_colorLabel->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(m_colorLabel->backgroundRole(), m_markColor);
    m_colorLabel->setPalette(palette);

    m_instruction = new QLabel(this);
    m_instruction->setFont(ft);
    m_instruction->setText("选择水印颜色，然后在图像上单击来确定水印位置，点击下一张显示下一张，全部完成后进入文案编辑");
    m_instruction->setWordWrap(true);

    m_finishBtn = new QPushButton(this);
    m_finishBtn->setText("文案编辑");
    m_finishBtn->setFont(ft);
    m_finishBtn->setEnabled(false);
    connect(m_finishBtn, SIGNAL(clicked()), this, SLOT(finishMarks()));

    m_prevBtn = new QPushButton(this);
    m_prevBtn->setText("上一张");
    m_prevBtn->setFont(ft);
    m_prevBtn->setEnabled(false);
    connect(m_prevBtn, SIGNAL(clicked()), this, SLOT(showPrev()));

    m_nextBtn = new QPushButton(this);
    m_nextBtn->setText("下一张");
    m_nextBtn->setFont(ft);
    m_nextBtn->setEnabled(false);
    connect(m_nextBtn, SIGNAL(clicked()),this, SLOT(showNext()));

    m_markImage = QImage("mark.png");
}

void SubMarkTool::setImages(const QList<QImage> &images)
{
    m_images = images;
    for (int i = 0; i < images.size(); ++i) {
        m_markPoints.push_back(QPoint());
    }
    showIndex(0);
}

const QList<QImage>& SubMarkTool::images() const
{
    return m_images;
}

void SubMarkTool::chooseColor()
{
    QColor c = QColorDialog::getColor(m_markColor);
    if (c.isValid()) {
        m_markColor = c;
        for (int i = 0; i < m_markImage.width(); i++) {
            for (int j = 0; j < m_markImage.height(); j++) {
                int p = m_markImage.pixel(i, j);
                if (qAlpha(p) > 0) {
                    m_markImage.setPixelColor(i, j, QColor(m_markColor.red(), m_markColor.green(), m_markColor.blue(), qAlpha(p)));
                }
            }
        }
        QPalette palette;
        palette.setBrush(m_colorLabel->backgroundRole(), m_markColor);
        m_colorLabel->setPalette(palette);
    }
}

void SubMarkTool::showPrev()
{
    showIndex(m_current-1);
}

void SubMarkTool::showNext()
{
    showIndex(m_current+1);
}

void SubMarkTool::finishMarks()
{
    for (int i = 0; i < m_images.size(); ++i) {
        QImage img = m_images.at(i);
        QPainter painter(&img);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
        QPoint p = m_markPoints.at(i);
        QRect r = getRect(i);
        QPoint realPoint((p.x()-r.x())/m_scaled, (p.y()-r.y())/m_scaled);
        QRect hr(realPoint.x()-s_markSize.width()/2, realPoint.y()-s_markSize.height()/2, s_markSize.width(), s_markSize.height());
        painter.drawImage(hr, m_markImage);
        m_images.replace(i, img);
    }

    emit enterNext();
    reset();
}

void SubMarkTool::paintEvent(QPaintEvent *)
{
    if (!isCurrentValid()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    const QImage& img = m_images.at(m_current);
    QRect r = getCurrentRect();
    if (!img.size().isEmpty()) {
        painter.drawImage(r, img);

        if (r.contains(m_markPoint)) {
            QSize s(s_markSize.width()*m_scaled, s_markSize.height()*m_scaled);
            QRect hr(m_markPoint.x() - s.width()/2, m_markPoint.y() - s.height()/2, s.width(), s.height());
            painter.drawImage(hr, m_markImage);
        }
    }
}

void SubMarkTool::resizeEvent(QResizeEvent *)
{
    m_paintRect = QRect(10, 10, height() - 20, height() - 20);
    m_instruction->setGeometry(height() + 20, 10, width()-height()-30, 200);
    m_colorBtn->setGeometry(height() + 20, 300, 150, 30);
    m_colorLabel->setGeometry(height() + 180, 300, 80, 30);
    m_prevBtn->setGeometry(height() + 20, 380, 150, 30);
    m_nextBtn->setGeometry(height() + 20, 430, 150, 30);
    m_finishBtn->setGeometry(height() + 20, 550, 100, 30);

    m_scaled = m_paintRect.width() / 730.0f;

    for (int i = 0; i < m_markPoints.size(); ++i) {
        const QPoint& p = m_markPoints.at(i);
        QRect r = getRect(i);
        if (!r.contains(p)) {
            m_markPoints.replace(i, QPoint());
        }
    }
    checkMarkPoints();
}

void SubMarkTool::mousePressEvent(QMouseEvent *e)
{
    if (!isCurrentValid()) {
        return;
    }

    QRect r = getCurrentRect();

    if (r.contains(e->pos())) {
        m_markPoint = e->pos();
        m_markPoints.replace(m_current, m_markPoint);
        checkMarkPoints();
        update();
    }
}

bool SubMarkTool::isCurrentValid() const
{
    return m_current >= 0 && m_current <= (m_images.size()-1);
}

QRect SubMarkTool::getCurrentRect() const
{
    return getRect(m_current);
}

QRect SubMarkTool::getRect(int index) const
{
    const QImage& img = m_images.at(index);
    int x = m_paintRect.x();
    int h = img.height()*m_scaled;
    int y = (m_paintRect.height()-h)/2 + m_paintRect.y();
    int w = m_paintRect.width();

    return QRect(x, y, w, h);
}

void SubMarkTool::showIndex(int index)
{
    if (index < 0 || index >= m_images.size() || index == m_current) {
        return;
    }
    m_current = index;
    m_markPoint = m_markPoints.at(index);

    if (m_current == 0) {
        m_prevBtn->setEnabled(false);
    }
    else {
        m_prevBtn->setEnabled(true);
    }

    if (m_current < m_images.size()-1) {
        m_nextBtn->setEnabled(true);
    }
    else {
        m_nextBtn->setEnabled(false);
    }

    update();
}

void SubMarkTool::checkMarkPoints()
{
    if (m_finishBtn->isEnabled()) {
        return;
    }

    for (int i = 0; i < m_markPoints.size(); ++i) {
        if (m_markPoints.at(i) == QPoint()) {
            return;
        }
    }

    m_finishBtn->setEnabled(true);
}

void SubMarkTool::reset()
{
    m_current = -1;
    m_images.clear();
    m_markPoints.clear();
    m_markPoint = QPoint();
    m_finishBtn->setEnabled(false);
    m_prevBtn->setEnabled(false);
    m_nextBtn->setEnabled(false);
}
