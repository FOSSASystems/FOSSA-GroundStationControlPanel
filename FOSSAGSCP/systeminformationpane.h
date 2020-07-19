#ifndef SYSTEMINFORMATIONPANE_H
#define SYSTEMINFORMATIONPANE_H

#include <QWidget>

#include "include/Interpreter.h"

namespace Ui {
class systeminformationpane;
}

class systeminformationpane : public QWidget
{
    Q_OBJECT

public:
    explicit systeminformationpane(QWidget *parent = nullptr);
    ~systeminformationpane();
    Ui::systeminformationpane *ui;
};

#endif // SYSTEMINFORMATIONPANE_H
