#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QColorDialog>
#include <QPainter>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include "doceditor.h"
#include "textarea.h"

DocEditor::DocEditor(QWidget *parent)
    : QWidget(parent)
    , m_textArea(nullptr)
    , m_richEdit(nullptr)
    , m_colorBtn(nullptr)
    , m_colorLabel(nullptr)
    , m_addBtn(nullptr)
    , m_deleteBtn(nullptr)
    , m_nextBtn(nullptr)
    , m_prevBtn(nullptr)
    , m_saveBtn(nullptr)
    , m_finishBtn(nullptr)
    , m_fontSelector(nullptr)
    , m_current(0)
{
    m_data.push_back(TextStruct());

    m_textArea = new TextArea(this);

    m_richEdit = new QTextEdit(this);
    QFont ft = m_richEdit->font();
    ft.setPointSize(14);
    m_richEdit->setFont(ft);
    connect(m_richEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));

    m_addBtn = new QPushButton(this);
    m_addBtn->setFont(ft);
    m_addBtn->setText("加一段");
    connect(m_addBtn, SIGNAL(clicked()), this, SLOT(addSegment()));

    m_deleteBtn = new QPushButton(this);
    m_deleteBtn->setFont(ft);
    m_deleteBtn->setText("删除本段");
    connect(m_deleteBtn, SIGNAL(clicked()), this, SLOT(removeSegment()));

    m_nextBtn = new QPushButton(this);
    m_nextBtn->setFont(ft);
    m_nextBtn->setEnabled(false);
    m_nextBtn->setText("下一段");
    connect(m_nextBtn, SIGNAL(clicked()), this, SLOT(showNext()));

    m_prevBtn = new QPushButton(this);
    m_prevBtn->setFont(ft);
    m_prevBtn->setEnabled(false);
    m_prevBtn->setText("上一段");
    connect(m_prevBtn, SIGNAL(clicked()), this, SLOT(showPrev()));

    m_saveBtn = new QPushButton(this);
    m_saveBtn->setFont(ft);
    m_saveBtn->setText("保存");
    connect(m_saveBtn, SIGNAL(clicked()), this, SLOT(saveFile()));

    m_finishBtn = new QPushButton(this);
    m_finishBtn->setFont(ft);
    m_finishBtn->setText("完成");
    m_finishBtn->setEnabled(false);
    connect(m_finishBtn, SIGNAL(clicked()), this, SLOT(requireEnterNext()));

    m_colorBtn = new QPushButton(this);
    m_colorBtn->setFont(ft);
    m_colorBtn->setText("本段颜色");
    connect(m_colorBtn, SIGNAL(clicked()), this, SLOT(chooseColor()));

    m_colorLabel = new QLabel(this);
    m_colorLabel->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(m_colorLabel->backgroundRole(), QColor(27, 27, 27));
    m_colorLabel->setPalette(palette);

    m_defaultColorBtn = new QPushButton(this);
    m_defaultColorBtn->setFont(ft);
    m_defaultColorBtn->setText("一键设置默认色");
    connect(m_defaultColorBtn, SIGNAL(clicked()), this, SLOT(restoreDefaultColors()));

    m_fontSelector = new QComboBox(this);
    m_fontSelector->addItem("华康方圆体W7");
    m_fontSelector->addItem("华康海报体W12(P)");
    m_fontSelector->addItem("华康黑体W12(P)");
    connect(m_fontSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(familyChanged(int)));

    m_lastDir = "D:\\";
}

void DocEditor::setImages(const QList<QImage> &images)
{
    m_images = images;
}

void DocEditor::textChanged()
{
    TextStruct ts = m_data.at(m_current);
    ts.text = m_richEdit->toPlainText();
    m_data.replace(m_current, ts);
    m_textArea->dataChanged(m_data);
}

void DocEditor::familyChanged(int index)
{
    switch (index) {
    case 0:
        m_textArea->fontFamilyChanged("DFFangYuanW7-GB");
        break;
    case 1:
        m_textArea->fontFamilyChanged("DFPHaiBaoW12-GB");
        break;
    case 2:
        m_textArea->fontFamilyChanged("DFPHeiW12-GB");
        break;
    default:
        break;
    }
}

void DocEditor::chooseColor()
{
    TextStruct ts = m_data.at(m_current);
    QColor c = QColorDialog::getColor(ts.color);
    if (c.isValid()) {
        ts.color = c;
        QPalette palette;
        palette.setBrush(m_colorLabel->backgroundRole(), ts.color);
        m_colorLabel->setPalette(palette);
        m_data.replace(m_current, ts);
        m_textArea->dataChanged(m_data);
    }
}

void DocEditor::addSegment()
{
    TextStruct ts;
    if (m_data.size() == 1) {
        ts.color = QColor(95, 82, 160);
    }
    else if (m_data.size() == 2) {
        ts.color = QColor(137, 87, 161);
    }
    else if (m_data.size() == 3) {
        ts.color = QColor(234, 104, 162);
    }
    m_data.push_back(ts);
    m_current = m_data.size()-1;
    m_richEdit->clear();
    checkCurrent();

    QPalette palette;
    palette.setBrush(m_colorLabel->backgroundRole(), ts.color);
    m_colorLabel->setPalette(palette);
}

void DocEditor::removeSegment()
{
    m_data.removeAt(m_current);
    if (m_data.empty()) {
        m_data.push_back(TextStruct());
        m_current = 0;
    }

    if (m_current >= m_data.size()) {
        showIndex(m_current-1);
    }
    else if (m_current <= m_data.size()-1) {
        m_current -= 1;
        showIndex(m_current+1);
    }
    else if (m_current >= 0) {
        m_current += 1;
        showIndex(m_current-1);
    }

    checkCurrent();
    m_textArea->dataChanged(m_data);
}

void DocEditor::showNext()
{
    showIndex(m_current+1);
    checkCurrent();
}

void DocEditor::showPrev()
{
    showIndex(m_current-1);
    checkCurrent();
}

void DocEditor::saveFile()
{
    QString name = QFileDialog::getSaveFileName(this, "保存文件", QDir(m_lastDir).filePath("描述.jpg"), "Images (*.jpg *.jpeg *.png)");

    if (name.isEmpty()) {
        return;
    }

    QPixmap textImage = m_textArea->getTextImage();
    float f = float(textImage.width())/710.0f;
    int h = textImage.height()/f;

    int th = 10;
    for (int i = 0; i < m_images.size(); ++i) {
        th += m_images.at(i).height();
        th += 10;
    }
    th += h + 20;
    QImage img(750, th, QImage::Format_RGBA8888);
    img.fill(Qt::white);
    QPainter painter(&img);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    th = 10;
    for(int i = 0; i < m_images.size(); ++i) {
        const QImage& image = m_images.at(i);
        QRect r(10, th, 730, image.height());
        painter.drawImage(r, image);
        th += r.height() + 10;
        if (i == 0) {
            QRect r1(20, th, 710, h);
            painter.drawPixmap(r1, textImage);
            th += h + 10;
        }
    }

    img.save(name, "jpg", 95);
    m_lastDir = QFileInfo(name).dir().absolutePath();
    QMessageBox::information(this, "提示", "保存成功！");
    m_finishBtn->setEnabled(true);
}

void DocEditor::restoreDefaultColors()
{
    for (int i = 0; i < m_data.size(); ++i) {
        switch (i) {
        case 0:
            m_data[i].color = QColor(27, 27, 27);
            break;
        case 1:
            m_data[i].color = QColor(95, 82, 160);
            break;
        case 2:
            m_data[i].color = QColor(137, 87, 161);
            break;
        case 3:
            m_data[i].color = QColor(234, 104, 162);
            break;
        default:
            break;
        }
    }
    m_textArea->dataChanged(m_data);
    QPalette palette;
    palette.setBrush(m_colorLabel->backgroundRole(), m_data[m_current].color);
    m_colorLabel->setPalette(palette);
}

void DocEditor::requireEnterNext()
{
    emit enterNext();
    reset();
}

void DocEditor::resizeEvent(QResizeEvent *)
{
    int w = height() - 20;
    if (w > 710) {
        w = 710;
    }
    m_textArea->setGeometry(10, 10, w, height() - 20);
    m_richEdit->setGeometry(w + 20, 10, width()-w-30, 200);

    m_colorBtn->setGeometry(w + 50, 220, 150, 30);
    m_colorLabel->setGeometry(w + 220, 220, 80, 30);
    m_fontSelector->setGeometry(w + 50, 270, 200, 30);

    m_addBtn->setGeometry(w + 50, 320, 150, 30);
    m_deleteBtn->setGeometry(w+50, 370, 150, 30);
    m_prevBtn->setGeometry(w + 50, 420, 150, 30);
    m_nextBtn->setGeometry(w + 50, 470, 150, 30);
    m_defaultColorBtn->setGeometry(w+50, 520, 150, 30);
    m_saveBtn->setGeometry(w+50, 600, 150, 30);
    m_finishBtn->setGeometry(w+50, 650, 150, 30);
}

void DocEditor::checkCurrent()
{
    if (m_current > 0) {
        m_prevBtn->setEnabled(true);
    }
    else {
        m_prevBtn->setEnabled(false);
    }
    if (m_current < m_data.size() - 1) {
        m_nextBtn->setEnabled(true);
    }
    else {
        m_nextBtn->setEnabled(false);
    }
}

void DocEditor::showIndex(int index)
{
    if (index < 0 || index >= m_data.size() || index == m_current) {
        return;
    }
    m_current = index;
    TextStruct ts = m_data.at(m_current);
    m_richEdit->setText(ts.text);
    QPalette palette;
    palette.setBrush(m_colorLabel->backgroundRole(), ts.color);
    m_colorLabel->setPalette(palette);
}

void DocEditor::reset()
{
    m_images.clear();
    m_data.clear();
    m_data.push_back(TextStruct());
    m_current = 0;
    m_richEdit->clear();
    QPalette palette;
    palette.setBrush(m_colorLabel->backgroundRole(), Qt::black);
    m_colorLabel->setPalette(palette);
    m_nextBtn->setEnabled(false);
    m_prevBtn->setEnabled(false);
    m_finishBtn->setEnabled(false);
}
