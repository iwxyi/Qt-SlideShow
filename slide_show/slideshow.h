#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QDebug>
#include "interactive_buttons/interactivebuttonbase.h"

class SlideShow : public QWidget
{
    Q_OBJECT
public:
    explicit SlideShow(QWidget *parent = nullptr);
    void setPixmapSize(QSize size);
    void addPixmap(const QPixmap& pixmap);
    void setCurrentIndex(int index);

private:
    void adjustLabels();
    void moveTo(QLabel* label, QRect geometry);

protected:
    void resizeEvent(QResizeEvent *event) override;

signals:

public slots:

private:
    QList<QLabel*> labels;
    QList<QPixmap> pixmaps;
    QList<InteractiveButtonBase*> indications;
    int currentIndex = 0;

    QColor normalColor = QColor(172, 128, 58);
    QColor selectColor = Qt::yellow;

    QSize oneSize;
    int margin = 9;
    QHBoxLayout* indicationLayout;
};

#endif // SLIDESHOW_H
