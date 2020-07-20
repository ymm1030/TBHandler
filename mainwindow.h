#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class QPushButton;
class ClipImageTool;
class MainMarkTool;
class ClipSubImageTool;
class SubMarkTool;
class DocEditor;
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void makeMainPic();
    void makeSubPic();
    void addMainMarks();
    void addSubMarks();
    void addDocs();

protected:
    void resizeEvent(QResizeEvent*);

private:
    void hideBtns();
    void ajustSize();

private:
    QPushButton*    m_makeMainBtn;
    QPushButton*    m_makeSubBtn;
    ClipImageTool*  m_clipImageTool;
    MainMarkTool*   m_mainMarkTool;
    ClipSubImageTool*   m_clipSubImageTool;
    SubMarkTool*    m_subMarkTool;
    DocEditor*      m_docEditor;
    bool            m_first;
};
#endif // MAINWINDOW_H
