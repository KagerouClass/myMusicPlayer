#ifndef MUSIC_RADIO_DRIVER_H
#define MUSIC_RADIO_DRIVER_H

#include "channelswidget.h"
#include "getsongreallink.h"
#include <QTableWidget>
class music_radio_driver  : public QWidget
{
     Q_OBJECT//must add

public slots:
    void channelChangedSlot(CHANNEL_INFO channel);//from m_channelWidget

public:
    music_radio_driver();
    void ShowChannelMenu(void);
private:
    void DownloadSongs_InTheList(int BeginIndex, int EndIndex, QTableWidget *t);//download all songs in the list, at most maximum songs
    void initSongIdList();
private:
    QList <QString> m_songIdList;
    int m_iCurrentSongIndex;// counting the index in the online list
    CHANNEL_INFO m_currentChannelId;//only a tiny struct
    ChannelsWidget *m_channelWidget;
    QNetworkCookieJar *m_cookJar;
    SONG_INFO m_currentSongInfo;// need #include "getsongreallink.h"
};

#endif // MUSIC_RADIO_DRIVER_H


//This is the only file that you need to care about
//procedure:
/*
music_radio_driver is the top module.
Get music( .mp3), lyrics(.lrc), picture(.jpg) from Baidu online music radio.
It is already sufficient to your use.

Note: the download folder is the folder where your Qt buiding your project
rather than the folder where your codes are.

How it works:

First:
initialize the menu:
ChannelsWidget::ChannelsWidget(QWidget *parent, QNetworkCookieJar *cookJar)
using the constructor

Second:
when button in channelswidget.ui (channel menu) is clicked:
void ChannelsWidget::channelSelected(int iChannel)
is called.

Then:
in class music_radio_driver:
void music_radio_driver::channelChangedSlot(CHANNEL_INFO channel)
is called as a  slot function.

download procedure begins:
void music_radio_driver::initSongIdList()
void music_radio_driver::DownloadSongs_InTheList(int BeginIndex, int EndIndex)//play(int index)

*/
