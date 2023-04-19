#ifndef CLICKEDCOMBOBOX_H
#define CLICKEDCOMBOBOX_H

#include <QComboBox>
#include <QMouseEvent>

class ClickedComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit ClickedComboBox(QWidget *parent);
    ~ClickedComboBox();

protected:
    void mousePressEvent(QMouseEvent *event);

signals:
    void clicked(bool);
};

#endif // CLICKEDCOMBOBOX_H
