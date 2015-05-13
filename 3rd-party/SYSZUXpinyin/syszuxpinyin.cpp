/***************************************************************************
**
**  <SYSZUXpinyin 1.0 , a chinese input method based on Qt for Embedded linux>
**  Copyright (C) <2010> <Gemfield> <gemfield@civilnet.cn>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License version 3 as published
**  by the Free Software Foundation.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**  If you have questions regarding the use of this file, please contact
**  Gemfield at gemfield@civilnet.cn or post your questions at
**  http://civilnet.cn/syszux/bbs
**
****************************************************************************/

#include "syszuxpinyin.h"
#include "syszuxim.h"
#include <QDebug>

static const QString syszux_lower_letter[] = {\
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", " = ", "←", \
    "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\\", \
    "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\'", "Enter", \
    "z", "x", "c", "v", "b", "n", "m", ", ", ".", "/", \
    "Shift", " "};

static const QString syszux_upper_letter[] = {\
    "!", "@", "#", "$", "%", "^", "&&", "*", "(", ")", "_", "+", "←", \
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "|", \
    "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"", "Enter", \
    "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", \
    "SHIFT", " "};

SyszuxPinyin::SyszuxPinyin(QWSInputMethod *im) : \
    QDialog(0, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint), \
    button_group(new QButtonGroup(this)), \
    input_method(0), lower_upper(0), page_count(0)
{
    setupUi(this);
    initButtonGroup();
    pinyin_file.setFileName(":/syszux/syszuxpinyin");

    if (!pinyin_file.open(QIODevice::ReadOnly))
    {
        //QMessageBox::warning(0, tr("syszuxpinyin"), tr("Can't find syszuxpinyin!"));
        qDebug() << "Can't find syszuxpinyin!";
    }

    reg_exp.setCaseSensitivity(Qt::CaseSensitive);
    reg_exp.setPattern(QString("([a-z]+)"));

    int ret;
    while (!pinyin_file.atEnd())
    {
        QByteArray buf = pinyin_file.readLine();
        ret = reg_exp.indexIn(QString(buf.data()), 0, QRegExp::CaretAtZero);
        pinyin_map.insert(reg_exp.cap(1), buf.left(ret));
    }
    connect(this, SIGNAL(sigCommitString(QString)), im, SLOT(slotCommitString(QString)));
    connect(this, SIGNAL(sigPreeditString(QString)), im, SLOT(slotPreeditString(QString)));
    //connect(lineEdit_window, SIGNAL(textChanged(QString)), im, SLOT(slotPreeditString(QString)));
}

SyszuxPinyin::~SyszuxPinyin()
{

}

void SyszuxPinyin::initButtonGroup()
{
    QPushButton *pushButton = new QPushButton(this);
    pushButton->hide();
    pushButton = pushButton_hanzi_left;
    for (int i = 1; i != 67; ++i)
    {
        //qDebug() << i;
        button_vector.push_back(pushButton);
        button_group->addButton(pushButton, i);
        pushButton = qobject_cast<QPushButton*>(pushButton->nextInFocusChain());
    }
    connect(button_group, SIGNAL(buttonClicked(int)), SLOT(buttonClickResponse(int)));
}

void SyszuxPinyin::buttonClickResponse(int key)
{
    if (key == 1)
    {
        selectHanziPre();
        return;
    }
    else if (key == 10)
    {
        selectHanziNext();
        return;
    }
    else if (key < 10)
    {
        lineEdit_window->insert(button_vector.at(key - 1)->text());
        lineEdit_pinyin->clear();
        clearString();

        emit sigPreeditString(lineEdit_window->text());

        return;
    }
    else if (key == 23)
    {
        deleteString();
        return;
    }
    else if (key == 48)
    {
        return;
    }
    else if (key == 59)
    {
        changeLowerUpper();
        return;
    }
    else if (key > 10 && key <= 60)
    {
        if (lower_upper)
            event = new QKeyEvent(QEvent::KeyPress, 0, Qt::NoModifier, syszux_upper_letter[key - 11]);
        else
            event = new QKeyEvent(QEvent::KeyPress, 0, Qt::NoModifier, syszux_lower_letter[key - 11]);
    }
    else if (key == 61)
    {
        changeInputMethod();
        return;
    }
    else if (key == 62)
    {
        commitString();
        return;
    }
    else if (key > 62)
    {
        switch(key)
        {
        case 63:
            event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
            break;
        case 64:
            event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
            break;
        case 65:
            event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
            break;
        case 66:
            event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
            break;
        }
    }
    if (input_method && \
        ((key >= 24 && key <= 33) || (key >= 37 && key <= 45) || (key >= 49 && key <= 55)))
    {
        lineEdit_pinyin->setFocus();
        QApplication::sendEvent(focusWidget(), event);
        matching(lineEdit_pinyin->text());
    }
    else
    {
        lineEdit_window->setFocus();
        QApplication::sendEvent(focusWidget(), event);
    }
    emit sigPreeditString(lineEdit_window->text());
}

void SyszuxPinyin::matching(QString pinyin)
{
    pinyin_list = pinyin_map.values(pinyin);
    changePage(0);
    page_count = 0;
}

void SyszuxPinyin::changePage(int index)
{
    int count = pinyin_list.size();
    int i = count - index * 8;
    for (int j = 1; j != 9; ++j)
    {
        button_vector.at(j)->setText(tr(pinyin_list.value(--i).toAscii()));
    }

    if (index == 0)
        pushButton_hanzi_left->setEnabled(false);
    else
        pushButton_hanzi_left->setEnabled(true);

    if (count > (index + 1) * 8)
        pushButton_hanzi_right->setEnabled(true);
    else
        pushButton_hanzi_right->setEnabled(false);
}

void SyszuxPinyin::selectHanziPre()
{
    changePage(--page_count);
}

void SyszuxPinyin::selectHanziNext()
{
    changePage(++page_count);
}

void SyszuxPinyin::clearString()
{
    for (int i = 1; i != 9; ++i)
    {
        button_vector.at(i)->setText("");
    }
}

void SyszuxPinyin::changeInputMethod()
{
    if (lower_upper)//pushButton_shift->text() == "SHIFT"
        return;

    lineEdit_pinyin->clear();
    if (input_method)//pushButton_is_hanzi->text() == "中"
    {
        input_method = 0;
        pushButton_is_hanzi->setText("En");
    }
    else
    {
        input_method = 1;
        pushButton_is_hanzi->setText(tr("中"));
    }
}
void SyszuxPinyin::changeLowerUpper()
{   
    const QString *syszux_letter;
    if (lower_upper)//pushButton_shift->Text() == "SHIFT"
    {
        lower_upper = 0;
        pushButton_shift->setText("Shift");

        syszux_letter = syszux_lower_letter;
    }
    else//pushButton_shift->Text() == "Shift"
    {
        lower_upper = 1;
        pushButton_shift->setText("SHIFT");

        input_method = 0;
        pushButton_is_hanzi->setText("En");

        syszux_letter = syszux_upper_letter;
    }

    for (int i = 10; i != 59; ++i)
    {
        button_vector.at(i)->setText(syszux_letter[i - 10]);
    }
}

void SyszuxPinyin::deleteString()
{
    event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
    if (input_method && !lineEdit_pinyin->text().isEmpty())
    {
        lineEdit_pinyin->setFocus();
        //qDebug() << lineEdit_pinyin->text();
        QApplication::sendEvent(focusWidget(), event);
        //qDebug() << lineEdit_pinyin->text();
        matching(lineEdit_pinyin->text());
    }
    else
    {
        lineEdit_window->setFocus();
        QApplication::sendEvent(focusWidget(), event);
    } 
    emit sigPreeditString(lineEdit_window->text());
}

void SyszuxPinyin::commitString()
{
    emit sigCommitString(lineEdit_window->text());
    this->hide();
}

void SyszuxPinyin::setLineEditText(QString text)
{
    lineEdit_window->setText(text);
}



