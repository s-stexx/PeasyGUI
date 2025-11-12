// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QLabel>
#include <QGraphicsOpacityEffect>
#include "control.h"

class ImageView : public QLabel, public Control
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = nullptr);

    void setSpecificProp(const QString& prop, const QString& value) override;
    QString getSpecificProp(const QString& prop) override;

signals:
    void imageViewEvent(const QString &name, const QString &event);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap image;
    QImage scaledImage;
    QGraphicsOpacityEffect *opacityEffect = nullptr;

    double scaling = 1.0;
    QString scalingMode = "AutoShrink";
    QPointF offset = QPointF(0, 0);

    double computeScaling(bool apply);
    void loadImage(const QString &path);

    QString getCurrentScaling();
    void scale(const QString &strScaling);

    void moveByOffset(const QString &offset);
};

#endif // IMAGEVIEW_H

