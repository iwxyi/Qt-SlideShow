#include "slideshow.h"

SlideShow::SlideShow(QWidget *parent) : QWidget(parent)
{
    setPixmapSize(QSize(400, 150));

    indicationLayout = new QHBoxLayout;
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch(1);
    mainLayout->addLayout(indicationLayout);
    indicationLayout->setAlignment(Qt::AlignCenter);
}

void SlideShow::setPixmapSize(QSize size)
{
    this->oneSize = size;
    this->setMinimumSize(int(oneSize.width() * (3 - backScale*2) + 6),
                         oneSize.height() + 6);
    for (int i = 0; i < labels.size(); i++)
    {
        labels.at(i)->setPixmap(getScaledRoundedPixmap(pixmaps.at(i)));
        labels.at(i)->setMinimumSize(1, 1);
        labels.at(i)->setMaximumSize(oneSize);
        labels.at(i)->resize(oneSize);
    }
}

void SlideShow::setPixmapScale(bool scale)
{
    this->scalePixmap = scale;
    for (int i = 0; i < labels.size(); i++)
        labels.at(i)->setPixmap(getScaledRoundedPixmap(pixmaps.at(i)));
}

void SlideShow::addPixmap(const QPixmap &pixmap)
{
    QLabel* label = new QLabel(this);
    label->setScaledContents(true);
    labels.append(label);

    pixmaps.append(pixmap); // 添加原图
    label->setPixmap(getScaledRoundedPixmap(pixmap));
    label->setMinimumSize(1, 1);
    label->resize(oneSize);
    CREATE_SHADOW(label);

    InteractiveButtonBase* btn = new InteractiveButtonBase(this);
    btn->setFixedSize(8, 8);
    btn->setRadius(4);
    btn->setNormalColor(normalColor);
    btn->setHoverColor(selectColor);
    indications.append(btn);
    indicationLayout->addWidget(btn);

    int index = indications.size() - 1;
    connect(btn, &InteractiveButtonBase::signalMouseEnter, this, [=]{
        setCurrentIndex(index);
    });

    for (int i = 0; i < indications.size(); i++)
        indications.at(i)->raise();
}

void SlideShow::setCurrentIndex(int index)
{
    int count = labels.size();
    bool leftToRight = currentIndex < index;
    if (currentIndex < indications.size())
        indications.at(currentIndex)->setNormalColor(normalColor);

    // 判断是否连续的
    bool bro = qAbs(index - currentIndex) == 1
            || (index == 0 && currentIndex == count-1)
            || (index == count-1 && currentIndex == 0);

    // 先提前升好（不然会露出其他背景）

    // 先提升目标
    labels.at(index)->raise();
    if (bro) // 如果是非连续，就不用还原了
    {
        // 当前的布局还原，动画消失
        if (!leftToRight)
            labels.at((currentIndex + 1) % count)->raise();
        labels.at((currentIndex + count - 1) % count)->raise();
        if (leftToRight)
            labels.at((currentIndex + 1) % count)->raise();
        labels.at(currentIndex)->raise();
    }
    for (int i = 0; i < indications.size(); i++)
        indications.at(i)->raise();

    // 开始切换
    currentIndex = index;
    adjustLabels();

    // 延迟raise
    QTimer::singleShot(100, [=]{
        if (leftToRight)
            labels.at((index + 1) % count)->raise();
        labels.at((index + count - 1) % count)->raise();
        if (!leftToRight)
            labels.at((index + 1) % count)->raise();
        labels.at(index)->raise();

        for (int i = 0; i < indications.size(); i++)
        {
            indications.at(i)->raise();
            indications.at(i)->setNormalColor(normalColor);
        }
        indications.at(index)->setNormalColor(selectColor);
    });
}

QPixmap SlideShow::getScaledRoundedPixmap(QPixmap pixmap) const
{
    // 缩放
    pixmap = pixmap.scaled(oneSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    // 裁剪
    if (!scalePixmap)
    {
        if (pixmap.width() > oneSize.width())
        {
            pixmap = pixmap.copy(pixmap.width()/2 - oneSize.width()/2, 0,
                                 oneSize.width(), oneSize.height());
        }
        else if (pixmap.height() > oneSize.height())
        {
            pixmap = pixmap.copy(0, pixmap.height()/2 - oneSize.height()/2,
                                 oneSize.width(), oneSize.height());
        }
    }

    // 圆角
    QPixmap dest(pixmap.size());
    dest.fill(Qt::transparent);
    QPainter painter(&dest);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    QRect rect = QRect(0, 0, pixmap.width(), pixmap.height());
    QPainterPath path;
    path.addRoundedRect(rect, 10, 10);
    painter.setClipPath(path);
    painter.drawPixmap(rect, pixmap);
    return dest;
}

void SlideShow::adjustLabels()
{
    if (!labels.size())
        return ;

    // 计算分布
    int sw = width(), sh = height();
    int w = oneSize.width(), h = oneSize.height();
    double scale = backScale;
    int marginTop = sh/2 - h / 2;

    // 计算尺寸
    QRect centerRect(sw/2 - w/2, marginTop, w, h);
    QRect leftRect(int(sw/2 - w*(1-(1-scale)*sideOffside)*3/4),
                   marginTop + int(h * (1 - scale) / 2),
                   int(w*scale),
                   int(h*scale));
    QRect rightRect(int(sw/2 + w*(1-(1-scale)*sideOffside)*3/4 - w*scale),
                    marginTop + int(h * (1 - scale) / 2),
                    int(w*scale),
                    int(h*scale));
    QRect backRect(int(sw/2 - w*scale/2),
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
        moveTo(labels.at(i % count), rect);
    }
}

void SlideShow::moveTo(QLabel *label, QRect geometry)
{
    if (label->geometry() == geometry)
        return ;

    QPropertyAnimation* ani = new QPropertyAnimation(label, "geometry");
    ani->setStartValue(label->geometry());
    ani->setEndValue(geometry);
    ani->setDuration(400);
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

