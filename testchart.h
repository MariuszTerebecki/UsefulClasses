#ifndef TESTCHART_H
#define TESTCHART_H

#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QGraphicsSceneWheelEvent>
#include <QWheelEvent>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QDateTimeAxis>
#include <QDateTime>
#include <QLineSeries>
#include <QRandomGenerator>
#include <QGraphicsScene>
#include "chartcomment.h"

class TestChart : public QChart {
    Q_OBJECT

public:
    explicit TestChart(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = {})
        : QChart(parent, wFlags)
        , axisX(new QDateTimeAxis(this))
        , axisY(new QValueAxis(this))
    {
        // Inicjalizacja osi
        axisX->setFormat("yyyy-MM-dd HH:mm");
        axisX->setTitleText("Czas");
        axisY->setTitleText("Wartość");

        // Dodanie osi do wykresu
        addAxis(axisX, Qt::AlignBottom);
        addAxis(axisY, Qt::AlignLeft);

        // Zakres początkowy
        axisX->setRange(QDateTime::currentDateTime().addDays(-2), QDateTime::currentDateTime().addDays(+2));
        axisY->setRange(-10, 10);

        addRandomLineSerie();
    }

    void addRandomLineSerie() {
        qint64 xMin = axisX->min().toMSecsSinceEpoch();
        qint64 xMax = axisX->max().toMSecsSinceEpoch();

        qint64 xRange = xMax - xMin;
        qreal xStep = xRange / 50.0;

        QLineSeries *xSeries = new QLineSeries();

        for (qint64 x = xMin; x <= xMax; x += xStep) {
            xSeries->append(x, getRandomQReal(-10, 10));
        }

        this->addSeries(xSeries);
        xSeries->attachAxis(axisX);
        xSeries->attachAxis(axisY);
    }

protected:
    // Obsługa przewijania myszą (zoom)
    void wheelEvent(QGraphicsSceneWheelEvent *event) override {
        double scalePercentage = (event->delta() > 0) ? -5.0 : 5.0;

        if (event->modifiers() & Qt::ControlModifier) {
            adjustDateTimeAxisRange(scalePercentage);
        } else {
            adjustValueAxisRange(scalePercentage);
        }

        // QList<ChartComment *> commentsList = getChartComments(this);

        // foreach (ChartComment *comment, commentsList) {
        //     comment->updatePosition();
        // }
        // scene()->update();  // Pełne odświeżenie sceny

        event->accept();

    }

    QList<ChartComment *> getChartComments(QChart *chart) {
        QList<ChartComment *> commentsList;

        // Pobierz wszystkie elementy ze sceny powiązanej z wykresem
        for (QGraphicsItem *item : chart->scene()->items()) {
            // Spróbuj dynamicznie przekonwertować element na ChartComment
            ChartComment *comment = dynamic_cast<ChartComment *>(item);
            if (comment) {
                commentsList.append(comment);  // Dodaj do listy, jeśli to ChartComment
            }
        }

        return commentsList;
    }


    // Obsługa przesuwania wykresu myszą
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        lastMousePosition = event->pos();  // Zapisanie pozycji kliknięcia
        event->accept();
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override {
        QPointF delta = event->pos() - lastMousePosition;  // Obliczenie przesunięcia
        lastMousePosition = event->pos();

        // Przesunięcie zakresu osi X
        qint64 xMin = axisX->min().toMSecsSinceEpoch();
        qint64 xMax = axisX->max().toMSecsSinceEpoch();
        qint64 xRange = xMax - xMin;

        qint64 xShift = static_cast<qint64>(-delta.x() / plotArea().width() * xRange);  // Przesunięcie w ms

        axisX->setRange(
            QDateTime::fromMSecsSinceEpoch(xMin + xShift),
            QDateTime::fromMSecsSinceEpoch(xMax + xShift)
            );

        // Przesunięcie zakresu osi Y
        double yMin = axisY->min();
        double yMax = axisY->max();
        double yShift = delta.y() / plotArea().height() * (yMax - yMin);

        axisY->setRange(yMin + yShift, yMax + yShift);
        event->accept();
    }

    void keyPressEvent(QKeyEvent *event) override {
        if (scene() && scene()->focusItem()) {
            QGraphicsItem *item = scene()->focusItem();
            QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem *>(item);
            if (textItem) {
                if (event->key() == Qt::Key_Escape) {
                    qDebug() << "ESCAPE - zakończenie edycji";
                    textItem->setTextInteractionFlags(Qt::NoTextInteraction);
                    textItem->clearFocus();
                } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
                    qDebug() << "ENTER - zakończenie edycji i zapisanie tekstu";
                    textItem->setTextInteractionFlags(Qt::NoTextInteraction);
                    textItem->clearFocus();
                } else {
                    QChart::keyPressEvent(event);  // Domyślne zachowanie
                }
            }
        } else {
            QChart::keyPressEvent(event);
        }
    }


private:
    QDateTimeAxis *axisX;
    QValueAxis *axisY;
    QPointF lastMousePosition;  // Przechowuje ostatnią pozycję myszy podczas przesuwania

    void adjustDateTimeAxisRange(double percentage) {
        QDateTime minTime = axisX->min();
        QDateTime maxTime = axisX->max();

        qint64 range = maxTime.toMSecsSinceEpoch() - minTime.toMSecsSinceEpoch();
        qint64 adjustment = static_cast<qint64>(range * (percentage / 100.0));

        QDateTime newMin = QDateTime::fromMSecsSinceEpoch(minTime.toMSecsSinceEpoch() - adjustment);
        QDateTime newMax = QDateTime::fromMSecsSinceEpoch(maxTime.toMSecsSinceEpoch() + adjustment);

        if (newMin > newMax) {
            std::swap(newMin, newMax);
        }

        axisX->setMin(newMin);
        axisX->setMax(newMax);
    }

    void adjustValueAxisRange(double percentage) {
        double minValue = axisY->min();
        double maxValue = axisY->max();

        double range = maxValue - minValue;
        double adjustment = range * (percentage / 100.0);

        double newMin = minValue - adjustment;
        double newMax = maxValue + adjustment;

        if (newMin > newMax) {
            std::swap(newMin, newMax);
        }

        axisY->setMin(newMin);
        axisY->setMax(newMax);
    }

    qreal getRandomQReal(qreal minValue, qreal maxValue) {
        return minValue + (maxValue - minValue) * QRandomGenerator::global()->generateDouble();
    }
};

#endif // TESTCHART_H


