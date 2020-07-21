#ifndef CLIPSUBIMAGETOOL_H
#define CLIPSUBIMAGETOOL_H

#include <QWidget>

class QPushButton;
class ImageArea;
class SizeController;
class QLabel;
class ClipSubImageTool : public QWidget
{
    Q_OBJECT
public:
    explicit ClipSubImageTool(QWidget *parent = nullptr);

    const QList<QImage>& images() const;

signals:
    void enterNext();

public slots:
    void vertPercentChanged(float f);
    void openFile();
    void reopenFile();
    void openPreview();
    void deleteAt(int index);
    void previewPanelClosed();
    void finished();

protected:
    void resizeEvent(QResizeEvent *) override;

private:
    void reset();

private:
    ImageArea*      m_imageArea;
    SizeController* m_horizon;
    SizeController* m_vertical;
    float           m_vertPercent;
    QLabel*         m_instruction;
    QPushButton*    m_openFile;
    QPushButton*    m_reopenFile;
    QPushButton*    m_preview;
    QPushButton*    m_finish;

    QString         m_lastDir;
    QList<QImage>   m_imageList;
    bool            m_imageShowing;
    QSize           m_areaSize;
};

#endif // CLIPSUBIMAGETOOL_H
