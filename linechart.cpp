#include "linechart.h"
#include "ui_linechart.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QLegend>

LineChart::LineChart(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LineChart)
{
    ui->setupUi(this);
}

LineChart::~LineChart()
{
    delete ui;
}

void LineChart::disposeData(QTextStream& in)
{
    DataTable   m_DataTable = generateData(in);
    QChartView *chartView;
    DataTable   datatable;

    datatable.append(m_DataTable[0]);
    chartView = new QChartView(createLineChart(datatable, "BandA"));
    ui->gridLayout->addWidget(chartView, 1, 0);

    datatable.clear();
    datatable.append(m_DataTable[1]);
    chartView = new QChartView(createLineChart(datatable, "BandB"));
    ui->gridLayout->addWidget(chartView, 1, 1);

    datatable.clear();
    datatable.append(m_DataTable[2]);
    chartView = new QChartView(createLineChart(datatable, "BandC"));
    ui->gridLayout->addWidget(chartView, 1, 2);
}

QChart * LineChart::createLineChart(DataTable datatable, QString name) const
{
    QChart *chart = new QChart();

    chart->setTitle("Band Power RateLine Chart");


    for (const DataList& list : datatable) {
        QLineSeries *series = new QLineSeries(chart);

        for (const Data& data : list) series->append(data.first);
        series->setName(name);
        chart->addSeries(series);
    }

    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(start_num, serial_number);
    chart->axes(Qt::Vertical).first()->setRange(0, 100);

    // Add space to label to add space between labels and axis
    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(


                                                       Qt::Vertical).first());

    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");

    return chart;
}

DataTable LineChart::generateData(QTextStream& in)
{
    DataTable datatable;                                   // 总数据包
    DataList  datalist_K, datalist_S, datalist_C;          // 每一个波段数据值列表
    QPointF   point_K(0, 0), point_S(0, 0), point_C(0, 0); // 每一波段在折线图中的坐标点


    while (1) {
        QString str_line = in.readLine();

        if (!str_line.size()) {
            break;
        }

        if (str_line.split('.', Qt::SkipEmptyParts)[0] == "2020") continue;

        //        if (str_line == "2020.11.26.11.24.16") continue;


        QStringList str_line_split_list = str_line.split('\t');
        serial_number = str_line_split_list[0].toInt(nullptr, 10);
        QStringList values_list = str_line_split_list[1].split(' ',
                                                               Qt::SkipEmptyParts);
        qDebug() << values_list;

        if (serial_number == 750) {
            serial_number = 750;
            serial_number = 750;
        }

        for (int i = 3; i < values_list.size() - 2;
             i += 2) {
            auto a =
                values_list[i].split('x', Qt::SkipEmptyParts)[1].toStdString();
            auto b =
                values_list[i + 1].split('x',
                                         Qt::SkipEmptyParts)[1].toStdString();
            long temp = function.HexToInt(b + a);
            QString label;

            switch (i) {
            case 3:
                label = "BandA: " + QString::number(serial_number);
                point_K.setX(serial_number);
                point_K.setY(temp);
                datalist_K.append(Data(point_K, label));
                break;

            case 5:
                label = "BandB: " + QString::number(serial_number);
                point_S.setX(serial_number);
                point_S.setY(temp);
                datalist_S.append(Data(point_S, label));
                break;

            case 7:
                label = "BandC: " + QString::number(serial_number);
                point_C.setX(serial_number);
                point_C.setY(temp);
                datalist_C.append(Data(point_S, label));
                break;
            }

            //            qDebug() << values_list[i];
        }

        if (start_num == -1) {
            if (point_C.y() || point_K.y() ||
                point_S.y()) start_num = serial_number;
        }
    }

    datatable.append(datalist_K);
    datatable.append(datalist_S);
    datatable.append(datalist_C);
    return datatable;
}
