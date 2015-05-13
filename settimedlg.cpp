#include "settimedlg.h"
#include "ui_settimedlg.h"

SetTimeDlg::SetTimeDlg(int hour, int minute, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetTimeDlg), \
    m_hour(hour), m_minute(minute)
{
    ui->setupUi(this);

    m_pDigitHour = new Digit(Digit::Flip, m_hour);
    ui->layoutDigitHour->addWidget(m_pDigitHour);

    m_pDigitMinute = new Digit(Digit::Flip, m_minute);
    ui->layoutDigitMinute->addWidget(m_pDigitMinute);

    ui->pushButtonHourInc->setStyleSheet( \
        "QPushButton{border-image: url(:/togglebutton/up.png);}"
        "QPushButton:pressed{border-image: url(:/togglebutton/up_pressed.png);}");

    ui->pushButtonHourDec->setStyleSheet( \
        "QPushButton{border-image: url(:/togglebutton/down.png);}"
        "QPushButton:pressed{border-image: url(:/togglebutton/down_pressed.png);}");

    ui->pushButtonMinuteInc->setStyleSheet( \
        "QPushButton{border-image: url(:/togglebutton/up.png);}"
        "QPushButton:pressed{border-image: url(:/togglebutton/up_pressed.png);}");

    ui->pushButtonMinuteDec->setStyleSheet( \
        "QPushButton{border-image: url(:/togglebutton/down.png);}"
        "QPushButton:pressed{border-image: url(:/togglebutton/down_pressed.png);}");

    connect(ui->pushButtonOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

SetTimeDlg::~SetTimeDlg()
{
    delete m_pDigitHour;
    delete m_pDigitMinute;
    delete ui;

}

void SetTimeDlg::on_pushButtonHourDec_clicked()
{
    if (m_hour > 0)
    {
        m_hour--;
    }
    else
    {
        m_hour = 23;
    }
    m_pDigitHour->transitTo(m_hour);
}

void SetTimeDlg::on_pushButtonHourInc_clicked()
{
    if (m_hour < 23)
    {
        m_hour++;
    }
    else
    {
        m_hour = 0;
    }
    m_pDigitHour->transitTo(m_hour);
}

void SetTimeDlg::on_pushButtonMinuteDec_clicked()
{
    if (m_minute > 0)
    {
        m_minute--;
    }
    else
    {
        m_minute = 59;
        on_pushButtonHourDec_clicked();
    }
    m_pDigitMinute->transitTo(m_minute);
}

void SetTimeDlg::on_pushButtonMinuteInc_clicked()
{
    if (m_minute < 59)
    {
        m_minute++;
    }
    else
    {
        m_minute = 0;
        on_pushButtonHourInc_clicked();
    }
    m_pDigitMinute->transitTo(m_minute);
}
