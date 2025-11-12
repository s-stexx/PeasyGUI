// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef GEOMETRYHANDLES_H
#define GEOMETRYHANDLES_H

#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QPoint>
#include <QList>

class GeometryHandles : public QObject
{
    Q_OBJECT

public:
    static GeometryHandles* instance(QWidget* parent = nullptr);
    void showHandlesFor(QWidget* target);
    void hideHandles();
    bool isDragging() const;
    void showOrHideGeometryHandles(QWidget *hovered);

signals:
    void handleClicked(int id);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    explicit GeometryHandles(QWidget* parent = nullptr);
    void updateHandlesGeometry();
    QRect getRectForHandle(int index, const QRect& rect) const;
    void setCursorForHandle(int index);
    void mouseClickOnHandle();


    QWidget* parentWindowWidget;
    QWidget* targetControlWidget = nullptr;
    QList<QLabel*> handles;
    QPoint dragStartPos;
    QRect originalGeometry;
    bool dragging = false;
    QLabel* activeHandle = nullptr;

    static GeometryHandles* s_instance;

    enum HandlePosition {
        Center = 0,
        TopLeft,
        Top,
        TopRight,
        Right,
        BottomRight,
        Bottom,
        BottomLeft,
        Left
    };
};


#endif // GEOMETRYHANDLES_H
