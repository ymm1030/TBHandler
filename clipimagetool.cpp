#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include "clipimagetool.h"
#include "imagearea.h"
#include "sizecontroller.h"

ClipImageTool::ClipImageTool(QWidget *parent)
    : QWidget(parent)
    , m_openBtn(nullptr)
    , m_nextBtn(nullptr)
    , m_imageArea(nullptr)
    , m_sizeController(nullptr)
    , m_instruction(nullptr)
{
    m_sizeController = new SizeController(this);
    m_sizeController->setEnabled(false);

    m_imageArea = new ImageArea(this);
    m_instruction = new QLabel(this);
    QFont ft = m_instruction->font();
    ft.setPointSize(14);
    m_instruction->setFont(ft);

    m_nextBtn = new QPushButton(this);
    m_nextBtn->setText("下一步");
    m_nextBtn->setFont(ft);
    m_nextBtn->setEnabled(false);
    connect(m_nextBtn, SIGNAL(clicked()), this, SLOT(requireAddMarks()));

    m_openBtn = new QPushButton(this);
    m_openBtn->setText("打开图片");
    m_openBtn->setFont(ft);
    connect(m_openBtn, SIGNAL(clicked()), this, SLOT(openFile()));

    connect(m_sizeController, SIGNAL(percentChanged(float)), m_imageArea, SLOT(percentChanged(float)));

    m_instruction->setText("通过缩放和移动来调整左边框内的图片，能够看到的部分就是最终导出的图片，其余部分会被丢弃。完成之后点击下一步进入打水印环节。");
    m_instruction->setWordWrap(true);

    m_lastDir = "D:\\";
}

QImage ClipImageTool::clippedImage() const
{
    return m_imageArea->getClippedImage();
}

QString ClipImageTool::selectedFileName() const
{
    return m_selectedFileName;
}

void ClipImageTool::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "打开图片", m_lastDir, "Image Files (*.png *.jpg *.bmp *.jpeg)");
    if (!fileName.isEmpty()) {
        m_imageArea->loadImage(fileName);
        m_sizeController->setEnabled(true);
        m_sizeController->adjustSize(m_imageArea->imageSize(), m_imageArea->size());
        m_nextBtn->setEnabled(true);
        m_selectedFileName = QFileInfo(fileName).fileName();
        m_lastDir = QFileInfo(fileName).dir().absolutePath();
    }
}

void ClipImageTool::requireAddMarks()
{
    QImage image = clippedImage();
    if (image.isNull()) {
        QMessageBox::warning(this, "警告", "检测到黑边，请重新调整一下！");
        return;
    }
    emit addMarks();
}

void ClipImageTool::resizeEvent(QResizeEvent *)
{
    m_sizeController->setGeometry(50, 10, height() - 100, 20);
    m_imageArea->setGeometry(10, 40, height() - 50, height() - 50);
    m_instruction->setGeometry(height() + 20, 40, width() - height() - 40, 200);
    m_openBtn->setGeometry(height() + 60, 300, 200, 50);
    m_nextBtn->setGeometry(height() + 60, 400, 200, 50);
}

void ClipImageTool::hideEvent(QHideEvent *)
{
    m_nextBtn->setEnabled(false);
    m_sizeController->setEnabled(false);
    m_selectedFileName = QString();
}
