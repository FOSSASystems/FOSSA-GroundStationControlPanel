#ifndef MAPPANE_H
#define MAPPANE_H

#include <QWidget>
#include <QFileInfo>
#include <QDir>

namespace Ui {
class MapPane;
}

class MapPane : public QWidget
{
    Q_OBJECT

public:
    explicit MapPane(QWidget *parent = nullptr);
    ~MapPane();

private:
    Ui::MapPane *ui;
};

#endif // MAPPANE_H
