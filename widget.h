#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class LatLonWidget;
class QComboBox;
class QLineEdit;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void button1Clicked();
    void button2Clicked();
    void button3Clicked();
    void formatChanged(int index);

private:
    LatLonWidget *w1;
    LatLonWidget *w2;
    LatLonWidget *w3;

    QLineEdit *test;

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
