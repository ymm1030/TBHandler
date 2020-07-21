#ifndef SUBMARKTOOL_H
#define SUBMARKTOOL_H

#include <QWidget>

class QPushButton;
class QLabel;
class SubMarkTool : public QWidget
{
    Q_OBJECT
public:
    explicit SubMarkTool(QWidget *parent = nullptr);

    void setImages(const QList<QImage>& images);
    const QList<QImage>& images() const;

signals:
    void enterNext();

public slots:
    void chooseColor();
    void showPrev();
    void showNext();
    void finishMarks();

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    bool isCurrentValid() const;
    QRect getCurrentRect() const;
    QRect getRect(int index) const;
    void showIndex(int index);
    void checkMarkPoints();
    void reset();

private:
    QPushButton*       m_colorBtn;
    QLabel*            m_colorLabel;
    QLabel*            m_instruction;
    QPushButton*       m_finishBtn;
    QPushButton*       m_prevBtn;
    QPushButton*       m_nextBtn;
    QRect              m_paintRect;
    QColor             m_markColor;
    QImage             m_markImage;
    QPoint             m_markPoint;
    int                m_current;
    QList<QImage>      m_images;
    QList<QPoint>      m_markPoints;
    float              m_scaled;
};

#endif // SUBMARKTOOL_H
