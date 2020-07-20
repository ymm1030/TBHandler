#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QColorDialog>
#include <QPainter>
#include <QMouseEvent>
#include <QFileDialog>
#include <QComboBox>
#include <QMessageBox>
#include <QFileInfo>
#include "mainmarktool.h"

static QSize s_markSize(130, 96);

MainMarkTool::MainMarkTool(QWidget *parent)
    : QWidget(parent)
    , m_colorBtn(nullptr)
    , m_colorLabel(nullptr)
    , m_instruction(nullptr)
    , m_saveBtn(nullptr)
    , m_finishBtn(nullptr)
    , m_bluewhiteChooser(nullptr)
    , m_leftopMark(nullptr)
    , m_mainImage(0, 0, QImage::Format_RGBA8888)
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
    m_instruction->setText("选择水印颜色，然后在图像上单击来确定水印位置，保存文件后，点击下一张编辑下一张");
    m_instruction->setWordWrap(true);

    m_saveBtn = new QPushButton(this);
    m_saveBtn->setText("保存");
    m_saveBtn->setFont(ft);
    connect(m_saveBtn, SIGNAL(clicked()), this, SLOT(saveFile()));

    m_finishBtn = new QPushButton(this);
    m_finishBtn->setText("下一张");
    m_finishBtn->setFont(ft);
    m_finishBtn->setEnabled(false);
    connect(m_finishBtn, SIGNAL(clicked()), this, SIGNAL(enterNext()));

    m_blueMark = QImage("bluemark.png");
    m_whiteMark = QImage("whitemark.png");
    m_markImage = QImage("mark.png");

    m_bluewhiteChooser = new QComboBox(this);
    m_bluewhiteChooser->addItem("蓝字");
    m_bluewhiteChooser->addItem("白字");
    m_bluewhiteChooser->setFont(ft);
    connect(m_bluewhiteChooser, SIGNAL(currentIndexChanged(int)), this, SLOT(blueWhiteChanged(int)));

    m_leftopMark = &m_blueMark;

    m_lastDir = "D:\\";
}

void MainMarkTool::setImage(const QImage &image)
{
    m_mainImage = image;
}

void MainMarkTool::setSelectedFileName(const QString &name)
{
    m_selectedFileName = name;
}

void MainMarkTool::chooseColor()
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

void MainMarkTool::saveFile()
{
    if (!m_paintRect.contains(m_markPoint)) {
        QMessageBox::warning(this, "警告", "你还没有打水印！");
        return;
    }

    QString name = QFileDialog::getSaveFileName(this, "保存文件", QDir(m_lastDir).filePath(m_selectedFileName), "Images (*.jpg *.jpeg *.png)");
    if (!name.isEmpty()) {
        QImage result = m_mainImage.copy();
        QPainter painter(&result);
        painter.drawImage(result.rect(), *m_leftopMark);
        QPoint realPoint((m_markPoint.x()-m_paintRect.x())/m_scaled, (m_markPoint.y()-m_paintRect.y())/m_scaled);
        QRect hr(realPoint.x()-s_markSize.width()/2, realPoint.y()-s_markSize.height()/2, s_markSize.width(), s_markSize.height());
        painter.drawImage(hr, m_markImage);
        result.save(name, "jpg");
        m_lastDir = QFileInfo(name).dir().absolutePath();
        QMessageBox::information(this, "提示", "保存成功！");
        m_finishBtn->setEnabled(true);
    }
}

void MainMarkTool::blueWhiteChanged(int idx)
{
    if (idx == 0) {
        m_leftopMark = &m_blueMark;
    }
    else {
        m_leftopMark = &m_whiteMark;
    }
    update();
}

void MainMarkTool::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (!m_mainImage.size().isEmpty()) {
        painter.drawImage(m_paintRect, m_mainImage);
        painter.drawImage(m_paintRect, *m_leftopMark);

        if (m_paintRect.contains(m_markPoint)) {
            QSize s(s_markSize.width()*m_scaled, s_markSize.height()*m_scaled);
            QRect hr(m_markPoint.x() - s.width()/2, m_markPoint.y() - s.height()/2, s.width(), s.height());
            painter.drawImage(hr, m_markImage);
        }
    }
}

void MainMarkTool::resizeEvent(QResizeEvent *)
{
    m_paintRect = QRect(10, 10, height() - 20, height() - 20);
    m_instruction->setGeometry(height() + 20, 10, width() - height() - 40, 200);
    m_bluewhiteChooser->setGeometry(height() + 20, 240, 100, 50);
    m_colorBtn->setGeometry(height() + 20, 300, 150, 50);
    m_colorLabel->setGeometry(height() + 180, 300, 80, 50);
    m_saveBtn->setGeometry(height() + 20, 350, 100, 50);
    m_finishBtn->setGeometry(height() + 20, 410, 100, 50);

    if (m_markPoint != QPoint(0, 0)) {
        m_markPoint = QPoint((m_markPoint.x()-m_paintRect.x())/m_scaled+m_paintRect.x(), (m_markPoint.y()-m_paintRect.y())/m_scaled+m_paintRect.y());
    }
    m_scaled = m_paintRect.width() / 800.0f;
    if (m_markPoint != QPoint(0, 0)) {
        m_markPoint = QPoint((m_markPoint.x()-m_paintRect.x())*m_scaled+m_paintRect.x(), (m_markPoint.y()-m_paintRect.y())*m_scaled+m_paintRect.y());
    }
}

void MainMarkTool::mousePressEvent(QMouseEvent *e)
{
    if (m_paintRect.contains(e->pos())) {
        m_markPoint = e->pos();
        update();
    }
}

void MainMarkTool::hideEvent(QHideEvent *)
{
    m_markPoint = QPoint(0, 0);
    m_finishBtn->setEnabled(false);
}
