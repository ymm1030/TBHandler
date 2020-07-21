#include <QLabel>
#include <QPushButton>
#include <QGuiApplication>
#include <QScreen>
#include "previewpanel.h"

PreviewPanel::PreviewPanel(QWidget *parent)
    : QWidget(parent)
    , m_currentImage(nullptr)
    , m_prev(nullptr)
    , m_next(nullptr)
    , m_delete(nullptr)
    , m_current(-1)
    , m_lastCanBeDelete(false)
{
    setWindowTitle("预览");
    m_currentImage = new QLabel(this);
    m_currentImage->setScaledContents(true);

    m_prev = new QPushButton(this);
    m_prev->setText("上一张");
    QFont ft = m_prev->font();
    ft.setPointSize(14);
    m_prev->setFont(ft);
    m_prev->setEnabled(false);
    connect(m_prev, SIGNAL(clicked()), this, SLOT(showPrev()));

    m_next = new QPushButton(this);
    m_next->setText("下一张");
    m_next->setFont(ft);
    m_next->setEnabled(false);
    connect(m_next, SIGNAL(clicked()), this, SLOT(showNext()));

    m_delete = new QPushButton(this);
    m_delete->setText("删除这张");
    m_delete->setFont(ft);
    m_delete->setEnabled(false);
    connect(m_delete, SIGNAL(clicked()), this, SLOT(requireDelete()));

    m_currentImage->setGeometry(10, 10, 584, 584);
    m_prev->setGeometry(40, 604, 150, 30);
    m_delete->setGeometry(227, 604, 150, 30);
    m_next->setGeometry(414, 604, 150, 30);
    setFixedSize(604, 644);
}

void PreviewPanel::loadImages(const QList<QImage> &images, bool lastCanBeDelete)
{
    m_data = images;
    m_lastCanBeDelete = lastCanBeDelete;
    if (!m_data.empty()) {
        showIndex(0);
        if (images.size() == 1 && !lastCanBeDelete) {
            m_delete->setEnabled(false);
        }
        else {
            m_delete->setEnabled(true);
        }
        if (m_data.size() > 1) {
            m_next->setEnabled(true);
        }
    }
}

void PreviewPanel::showPrev()
{
    showIndex(m_current-1);
    if (m_current == 0) {
        m_prev->setEnabled(false);
    }
    if (m_current < m_data.size() -1) {
        m_next->setEnabled(true);
        m_delete->setEnabled(true);
    }
    else {
        if (!m_lastCanBeDelete) {
            m_delete->setEnabled(false);
        }
    }
}

void PreviewPanel::showNext()
{
    showIndex(m_current+1);
    if (m_current == m_data.size()-1) {
        m_next->setEnabled(false);
        if (!m_lastCanBeDelete) {
            m_delete->setEnabled(false);
        }
    }
    else {
        m_delete->setEnabled(true);
    }
    if (m_current > 0) {
        m_prev->setEnabled(true);
    }
}

void PreviewPanel::requireDelete()
{
    emit deleteAt(m_current);
    m_data.removeAt(m_current);

    if (m_data.empty()) {
        close();
        return;
    }

    if (m_current >= m_data.size()) {
        showPrev();
    }
    else if (m_current <= m_data.size()-1) {
        m_current -=1;
        showNext();
    }
    else if (m_current >= 0) {
        m_current += 1;
        showPrev();
    }
}

void PreviewPanel::showIndex(int idx)
{
    m_current = idx;
    QImage img = m_data.at(idx);
    m_currentImage->resize(584, img.height()*0.8);
    m_currentImage->setPixmap(QPixmap::fromImage(img));
    int y = (584 - img.height()*0.8)/2;
    m_currentImage->move(10, y+10);
}

void PreviewPanel::closeEvent(QCloseEvent *)
{
    emit closed();
}
