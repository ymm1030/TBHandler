#ifndef PREVIEWPANEL_H
#define PREVIEWPANEL_H

#include <QWidget>

class QLabel;
class QPushButton;
class PreviewPanel : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewPanel(QWidget *parent = nullptr);

    void loadImages(const QList<QImage>& images, bool lastCanBeDelete);

signals:
    void deleteAt(int index);
    void closed();

public slots:
    void showPrev();
    void showNext();
    void requireDelete();

protected:
    void closeEvent(QCloseEvent *) override;

private:
    void showIndex(int idx);

private:
    QLabel*         m_currentImage;
    QPushButton*    m_prev;
    QPushButton*    m_next;
    QPushButton*    m_delete;
    int             m_current;
    QList<QImage>   m_data;
    bool            m_lastCanBeDelete;
};

#endif // PREVIEWPANEL_H
