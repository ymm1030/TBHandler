#ifndef SIZECONTROLLER_H
#define SIZECONTROLLER_H

#include <QWidget>

class QProgressBar;
class QPushButton;
class SizeController : public QWidget
{
    Q_OBJECT
public:
    explicit SizeController(QWidget *parent = nullptr, bool isHorizontal = true);

    int value() const;
    void setValue(int v);

    void incValue(int v);
    void decValue(int v);

    void adjustSize(const QSize& imageSize, const QSize& areaSize);

signals:
    void percentChanged(float p);

public slots:
    void incValue();
    void decValue();

private slots:
    void valueChanged(int v);

protected:
    void resizeEvent(QResizeEvent *) override;
    void mousePressEvent(QMouseEvent * e) override;
    void mouseMoveEvent(QMouseEvent *e) override;

private:
    QProgressBar*   m_pbar;
    QPushButton*    m_decBtn;
    QPushButton*    m_incBtn;
    bool            m_horizontal;
};

#endif // SIZECONTROLLER_H
