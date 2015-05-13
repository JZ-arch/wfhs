#include "addroomdlg.h"
#include "ui_addroomdlg.h"

#include <QMessageBox>
#include <QDebug>
#include "global.h"

AddRoomDlg::AddRoomDlg(QWidget *parent) :
    QDialog(parent), \
    ui(new Ui::AddRoomDlg), \
    m_centeredPos(0, 0), \
    m_roomPrio(ROOM_PRIO_NONE), \
    m_pNewRoom(NULL)
{
    ui->setupUi(this);

    ui->lineEditRoomIdx->installEventFilter(this);
    ui->lineEditRoomName->installEventFilter(this);

    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
#ifdef _RELEASE
    QWSServer::setCurrentInputMethod(g_im);
#endif
}

AddRoomDlg::~AddRoomDlg()
{
    delete ui;
}

bool AddRoomDlg::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->lineEditRoomName || watched == ui->lineEditRoomIdx)
    {
        if (event->type() == QEvent::FocusIn)
        {
            //qDebug() << "QEvent::FocusIn";
            if (m_centeredPos.isNull())
            {
                m_centeredPos = this->pos();
                //qDebug() << m_centeredPos;
            }

            if (this->pos().y() != 0)
            {
                this->move(m_centeredPos.x(), 0);
            }
        }
        /*else if (event->type() == QEvent::FocusOut)
        {
            //qDebug() << "QEvent::FocusOut";
            this->move(m_centeredPos);
        }*/
    }
    return QDialog::eventFilter(watched, event);
}

void AddRoomDlg::on_pushButtonOk_clicked()
{
    QString roomName = ui->lineEditRoomName->text();
    bool roomIdxOk;
    int roomIdx = ui->lineEditRoomIdx->text().toInt(&roomIdxOk);

    if (roomName.isEmpty() \
            || !roomIdxOk || !(roomIdx >= 1 && roomIdx <= 100) \
            || m_roomPrio == ROOM_PRIO_NONE)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("房间信息不全或错误"));
    }
    else
    {
        m_pNewRoom = new Room;
        m_pNewRoom->setRoomIdx(roomIdx);
        m_pNewRoom->setRoomName(roomName);
        m_pNewRoom->setRoomPrio(m_roomPrio);
        QDialog::accept();
    }
}

void AddRoomDlg::on_radioButtonPrioLow_clicked()
{
    m_roomPrio = ROOM_PRIO_LOW;
}

void AddRoomDlg::on_radioButtonPrioMid_clicked()
{
    m_roomPrio = ROOM_PRIO_MEDIUM;
}

void AddRoomDlg::on_radioButtonPrioHigh_clicked()
{
    m_roomPrio = ROOM_PRIO_HIGH;
}
