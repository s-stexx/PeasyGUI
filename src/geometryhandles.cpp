// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "geometryhandles.h"
#include "control.h"
#include <QMouseEvent>
#include <QCursor>


GeometryHandles* GeometryHandles::s_instance = nullptr;

GeometryHandles* GeometryHandles::instance(QWidget* parent)
{
    if (!s_instance) {
        s_instance = new GeometryHandles(parent);
    }

    return s_instance;
}


GeometryHandles::GeometryHandles(QWidget* parent)
    : QObject(parent)
{
    parentWindowWidget = parent;
    for (int i = 0; i < 9; ++i) {
        QLabel* handle = new QLabel(parent);
        handle->setFixedSize(8, 8);
        handle->setStyleSheet("background-color: darkblue;");
        handle->installEventFilter(this);
        handle->hide();
        handles.append(handle);
    }
}


void GeometryHandles::updateHandlesGeometry()
{
    QRect rect = targetControlWidget->geometry();
    for (int i = 0; i < 9; ++i) {
        handles[i]->move(getRectForHandle(i, rect).topLeft());
    }
}


QRect GeometryHandles::getRectForHandle(int index, const QRect& rect) const
{
    int x = rect.x(), y = rect.y(), w = rect.width(), h = rect.height();
    int cx = x + w / 2 - 4, cy = y + h / 2 - 4;

    switch (index) {
        case Center: return QRect(cx, cy, 8, 8);
        case TopLeft: return QRect(x, y, 8, 8);
        case Top: return QRect(cx, y, 8, 8);
        case TopRight: return QRect(x + w - 8, y, 8, 8);
        case Right: return QRect(x + w - 8, cy, 8, 8);
        case BottomRight: return QRect(x + w - 8, y + h - 8, 8, 8);
        case Bottom: return QRect(cx, y + h - 8, 8, 8);
        case BottomLeft: return QRect(x, y + h - 8, 8, 8);
        case Left: return QRect(x, cy, 8, 8);
        default: return QRect();
    }
}


void GeometryHandles::setCursorForHandle(int index)
{
    Qt::CursorShape cursor;

    switch (index) {
        case Center: cursor = Qt::SizeAllCursor; break;
        case TopLeft: case BottomRight: cursor = Qt::SizeFDiagCursor; break;
        case TopRight: case BottomLeft: cursor = Qt::SizeBDiagCursor; break;
        case Top: case Bottom: cursor = Qt::SizeVerCursor; break;
        case Left: case Right: cursor = Qt::SizeHorCursor; break;
        default: cursor = Qt::ArrowCursor; break;
    }

    parentWindowWidget->setCursor(cursor);
}


bool GeometryHandles::eventFilter(QObject* watched, QEvent* event)
{
    QLabel* handle = qobject_cast<QLabel*>(watched);
    if (!targetControlWidget || !handle) {
        return false;
    }

    if (event->type() == QEvent::Enter) {
        int index = handles.indexOf(handle);
        setCursorForHandle(index);

    } else if (event->type() == QEvent::Leave) {
        parentWindowWidget->setCursor(Qt::ArrowCursor);

    } else if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->button() == Qt::LeftButton) {
            dragging = true;
            activeHandle = static_cast<QLabel*>(watched);
            activeHandle->grabMouse();
            dragStartPos = mouseEvent->globalPosition().toPoint();
            originalGeometry = targetControlWidget->geometry();
            mouseClickOnHandle();
        }

    } else if (event->type() == QEvent::MouseButtonRelease) {
        dragging = false;
        static_cast<QWidget*>(watched)->releaseMouse();
        if (activeHandle) {
            activeHandle->releaseMouse();
        }
        activeHandle = nullptr;
        mouseClickOnHandle();

    } else if (event->type() == QEvent::MouseMove && dragging && watched == activeHandle) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint delta = mouseEvent->globalPosition().toPoint() - dragStartPos;
        QRect newGeom = originalGeometry;
        HandlePosition pos = static_cast<HandlePosition>(handles.indexOf(activeHandle));

        switch (pos) {
            case Center: newGeom.translate(delta); break;
            case TopLeft: newGeom.setTopLeft(originalGeometry.topLeft() + delta); break;
            case Top: newGeom.setTop(originalGeometry.top() + delta.y()); break;
            case TopRight: newGeom.setTopRight(originalGeometry.topRight() + delta); break;
            case Right: newGeom.setRight(originalGeometry.right() + delta.x()); break;
            case BottomRight: newGeom.setBottomRight(originalGeometry.bottomRight() + delta); break;
            case Bottom: newGeom.setBottom(originalGeometry.bottom() + delta.y()); break;
            case BottomLeft: newGeom.setBottomLeft(originalGeometry.bottomLeft() + delta); break;
            case Left: newGeom.setLeft(originalGeometry.left() + delta.x()); break;
        }
        if (newGeom.x() < 0) newGeom.setX(0);
        if (newGeom.y() < 0) newGeom.setY(0);
        if (newGeom.width() < 20) newGeom.setWidth(20);
        if (newGeom.height() < 20) newGeom.setHeight(20);

        targetControlWidget->setGeometry(newGeom);
        updateHandlesGeometry();
    }

    return false;
}


bool GeometryHandles::isDragging() const
{
    return dragging;
}


void GeometryHandles::showOrHideGeometryHandles(QWidget* hoveredWidget)
{
    if (hoveredWidget && hoveredWidget != targetControlWidget) {
        showHandlesFor(hoveredWidget);
    } else if (!hoveredWidget && targetControlWidget) {
        hideHandles();
    }
}


void GeometryHandles::showHandlesFor(QWidget* target)
{
    targetControlWidget = target;
    updateHandlesGeometry();
    for (QLabel* handle : handles) {
        handle->show();
        handle->raise();
    }
}


void GeometryHandles::hideHandles()
{
    for (QLabel* handle : handles) {
        handle->hide();
    }

    targetControlWidget = nullptr;
    parentWindowWidget->setCursor(Qt::ArrowCursor);
}

void GeometryHandles::mouseClickOnHandle()
{
    Control* control = dynamic_cast<Control*>(targetControlWidget);
    if (control) {
        emit handleClicked(control->getId());
    }
}
