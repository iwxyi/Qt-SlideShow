#include "slideshow.h"

SlideShow::SlideShow(QWidget *parent) : QWidget(parent)
{
    setPixmapSize(QSize(400, 100));

    indicationLayout = new QHBoxLayout;
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch(1);
    mainLayout->addLayout(indicationLayout);
    indicationLayout->setAlignment(Qt::AlignCenter);
}

void SlideShow::setPixmapSize(QSize size)
{
    this->oneSize = size;
    this->setMinimumSize(oneSize.width() * 5 / 4 + margin * 2, oneSize.height() + margin * 2);
    for (int i = 0; i < labels.size(); i++)
    {
        pixmaps[i] = pixmaps.at(i).scaled(oneSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        labels.at(i)->setPixmap(pixmaps.at(i));
        labels.at(i)->setMinimumSize(1, 1);
        labels.at(i)->setMaximumSize(oneSize);
        labels.at(i)->resize(oneSize);
    }
}

void SlideShow::addPixmap(const QPixmap &pixmap)
{
    QLabel* label = new QLabel(this);
    labels.append(label);

    pixmaps.append(pixmap); // 添加原图
    pixmap.scaled(oneSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    label->setPixmap(pixmap);
    label->setMinimumSize(1, 1);
    label->resize(oneSize);

    InteractiveButtonBase* btn = new InteractiveButtonBase(this);
    btn->setFixedSize(8, 8);
    btn->setRadius(4);
    btn->setNormalColor(normalColor);
    btn->setHoverColor(selectColor);
    indications.append(btn);
    indicationLayout->addWidget(btn);

    int index = indications.size() - 1;
    connect(btn, &InteractiveButtonBase::clicked, this, [=]{
        setCurrentIndex(index);
    });

    for (int i = 0; i < indications.size(); i++)
        indications.at(i)->raise();
}

void SlideShow::setCurrentIndex(int index)
{
    if (currentIndex < indications.size())
        indications.at(currentIndex)->setNormalColor(normalColor);
    currentIndex = index;
    adjustLabels();

    labels.at(index)->raise();
    for (int i = 0; i < indications.size(); i++)
        indications.at(i)->raise();
    indications.at(index)->setNormalColor(selectColor);
}

void SlideShow::adjustLabels()
{
    if (!labels.size())
        return ;

    // 计算分布
    int sw = width(), sh = height();
    int w = oneSize.width(), h = oneSize.height();
    double scale = 0.8;

    // 计算尺寸
    QRect centerRect(sw/2 - w/2, margin, w, h);
    QRect leftRect(sw/2 - w*3/4,
                   margin + int(h * (1 - scale) / 2),
                   int(w*scale),
                   int(h*scale));
    QRect rightRect(int(sw/2 + w*3/4 - w*scale),
                    margin + int(h * (1 - scale) / 2),
                    int(w*scale),
                    int(h*scale));
    QRect backRect(int(sw/2 - w*scale/2),
                   margin + int(h * (1 - scale) / 2),
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
    ani->setDuration(300);
    connect(ani, SIGNAL(finished()), ani, SLOT(deleteLater()));
    ani->start();
}

void SlideShow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 调整已有图片的大小
    adjustLabels();
}

