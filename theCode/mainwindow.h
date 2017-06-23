#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "VictorCode/music_radio_driver.h"
#include <QMainWindow>
#include <QMediaPlayer>
#include <QStringList>
#include <QFileDialog>
#include <QMediaPlaylist>
#include <QWidget>
#include <QSystemTrayIcon>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QMediaPlayer Player;
    QMediaPlaylist playerlist;
    QString f;
    QStringList list;
    QMap<qint64, QString> lrc_map;
    QSystemTrayIcon *mSysTrayIcon;
    void createActions();
    void createMenu();
    int flag = 0, now = 0;

private slots:
        void on_ChannelMenu_clicked();

private:
        music_radio_driver *m_music_radio_driver;

        QMenu *mMenu;
        QAction *mShowMainAction;
        QAction *mExitAppAction;

        QMenu *MusicMenu;//tool bar music
        QAction *addMusicfromFile;
        QAction *addMusicfromFolder;
        QAction *addMusicfromChannel;

        QMenu *Operate;//tool bar operate
        QAction *deleteSong;
        QAction *saveList;
        QAction *readList;

        QMenu *helpMenu;//tool bar help
        QAction *aboutSoftware;
        QAction *howToUse;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_verticalSlider_valueChanged();
    void setPosition(int position);
    void updateDuration(qint64 duration);
    void updatePosition(qint64 position);
    void AddMusicFiles();
    void AddMusicFolder();
    void TraverseFiles(QString folderPath, QStringList filters, QStringList &filesList);
    void playTo(int i);
    void setEditline();
    void DeleteSongFromList();
    void SaveMusicList();
    void ReadMusicList();
    void AddMusicToList(const QString &musicpath);
    int singlemusic(QString music);
    void message_music();
    void UpdateTime(qint64 time);
    void resolve_lrc(const QString &source_file_name);
    void AddMusicDoadFolder();
    void setEditline_2();
    void on_horizontalSlider_valueChanged();
    void on_tableWidget_customContextMenuRequested();
    void on_tableWidget_doubleClicked(const QModelIndex &index);
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_pushButton_6_clicked();
    void on_Tray_clicked();
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
    void on_showMainAction();
    void on_exitAppAction();
    void on_addmusicfromfile();
    void on_addmusicfromfolder();
    void on_addmusicfromchannel();
    void on_deletesong();
    void on_savelist();
    void on_readlist();
    void on_aboutsoftware();
    void on_howtouse();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
