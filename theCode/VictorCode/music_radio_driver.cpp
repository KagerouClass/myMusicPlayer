#include "music_radio_driver.h"
#include "getsongplaylistid.h"
#include "downloadfile.h"
#include "getsongreallink.h"
#include "getlrc.h"
#include "getaristpic.h"

#include <QDir>
#include <QTableWidget>
#include <QMessageBox>
#include <QHeaderView>
const int MAX_SONG_NUM = 1;
const QDir dir;
const QString ONLINE_RADIO_DOWNLOAD_DIR = dir.currentPath() + "/OnlineRadioDownloadDir";

music_radio_driver::music_radio_driver()
{
    m_cookJar = new QNetworkCookieJar;
    m_channelWidget = new ChannelsWidget(0, m_cookJar);
    QObject::connect(m_channelWidget, SIGNAL(channelChanged(CHANNEL_INFO)), this, SLOT(channelChangedSlot(CHANNEL_INFO)));

}

void music_radio_driver::ShowChannelMenu(void) {
    m_channelWidget->show();//show ui
}

void music_radio_driver::channelChangedSlot(CHANNEL_INFO channel)
{
    m_songIdList.clear();//local: QList <QString> m_songIdList;
    m_iCurrentSongIndex = 0;//int
                            //CHANNEL_INFO m_currentChannelId;//only a tiny struct
    m_currentChannelId.channelId = channel.channelId;
    m_currentChannelId.channelName = channel.channelName;
    //初始化某个频道列表下的所有歌曲
    initSongIdList();
}

//
void music_radio_driver::initSongIdList()
{
    //第二步,获取某个频道下的歌曲ID列表
    GetSongPlayListId getPlayListId(this, m_cookJar);//need #include "VictorCode/getsongplaylistid.h"
    m_songIdList = getPlayListId.getSongIdList(m_currentChannelId.channelId);//local: QList <QString> m_songIdList;

    qDebug() << "频道:" << m_currentChannelId.channelName << m_currentChannelId.channelId << "共有歌曲" << m_songIdList.size();

    QTableWidget *t = new QTableWidget(MAX_SONG_NUM+1, 1);
    t->setBaseSize(100,50);
    t->setWindowTitle(QObject::tr("电台歌曲"));
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setSelectionMode(QAbstractItemView::NoSelection);
    QString info = m_currentChannelId.channelName + " 共有歌曲: " + QString::number(m_songIdList.size());
    qDebug() << info;
    DownloadSongs_InTheList(0, MAX_SONG_NUM,t);
}


void music_radio_driver::DownloadSongs_InTheList(int BeginIndex, int EndIndex, QTableWidget *t)//play(int index)
{
    QMessageBox::information(NULL, "电台歌曲", "确认后请耐心等待下载");
    for (int index = BeginIndex; index <= m_songIdList.size() && index <= EndIndex; index++) {
        GetSongRealLink getSongLink;//the web link getter
        m_currentSongInfo = getSongLink.getSongRealLinkById(m_songIdList.at(index));
        //begin the download:
        QString fileName;
        DownLoadFile downMp3;//need #include "downloadfile.h"

        QDir dir;
        if (!dir.exists(ONLINE_RADIO_DOWNLOAD_DIR))
        {
            dir.mkdir(ONLINE_RADIO_DOWNLOAD_DIR);
        }
        t->setItem(index-1, 1, new QTableWidgetItem(m_currentSongInfo.songName));
        fileName = ONLINE_RADIO_DOWNLOAD_DIR + "/" + m_currentSongInfo.songName + ".mp3";//need: SONG_INFO m_currentSongInfo;
        qDebug() << fileName;
        downMp3.getMp3File(fileName, m_currentSongInfo.songRealLink);
        qDebug() << "下载完成" + fileName;
        //获取歌词
        fileName = ONLINE_RADIO_DOWNLOAD_DIR + "/" + m_currentSongInfo.songName + ".lrc";
        GetLrc getLrc;
        QString lrc = getLrc.getLrc(m_currentSongInfo.lrcLink);
        //write procedure
        QFile file(fileName);
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);
            stream << lrc << endl;
        }
        fileName = ONLINE_RADIO_DOWNLOAD_DIR + "/" + m_currentSongInfo.songName + ".jpg";
        GetAristPic getPic;
        QPixmap picMap = getPic.getAristPic(m_currentSongInfo.songPicRadio);
        picMap.save(fileName, "JPG");
    }
    QStringList header;
    header << "下载歌曲名称";
    t->setHorizontalHeaderLabels(header);
    t->resizeColumnsToContents();
    t->horizontalHeader()->setStretchLastSection(true);
    t->show();
}
