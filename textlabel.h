#ifndef TEXTLABEL_H
#define TEXTLABEL_H

#include <QLabel>

class TextLabel : public QLabel
{
    Q_OBJECT
public:
    TextLabel(QWidget* parent = nullptr);

    void setText(const QString& text);

    QRect boundingRect() const;

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QStringList m_lines;
};

#endif // TEXTLABEL_H
