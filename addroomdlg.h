#ifndef ADDROOMDLG_H
#define ADDROOMDLG_H

#include <QDialog>
#include <QPoint>
#include "room.h"

namespace Ui {
class AddRoomDlg;
}

class AddRoomDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddRoomDlg(QWidget *parent = 0);
    ~AddRoomDlg();
    Room* getNewRoom() const { return m_pNewRoom; }

public slots:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void on_pushButtonOk_clicked();

    void on_radioButtonPrioLow_clicked();

    void on_radioButtonPrioMid_clicked();

    void on_radioButtonPrioHigh_clicked();

private:
    typedef enum
    {
        ROOM_PRIO_NONE = 0, \
        ROOM_PRIO_HIGH, \
        ROOM_PRIO_MEDIUM, \
        ROOM_PRIO_LOW
    }roomPrio_t;

    Ui::AddRoomDlg *ui;
    QPoint m_centeredPos;
    roomPrio_t m_roomPrio;
    Room *m_pNewRoom;
};

#endif // ADDROOMDLG_H
