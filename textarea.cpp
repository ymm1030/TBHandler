#include <QDebug>
#include <QLabel>
#include <QFontMetrics>
#include <QScrollBar>
#include "textarea.h"
#include "textlabel.h"

TextArea::TextArea(QWidget *parent)
    : QScrollArea(parent)
    , m_container(nullptr)
{
    m_container = new QWidget(this);

    QFont ft = font();
    ft.setFamily("DFFangYuanW7-GB");
    ft.setPointSize(18);
    ft.setLetterSpacing(QFont::PercentageSpacing, 105);
    ft.setStyleStrategy(QFont::PreferAntialias);
    setFont(ft);

    setWidget(m_container);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(backgroundRole(), Qt::white);
    setPalette(palette);
}

void TextArea::dataChanged(const QList<TextStruct> &data)
{
    m_data = data;
    while (m_labels.size() < data.size()) {
        m_labels.push_back(new TextLabel(m_container));
    }

    while (m_labels.size() > data.size()) {
        TextLabel* label = m_labels.back();
        delete label;
        m_labels.pop_back();
    }
    ajustLabels();
}

void TextArea::fontFamilyChanged(const QString &family)
{
    QFont ft = font();
    ft.setFamily(family);
    setFont(ft);
    ajustLabels();
}

void TextArea::ajustLabels()
{
    QFontMetrics fm(font());
    int segmentSpacing = fm.boundingRect(QChar('a')).height();
    QPalette palette;
    int h = 0;
    int maxw = 710;
    for (int i = 0; i < m_labels.size(); ++i) {
        TextLabel* label = m_labels.at(i);
        label->show();
        label->setFont(font());
        TextStruct ts = m_data.at(i);
        label->setText(ts.text);
        palette.setColor(label->foregroundRole(), ts.color);
        label->setPalette(palette);
        QRect r(0, 0, 710, 710);
        r = label->boundingRect();
        label->resize(r.width(), r.height()+segmentSpacing);
        label->move(0, h);
        h += label->height() + segmentSpacing;
        if (maxw < r.width()) {
            maxw = r.width();
        }
    }
    m_container->resize(maxw, h);
}

QPixmap TextArea::getTextImage() const
{
    return m_container->grab();
}
