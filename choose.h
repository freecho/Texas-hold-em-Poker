#ifndef CHOOSE_H
#define CHOOSE_H

#include <QWidget>

namespace Ui {
class Choose;
}

class Choose : public QWidget
{
    Q_OBJECT

public:
    explicit Choose(QWidget *parent = nullptr);
    ~Choose();

private:
    Ui::Choose *ui;
};

#endif // CHOOSE_H
