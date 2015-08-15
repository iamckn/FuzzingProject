#ifndef ABOUTD_H
#define ABOUTD_H

#include <QDialog>

namespace Ui {
    class aboutD;
}

class aboutD : public QDialog
{
    Q_OBJECT

public:
    explicit aboutD(QWidget *parent = 0);
    ~aboutD();

private:
    Ui::aboutD *ui;
};

#endif // ABOUTD_H
