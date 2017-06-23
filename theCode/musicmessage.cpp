#include "musicmessage.h"
#include "ui_musicmessage.h"
#pragma execution_character_set("utf-8")
musicmessage::musicmessage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::musicmessage)
{
    ui->setupUi(this);
    this->setWindowTitle("音乐播放器试用版1.0");
    ui->label_5->setText("敬请期待！");
    ui->label_6->setText("敬请期待！");
    ui->label_7->setText("敬请期待！");
}

musicmessage::~musicmessage()
{
    delete ui;
}
