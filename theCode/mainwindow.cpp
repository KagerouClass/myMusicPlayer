#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "musicmessage.h"
#include <QFileDialog>
#include <QStringList>
#include <QMediaPlayer>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTime>
#include <QHeaderView>
#include <QDirIterator>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QIODevice>
#include <QTextCodec>
#include <QRegExp>
#include <QtGui>
#include <QWidget>
#include <QSystemTrayIcon>
#include <QPixmap>

const QDir dir;
const QString ONLINE_RADIO_DOWNLOAD_DIR = dir.currentPath() + "/OnlineRadioDownloadDir";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_music_radio_driver = new music_radio_driver();//constructor, may be moved to the initialization list
    playerlist.setCurrentIndex(1);//设置播放队列播放开始
    Player.setPlaylist(&playerlist);//设置播放队列到播放器

    ui->setupUi(this);
    this->setWindowTitle("音乐播放器试用版1.2");//主界面名字
    this->setWindowIcon(QIcon(":/new/prefix1/image/my.ico"));//图标载入
    ui->pushButton_2->setIcon(QIcon(":/new/prefix1/play.png"));
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置列表不可写
    ui->lineEdit->setEnabled(false);//设置不可写
    ui->lineEdit_2->setEnabled(false);

    connect(ui->verticalSlider, SIGNAL(valueChanged(int)), this, SLOT(setEditline_2(int)));//将音量与数字关联，调节音量的同时改变数字
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(setPosition(int)));//将进度与歌曲状态关联
    connect(&Player, &QMediaPlayer::positionChanged, this, &MainWindow::updatePosition);//同上
    connect(&Player, &QMediaPlayer::durationChanged, this, &MainWindow::updateDuration);//同上
    connect(&Player, &QMediaPlayer::positionChanged, this, &MainWindow::UpdateTime);
    connect(ui->tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(playTo(int)));//将双击列表与切换音乐关联
    connect(&playerlist, &QMediaPlaylist::currentIndexChanged, this, &MainWindow::setEditline);//将播放音乐与显示当前音乐关联

    ui->verticalSlider->setValue(50);//音量初始值
    ui->horizontalSlider->setValue(0);//进度初始值
    ui->tableWidget->setColumnCount(1);
    QStringList p;
    p << QString("歌曲名称");
    ui->tableWidget->setHorizontalHeaderLabels(p);//设置表头
    ui->tableWidget->resizeColumnToContents(0);//设置表格大小随内容变化
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    MusicMenu = this->menuBar()->addMenu("歌曲操作");
    addMusicfromFile = new QAction("打开", this);
    addMusicfromFile->setShortcut((tr("CTRL+O")));
    MusicMenu->addAction(addMusicfromFile);
    connect(addMusicfromFile, SIGNAL(triggered(bool)), this, SLOT(on_addmusicfromfile()));

    addMusicfromFolder = new QAction("从文件夹添加");
    addMusicfromFolder->setShortcut(tr("CTRL+F"));
    MusicMenu->addAction(addMusicfromFolder);
    connect(addMusicfromFolder, SIGNAL(triggered(bool)), this, SLOT(on_addmusicfromfolder()));

    addMusicfromChannel = new QAction("从电台添加");
    addMusicfromChannel->setShortcut((tr("CTRL+N")));
    MusicMenu->addAction(addMusicfromChannel);
    connect(addMusicfromChannel, SIGNAL(triggered(bool)), this, SLOT(on_addmusicfromchannel()));

    Operate = this->menuBar()->addMenu("操作");
    deleteSong = new QAction("删除歌曲");
    deleteSong->setShortcut((tr("CTRL+Delete")));
    Operate->addAction(deleteSong);
    connect(deleteSong, SIGNAL(triggered(bool)), this, SLOT(on_deletesong()));

    saveList = new QAction("保存列表");
    saveList->setShortcut((tr("CTRL+S")));
    Operate->addAction(saveList);
    connect(saveList, SIGNAL(triggered(bool)), this, SLOT(on_savelist()));

    readList = new QAction("读取列表");
    readList->setShortcut((tr("CTRL+L")));
    Operate->addAction(readList);
    connect(readList, SIGNAL(triggered(bool)), this, SLOT(on_readlist()));

    helpMenu = this->menuBar()->addMenu("帮助");
    aboutSoftware = new QAction("关于软件");
    helpMenu->addAction(aboutSoftware);
    connect(aboutSoftware, SIGNAL(triggered(bool)), this, SLOT(on_aboutsoftware()));

    howToUse = new QAction("如何使用");
    helpMenu->addAction(howToUse);
    connect(howToUse, SIGNAL(triggered(bool)), this, SLOT(on_howtouse()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()//文件输入1（单文件
{
    const QStringList musicPaths = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
    const QString fileDir =
        QFileDialog::getOpenFileName(this, tr("Open File"),
            musicPaths.isEmpty() ? QDir::homePath() : musicPaths.first(),
            tr("Music Files(*.mp3 *.flac *.wav *.wma);;All files (*.*)"));
    f = fileDir;
    list << f;//将文件路径以字符串存入播放列表1用于存储
    if (fileDir.isEmpty()) {
        qDebug("Unable to find the music file!");
        return;
    }
    QString p = QFileInfo(f).fileName();//得到文件名
    if (singlemusic(p)) {
        int nNowRow = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(nNowRow);

        QTableWidgetItem *pItemInfo = new QTableWidgetItem(p);
        qDebug() << p;
        ui->tableWidget->setItem(nNowRow, 0, pItemInfo);//将文件名插入表格中
        ui->tableWidget->resizeColumnToContents(0);
        playerlist.addMedia(QUrl::fromLocalFile(fileDir));//将文件插入播放队列
                                                          //qDebug()<<playerlist.currentMedia();
        if (!(Player.state() == QMediaPlayer::PlayingState)) {//如果当前没有音乐播放，为新插入的音乐准备就绪
            int u = playerlist.mediaCount();
            qDebug() << u;
            playerlist.setCurrentIndex(u - 1);
            ui->lineEdit->setText(p);
            playerlist.setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        }
    }
}

void MainWindow::AddMusicFiles()//插入多个文件
{
    QFileDialog* fd = new QFileDialog(this);
    // 多选
    fd->setFileMode(QFileDialog::ExistingFiles);
    // 过滤器
    fd->setNameFilter("Music Files(*.mp3 *.flac *.wav *.wma);;All Files(*.*)"); //设置文件过滤器
                                                                                // 详情
    fd->setViewMode(QFileDialog::Detail);
    // 执行
    QStringList fileNames;
    if (fd->exec() == QDialog::Accepted)
    {
        // 返回选择的文件列表
        fileNames = fd->selectedFiles();
    }
    else {
        return;
    }
    // 插入列表中
    for (int i = 0; i < fileNames.length(); i++)
    {
        QString f = QFileInfo(fileNames.at(i)).fileName();
        if (!singlemusic(f))
            continue;
        int nNowRow = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(nNowRow);
        QString u = fileNames.at(i);
        list << u;
        playerlist.addMedia(QUrl::fromLocalFile(fileNames.at(i)));

        QTableWidgetItem *pItemInfo = new QTableWidgetItem(f);
        qDebug() << f;
        ui->tableWidget->setItem(nNowRow, 0, pItemInfo);
        ui->tableWidget->resizeColumnToContents(0);
    }
    if (Player.mediaStatus() == QMediaPlayer::NoMedia) {
        playerlist.setCurrentIndex(0);
        //Player.play();
        playerlist.setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        setEditline();
        //qDebug(Player.mediaStatus());
    }
}

void MainWindow::TraverseFiles(QString folderPath, QStringList filters, QStringList &filesList)
{
    //判断路径是否存在
    QDir dir(folderPath);
    if (!dir.exists())
    {
        return;
    }
    //定义迭代器并设置过滤器
    QDirIterator dir_iterator(folderPath,
        filters,
        QDir::Files | QDir::NoSymLinks,
        QDirIterator::Subdirectories);
    while (dir_iterator.hasNext())
    {
        dir_iterator.next();
        QFileInfo file_info = dir_iterator.fileInfo();
        QString file_path = file_info.absoluteFilePath();
        filesList.append(file_path);
    }
}

void MainWindow::AddMusicDoadFolder() //插入文件夹
{
    QDir dir;
    QString folderName = dir.currentPath() + "/OnlineRadioDownloadDir";
    //QString foldername =
    QStringList filters;
    filters << QString("*.mp3") << QString("*.flac") << QString("*.wav") << QString("*.wma");
    QStringList fileNames;
    TraverseFiles(folderName, filters, fileNames);
    // 插入列表中
    for (int i = 0; i < fileNames.length(); i++)
    {
        QString f = QFileInfo(fileNames.at(i)).fileName();
        if (!singlemusic(f))
            continue;
        int nNowRow = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(nNowRow);
        QString u = fileNames.at(i);
        list << u;
        playerlist.addMedia(QUrl::fromLocalFile(fileNames.at(i)));

        QTableWidgetItem *pItemInfo = new QTableWidgetItem(f);
        qDebug() << f;
        ui->tableWidget->setItem(nNowRow, 0, pItemInfo);
        ui->tableWidget->resizeColumnToContents(0);
    }
    if (Player.mediaStatus() == QMediaPlayer::NoMedia) {
        playerlist.setCurrentIndex(0);
        //Player.play();
        playerlist.setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        setEditline();
        //qDebug(Player.mediaStatus());
    }
}

void MainWindow::AddMusicFolder()//插入文件夹
{
    // 创建对话框
    QFileDialog* fd = new QFileDialog(this);
    // 文件夹
    fd->setFileMode(QFileDialog::Directory);
    // 执行
    QString folderName;
    if (fd->exec() == QDialog::Accepted)
    {
        // 返回选择的文件夹
        folderName = fd->selectedFiles().at(0);
    }
    //获取所选文件类型过滤器
    QStringList filters;
    filters << QString("*.mp3") << QString("*.flac") << QString("*.wav") << QString("*.wma");
    QStringList fileNames;
    TraverseFiles(folderName, filters, fileNames);
    // 插入列表中
    for (int i = 0; i < fileNames.length(); i++)
    {
        QString f = QFileInfo(fileNames.at(i)).fileName();
        if (!singlemusic(f))
            continue;
        int nNowRow = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(nNowRow);
        QString u = fileNames.at(i);
        list << u;
        playerlist.addMedia(QUrl::fromLocalFile(fileNames.at(i)));

        QTableWidgetItem *pItemInfo = new QTableWidgetItem(f);
        qDebug() << f;
        ui->tableWidget->setItem(nNowRow, 0, pItemInfo);
        ui->tableWidget->resizeColumnToContents(0);
    }
    if (Player.mediaStatus() == QMediaPlayer::NoMedia) {
        playerlist.setCurrentIndex(0);
        //Player.play();
        playerlist.setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        setEditline();
        //qDebug(Player.mediaStatus());
    }
}

void MainWindow::setEditline()//改变当前播放文字
{
    int m = playerlist.currentIndex();
    QString p = ui->tableWidget->item(m, 0)->text();
    switch (now%4) {
    case 1:
        ui->tableWidget->setStyleSheet("border-image:url(:/new/prefix1/backpicture/back1.jpg);");
        break;
    case 2:
        ui->tableWidget->setStyleSheet("border-image:url(:/new/prefix1/backpicture/back2.jpg);");
        break;
    case 3:
        ui->tableWidget->setStyleSheet("border-image:url(:/new/prefix1/backpicture/back3.jpg);");
        break;
    default:
        ui->tableWidget->setStyleSheet("border-image:url(:/new/prefix1/backpicture/back4.jpg);");
        break;
    }
    now++;
    ui->lineEdit->setText(p);
    resolve_lrc(p);
}

int MainWindow::singlemusic(QString music)//检查播放列表中是否已经存在歌曲
{
    int i;
    for (i = 0; i<playerlist.mediaCount(); i++) {
        if (music == ui->tableWidget->item(i, 0)->text()) {
            return 0;
        }
    }
    return 1;
}

void MainWindow::playTo(int i)//双击切换播放歌曲
{
    playerlist.setCurrentIndex(i);
    Player.play();
    QString p = ui->tableWidget->item(i, 0)->text();
    ui->lineEdit->setText(p);
}


void MainWindow::on_pushButton_2_clicked()//设置播放和暂停
{
    if (Player.state() == QMediaPlayer::PlayingState) {
        ui->pushButton_2->setIcon(QIcon(":/new/prefix1/play.png"));
        Player.pause();
    }
    else if (playerlist.isEmpty()) {
        QMessageBox::about(NULL, "通知", "列表为空");
        return;
    }
    else if (Player.mediaStatus() == QMediaPlayer::NoMedia) {
        playerlist.setCurrentIndex(0);
        Player.play();
        setEditline();
    }
    else {
        ui->pushButton_2->setIcon(QIcon(":/new/prefix1/pause.png"));
        Player.play();
        setEditline();
    }
}

void MainWindow::on_pushButton_3_clicked()//设置停止（暂停
{
    Player.pause();
}

void MainWindow::setEditline_2()//音量数字输出
{
    int p;
    p = ui->verticalSlider->value();
    QString pos = QString("%1").arg(p);
    ui->lineEdit_2->setText(pos);
}

void MainWindow::updateDuration(qint64 duration)//更新时间
{
    ui->horizontalSlider->setRange(0, duration);
    ui->horizontalSlider->setEnabled(duration>0);
    ui->horizontalSlider->setPageStep(duration / 10);
}

void MainWindow::updatePosition(qint64 position)//更新状态
{
    ui->horizontalSlider->setValue(position);
    QTime duration(0, position / 60000, qRound((position % 60000) / 1000.0));
    ui->label->setText(duration.toString(tr("mm:ss")));
}

void MainWindow::on_verticalSlider_valueChanged()//音量条变化时更新数字
{
    Player.setVolume(ui->verticalSlider->value());
    setEditline_2();
}

void MainWindow::setPosition(int position)//根据进度条设置音乐状态
{
    if (qAbs(Player.position() - position) > 99)
        Player.setPosition(position);
}

void MainWindow::on_horizontalSlider_valueChanged() {}

void MainWindow::DeleteSongFromList()//从列表中删除当前选中的歌曲
{
    if (ui->tableWidget->rowCount() == 0)
        return;
    int dindex = ui->tableWidget->currentRow();
    if (playerlist.currentIndex() == dindex) {
        if (playerlist.currentIndex() != 0) {
            playerlist.previous();
            Player.play();
            setEditline();
        }
        else {
            Player.stop();
            ui->lineEdit->setText("");
        }
    }
    playerlist.removeMedia(dindex);
    list.removeAt(dindex);
    int nSelectedRow = ui->tableWidget->currentRow();
    ui->tableWidget->removeRow(nSelectedRow);
}

void MainWindow::SaveMusicList()//保存当前的列表进入一个xml文件里
{
    QDir dir;
    QString filePath = dir.currentPath() + "/DefaultList.xml";//路径
    qDebug() << filePath;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
        return;
    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();                // 写文档头
    writer.writeStartElement("Default_Music_List");
    for (int i = 0; i < playerlist.mediaCount(); i++)
    {
        filePath = list.at(i);
        writer.writeStartElement("song");
        writer.writeAttribute("ID", QString::number(i + 1, 10));
        writer.writeTextElement("path", filePath);
        writer.writeEndElement();
    }
    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();
    if (playerlist.mediaCount() == 0)
        system("del DefaultList.xml");
}

void MainWindow::AddMusicToList(const QString &musicpath)//将xml文件中的歌曲添加进来
{
    int nNowRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(nNowRow);
    QString u = musicpath;
    list << u;
    playerlist.addMedia(QUrl::fromLocalFile(musicpath));
    QString f = QFileInfo(musicpath).fileName();
    QTableWidgetItem *pItemInfo = new QTableWidgetItem(f);
    ui->tableWidget->setItem(nNowRow, 0, pItemInfo);
    ui->tableWidget->resizeColumnToContents(0);
}

void MainWindow::ReadMusicList()//读取当前存在的xml文件
{
    QDir dir;
    QString fileName = dir.currentPath() + "/DefaultList.xml";
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "NO!";
        return;
    }
    QXmlStreamReader reader;
    reader.setDevice(&file);
    while (!reader.atEnd())
    {
        reader.readNext();
        if (reader.isStartElement())
        {
            if (reader.name() == "path")
            {
                AddMusicToList(reader.readElementText());
            }
        }
    }
    file.close();
    if (Player.mediaStatus() == QMediaPlayer::NoMedia) {
        playerlist.setCurrentIndex(0);
        setEditline();
    }
}

void MainWindow::message_music()//弹出歌曲详细信息
{
    musicmessage *m = new musicmessage;
    m->show();
}

void MainWindow::on_tableWidget_customContextMenuRequested()//右键菜单栏
{
    QMenu *pPopMenu = new QMenu(this);
    QMenu addMenu("添加音乐");
    addMenu.addAction("文件", this, SLOT(AddMusicFiles()));//添加菜单
    addMenu.addAction("电台下载", this, SLOT(AddMusicDoadFolder()));
    addMenu.addAction("其他文件夹", this, SLOT(AddMusicFolder()));

    pPopMenu->addMenu(&addMenu);
    pPopMenu->addAction("删除这首歌", this, SLOT(DeleteSongFromList()));//添加内容
    pPopMenu->addAction("保存歌曲列表", this, SLOT(SaveMusicList()));
    pPopMenu->addAction("读取歌曲列表", this, SLOT(ReadMusicList()));
    pPopMenu->addAction("歌曲详细信息", this, SLOT(message_music()));
    pPopMenu->exec(QCursor::pos());
}

void MainWindow::on_tableWidget_doubleClicked(const QModelIndex &index) {}

void MainWindow::on_pushButton_4_clicked()//上一首播放
{
    if (playerlist.isEmpty()) {
        QMessageBox::about(NULL, "通知", "列表为空");
        return;
    }
    if (playerlist.currentIndex() == 0) {
        QMessageBox::about(NULL, "通知", "没有上一首了");
        return;
    }
    playerlist.previous();
    Player.play();
    setEditline();
}

void MainWindow::on_pushButton_5_clicked()//下一首播放
{
    int cindex = playerlist.currentIndex();
    if (playerlist.isEmpty()) {
        QMessageBox::about(NULL, "通知", "列表为空");
        return;
    }
    if (++cindex == playerlist.mediaCount())
        playerlist.setCurrentIndex(0);
    else
        playerlist.setCurrentIndex(cindex);
    setEditline();
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)//设置播放顺序
{
    if (ui->comboBox->currentIndex() == 0)
        playerlist.setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    else if (ui->comboBox->currentIndex() == 1)
        playerlist.setPlaybackMode(QMediaPlaylist::Random);
    else if (ui->comboBox->currentIndex() == 2)
        playerlist.setPlaybackMode(QMediaPlaylist::Loop);
    else if (ui->comboBox->currentIndex() == 3)
        playerlist.setPlaybackMode(QMediaPlaylist::Loop);
    else
        playerlist.setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
}

void MainWindow::on_pushButton_6_clicked()
{
    QMessageBox::about(NULL, "关于我们", "oop修仙小组");
}

void MainWindow::resolve_lrc(const QString &source_file_name)//liric
{
    lrc_map.clear();
    if (source_file_name.isEmpty())
        return;
    QString file_name = source_file_name;
    QString lrc_file_name = file_name.remove(file_name.right(3)) + "lrc";//把音频文件的后缀改成lrc后缀

    QDir dir;
    QString filefolder = dir.currentPath() + "/OnlineRadioDownloadDir/" + lrc_file_name;
    // 打开歌词文件
    QFile file(filefolder);
    if (!file.open(QIODevice::ReadOnly)) {
        QString filefolder2 = dir.currentPath() + "/" + lrc_file_name;
        QFile file2(filefolder2);
        if (!file2.open(QIODevice::ReadOnly)) {
            ui->label_lrc->setText("暂无歌词！");
            return;
        }
    }
    // 设置字符串编码
    QString p = QString::fromLocal8Bit(file.readAll());
    QString all_text = QObject::tr("%1").arg(p);
    qDebug() << all_text;
    file.close();
    // 将歌词按行分解为歌词列表
    QStringList lines = all_text.split("\n");

    //这个是时间标签的格式[00:05.54]
    //正则表达式d{2}表示匹配2个数字
    QRegExp rx("\\[\\d{2}:\\d{2}\\.\\d{2}\\]");
    foreach(QString oneline, lines) {
        QString temp = oneline;
        temp.replace(rx, "");//用空字符串替换正则表达式中所匹配的地方,这样就获得了歌词文本
                             // 然后依次获取当前行中的所有时间标签，并分别与歌词文本存入QMap中
                             //indexIn()为返回第一个匹配的位置，如果返回为-1，则表示没有匹配成功
                             //正常情况下pos后面应该对应的是歌词文件
        int pos = rx.indexIn(oneline, 0);
        while (pos != -1) { //表示匹配成功
            QString cap = rx.cap(0);//返回第0个表达式匹配的内容
                                    // 将时间标签转换为时间数值，以毫秒为单位
            QRegExp regexp;
            regexp.setPattern("\\d{2}(?=:)");
            regexp.indexIn(cap);
            int minute = regexp.cap(0).toInt();
            regexp.setPattern("\\d{2}(?=\\.)");
            regexp.indexIn(cap);
            int second = regexp.cap(0).toInt();
            regexp.setPattern("\\d{2}(?=\\])");
            regexp.indexIn(cap);
            int millisecond = regexp.cap(0).toInt();
            qint64 totalTime = minute * 60000 + second * 1000 + millisecond * 10;
            // 插入到lrc_map中
            qDebug() << temp;
            lrc_map.insert(totalTime, temp);
            pos += rx.matchedLength();
            pos = rx.indexIn(oneline, pos);//匹配全部
        }
    }
    // 如果lrc_map为空
    if (lrc_map.isEmpty()) {
        ui->label_lrc->setText("文件内容错误！");
        return;
    }
}

void MainWindow::UpdateTime(qint64 time)//updata time
{
    //qint64 total_time_value = //直接获取该音频文件的总时长参数，单位为毫秒
    //这3个参数分别代表了时，分，秒；60000毫秒为1分钟，所以分钟第二个参数是先除6000,第3个参数是直接除1s
    //QTime total_time(0, (total_time_value/60000)%60, (total_time_value/1000)%60);
    //QTime current_time(0, (time/60000)%60, (time/1000)%60);//传进来的time参数代表了当前的时间
    //QString str = current_time.toString("mm:ss") + "/" + total_time.toString("mm:ss");
    //time_label->setText(str);

    // 获取当期时间对应的歌词
    if (!lrc_map.isEmpty()) {
        // 获取当前时间在歌词中的前后两个时间点
        qint64 previous = 0;
        qint64 later = 0;
        //keys()方法返回lrc_map列表
        foreach(qint64 value, lrc_map.keys()) {
            if (time >= value) {
                previous = value;
            }
            else {
                later = value;
                break;
            }
        }

        // 达到最后一行,将later设置为歌曲总时间的值
        // 获取当前时间所对应的歌词内容
        QString current_lrc = lrc_map.value(previous);

        // 如果是新的一行歌词，那么重新开始显示歌词遮罩
        if (current_lrc != ui->label_lrc->text()) {
            ui->label_lrc->setText(current_lrc);
            //top_label->setText(current_lrc);
            //qint64 interval_time = later - previous;
            //lrc->start_lrc_mask(interval_time);
        }
    }
    else {  // 如果没有歌词文件，则在顶部标签中显示歌曲标题
        return;
    }
}

void MainWindow::on_Tray_clicked()
{
    //隐藏主界面
    this->hide();
    if (flag == 0) {
        //新建QSystemTrayIcon对象
        mSysTrayIcon = new QSystemTrayIcon(this);
        //将新建的icon设到QSystemTrayIcon对象中
        mSysTrayIcon->setIcon(QIcon(":/new/prefix1/my.ico"));
        //当鼠标移动到托盘上的图标时，会显示message
        mSysTrayIcon->setToolTip(QObject::trUtf8("音乐播放器"));
        connect(mSysTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
        //新建托盘菜单
        createActions();
        createMenu();
        //在系统托盘显示此对象
        mSysTrayIcon->show();
        flag++;
    }
}

void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        //单击托盘图标：无反应
        break;
    case QSystemTrayIcon::DoubleClick:
        //双击托盘图标
        //双击后才会显示主程序窗口
        this->show();
        break;
    default:
        break;
    }
}

void MainWindow::createActions()
{
    mShowMainAction = new QAction(QObject::trUtf8("显示主界面"), this);
    connect(mShowMainAction, SIGNAL(triggered()), this, SLOT(on_showMainAction()));

    mExitAppAction = new QAction(QObject::trUtf8("退出程序"), this);
    connect(mExitAppAction, SIGNAL(triggered()), this, SLOT(on_exitAppAction()));
}

void MainWindow::createMenu()
{
    mMenu = new QMenu(this);
    //新增选项---显示主界面
    mMenu->addAction(mShowMainAction);
    //加上一个分隔符
    mMenu->addSeparator();
    //新增选项---退出
    mMenu->addAction(mExitAppAction);
    //把QMenu加入QSystemTrayIcon的对象
    mSysTrayIcon->setContextMenu(mMenu);
}

void MainWindow::on_showMainAction()
{
    this->show();
}

void MainWindow::on_exitAppAction()
{
    exit(0);
}

void MainWindow::on_addmusicfromfile()
{
    this->AddMusicFiles();
}

void MainWindow::on_addmusicfromfolder()
{
    this->AddMusicFolder();
}

void MainWindow::on_addmusicfromchannel()
{
    this->AddMusicDoadFolder();
}

void MainWindow::on_deletesong()
{
    this->DeleteSongFromList();
}

void MainWindow::on_savelist()
{
    this->SaveMusicList();
}

void MainWindow::on_readlist()
{
    this->ReadMusicList();
}

void MainWindow::on_aboutsoftware()
{
    QMessageBox::information(this, "关于软件", "oop修仙小组");
}

void MainWindow::on_howtouse()
{
    QMessageBox::information(this, "如何使用", "使用规则：类似于普通音乐播放器");
}

void MainWindow::on_ChannelMenu_clicked()
{
    m_music_radio_driver->ShowChannelMenu();
}
