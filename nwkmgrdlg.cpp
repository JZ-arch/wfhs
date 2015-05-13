#include "nwkmgrdlg.h"
#include "ui_nwkmgrdlg.h"

NwkMgrDlg::NwkMgrDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NwkMgrDlg),
    panidhighbit(0),
    panidlowbit(0)\
{
    ui->setupUi(this);
    m_DigitpanidHigt = new Digit(Digit::Flip,0);
    m_DigitpanidLow  = new Digit(Digit::Flip,0);
    ui->PanIdLayoutHigh->addWidget(m_DigitpanidHigt);
    ui->PanIdLayoutLow->addWidget(m_DigitpanidLow);
    ui->pushButtonHighAdd->setStyleSheet( \
        "QPushButton{border-image: url(:/togglebutton/up.png);}"
        "QPushButton:pressed{border-image: url(:/togglebutton/up_pressed.png);}");

    ui->pushButtonHighDec->setStyleSheet( \
        "QPushButton{border-image: url(:/togglebutton/down.png);}"
        "QPushButton:pressed{border-image: url(:/togglebutton/down_pressed.png);}");

    ui->pushButtonLowAdd->setStyleSheet( \
        "QPushButton{border-image: url(:/togglebutton/up.png);}"
        "QPushButton:pressed{border-image: url(:/togglebutton/up_pressed.png);}");

    ui->pushButtonLowDec->setStyleSheet( \
        "QPushButton{border-image: url(:/togglebutton/down.png);}"
        "QPushButton:pressed{border-image: url(:/togglebutton/down_pressed.png);}");
}

NwkMgrDlg::~NwkMgrDlg()
{
    delete ui;
}

void NwkMgrDlg::on_pushButtonHighAdd_clicked()
{
    if( panidhighbit >= 2)
    {
        panidhighbit = 0;
    }
    else
    {
         panidhighbit++;
         if(panidhighbit == 2 && panidlowbit > 55)
         {
            panidlowbit = 0;
            m_DigitpanidLow->transitTo(panidlowbit);
         }

    }
    m_DigitpanidHigt->transitTo(panidhighbit);
}


void NwkMgrDlg::on_pushButtonHighDec_clicked()
{
    if(panidhighbit == 0)
    {
        panidhighbit = 2;
        if(panidlowbit > 55)
        {
           panidlowbit = 0;
           m_DigitpanidLow->transitTo(panidlowbit);
        }
    }
    else
    {
        panidhighbit--;
    }
    m_DigitpanidHigt->transitTo(panidhighbit);
}

void NwkMgrDlg::on_pushButtonLowAdd_clicked()
{
    if( panidhighbit >= 2)
    {
        if(panidlowbit >= 55)
        {
            panidlowbit = 0;
            on_pushButtonHighAdd_clicked();
        }
        else
        {
           panidlowbit++;
        }
    }
    else
    {
        if(panidlowbit >= 99)
        {
            panidlowbit = 0;
            on_pushButtonHighAdd_clicked();
        }
        else
        {
           panidlowbit++;
        }
    }
    m_DigitpanidLow->transitTo(panidlowbit);
}

void NwkMgrDlg::on_pushButtonLowDec_clicked()
{
    if(panidhighbit == 2)
    {
        if(panidlowbit ==0)
        {
            panidlowbit = 55;
            on_pushButtonHighDec_clicked();
        }
        else
        {
            panidlowbit--;
        }
    }
    else
    {
        if(panidlowbit == 0)
        {
            panidlowbit = 99;
            on_pushButtonHighDec_clicked();
        }
        else
        {
            panidlowbit--;
        }
    }
    m_DigitpanidLow->transitTo(panidlowbit);
}

void NwkMgrDlg::on_pushButtonOk_clicked()
{
  accept();
}

void NwkMgrDlg::on_pushButtonCancel_clicked()
{
  reject();
}
