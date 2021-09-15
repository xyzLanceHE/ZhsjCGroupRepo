#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QTextStream>


#pragma execution_character_set("utf-8")

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

	//setWindowFlags(Qt::Window);
    //添加折线
    ui->customplot->addGraph();

    //设置X轴属性
    ui->customplot->xAxis->setRange(0, 500);
    ui->customplot->xAxis->setLabel("X轴");
    //ui->customplot->xAxis->setLabelColor(Qt::red);
    ui->customplot->xAxis->setLabelFont(QFont("宋体", 13, QFont::Bold));
	ui->customplot->xAxis->setTickPen(QPen(QColor(0, 0, 255), 2));
	ui->customplot->xAxis->setSubTickPen(QPen(QColor(0, 0, 255)));
	ui->customplot->xAxis->setTickLabelColor(QColor(0, 0, 255));
	ui->customplot->xAxis->setLabelColor(QColor(0, 0, 255));
	ui->customplot->xAxis->setBasePen(QPen(QColor(0, 0, 255)));

    //设置Y轴属性
	ui->customplot->yAxis->setTickPen(QPen(QColor(0, 0, 255), 2));
	ui->customplot->yAxis->setSubTickPen(QPen(QColor(0, 0, 255)));
    ui->customplot->yAxis->setRange(0, 500);
    ui->customplot->yAxis->setLabel("Y轴");
	ui->customplot->yAxis->setTickLabelColor(QColor(0, 0, 255));
    //ui->customplot->yAxis->setLabelColor(Qt::red);
    ui->customplot->yAxis->setLabelFont(QFont("宋体", 13, QFont::Bold));
	ui->customplot->yAxis->setLabelColor(QColor(0, 0, 255));
	ui->customplot->yAxis->setBasePen(QPen(QColor(0, 0, 255)));
	//ui->customplot->yAxis->setTickPen(QPen(QColor(255, 0, 0), 2));

    //ui->customplot->legend->setVisible(true);
    //设置折线属性
    ui->customplot->graph(0)->setName(tr("折线"));
    ui->customplot->graph(0)->setPen(QPen(QColor(255, 0, 0), 2));
    ui->customplot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));

    //ui->customplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	m_pix.load("./back.jpg");
	//ui->customplot->setBackground(QBrush(QColor(255, 0, 0)));
	if (!m_pix.isNull())
	{
        //设置背景图片
		ui->customplot->setBackground(m_pix);
	}
	
    connect(ui->pushButton, &QPushButton::clicked, [=](){
        //弹出选择文件框
        QString strPath = QFileDialog::getOpenFileName(this, tr("打开文件"), tr("./"), tr("file (*.txt)"));
        vecX.clear();
        vecY.clear();

        qDebug() << strPath;
        //读取x y 轴数据
        ReadFile(strPath, vecX, vecY);

        //更新数据到图标里
        ui->customplot->graph(0)->setData(vecX, vecY);
        ui->customplot->graph(0)->rescaleAxes(true);
        ui->customplot->replot();
    });

	//
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *event)
{
	//QPainter painter(this);
	//painter.drawPixmap(this->rect(), m_pix);

}

void Widget::ReadFile(QString strPath, QVector<double>& vecX, QVector<double>& vecY)
{
    QFile file(strPath);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug() << "file open fail";
        return;
    }

    QTextStream out(&file);

    QString strLine;

    while(!out.atEnd())
    {
        //按行读取数据
        strLine = out.readLine();
        QStringList listStr = strLine.split(" ");
        qDebug() << listStr;
        if(listStr.size() == 2)
        {
            vecX.push_back(listStr.at(0).toDouble());
            vecY.push_back(listStr.at(1).toDouble());
        }
    }

    file.close();
}
