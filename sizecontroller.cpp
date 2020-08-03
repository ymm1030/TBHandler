#include <QProgressBar>
#include <QPushButton>
#include <QMouseEvent>
#include <QDebug>
#include "sizecontroller.h"

SizeController::SizeController(QWidget *parent, bool isHorizontal)
    : QWidget(parent)
    , m_pbar(nullptr)
    , m_decBtn(nullptr)
    , m_incBtn(nullptr)
    , m_horizontal(isHorizontal)
    , m_minimumPercent(1.0f)
{
    m_pbar = new QProgressBar(this);
    m_decBtn = new QPushButton(this);
    m_incBtn = new QPushButton(this);

    m_pbar->setRange(0, 1000);
    m_pbar->setValue(1000);
    m_pbar->setTextVisible(false);
    m_decBtn->setText("缩小");
    m_incBtn->setText("放大");

    connect(m_incBtn, SIGNAL(clicked()), this, SLOT(incValue()));
    connect(m_decBtn, SIGNAL(clicked()), this, SLOT(decValue()));
    connect(m_pbar, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));

    if (m_horizontal) {
        m_pbar->setOrientation(Qt::Horizontal);
    }
    else {
        m_pbar->setOrientation(Qt::Vertical);
    }
}

int SizeController::value() const
{
    return m_pbar->value();
}

void SizeController::setValue(int v)
{
    if (v == m_pbar->value()) {
        if (v < m_pbar->maximum()) {
            v = v+1;
        }
        else {
            v = v-1;
        }
    }
    if (m_horizontal) {
        int v1 = m_pbar->maximum() * m_minimumPercent;
        if (v < v1) {
           v = v1;
        }
    }
    m_pbar->setValue(v);
}

void SizeController::incValue(int v)
{
    setValue(value() + v);
}

void SizeController::decValue(int v)
{
    setValue(value() - v);
}

void SizeController::incValue()
{
    incValue(m_pbar->maximum()/100);
}

void SizeController::decValue()
{
    decValue(m_pbar->maximum()/100);
}

void SizeController::notifyMinimumPercent(float f)
{
    m_minimumPercent = f;
    float f1 = float(m_pbar->value()) / float(m_pbar->maximum());
    if (f1 < f) {
        setValue(m_pbar->maximum() * f);
    }
}

void SizeController::resizeEvent(QResizeEvent *)
{
    if (m_horizontal) {
        m_decBtn->setGeometry(0, 0, 50, height());
        m_incBtn->setGeometry(width() - 50, 0, 50, height());
        m_pbar->setGeometry(60, 0, width() - 120, height());
    }
    else {
        m_incBtn->setGeometry(0, 0, width(), 20);
        m_decBtn->setGeometry(0, height() - 20, width(), 20);
        m_pbar->setGeometry(0, 30, width(), height() - 60);
    }
}

void SizeController::mousePressEvent(QMouseEvent *e)
{
    QPoint p = e->pos();
    if (m_pbar->geometry().contains(p)) {
        float f = 0.0f;
        if (m_horizontal) {
            f = float(p.x() - m_pbar->x())/float(m_pbar->width());
        }
        else {
            f = float(m_pbar->y() + m_pbar->height() - p.y())/float(m_pbar->height());
        }
        int v = f * m_pbar->maximum();
        setValue(v);
    }
}

void SizeController::mouseMoveEvent(QMouseEvent *e)
{
    QPoint p = e->pos();
    if (m_pbar->geometry().contains(p)) {
        float f = 0.0f;

        if (m_horizontal) {
            f = float(p.x() - m_pbar->x())/float(m_pbar->width());
        }
        else {
            f = float(m_pbar->y() + m_pbar->height() - p.y())/float(m_pbar->height());
        }
        int v = f * m_pbar->maximum();
        setValue(v);
    }
}

void SizeController::valueChanged(int v)
{
    float f = float(v) / float(m_pbar->maximum());
    emit percentChanged(f);
}

void SizeController::adjustSize(const QSize &imageSize, const QSize &areaSize)
{
    int w = areaSize.width() * 1.3;
    int h = areaSize.height() * 1.3;

    float f1 = float(w) / float(imageSize.width());
    float f2 = float(h) / float(imageSize.height());

    float f = qMax(f1, f2);
    setValue(m_pbar->maximum() * f);
}
