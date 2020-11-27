#ifndef LINECHART_H
#define LINECHART_H

#include <QChart>
#include <QtDebug>
#include <QDialog>
#include <QTextStream>
#include <QtCharts/QChartGlobal>
#include <QValueAxis>
#include <QVariant>
#include <functions.h>

using namespace std;

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

typedef QPair<QPointF, QString>Data;
typedef QList<Data>            DataList;
typedef QList<DataList>        DataTable;

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class LineChart;
}

class LineChart : public QDialog {
    Q_OBJECT

public:

    explicit LineChart(QWidget *parent = nullptr);
    ~LineChart();

    void disposeData(QTextStream & in);

    functions function;

private:

    Ui::LineChart *ui;
    QChart* createLineChart(DataTable datatable,
                            QString   name) const;
    DataTable generateData(QTextStream & in);
    qlonglong serial_number;
    qlonglong start_num = -1;
};

#endif // LINECHART_H
