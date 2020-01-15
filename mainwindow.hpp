#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <dict/dict.hpp>
#include <QMainWindow>
#include <QListWidgetItem>
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QList<qint32>* getCheckedDictId();
    void loadArticle(QString text);
    void initDictActions();
    void cleanDictActions();
    void search(QString& word);
protected:
      void contextMenuEvent(QContextMenuEvent *event);
      void closeEvent(QCloseEvent*event);
      void writeSettings();
      void readSettings();
private slots:
    void on_searchEdit_returnPressed();
    void on_actionAdd_triggered();
    void on_fontSmallButton_clicked();
    void on_fontBigButton_clicked();
    void on_backButton_clicked();
    void on_forwardButton_clicked();
    void on_listWidget_itemSelectionChanged();

    void changeDict(QAction* sender, qint32 dictId);
    void refreshDict();
    void about();
private:
    Ui::MainWindow *ui;
    QFont font;
    Dict* m_dict;
    QMap<QAction*, DictInfo*>* m_dictActionMap;
    QList<QAction*>* m_dictActionList;
    QMenu* m_popupMenu;
//    QList<QMetaObject::Connection>* m_popupConnections;
    qint32 m_currentDictId;
    QList<QString>* m_seachWordList;
    QList<QString>::iterator m_seachWordIndex;
    QAction* m_dictBeforeAction;
};

#endif // MAINWINDOW_HPP
