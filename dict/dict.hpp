#ifndef DICT_HPP
#define DICT_HPP

#include <QString>
#include <QFile>

class DictEntry;
class DictInfo;
class DictDatabase;

class Dict
{
public:
    Dict();
    ~Dict();
    QList<DictEntry*>* search(QString word, QList<qint32>* dictId);
    QString loadArticle(QString word);
    void loadArticle(const DictEntry* entry, QFile* dataFile, QString& articleText);
    void convertDict(QString fileName);
    void addDict2Database(QString& dictName);
    void addDict2Database(QString dictId, QString dictPath);
    void initDictInfoList();
    QList<DictInfo*>* getDictInfoList();
    void reloadDict();
    void clearDictInfo();
private:
    QString m_dataHome;
    DictDatabase* m_database;
    QList<DictEntry*>* m_result;
    QMap<qint32,QFile*>* m_dataFileMap;
    QMap<qint32,DictInfo*>* m_dictInfoMap;
    QList<DictInfo*>* m_dictInfos;
};

#endif // DICT_HPP
