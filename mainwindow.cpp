#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "dict/dict.hpp"
#include "dict/dictentry.hpp"
#include <QFileDialog>
#include <QDebug>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_dictActionMap(nullptr),
    m_dictActionList(nullptr),
    m_popupMenu(nullptr),
    m_currentDictId(-1),
    m_seachWordList(nullptr),
    m_dictBeforeAction(nullptr)
{
    ui->setupUi(this);

    ui->resultSplitter->setStretchFactor(0, 1);
    ui->resultSplitter->setStretchFactor(1, 3);

    m_dict = new Dict();

    m_seachWordList = new QList<QString>();
    m_seachWordIndex = m_seachWordList->end();

    m_dictActionMap = new QMap<QAction*, DictInfo*>();
    m_dictActionList = new QList<QAction*>();

    initDictActions();

    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_dict;
    delete m_popupMenu;
    delete m_seachWordList;
}


void MainWindow::writeSettings(){
    QSettings settings("mydict.yikeo.com", "mydict");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void MainWindow::readSettings(){
    QSettings settings("mydict.yikeo.com","mydict");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(700, 350)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::about(){
    QMessageBox message(QMessageBox::NoIcon, "关于词典", "小词典，欢迎留言交流.");
    message.setIconPixmap(QPixmap("://resources/icons.iconset/icon.png"));
    message.exec();
}

void MainWindow::cleanDictActions(){
    if(m_dictActionList && m_dictActionList->size() > 0) {
        while(m_dictActionList->size() > 0){
            QAction* action = m_dictActionList->takeFirst();
            action->disconnect(this);
            m_popupMenu->removeAction(action);
            m_dictActionMap->remove(action);
            delete action;
        }
    }
}

void MainWindow::initDictActions()
{
    if(m_popupMenu == nullptr) {
        m_popupMenu = new QMenu();
        QAction *addDictAction = m_popupMenu->addAction("添加词典");
        connect(addDictAction, &QAction::triggered, [this] {
            emit on_actionAdd_triggered();
        });

        QAction *refreshDictAction = m_popupMenu->addAction("刷新词典数据");
        connect(refreshDictAction, &QAction::triggered, [this] {
            emit refreshDict();
        });

        m_popupMenu->addSeparator();

        m_dictBeforeAction = m_popupMenu->addSeparator();
        QAction *aboutAction = m_popupMenu->addAction("关于");
        connect(aboutAction, &QAction::triggered, [this] {
            emit about();
        });

    }


    m_dictActionMap->empty();
    m_dictActionList->empty();

    QAction *allDictAction = new QAction("全部");
    allDictAction->setCheckable(true);
    qint32 dictId = -1;
    connect(allDictAction, &QAction::triggered, [this, allDictAction, dictId] {
        emit changeDict(allDictAction, dictId);
    });
    m_dictActionMap->insert(allDictAction, nullptr);
    m_dictActionList->append(allDictAction);

    QList<DictInfo*>* dictInfos = m_dict->getDictInfoList();
    if(dictInfos && dictInfos->size() > 0) {

        for (QList<DictInfo*>::iterator i = dictInfos->begin(); i != dictInfos->end(); ++i) {
            DictInfo* info = *i;

            QAction *dictAction = new QAction(info->bookname().trimmed());
            dictAction->setCheckable(true);

            dictId = info->id();
            connect(dictAction, &QAction::triggered, [this, dictAction, dictId] {
                emit changeDict(dictAction, dictId);
            });

            m_dictActionMap->insert(dictAction, info);
            m_dictActionList->append(dictAction);
        }
    }

    m_popupMenu->insertActions(m_dictBeforeAction, *m_dictActionList);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    m_popupMenu->popup(event->globalPos());
    event->accept();
}


void MainWindow::closeEvent(QCloseEvent* event){
    writeSettings();
    event->accept();
}

void MainWindow::changeDict(QAction* sender, qint32 dictId)
{
    bool checkFirst = false;
    bool checkOther = false;
    bool changeOther = false;
    bool checkCurrent = false;

    if(sender->isChecked()){
        if(dictId == -1) {
            //select all
            checkFirst = true;
            changeOther = true;
            checkOther = true;
            checkCurrent = true;
        } else {
            //select each
            checkFirst = false;
            changeOther = false;
            checkOther = false;
            checkCurrent = true;
        }
    } else {
        if(dictId == -1) {
            //unselect all
            checkFirst = false;
            changeOther = true;
            checkOther = false;
            checkCurrent = false;
        } else {
            //unselect each
            checkFirst = false;
            changeOther = false;
            checkOther = false;
            checkCurrent = false;
        }
    }

    for (QList<QAction*>::iterator i = m_dictActionList->begin(); i != m_dictActionList->end(); ++i) {
        QAction* key = *i;
        if(i == m_dictActionList->begin()) {
            key->setChecked(checkFirst);
        } else if(key == sender) {
            key->setChecked(checkCurrent);
        } else {
            if(changeOther) {
                key->setChecked(checkOther);
            }
        }
    }

}

QList<qint32>* MainWindow::getCheckedDictId(){

    QList<qint32>* dictIdList = new QList<qint32>();
    if(!m_dictActionList || m_dictActionList->size() <= 1 || (*m_dictActionList->begin())->isChecked()) {
        dictIdList->append(-1);
    } else {
        for (QList<QAction*>::iterator i = m_dictActionList->begin(); i != m_dictActionList->end(); ++i) {
            if(i != m_dictActionList->begin()) {
                QAction* key = *i;
                if(key->isChecked()) {
                    dictIdList->append(m_dictActionMap->value(key)->id());
                }
            }

        }
    }
    return dictIdList;
}


void MainWindow::on_searchEdit_returnPressed()
{
    QString word = ui->searchEdit->text();

    if(word.size() > 0) {
        m_seachWordList->append(word);
        m_seachWordIndex = m_seachWordList->end() -1;
        search(word);
    }
}

void MainWindow::search(QString& word){
    ui->listWidget->clear();
    QList<qint32>* dictId = getCheckedDictId();
    QList<DictEntry*>* result = m_dict->search(word, dictId);
    if(result && result->size() > 0) {
        QList<QString> keyList;
        for (QList<DictEntry*>::iterator i = result->begin(); i != result->end() ; ++i) {
            QString keyword = (*i)->key.toLower();
            if(!keyList.contains(keyword)){
                keyList << keyword;
                ui->listWidget->addItem(keyword);
            }
        }
        ui->listWidget->setCurrentRow(0);
    }
}

void MainWindow::on_listWidget_itemSelectionChanged()
{
    qDebug() << "on_listWidget_itemSelectionChanged";
    QList<QListWidgetItem*> selectedList = ui->listWidget->selectedItems();
    if(selectedList.size() > 0) {
        QListWidgetItem* item = *selectedList.begin();
        if(item && item->text().size() > 0) {
            loadArticle(item->text());
        }
    }
}


void MainWindow::loadArticle(QString text){
    QString articleHtml = m_dict->loadArticle(text);
    ui->textBrowser->setHtml(articleHtml);
}

void MainWindow::on_actionAdd_triggered()
{
    QString dictFileName = QFileDialog::getOpenFileName(nullptr,"选择词典文件","","*.*");
    if(dictFileName != nullptr && dictFileName.size() > 0) {
        m_dict->convertDict(dictFileName);
    }
}

void MainWindow::refreshDict()
{
    m_dict->reloadDict();
    cleanDictActions();
    initDictActions();
}


void MainWindow::on_fontSmallButton_clicked()
{
    QFont font = ui->textBrowser->font();
    qDebug() << font.pixelSize();
    font.setPixelSize(font.pixelSize() - 1);
    ui->textBrowser->setFont(font);
}

void MainWindow::on_fontBigButton_clicked()
{
    QFont font = ui->textBrowser->font();
    qDebug() << font.pixelSize();
    font.setPixelSize(font.pixelSize() + 1);
    ui->textBrowser->setFont(font);
}

void MainWindow::on_backButton_clicked()
{
    if(m_seachWordList) {
        if(m_seachWordIndex > m_seachWordList->begin()) {
            --m_seachWordIndex;
            QString word = *m_seachWordIndex;
            ui->searchEdit->setText(word);
            search(word);
        }
    }
}

void MainWindow::on_forwardButton_clicked()
{
    if(m_seachWordList) {
        if(m_seachWordIndex < m_seachWordList->end() - 1) {
            ++m_seachWordIndex;
            QString word = *m_seachWordIndex;
            ui->searchEdit->setText(word);
            search(word);
        }
    }
}
