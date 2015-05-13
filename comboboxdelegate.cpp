#include "comboboxdelegate.h"
#include <QComboBox>

ComboBoxDelegate::ComboBoxDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    QComboBox *comboBox = new QComboBox(parent);
    comboBox->addItem(tr("高"));
    comboBox->addItem(tr("中"));
    comboBox->addItem(tr("低"));
    comboBox->installEventFilter(const_cast<ComboBoxDelegate*>(this));
    return comboBox;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    if (comboBox)
    {
        int currentIndex = -1;
        QString indexText = index.model()->data(index, Qt::EditRole).toString();
        if (indexText == tr("高"))
        {
            currentIndex = 0;
        }
        else if (indexText == tr("中"))
        {
            currentIndex = 1;
        }
        else if (indexText == tr("低"))
        {
            currentIndex = 2;
        }
        comboBox->setCurrentIndex(currentIndex);
    }
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    if (comboBox)
    {
        model->setData(index, comboBox->currentText());
    }
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}
