#ifndef MAINMARKTOOL_H
#define MAINMARKTOOL_H

#include <QWidget>

class QPushButton;
class QLabel;
class QComboBox;
class MainMarkTool : public QWidget
{
    Q_OBJECT
public:
    explicit MainMarkTool(QWidget *parent = nullptr);

    void setImage(const QImage& image);
    void setSelectedFileName(const QString& name);

signals:
    void enterNext();

public slots:
    void chooseColor();
    void saveFile();
    void blueWhiteChanged(int idx);
    void requireEnterNext();

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    void reset();

private:
    QPushButton*       m_colorBtn;
    QLabel*            m_colorLabel;
    QLabel*            m_instruction;
    QPushButton*       m_saveBtn;
    QPushButton*       m_finishBtn;
    QComboBox*         m_bluewhiteChooser;
    QRect              m_paintRect;
    QColor             m_markColor;
    QImage             m_blueMark;
    QImage             m_whiteMark;
    QImage*            m_leftopMark;
    QImage             m_mainImage;
    QImage             m_markImage;
    QPoint             m_markPoint;
    float              m_scaled;
    QString            m_selectedFileName;
    QString            m_lastDir;
};

#endif // MAINMARKTOOL_H
