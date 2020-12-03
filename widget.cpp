#include "widget.h"
#include "latlonwidget.h"
#include <QComboBox>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

#include "utm.h"

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    w1 = new LatLonWidget(LatLonWidget::eDECIMAL_DEG);
    w2 = new LatLonWidget(LatLonWidget::eDMS);
    w3 = new LatLonWidget(LatLonWidget::eDECIMAL_DEG);

    QStringList formatTypes;
    formatTypes << "Decimal Degree" << "Degree, Minute, Second" << "UTM";
    QComboBox *cbox1 = new QComboBox();
    QComboBox *cbox2 = new QComboBox();
    QComboBox *cbox3 = new QComboBox();

    cbox1->setObjectName("Pos-1");    
    cbox2->setObjectName("Pos-2");
    cbox2->setObjectName("Pos-3");

    cbox1->insertItems(0, formatTypes);
    cbox2->insertItems(0, formatTypes);
    cbox2->setCurrentIndex(1);
    cbox3->insertItems(0, formatTypes);
    cbox3->setCurrentIndex(1);

    connect(cbox1, SIGNAL(currentIndexChanged(int)), this, SLOT(formatChanged(int)));
    connect(cbox2, SIGNAL(currentIndexChanged(int)), this, SLOT(formatChanged(int)));
    connect(cbox3, SIGNAL(currentIndexChanged(int)), this, SLOT(formatChanged(int)));

    QPushButton *button1 = new QPushButton("Update Position - 2/3");
    QPushButton *button2 = new QPushButton("Update Position - 1/3");
    QPushButton *button3 = new QPushButton("Update Position - 1/2");

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addWidget(new QLabel("<b>Position - 1</b>"), 0, 0);
    mainLayout->addWidget(new QLabel("<b>Position - 2</b>"), 0, 1);
    mainLayout->addWidget(new QLabel("<b>Position - 3</b>"), 0, 2);
    mainLayout->addWidget(w1, 1, 0);
    mainLayout->addWidget(w2, 1, 1);
    mainLayout->addWidget(w3, 1, 2);
    mainLayout->addWidget(cbox1, 2, 0);
    mainLayout->addWidget(cbox2, 2, 1);
    mainLayout->addWidget(cbox3, 2, 2);
    mainLayout->addWidget(button1, 3, 0);
    mainLayout->addWidget(button2, 3, 1);
    mainLayout->addWidget(button3, 3, 2);

//    test = new QLineEdit();
//    QRegularExpressionValidator *m_latDegValidator = new QRegularExpressionValidator();
//    QString latDegRegExp = QStringLiteral("(-)?\\d{1,2}\\.\\d{0,6}\u00B0");
//    m_latDegValidator->setRegularExpression(QRegularExpression(latDegRegExp));
//    test->setValidator(m_latDegValidator);
//    test->setText("0.000000\u00B0");
//    mainLayout->addWidget(test, 4, 0);

    setLayout(mainLayout);

    connect(button1, &QPushButton::clicked, this, &Widget::button1Clicked);
    connect(button2, &QPushButton::clicked, this, &Widget::button2Clicked);
    connect(button3, &QPushButton::clicked, this, &Widget::button3Clicked);
}

void Widget::button1Clicked()
{
    double lat, lon;
    w1->getPosition(lat, lon);

    w2->setPosition(lat, lon);
    w3->setPosition(lat, lon);
}

void Widget::button2Clicked()
{
    double lat, lon;
    w2->getPosition(lat, lon);

    w1->setPosition(lat, lon);
    w3->setPosition(lat, lon);
}

void Widget::button3Clicked()
{
    double lat, lon;
    w3->getPosition(lat, lon);

    w1->setPosition(lat, lon);
    w2->setPosition(lat, lon);
}

void Widget::formatChanged(int index)
{
    QComboBox *cbox = reinterpret_cast<QComboBox *>(sender());
    if(cbox->objectName() == "Pos-1") {
        w1->positionFormatChanged(index);
    } else if(cbox->objectName() == "Pos-2") {
        w2->positionFormatChanged(index);
    } else {
        w3->positionFormatChanged(index);
    }
}
