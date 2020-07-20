#include <QPushButton>
#include <QDebug>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QFontDatabase>
#include "mainwindow.h"
#include "clipimagetool.h"
#include "mainmarktool.h"
#include "clipsubimagetool.h"
#include "submarktool.h"
#include "doceditor.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , m_makeMainBtn(nullptr)
    , m_makeSubBtn(nullptr)
    , m_clipImageTool(nullptr)
    , m_mainMarkTool(nullptr)
    , m_clipSubImageTool(nullptr)
    , m_subMarkTool(nullptr)
    , m_docEditor(nullptr)
    , m_first(true)
{
    setFixedSize(375, 200);

    QFontDatabase::addApplicationFont("yuanti.ttf");
    QFontDatabase::addApplicationFont("haibaoti.ttf");
    QFontDatabase::addApplicationFont("heiti.TTF");

    m_makeMainBtn = new QPushButton(this);
    m_makeMainBtn->setGeometry(25, 50, 150, 100);
    m_makeMainBtn->setText("制作宝贝主图");
    connect(m_makeMainBtn, SIGNAL(clicked()), this, SLOT(makeMainPic()));

    m_makeSubBtn = new QPushButton(this);
    m_makeSubBtn->setGeometry(200, 50, 150, 100);
    m_makeSubBtn->setText("制作宝贝描述图");
    connect(m_makeSubBtn, SIGNAL(clicked()), this, SLOT(makeSubPic()));

    m_clipImageTool = new ClipImageTool(this);
    m_clipImageTool->hide();
    connect(m_clipImageTool, SIGNAL(addMarks()), this, SLOT(addMainMarks()));

    m_mainMarkTool = new MainMarkTool(this);
    m_mainMarkTool->hide();
    connect(m_mainMarkTool, SIGNAL(enterNext()), this, SLOT(makeMainPic()));

    m_clipSubImageTool = new ClipSubImageTool(this);
    m_clipSubImageTool->hide();
    connect(m_clipSubImageTool, SIGNAL(enterNext()), this, SLOT(addSubMarks()));

    m_subMarkTool = new SubMarkTool(this);
    m_subMarkTool->hide();
    connect(m_subMarkTool, SIGNAL(enterNext()), this, SLOT(addDocs()));

    m_docEditor = new DocEditor(this);
    m_docEditor->hide();
    connect(m_docEditor, SIGNAL(enterNext()), this, SLOT(makeSubPic()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::makeMainPic()
{
    m_mainMarkTool->hide();
    hideBtns();
    ajustSize();

    m_clipImageTool->show();
}

void MainWindow::makeSubPic()
{
    m_docEditor->hide();
    hideBtns();
    ajustSize();

    m_clipSubImageTool->show();
}

void MainWindow::addMainMarks()
{
    m_mainMarkTool->setImage(m_clipImageTool->clippedImage());
    m_mainMarkTool->setSelectedFileName(m_clipImageTool->selectedFileName());
    m_clipImageTool->hide();
    m_mainMarkTool->show();
}

void MainWindow::addSubMarks()
{
    m_subMarkTool->setImages(m_clipSubImageTool->images());
    m_clipSubImageTool->hide();
    m_subMarkTool->show();
}

void MainWindow::addDocs()
{
    m_docEditor->setImages(m_subMarkTool->images());
    m_subMarkTool->hide();
    m_docEditor->show();
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    m_clipImageTool->setGeometry(0, 0, width(), height());
    m_mainMarkTool->setGeometry(0, 0, width(), height());
    m_clipSubImageTool->setGeometry(0, 0, width(), height());
    m_subMarkTool->setGeometry(0, 0, width(), height());
    m_docEditor->setGeometry(0, 0, width(), height());
}

void MainWindow::hideBtns()
{
    m_makeMainBtn->hide();
    m_makeSubBtn->hide();
}

void MainWindow::ajustSize()
{
    if (!m_first) {
        return;
    }
    m_first = false;

    setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    QRect r = QGuiApplication::screens().at(0)->geometry();
    QRect r1;

    if (r.width() >= 1300) {
        r1.setWidth(1200);
    }
    else {
        r1.setWidth(r.width() - 100);
    }

    if (r.height() >= 1000) {
        r1.setHeight(900);
    }
    else {
        r1.setHeight(r.height() - 100);
    }

    r1 = QRect((r.width()-r1.width())/2, (r.height()-r1.height())/2, r1.width(), r1.height());
    setGeometry(r1);
}

