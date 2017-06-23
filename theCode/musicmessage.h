#ifndef MUSICMESSAGE_H
#define MUSICMESSAGE_H

#include <QDialog>

namespace Ui {
class musicmessage;
}

class musicmessage : public QDialog
{
    Q_OBJECT

public:
    explicit musicmessage(QWidget *parent = 0);
    ~musicmessage();

private:
    Ui::musicmessage *ui;
};

#endif // MUSICMESSAGE_H
