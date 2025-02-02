#ifndef CHARTCOMMENT_H
#define CHARTCOMMENT_H

#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QPen>
#include <QBrush>
#include <QtCharts/QChart>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QTimer>

// Klasa reprezentująca komentarz na wykresie
class ChartComment : public QObject, public QGraphicsItemGroup {
    Q_OBJECT

private:
    QGraphicsEllipseItem *m_point;
    QGraphicsTextItem *m_label;
    QGraphicsLineItem *m_line;

    QPointF m_valuePos;
    QPointF m_labelOffset;
    QChart *m_chart;

    bool m_isDraggingPoint = false;
    bool m_isDraggingLabel = false;

    QPointF m_labelDragOffset;

public:
    explicit ChartComment(QPointF valuePos, const QString &text, QChart *chart)
        : QObject(nullptr)
        , m_valuePos(valuePos)
        , m_chart(chart)
    {
        QPointF pointPos = chart->mapToPosition(m_valuePos);

        m_point = new QGraphicsEllipseItem(-5, -5, 10, 10);
        m_point->setBrush(QBrush(Qt::red));
        m_point->setPen(QPen(Qt::black));
        m_point->setPos(pointPos);
        m_point->setFlag(QGraphicsItem::ItemIsMovable);
        m_point->setZValue(51);

        m_label = new QGraphicsTextItem(text);
        m_label->setFlag(QGraphicsItem::ItemIsMovable);
        m_label->setDefaultTextColor(Qt::blue);
        m_labelOffset = QPointF(20, -10);
        m_label->setPos(pointPos + m_labelOffset);
        m_label->setZValue(51);

        m_line = new QGraphicsLineItem();
        m_line->setPen(QPen(Qt::black, 1, Qt::DashLine));
        m_line->setZValue(50);
        updateLine();  // Pierwsza aktualizacja linii

        addToGroup(m_point);
        addToGroup(m_label);
        addToGroup(m_line);

        chart->scene()->addItem(this);

        connect(chart, &QChart::plotAreaChanged, this, [this]() {
            QMetaObject::invokeMethod(this, "updatePosition", Qt::QueuedConnection);
        });

        QValueAxis *xAxis = qobject_cast<QValueAxis *>(chart->axisX());
        if (xAxis) {
            connect(xAxis, &QValueAxis::rangeChanged, this, &ChartComment::updatePosition);
        }

        QValueAxis *yAxis = qobject_cast<QValueAxis *>(chart->axisY());
        if (yAxis) {
            connect(yAxis, &QValueAxis::rangeChanged, this, &ChartComment::updatePosition);
        }

    }

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override {

        QPointF localPos = m_label->mapFromScene(event->scenePos());
        if (m_label->contains(localPos)) {

            QMenu menu;

            QAction *editAction = menu.addAction("Edit");
            QAction *deleteAction = menu.addAction("Delete");

            QAction *selectedAction = menu.exec(event->screenPos());

            if (selectedAction == editAction) {
                // enterEditMode();
            } else if (selectedAction == deleteAction) {
                deleteComment();
            }
        }
    }

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            if (m_point->contains(event->pos() - m_point->pos())) {
                qDebug() << "Rozpoczęcie przeciągania punktu";
                m_isDraggingPoint = true;
                event->accept();
            } else if (m_label->contains(event->pos() - m_label->pos())) {
                m_isDraggingLabel = true;
                m_labelDragOffset = event->pos() - m_label->pos();  // Zapamiętaj przesunięcie
                event->accept();
            }
        }
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override {
        if (m_isDraggingPoint) {
            m_point->setPos(event->scenePos());
            m_label->setPos(m_point->pos() + m_labelOffset);
            updateLine();
        } else if (m_isDraggingLabel) {
            // Przesunięcie etykiety względem zapamiętanego offsetu
            m_label->setPos(event->scenePos() - m_labelDragOffset);
            m_labelOffset = m_label->pos() - m_point->pos();
            updateLine();
        }
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            if (m_isDraggingPoint) {
                m_valuePos = m_chart->mapToValue(m_point->pos());
                m_isDraggingPoint = false;
                event->accept();
            } else if (m_isDraggingLabel) {
                m_isDraggingLabel = false;
                event->accept();
            }
        }
    }
    // void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private slots:
    void updateLine() {
        QPointF startPoint = m_point->mapToScene(m_point->boundingRect().center());
        QRectF labelRect = m_label->boundingRect().translated(m_label->scenePos());
        QPointF endPoint = labelRect.center();

        QLineF lineToCenter(startPoint, endPoint);
        QPointF intersectionPoint;
        for (int i = 0; i < 4; ++i) {
            QLineF side(labelRect.topLeft(), labelRect.topRight());
            if (i == 1) side = QLineF(labelRect.topRight(), labelRect.bottomRight());
            if (i == 2) side = QLineF(labelRect.bottomRight(), labelRect.bottomLeft());
            if (i == 3) side = QLineF(labelRect.bottomLeft(), labelRect.topLeft());
            if (side.intersects(lineToCenter, &intersectionPoint) == QLineF::BoundedIntersection) {
                endPoint = intersectionPoint;
                break;
            }
        }

        m_line->setLine(QLineF(startPoint, endPoint));
    }

    void updatePosition() {
        QPointF newPointPos = m_chart->mapToPosition(m_valuePos);
        m_point->setPos(newPointPos);
        m_label->setPos(newPointPos + m_labelOffset);
        updateLine();
    }

    void deleteComment() {
        m_chart->scene()->removeItem(this);
        delete this;
    }
};

#endif // CHARTCOMMENT_H
