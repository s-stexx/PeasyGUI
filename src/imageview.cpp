// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "imageview.h"

#include "apppaths.h"

#include <QImageReader>
#include <QPainter>
#include <QMouseEvent>

ImageView::ImageView(QWidget *parent) : QLabel(parent)
{
    setMouseTracking(true);

    opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(1.0);
    propsDict["Opacity"] = "1.0";
    this->setGraphicsEffect(opacityEffect);
}


void ImageView::setSpecificProp(const QString& prop, const QString& value)
{
    if (prop == "Image") {
        propsDict["Image"] = value;
        propsDict["Offsets"] = "0,0";
        loadImage(value);

    } else if (prop == "Scaling") {
        scale(value);

    } else if (prop == "Offsets") {
        propsDict["Offsets"] = value;
        moveByOffset(value);

    } else if (prop == "Opacity") {
        propsDict["Opacity"] = value;
        bool ok;
        double doubleValue = value.toDouble(&ok);
        if (ok) {
            opacityEffect->setOpacity(doubleValue);
        }

    }
}


QString ImageView::getSpecificProp(const QString& prop)
{
    QString value;

    if (prop == "Image") {
        value = propsDict["Image"];

    } else if (prop == "Scaling") {
        value = getCurrentScaling();

    } else if (prop == "Offsets") {
        value = propsDict["Offsets"];

    } else if (prop == "Opacity") {
        value = propsDict["Opacity"];

    } else if (prop == "Dimensions") {
        value = QString::number(image.width()) + "," + QString::number(image.height());
    }

    return value;
}


void ImageView::loadImage(const QString &path)
{
    QImageReader reader(AppPaths::cleanAbsPath(path));

    // rotate image, if written so in JPG Exif data
    reader.setAutoTransform(true);
    image = QPixmap::fromImage(reader.read());
    scalingMode = "AutoShrink";
    computeScaling(true);
    offset = QPoint(0, 0);

    update();
}


double ImageView::computeScaling(bool apply)
{
    if (image.isNull())
        return 1.0;

    if (scalingMode == "AutoShrink") {
        double scaleX = double(width()) / image.width();
        double scaleY = double(height()) / image.height();
        double shrink = qMin(scaleX, scaleY);
        double result = qMin(shrink, 1.0);
        if (apply)
            scaling = result;
        return result;
    }

    return scaling;
}


void ImageView::scale(const QString &strScaling)
{
    scalingMode = strScaling;

    if (scalingMode == "AutoShrink") {
        computeScaling(true);
    } else {
        double val = strScaling.toDouble();
        if (val > 0) {
            scaling = val;
        }
    }

    update();
}


QString ImageView::getCurrentScaling()
{
    double effScaling = computeScaling(false);

    // Magic number must match to String "1.000000"
    return QString::number(effScaling, 'f', 6);
}


void ImageView::moveByOffset(const QString &offsetStr)
{
    QStringList parts = offsetStr.split(',');

    double x = parts[0].toDouble();
    double y = parts[1].toDouble();
    offset = QPointF(x, y);

    update();
}


void ImageView::mousePressEvent(QMouseEvent *event)
{
    if (propsDict["Events"].contains("ClickedAtPos")) {
        int x = qFloor(event->position().x());
        int y = qFloor(event->position().y());

        emit imageViewEvent(propsDict["Name"],
                            "ClickedAtPos`" +
                            QString::number(x) + "," +
                            QString::number(y)
                            );

        // Good debug: Draw a point at clicked position
        // if (!image.isNull()) {
        //     QPainter painter(&image);
        //     painter.setPen(QColor(128, 0, 128));
        //     painter.drawPoint(x,y);
        //     painter.end();
        //     update();
        // }
    }
}


void ImageView::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    if (image.isNull()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    qreal dpr = devicePixelRatioF();
    double effScaling = computeScaling(false);
    QSize targetSize = image.size() * effScaling * dpr;

    QPixmap pix = image.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pix.setDevicePixelRatio(dpr);

    QPointF drawPos = QPointF(0, 0) - (offset * computeScaling(false));
    painter.drawPixmap(drawPos.toPoint(), pix);

}
