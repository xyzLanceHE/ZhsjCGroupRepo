#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    QVector<double> vecX;
    QVector<double> vecY;

protected:

	void paintEvent(QPaintEvent *event);

public:

    void ReadFile(QString strPath, QVector<double>& vecX, QVector<double>& vecY);

	QPixmap	m_pix;
};

#endif // WIDGET_H
