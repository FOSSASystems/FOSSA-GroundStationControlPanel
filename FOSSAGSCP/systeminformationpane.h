#ifndef SYSTEMINFORMATIONPANE_H
#define SYSTEMINFORMATIONPANE_H

#include <QWidget>

namespace Ui {
class systeminformationpane;
}

class systeminformationpane : public QWidget
{
    Q_OBJECT

public:
    explicit systeminformationpane(QWidget *parent = nullptr);
    ~systeminformationpane();

private:
    Ui::systeminformationpane *ui;
};

#endif // SYSTEMINFORMATIONPANE_H
