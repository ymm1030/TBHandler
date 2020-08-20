#include <QPainter>
#include <QFontMetrics>
#include <QDebug>
#include "textlabel.h"

static int s_line_spacing = 10;

TextLabel::TextLabel(QWidget* parent)
    : QLabel(parent)
{
}

void TextLabel::setText(const QString &text)
{
    m_lines.clear();
    if (text.isEmpty()) {
        return;
    }

    QStringList list = text.split('\n');

    QFontMetrics fm(font());
    int maxw = 710;

    for (int i = 0; i < list.size(); ++i) {
        QString current = list.at(i);
        int start = 0;
        int size = 1;
        QString sub;
        while (size <= current.size()) {
            sub = current.mid(start, size - start);
            if (fm.boundingRect(sub).width() > maxw) {
                m_lines.push_back(current.mid(start, size-start-1));
                start += size-start-1;
            }
            else {
                size++;
            }
        }
        m_lines.push_back(current.mid(start, size-start));
    }
    update();
}

QRect TextLabel::boundingRect() const
{
    QFontMetrics fm(font());
    int h = 0;
    for (int i = 0; i < m_lines.size(); ++i) {
        h += fm.boundingRect(m_lines.at(i)).height() + s_line_spacing;
    }
    h -= s_line_spacing;
    return QRect(0, 0, 710, h);
}

void TextLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::TextAntialiasing);
    QPen pen = painter.pen();

    QFontMetrics fm(font());
    int h = 0;
    for (int i = 0; i < m_lines.size(); ++i) {
        QString s = m_lines.at(i);
        h += fm.boundingRect(s).height();
        painter.setPen(QColor(17, 17, 17, 50));
        painter.drawText(1, h+1, s);
        painter.setPen(pen);
        painter.drawText(0, h, s);
        h += s_line_spacing;
    }
}
