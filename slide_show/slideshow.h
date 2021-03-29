#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include "interactive_buttons/interactivebuttonbase.h"

#define CREATE_SHADOW(x)                                                      \
    do {                                                                      \
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(x); \
        effect->setOffset(3, 3);                                              \
        effect->setBlurRadius(12);                                            \
        effect->setColor(QColor(64, 64, 64, 64));                         \
        x->setGraphicsEffect(effect);                                         \
    } while (0)

class SlideShow : public QWidget
{
    Q_OBJECT
public:
    explicit SlideShow(QWidget *parent = nullptr);

    void setPixmapSize(QSize size);
    void setPixmapScale(bool scale);

    void addPixmap(const QPixmap& pixmap);
    void setCurrentIndex(int index);

private:
    QPixmap getScaledRoundedPixmap(QPixmap pixmap) const;
    void adjustLabels();
    void moveTo(QLabel* label, QRect geometry);

protected:
    void resizeEvent(QResizeEvent *event) override;

public slots:

private:
    QList<QLabel*> labels;
    QList<QPixmap> pixmaps;
    QList<InteractiveButtonBase*> indications;
    int currentIndex = 0;

    QColor normalColor = QColor(172, 128, 58);
    QColor selectColor = Qt::yellow;

    QSize oneSize;            // 默认图片大小
    double backScale = 0.8;   // 两侧卡片的缩放程度
    double sideOffside = 0.5; // 两侧卡片的位置偏移
    bool scalePixmap = false; // 图片居中裁剪还是大小缩放
    QHBoxLayout* indicationLayout;
};

#endif // SLIDESHOW_H
