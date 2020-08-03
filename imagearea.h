#ifndef IMAGEAREA_H
#define IMAGEAREA_H

#include <QWidget>

class QLabel;
class ImageArea : public QWidget
{
    Q_OBJECT
public:
    ImageArea(QWidget* parent = nullptr, bool isMain = true);

    QSize imageSize() const;

    void loadImage(const QString& path);

    QImage getClippedImage() const;

    void reset();

    void calcMinimumPercent();

signals:
    void notifyMinimumPercent(float);

public slots:
    void percentChanged(float p);

protected:
    void resizeEvent(QResizeEvent *) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;

private:
    QLabel*     m_label;
    QImage      m_image;
    float       m_percent;
    float       m_resizePercent;
    QPoint      m_start;
    bool        m_isMain;
};

#endif // IMAGEAREA_H
