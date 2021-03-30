#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QTimer>
#include "interactive_buttons/interactivebuttonbase.h"
#include "sidehidelabel.h"

#define SHADOW_RADIUS 12

#define CREATE_SHADOW(x)                                                      \
    do {                                                                      \
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(x); \
        effect->setOffset(3, 3);                                              \
        effect->setBlurRadius(SHADOW_RADIUS);                                            \
        effect->setColor(QColor(64, 64, 64, 64));                         \
        x->setGraphicsEffect(effect);                                         \
    } while (0)

class SlideShow : public QWidget
{
    Q_OBJECT
public:
    explicit SlideShow(QWidget *parent = nullptr);

    void setPixmapSize(QSize size);
    void setPixmapScale(bool scale); // 已废弃
    void setAutoSlide(int interval = 5000); // 设置自动轮播，interval=0时停止
    int getCurrentIndex() const;
    void adjustLabels(SideHideLabel* hidingLabel = nullptr);

public slots:
    void addImage(const QPixmap& pixmap, QString text = "");
    void insertImage(int index, const QPixmap& pixmap, QString text = "");
    void removeImage(int index);

    void setCurrentIndex(int index);
    void slideToLeft();
    void slideToRight();

private:
    QPixmap getScaledRoundedPixmap(QPixmap pixmap) const;
    void moveTo(SideHideLabel *label, QRect geometry);

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject * obj, QEvent * event) override;

signals:
    void signalImageClicked(int index);
    void signalTextActivated(QString text);

private:
    QList<SideHideLabel*> labels;
    QList<QPixmap> pixmaps;
    QList<QString> texts;
    QList<InteractiveButtonBase*> indications;
    int currentIndex = -1;
    QTimer* autoSlideTimer;
    SideHideLabel* hidingLabel = nullptr;

    QColor normalColor = QColor(172, 128, 58);
    QColor selectColor = Qt::yellow;

    QSize oneSize;            // 默认图片大小
    double backScale = 0.8;   // 两侧卡片的缩放程度
    double sideOffside = 0.5; // 两侧卡片的位置偏移，越大越靠近中间
    double imgOffside = 0.15; // 图片内偏移比例
    bool scalePixmap = false; // 图片居中裁剪还是大小缩放（现在只能为false了）
    QHBoxLayout* indicationLayout;

    QRect centerRect, leftRect, rightRect, backRect;
};

#endif // SLIDESHOW_H
