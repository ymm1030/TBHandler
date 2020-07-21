#ifndef CLIPIMAGETOOL_H
#define CLIPIMAGETOOL_H

#include <QWidget>

class QPushButton;
class ImageArea;
class SizeController;
class QLabel;
class ClipImageTool : public QWidget
{
    Q_OBJECT
public:
    explicit ClipImageTool(QWidget *parent = nullptr);

    QImage clippedImage() const;
    QString selectedFileName() const;

signals:
    void addMarks();

public slots:
    void openFile();
    void requireAddMarks();

protected:
    void resizeEvent(QResizeEvent *) override;

private:
    void reset();

private:
    QPushButton*    m_openBtn;
    QPushButton*    m_nextBtn;
    ImageArea*      m_imageArea;
    SizeController* m_sizeController;
    QLabel*         m_instruction;
    QString         m_selectedFileName;
    QString         m_lastDir;
};

#endif // CLIPIMAGETOOL_H
