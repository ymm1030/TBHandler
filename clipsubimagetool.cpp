#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include "clipsubimagetool.h"
#include "sizecontroller.h"
#include "imagearea.h"
#include "previewpanel.h"

ClipSubImageTool::ClipSubImageTool(QWidget *parent)
    : QWidget(parent)
    , m_imageArea(nullptr)
    , m_horizon(nullptr)
    , m_vertical(nullptr)
    , m_vertPercent(1.0f)
    , m_instruction(nullptr)
    , m_openFile(nullptr)
    , m_reopenFile(nullptr)
    , m_preview(nullptr)
    , m_finish(nullptr)
    , m_imageShowing(false)
{
    m_horizon = new SizeController(this);
    m_horizon->setEnabled(false);

    m_instruction = new QLabel(this);
    QFont ft = m_instruction->font();
    ft.setPointSize(14);
    m_instruction->setFont(ft);

    m_vertical = new SizeController(this, false);

    m_imageArea = new ImageArea(this, false);
    connect(m_horizon, SIGNAL(percentChanged(float)), m_imageArea, SLOT(percentChanged(float)));
    connect(m_vertical, SIGNAL(percentChanged(float)), this, SLOT(vertPercentChanged(float)));

    m_instruction->setText("竖的滚动条可以调整图片高度。\n开下一张会自动保存当前这张，如果不想保存就点放弃重开。");
    m_instruction->setWordWrap(true);

    m_openFile = new QPushButton(this);
    m_openFile->setText("下一张");
    m_openFile->setFont(ft);
    connect(m_openFile, SIGNAL(clicked()), this, SLOT(openFile()));

    m_reopenFile = new QPushButton(this);
    m_reopenFile->setText("放弃重开");
    m_reopenFile->setFont(ft);
    connect(m_reopenFile, SIGNAL(clicked()), this, SLOT(reopenFile()));

    m_preview = new QPushButton(this);
    m_preview->setText("预览");
    m_preview->setFont(ft);
    m_preview->setEnabled(false);
    connect(m_preview, SIGNAL(clicked()), this, SLOT(openPreview()));

    m_finish = new QPushButton(this);
    m_finish->setText("统一打水印");
    m_finish->setFont(ft);
    m_finish->setEnabled(false);
    connect(m_finish, SIGNAL(clicked()), this, SLOT(finished()));

    m_lastDir = "D:\\";
    m_vertical->setValue(670);
}

const QList<QImage>& ClipSubImageTool::images() const
{
    return m_imageList;
}

void ClipSubImageTool::vertPercentChanged(float f)
{
    m_vertPercent = f;
    int w = m_imageArea->width();
    int h = w * f;
    int x = m_imageArea->x();
    int y = 40 + (w-h)/2;
    m_imageArea->setGeometry(x, y, w, h);
}

void ClipSubImageTool::openFile()
{
    if (m_imageShowing) {
        QImage img = m_imageArea->getClippedImage();
        if (img.isNull()) {
            QMessageBox::warning(this, "警告", "检测到黑边，请重新调整一下！");
            return;
        }
    }

    QString fileName = QFileDialog::getOpenFileName(this, "打开图片", m_lastDir, "Image Files (*.png *.jpg *.bmp *.jpeg)");
    if (!fileName.isEmpty()) {
        if (m_imageShowing) {
            m_imageList.push_back(m_imageArea->getClippedImage());
        }

        m_imageArea->loadImage(fileName);
        m_horizon->setEnabled(true);
        m_horizon->adjustSize(m_imageArea->imageSize(), m_areaSize);
        m_finish->setEnabled(true);
        m_preview->setEnabled(true);
        m_lastDir = QFileInfo(fileName).dir().absolutePath();
        m_imageShowing = true;
    }
}

void ClipSubImageTool::reopenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "打开图片", m_lastDir, "Image Files (*.png *.jpg *.bmp *.jpeg)");
    if (!fileName.isEmpty()) {
        m_imageArea->loadImage(fileName);
        m_horizon->setEnabled(true);
        m_horizon->adjustSize(m_imageArea->imageSize(), m_areaSize);
        m_finish->setEnabled(true);
        m_preview->setEnabled(true);
        m_lastDir = QFileInfo(fileName).dir().absolutePath();
        m_imageShowing = true;
    }
}

void ClipSubImageTool::openPreview()
{
    QList<QImage> l = m_imageList;
    QImage img = m_imageArea->getClippedImage();
    if (!img.isNull()) {
        l.push_back(img);
    }
    else {
        QMessageBox::warning(this, "警告", "检测到当前编辑图片有黑边，不予显示！");
    }

    if (l.empty()) {
        QMessageBox::warning(this, "警告", "当前没有图可以预览！");
        return;
    }

    PreviewPanel* panel = new PreviewPanel;
    connect(panel, SIGNAL(deleteAt(int)), this, SLOT(deleteAt(int)));
    connect(panel, SIGNAL(closed()), this, SLOT(previewPanelClosed()));
    panel->loadImages(l, img.isNull());
    setEnabled(false);
    panel->show();
}

void ClipSubImageTool::previewPanelClosed()
{
    setEnabled(true);
}

void ClipSubImageTool::deleteAt(int index)
{
    m_imageList.removeAt(index);
}

void ClipSubImageTool::finished()
{
    if (m_imageShowing) {
        QImage img = m_imageArea->getClippedImage();
        if (img.isNull()) {
            QMessageBox::warning(this, "警告", "检测到黑边，请重新调整一下！");
            return;
        }
        m_imageList.push_back(img);
    }
    emit enterNext();
    reset();
}

void ClipSubImageTool::resizeEvent(QResizeEvent *)
{
    m_horizon->setGeometry(50, 10, height() - 100, 20);
    m_vertical->setGeometry(height() - 30, 50, 50, height() - 100);
    m_imageArea->setGeometry(10, 40, height() - 50, height() - 50);
    m_areaSize = m_imageArea->size();
    vertPercentChanged(m_vertPercent);

    m_instruction->setGeometry(height() + 30, 40, width() - height() - 40, 200);
    m_openFile->setGeometry(height() + 60, 300, 200, 30);
    m_reopenFile->setGeometry(height() + 60, 350, 200, 30);
    m_preview->setGeometry(height() + 60, 400, 200, 30);
    m_finish->setGeometry(height() + 60, 450, 200, 30);
}

void ClipSubImageTool::reset()
{
    m_horizon->setEnabled(false);
    m_preview->setEnabled(false);
    m_finish->setEnabled(false);
    m_imageList.clear();
    m_imageShowing = false;
    m_imageArea->reset();
}
