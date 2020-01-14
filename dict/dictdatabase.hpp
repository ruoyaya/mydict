#ifndef DICTDATABASE_HPP
#define DICTDATABASE_HPP

#include <QObject>
#include <QString>
#include <QSqlDatabase>

class DictInfo;
class DictEntry;

class DictDatabase
{
public:
    DictDatabase(QString fileName);
    ~DictDatabase();

    bool tableExists(QString tableName);
    bool dictExists(QString dictName);
    bool dictIndexExists(int dictId);

    void initTable();
    int getMaxDictId();

    int getDictId(QString dictName);
    int addDict(DictInfo &dictInfo);
    int addDictIndex(int dictId, QString& word, QString& offset, QString& size);
    int addDictIndex(QList<DictEntry*>& list);

    QList<DictInfo*>* getAllDictInfo();
    QList<DictEntry*>* search(QString& word, QList<qint32>* dictId);
    void updateDict(QList<QString>* dictNames);
private:
    QString m_fileName;
    QSqlDatabase database;
};

#endif // DICTDATABASE_HPP
