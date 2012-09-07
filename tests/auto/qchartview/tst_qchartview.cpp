/****************************************************************************
**
** Copyright (C) 2012 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the Qt Commercial Charts Add-on.
**
** $QT_BEGIN_LICENSE$
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <qchartview.h>
#include <qlineseries.h>
#include <cmath>
#include <tst_definitions.h>
#include <qvalueaxis.h>

QTCOMMERCIALCHART_USE_NAMESPACE


Q_DECLARE_METATYPE(QChart*)
Q_DECLARE_METATYPE(QChartView::RubberBands)
Q_DECLARE_METATYPE(Qt::Key)

class tst_QChartView : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private Q_SLOTS:
    void qchartview_data();
    void qchartview();
    void chart_data();
    void chart();
    void rubberBand_data();
    void rubberBand();

private:
    QChartView* m_view;
};

void tst_QChartView::initTestCase()
{
    //test tracks mouse, give a while to user to relese it
    QTest::qWait(1000);
}

void tst_QChartView::cleanupTestCase()
{
}

void tst_QChartView::init()
{
    m_view = new QChartView(new QChart());
    m_view->chart()->legend()->setVisible(false);
}

void tst_QChartView::cleanup()
{
    delete m_view;
    m_view =0;
}

void tst_QChartView::qchartview_data()
{

}

void tst_QChartView::qchartview()
{
    QVERIFY(m_view->chart());
    QCOMPARE(m_view->rubberBand(), QChartView::NoRubberBand);
    m_view->show();
    QTest::qWaitForWindowShown(m_view);

    delete(new QChartView());

    QChartView view;
    QVERIFY(view.chart());

}

void tst_QChartView::chart_data()
{

    QTest::addColumn<QChart*>("chart");
    QTest::newRow("qchart") << new QChart();
}

void tst_QChartView::chart()
{
    QFETCH(QChart*, chart);
    QChartView* view = new QChartView(chart);
    QCOMPARE(view->chart(), chart);
    delete view;
}

void tst_QChartView::rubberBand_data()
{
    QTest::addColumn<QChartView::RubberBands>("rubberBand");
    QTest::addColumn<int>("Xcount");
    QTest::addColumn<int>("Ycount");

    QTest::addColumn<QPoint>("min");
    QTest::addColumn<QPoint>("max");

    QTest::newRow("HorizonalRubberBand") << QChartView::RubberBands(QChartView::HorizonalRubberBand) << 0 << 1 << QPoint(5,5) << QPoint(5,5);
    QTest::newRow("VerticalRubberBand") <<  QChartView::RubberBands(QChartView::VerticalRubberBand) << 1 << 0 << QPoint(5,5) << QPoint(5,5);
    QTest::newRow("RectangleRubberBand") <<  QChartView::RubberBands(QChartView::RectangleRubberBand) << 1 << 1 << QPoint(5,5) << QPoint(5,5);
}

void tst_QChartView::rubberBand()
{
    QFETCH(QChartView::RubberBands, rubberBand);
    QFETCH(int, Xcount);
    QFETCH(int, Ycount);
    QFETCH(QPoint, min);
    QFETCH(QPoint, max);

    m_view->setRubberBand(rubberBand);

    QCOMPARE(m_view->rubberBand(), rubberBand);

    QLineSeries* line = new QLineSeries();
    *line << QPointF(0, 0) << QPointF(200, 200);

    m_view->chart()->addSeries(line);
    m_view->chart()->createDefaultAxes();
    m_view->show();

    QRectF plotArea = m_view->chart()->plotArea();
    //this is hack since view does not get events otherwise
    m_view->setMouseTracking(true);

    QAbstractAxis* axisY = m_view->chart()->axisY();
    QSignalSpy spy0(axisY, SIGNAL(rangeChanged(qreal,qreal)));
    QAbstractAxis* axisX = m_view->chart()->axisX();
    QSignalSpy spy1(axisX, SIGNAL(rangeChanged(qreal,qreal)));


    QValueAxis* vaxisX = qobject_cast<QValueAxis*>(axisX);
    QValueAxis* vaxisY = qobject_cast<QValueAxis*>(axisY);

    int minX = vaxisX->min();
    int minY = vaxisY->min();
    int maxX = vaxisX->max();
    int maxY = vaxisY->max();

    QTest::qWaitForWindowShown(m_view);
    QTest::mouseMove(m_view->viewport(),  min + plotArea.topLeft().toPoint());
    QTest::qWait(2000);
    QTest::mousePress(m_view->viewport(), Qt::LeftButton, 0,  min + plotArea.topLeft().toPoint());
    QTest::qWait(2000);
    QTest::mouseMove(m_view->viewport(),  plotArea.bottomRight().toPoint() - max);
    QTest::qWait(2000);
    QTest::mouseRelease(m_view->viewport(), Qt::LeftButton, 0, plotArea.bottomRight().toPoint() - max);

    TRY_COMPARE(spy0.count(), Xcount);
    TRY_COMPARE(spy1.count(), Ycount);

    //this is hack since view does not get events otherwise
    m_view->setMouseTracking(false);

    QVERIFY(vaxisX->min() >= minX );
    QVERIFY(vaxisX->max() <= maxX );
    QVERIFY(vaxisY->min() >= minY );
    QVERIFY(vaxisY->max() <= maxY );

}

QTEST_MAIN(tst_QChartView)
#include "tst_qchartview.moc"
