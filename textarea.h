#ifndef TEXTAREA_H
#define TEXTAREA_H

#include <QScrollArea>
#include "doceditor.h"

class TextRender;
class TextLabel;
class TextArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit TextArea(QWidget *parent = nullptr);

    void dataChanged(const QList<TextStruct>& data);
    void fontFamilyChanged(const QString& family);

    QPixmap getTextImage() const;

signals:

private:
    void ajustLabels();

private:
    QWidget*             m_container;
    QList<TextLabel*>    m_labels;
    QList<TextStruct>    m_data;
};

#endif // TEXTAREA_H
