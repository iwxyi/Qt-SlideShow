#include <QEvent>
#include "slideshow.h"

SlideShow::SlideShow(QWidget *parent) : QWidget(parent)
{
    setPixmapSize(QSize(400, 150));

    indicationLayout = new QHBoxLayout;
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch(1);
    mainLayout->addLayout(indicationLayout);
    indicationLayout->setAlignment(Qt::AlignCenter);

    autoSlideTimer = new QTimer(this);
    autoSlideTimer->setInterval(5000);
    connect(autoSlideTimer, &QTimer::timeout, this, &SlideShow::slideToRight);
}

void SlideShow::setPixmapSize(QSize size)
{
    this->oneSize = size;
    this->setMinimumSize(int(oneSize.width() * (3 - backScale*2) + SHADOW_RADIUS),
                         oneSize.height() + SHADOW_RADIUS);
    for (int i = 0; i < labels.size(); i++)
    {
        labels.at(i)->setPixmap(getScaledRoundedPixmap(pixmaps.at(i)), oneSize.width() * imgOffside);
        labels.at(i)->setMinimumSize(1, 1);
        labels.at(i)->setMaximumSize(oneSize);
        labels.at(i)->resize(oneSize);
    }
}

void SlideShow::setPixmapScale(bool scale)
{
    this->scalePixmap = scale;
    for (int i = 0; i < labels.size(); i++)
        labels.at(i)->setPixmap(getScaledRoundedPixmap(pixmaps.at(i)), oneSize.width() * imgOffside);
}

void SlideShow::setAutoSlide(int interval)
{
    if (interval)
    {
        autoSlideTimer->setInterval(interval);
        autoSlideTimer->start();
    }
    else
    {
        autoSlideTimer->stop();
    }
}

void SlideShow::addImage(const QPixmap &pixmap, QString text)
{
    insertImage(labels.size(), pixmap, text);
}

void SlideShow::insertImage(int index, const QPixmap &pixmap, QString text)
{
    // 图片
    SideHideLabel* label = new SideHideLabel(this);
    label->setScaledContents(true);
    labels.insert(index, label);
    texts.insert(index, text);

    pixmaps.insert(index, pixmap); // 添加原图
    label->setPixmap(getScaledRoundedPixmap(pixmap), oneSize.width() * imgOffside);
    label->setMinimumSize(1, 1);
    label->resize(oneSize);
    CREATE_SHADOW(label);
    label->show();
    label->installEventFilter(this);

    // 指示球
    InteractiveButtonBase* btn = new InteractiveButtonBase(this);
    btn->setFixedSize(8, 8);
    btn->setRadius(4);
    btn->setNormalColor(normalColor);
    btn->setHoverColor(selectColor);
    indications.insert(index, btn);
    indicationLayout->insertWidget(index, btn);

    connect(btn, &InteractiveButtonBase::signalMouseEnter, this, [=]{
        int index = indications.indexOf(btn);
        setCurrentIndex(index);
    });

    for (int i = 0; i < indications.size(); i++)
        indications.at(i)->raise();

    if (currentIndex > index)
    {
        currentIndex++;
        setCurrentIndex(currentIndex);
    }
}

void SlideShow::removeImage(int index)
{
    labels.takeAt(index)->deleteLater();
    pixmaps.removeAt(index);
    texts.removeAt(index);
    indications.takeAt(index)->deleteLater();

    // 加了阴影，如果没有图片从而没有动画的话，阴影会残留下来
    if (!labels.size())
        update();
    else
    {
        if (currentIndex > index)
            currentIndex--;
        setCurrentIndex(currentIndex);
    }
}

void SlideShow::setCurrentIndex(int index)
{
    int count = labels.size();
    if (index >= count)
        index = count-1;
    if (index < 0)
        index = 0;
    if (index >= count)
        return ;
    if (autoSlideTimer->isActive())
        autoSlideTimer->start();

    bool leftToRight = currentIndex < index;
    if (currentIndex > 0 && currentIndex < count)
        indications.at(currentIndex)->setNormalColor(normalColor);

    // 设置即将离开的label
    SideHideLabel* leavingLabel = nullptr;
    if (currentIndex >= 0 && currentIndex < count)
    {
        leavingLabel = labels.at(currentIndex);
    }

    // 开始切换
    currentIndex = index;
    adjustLabels(leavingLabel);

    // raise界面
    if (leftToRight)  // 从左到右，左边在上
        labels.at((index + 1) % count)->raise();
    labels.at((index + count - 1) % count)->raise();
    if (!leftToRight) // 从右到左，右边在上
        labels.at((index + 1) % count)->raise();
    labels.at(index)->raise();
    if (hidingLabel)
        hidingLabel->raise();

    for (int i = 0; i < indications.size(); i++)
    {
        indications.at(i)->raise();
        indications.at(i)->setNormalColor(normalColor);
    }
    indications.at(index)->setNormalColor(selectColor);
}

int SlideShow::getCurrentIndex() const
{
    return currentIndex;
}

void SlideShow::slideToLeft()
{
    if (!labels.size())
        return ;
    setCurrentIndex((currentIndex + labels.size() - 1) % labels.size());
}

void SlideShow::slideToRight()
{
    if (!labels.size())
        return ;
    setCurrentIndex((currentIndex + 1) % labels.size());
}

QPixmap SlideShow::getScaledRoundedPixmap(QPixmap pixmap) const
{
    // 缩放
    int needWidth = int(oneSize.width() * (1 + imgOffside * 2));
    pixmap = pixmap.scaled(needWidth, oneSize.height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    // 裁剪
    if (/* DISABLES CODE */ (true) || !scalePixmap)
    {
        if (pixmap.width() > needWidth)
        {
            pixmap = pixmap.copy(pixmap.width()/2 - needWidth/2, 0,
                                 needWidth, oneSize.height());
        }
        else if (pixmap.height() > oneSize.height())
        {
            pixmap = pixmap.copy(0, pixmap.height()/2 - oneSize.height()/2,
                                 needWidth, oneSize.height());
        }
    }

    return pixmap;

    // 圆角
    /* QPixmap dest(pixmap.size());
    dest.fill(Qt::transparent);
    QPainter painter(&dest);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    QRect rect = QRect(0, 0, pixmap.width(), pixmap.height());
    QPainterPath path;
    path.addRoundedRect(rect, 10, 10);
    painter.setClipPath(path);
    painter.drawPixmap(rect, pixmap);
    return dest; */
}

void SlideShow::adjustLabels(SideHideLabel *leavingLabel)
{
    if (!labels.size())
        return ;

    // 计算分布
    int sw = width(), sh = height();
    int w = oneSize.width(), h = oneSize.height();
    double scale = backScale;
    int marginTop = sh/2 - h / 2 - SHADOW_RADIUS/2;

    // 计算尺寸
    centerRect = QRect(sw/2 - w/2, marginTop, w, h);
    leftRect = QRect(int(sw/2 - w*(1-(1-scale)*sideOffside)*3/4),
                   marginTop + int(h * (1 - scale) / 2),
                   int(w*scale),
                   int(h*scale));
    rightRect = QRect(int(sw/2 + w*(1-(1-scale)*sideOffside)*3/4 - w*scale),
                    marginTop + int(h * (1 - scale) / 2),
                    int(w*scale),
                    int(h*scale));
    backRect = QRect(int(sw/2 - w*scale/2),
                   marginTop + int(h * (1 - scale) / 2),
                   int(w*scale),
                   int(h*scale));

    int count = labels.size();
    for (int i = currentIndex; i < count + currentIndex; i++)
    {
        QRect rect = backRect;
        if (i == currentIndex)
            rect = centerRect;
        else if (i == currentIndex + 1)
            rect = rightRect;
        else if ((i + 1) % count == currentIndex)
            rect = leftRect;
        auto label = labels.at(i % count);
        moveTo(label, rect);

        // 是否同样进行一个透明度渐变的拷贝动作
        if (leavingLabel && label == leavingLabel)
        {
            hidingLabel = SideHideLabel::copy(leavingLabel);
            auto opaLabel = hidingLabel;

            moveTo(opaLabel, rect);
            opaLabel->show();

            // 设置透明度
            QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(opaLabel);
            effect->setOpacity(1);
            opaLabel->setGraphicsEffect(effect);

            // 透明度动画
            QPropertyAnimation* ani = new QPropertyAnimation(effect, "opacity");
            ani->setStartValue(1);
            ani->setEndValue(0);
            ani->setEasingCurve(QEasingCurve::OutQuad);
            ani->setDuration(200);
            connect(ani, &QPropertyAnimation::finished, this, [=]{
                if (hidingLabel == opaLabel)
                    hidingLabel = nullptr;
                ani->deleteLater();
                opaLabel->deleteLater();
                effect->deleteLater();
            });
            ani->start();
        }
    }
}

void SlideShow::moveTo(SideHideLabel *label, QRect geometry)
{
    if (label->geometry() == geometry)
        return ;

    QPropertyAnimation* ani = new QPropertyAnimation(label, "geometry");
    ani->setStartValue(label->geometry());
    ani->setEndValue(geometry);
    ani->setDuration(300);
    ani->setEasingCurve(QEasingCurve::OutQuad);
    connect(ani, SIGNAL(finished()), ani, SLOT(deleteLater()));
    ani->start();

    ani = new QPropertyAnimation(label, "sideOffset");
    ani->setStartValue(label->getSideOffset());
    double offset = 0;
    if (geometry.x() == leftRect.x())
        offset = -label->getMaxOffset();
    else if (geometry.x() == rightRect.x())
        offset = label->getMaxOffset();
    ani->setEndValue(offset);
    ani->setDuration(500);
    ani->setEasingCurve(QEasingCurve::OutQuad);
    connect(ani, SIGNAL(finished()), ani, SLOT(deleteLater()));
    ani->start();
}

void SlideShow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 调整已有图片的大小
    adjustLabels();
}

bool SlideShow::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        if (labels.contains(static_cast<SideHideLabel*const>(obj)))
        {
            // 图片被单击
            int index = labels.indexOf(static_cast<SideHideLabel*const>(obj));
            if (currentIndex == index) // 正面图片被单击
            {
                emit signalImageClicked(index);
                emit signalTextActivated(texts.at(index));
            }
            else // 不是当前图片，可能是动画或者两侧的
                setCurrentIndex(index);
        }
    }

    return  QWidget::eventFilter(obj, event);
}

