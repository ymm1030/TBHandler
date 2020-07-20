#ifndef DOCEDITOR_H
#define DOCEDITOR_H

#include <QWidget>

struct TextStruct
{
    QString         text;
    QColor          color;

    TextStruct()
        : color(Qt::black)
    {
    }
};

class TextArea;
class QTextEdit;
class QPushButton;
class QLabel;
class QComboBox;
class DocEditor : public QWidget
{
    Q_OBJECT
public:
    explicit DocEditor(QWidget *parent = nullptr);
    void setImages(const QList<QImage>& images);

signals:
    void enterNext();

public slots:
    void textChanged();
    void familyChanged(int index);
    void chooseColor();
    void addSegment();
    void removeSegment();
    void showNext();
    void showPrev();
    void saveFile();

protected:
    void resizeEvent(QResizeEvent*) override;
    void hideEvent(QHideEvent *) override;

private:
    void checkCurrent();
    void showIndex(int index);

private:
    QList<QImage>       m_images;
    QList<TextStruct>   m_data;
    TextArea*           m_textArea;
    QTextEdit*          m_richEdit;
    QPushButton*        m_colorBtn;
    QLabel*             m_colorLabel;
    QPushButton*        m_addBtn;
    QPushButton*        m_deleteBtn;
    QPushButton*        m_nextBtn;
    QPushButton*        m_prevBtn;
    QPushButton*        m_saveBtn;
    QPushButton*        m_finishBtn;
    QComboBox*          m_fontSelector;
    int                 m_current;
    QString             m_lastDir;
};

#endif // DOCEDITOR_H
