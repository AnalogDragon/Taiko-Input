#include "clickedcombobox.h"

ClickedComboBox::ClickedComboBox(QWidget *parent):QComboBox(parent)
{

}
ClickedComboBox::~ClickedComboBox()
{

}

void ClickedComboBox::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit clicked(true);
    }
    QComboBox::mousePressEvent(event);
}

