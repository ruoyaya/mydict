#include "dictdatabase.hpp"
#include "dictinfo.hpp"
#include "dictentry.hpp"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <typeinfo>

#define DICT_CREATE_SQL "CREATE TABLE T_DICT (ID INTEGER PRIMARY KEY,DICT_NAME TEXT,DICT_PATH TEXT,WORD_COUNT INTEGER,SYN_WORD_COUNT INTEGER,IDX_FILE_SIZE INTEGER,IDX_OFFSET_BITS INTEGER,VERSION TEXT,BOOKNAME TEXT,AUTHOR TEXT,EMAIL TEXT,WEBSITE TEXT,DATE TEXT,COPYRIGHT TEXT,DESCRIPTION TEXT,SAMETYPESEQUENCE TEXT,DICT_TYPE TEXT,INFO_FILE TEXT, INDEX_FILE TEXT,DATA_FILE TEXT,MEMO TEXT)"
#define DICT_INSERT_SQL "INSERT INTO T_DICT (ID,DICT_NAME,DICT_PATH,WORD_COUNT,SYN_WORD_COUNT,IDX_FILE_SIZE,IDX_OFFSET_BITS,VERSION,BOOKNAME,AUTHOR,EMAIL,WEBSITE,DATE,COPYRIGHT,DESCRIPTION,SAMETYPESEQUENCE,DICT_TYPE,INFO_FILE, INDEX_FILE,DATA_FILE,MEMO) VALUES(:id,:dict_name,:dict_path,:word_count,:syn_word_count,:idx_file_size,:idx_offset_bits,:version,:bookname,:author,:email,:website,:date,:copyright,:description,:sametypesequence,:dict_type,:info_file,:index_file,:data_file,:memo)"
#define DICT_NAME_EXISTS_SQL "select count(1) num from T_DICT where DICT_NAME = :dict_name "
#define DICT_QUERY_ALL_SQL "SELECT ID,DICT_NAME,DICT_PATH,WORD_COUNT,SYN_WORD_COUNT,IDX_FILE_SIZE,IDX_OFFSET_BITS,VERSION,BOOKNAME,AUTHOR,EMAIL,WEBSITE,DATE,COPYRIGHT,DESCRIPTION,SAMETYPESEQUENCE,DICT_TYPE,INFO_FILE, INDEX_FILE,DATA_FILE,MEMO FROM T_DICT"

#define DICT_INDEX_CREATE_SQL "CREATE TABLE T_DICT_INDEX (WORD TEXT,OFFSET INTEGER,WORD_COUNT SIZE, DICT_ID INTEGER)"
#define DICT_INDEX_INSERT_SQL "INSERT INTO T_DICT_INDEX (WORD,OFFSET,WORD_COUNT,DICT_ID) VALUES(:word,:offset,:word_count,:dict_id)"
#define DICT_INDEX_QUERY_SQL "SELECT WORD,OFFSET,WORD_COUNT,DICT_ID FROM T_DICT_INDEX WHERE WORD like '%1%'"

//#define DICT_FILE_CREATE_SQL "CREATE TABLE T_DICT_FILE (ID INTEGER PRIMARY KEY,NAME TEXT,INFO_PATH TEXT,INDEX_PATH TEXT,DATA_PATH TEXT)"
//#define DICT_FILE_INSERT_SQL "INSERT INTO T_DICT_FILE (ID,NAME,INFO_PATH,INDEX_PATH,DATA_PATH) VALUES(:id,:name,:info_path,:index_path,:data_path)"
//#define DICT_FILE_QUERY_ALL_SQL "SELECT ID,NAME,INFO_PATH,INDEX_PATH,DATA_PATH FROM T_DICT_FILE"


DictDatabase::DictDatabase(QString fileName):
    m_fileName(fileName)
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(fileName);
    if (!database.open()) {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    }
    else {
        qDebug() << "Succeed to connect database." ;
    }

    initTable();
}


DictDatabase::~DictDatabase() {
    if(database.isOpen()) {
        database.close();
    }
}

bool DictDatabase::tableExists(QString tableName) {
    QSqlQuery sql_query;
    sql_query.prepare("SELECT count(1) FROM sqlite_master WHERE type='table' AND name=:name");
    sql_query.bindValue(":name", tableName);
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
        return false;
    }
    else
    {
        sql_query.next();
        int count = sql_query.value(0).toInt();
        if(count > 0) {
            return true;
        } else {
            return false;
        }
    }
}

void DictDatabase::initTable() {
    if(!database.isOpen()) {
        return;
    }
    QSqlQuery sql_query;

    if(!tableExists("T_DICT")) {
        qDebug() << "DICT_CREATE_SQL";
        sql_query.exec(DICT_CREATE_SQL);
    }
    if(!tableExists("T_DICT_INDEX")) {
        qDebug() << "DICT_INDEX_CREATE_SQL";
        sql_query.exec(DICT_INDEX_CREATE_SQL);
    }
}

bool DictDatabase::dictExists(QString dictName) {
    QSqlQuery sql_query;
    sql_query.prepare(DICT_NAME_EXISTS_SQL);
    sql_query.bindValue(":dict_name", dictName);
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
        return false;
    }
    else
    {
        sql_query.next();
        int count = sql_query.value(0).toInt();
        if(count > 0) {
            return true;
        } else {
            return false;
        }
    }
}

bool DictDatabase::dictIndexExists(int dictId) {
    QSqlQuery sql_query;
    sql_query.prepare("select count(1) from T_DICT_INDEX where DICT_ID = :dictid ");
    sql_query.bindValue(":dictid", dictId);
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
        return false;
    }
    else
    {
        sql_query.next();
        int count = sql_query.value(0).toInt();
        if(count > 0) {
            return true;
        } else {
            return false;
        }
    }
}

int DictDatabase::getMaxDictId() {
    QSqlQuery sql_query("SELECT MAX(ID) ID FROM T_DICT");
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
        return 0;
    }
    else
    {
        sql_query.next();
        return sql_query.value(0).toInt();
    }
}

int DictDatabase::getDictId(QString dictName) {
    QSqlQuery sql_query;
    sql_query.prepare("SELECT ID FROM T_DICT where DICT_NAME = :name");
    sql_query.bindValue(":name", dictName);
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
        return -1;
    }
    else
    {
        sql_query.next();
        return sql_query.value(0).toInt();
    }
}

int DictDatabase::addDict(DictInfo &dictInfo) {
    int id = getDictId(dictInfo.dictName());
    if(id > 0) {
        return id;
    }

    qDebug() << "getMaxDictId";
    int newId = getMaxDictId() + 1;

    qDebug() << "DICT_INSERT_SQL";
    QSqlQuery sql_query;
    sql_query.prepare(DICT_INSERT_SQL);
    sql_query.bindValue(":id", newId);
    sql_query.bindValue(":dict_name", dictInfo.dictName());
    sql_query.bindValue(":dict_path", dictInfo.dictPath());
    sql_query.bindValue(":word_count", dictInfo.wordcount());
    sql_query.bindValue(":syn_word_count", dictInfo.synwordcount());
    sql_query.bindValue(":idx_file_size", dictInfo.idxfilesize());
    sql_query.bindValue(":idx_offset_bits", dictInfo.idxoffsetbits());
    sql_query.bindValue(":version", dictInfo.version());
    sql_query.bindValue(":bookname", dictInfo.bookname());
    sql_query.bindValue(":author", dictInfo.author());
    sql_query.bindValue(":email", dictInfo.email());
    sql_query.bindValue(":website", dictInfo.website());
    sql_query.bindValue(":date", dictInfo.date());
    sql_query.bindValue(":copyright", dictInfo.copyright());
    sql_query.bindValue(":description", dictInfo.description());
    sql_query.bindValue(":sametypesequence", dictInfo.sametypesequence());
    sql_query.bindValue(":dict_type", dictInfo.dicttype());
    sql_query.bindValue(":info_file", dictInfo.infoFile());
    sql_query.bindValue(":index_file", dictInfo.indexFile());
    sql_query.bindValue(":data_file", dictInfo.dataFile());
    sql_query.bindValue(":memo", dictInfo.memo());


    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
        return -1;
    }
    else
    {
        return newId;
    }
}


int DictDatabase::addDictIndex(int dictId, QString& word, QString& offset, QString& size) {
    qDebug() << "DICT_INDEX_INSERT_SQL";
    QSqlQuery sql_query;
    sql_query.prepare(DICT_INDEX_INSERT_SQL);
    sql_query.bindValue(":word", word);
    sql_query.bindValue(":offset", offset);
    sql_query.bindValue(":word_count", size);
    sql_query.bindValue(":dict_id", dictId);

    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
        return -1;
    }
    else
    {
        return 1;
    }
}

int DictDatabase::addDictIndex(QList<DictEntry*>& list){
    qDebug() << "BATCH DICT_INDEX_INSERT_SQL";

    database.transaction();
    QSqlQuery sql_query;

    sql_query.prepare(DICT_INDEX_INSERT_SQL);

    for (QList<DictEntry*>::iterator index = list.begin();index != list.end(); ++index) {
        DictEntry* entry = *index;
        sql_query.bindValue(":word", entry->key);
        sql_query.bindValue(":offset", entry->offset);
        sql_query.bindValue(":word_count", entry->size);
        sql_query.bindValue(":dict_id", entry->dictId);
        if(!sql_query.exec()){
            qDebug()<<sql_query.lastError();
        }
    }
    database.commit();
    return list.size();
}

QList<DictEntry*>* DictDatabase::search(QString& word, QList<qint32>* dictId) {

    QString sql(DICT_INDEX_QUERY_SQL);
    sql = sql.arg(word);

    if(dictId != nullptr && dictId->size() > 0 && !dictId->contains(-1)) {
        sql.append(" AND DICT_ID in ( ");
        for (QList<qint32>::iterator i = dictId->begin(); i != dictId->end(); ++i) {
            if(i == dictId->end() -1) {
                sql.append("%1");
            } else {
                sql.append("%1,");
            }
            sql = sql.arg(*i);
        }
        sql.append(" ) ");
    }

    sql.append(" ORDER BY LOWER(WORD),DICT_ID ");

    qDebug() << sql;

    QSqlQuery sql_query;
    if(!sql_query.exec(sql))
    {
        qDebug()<<sql_query.lastError();
        return nullptr;
    }
    else
    {
        QList<DictEntry*>* retList = new QList<DictEntry*>();
        //WORD,OFFSET,WORD_COUNT,DICT_ID
        while(sql_query.next()) {
            QString word = sql_query.value(0).toString();
            quint32 offset = sql_query.value(1).toUInt();
            quint32 word_count = sql_query.value(2).toUInt();
            qint32 dict_id = sql_query.value(3).toInt();
            retList->append(new DictEntry(dict_id, word, offset, word_count));

        }
        return retList;
    }
}

QList<DictInfo*>* DictDatabase::getAllDictInfo() {

    QSqlQuery sql_query;
    if(!sql_query.exec(DICT_QUERY_ALL_SQL))
    {
        qDebug()<<sql_query.lastError();
        return nullptr;
    }
    else
    {
        QList<DictInfo*>* retList = new QList<DictInfo*>();
        //ID,DICT_NAME,DICT_PATH,WORD_COUNT,SYN_WORD_COUNT,IDX_FILE_SIZE,IDX_OFFSET_BITS,VERSION,BOOKNAME,AUTHOR,EMAIL,WEBSITE,DATE,COPYRIGHT,DESCRIPTION,SAMETYPESEQUENCE,DICT_TYPE,INFO_FILE, INDEX_FILE,DATA_FILE,MEMO
        while(sql_query.next()) {

            qint32 id = sql_query.value(0).toInt();
            QString dict_name = sql_query.value(1).toString();
            QString dict_path = sql_query.value(2).toString();
            qint32 word_count = sql_query.value(3).toInt();
            qint32 syn_word_count = sql_query.value(4).toInt();
            qint32 idx_file_size = sql_query.value(5).toInt();
            qint32 idx_offset_bits = sql_query.value(6).toInt();
            QString version = sql_query.value(7).toString();
            QString bookname = sql_query.value(8).toString();
            QString author = sql_query.value(9).toString();
            QString email = sql_query.value(10).toString();
            QString website = sql_query.value(11).toString();
            QString date = sql_query.value(12).toString();
            QString copyright = sql_query.value(13).toString();
            QString description = sql_query.value(14).toString();
            QString sametypesequence = sql_query.value(15).toString();
            QString dict_type = sql_query.value(16).toString();
            QString info_file = sql_query.value(17).toString();
            QString index_file = sql_query.value(18).toString();
            QString data_file = sql_query.value(19).toString();
            QString memo = sql_query.value(20).toString();

            retList->append(new DictInfo(id,word_count,syn_word_count,idx_file_size,idx_offset_bits,
                                         version,bookname,author,email,website,date,copyright,description,
                                         sametypesequence,dict_type,dict_name,dict_path,info_file,index_file,
                                         data_file,memo));
        }
        return retList;
    }
}

void DictDatabase::updateDict(QList<QString>* dictNames){

    QString sql("SELECT ID,DICT_NAME FROM T_DICT");

    if(dictNames != nullptr && dictNames->size() > 0) {
        sql.append(" WHERE DICT_NAME not in ( ");
        for (QList<QString>::iterator i = dictNames->begin(); i != dictNames->end(); ++i) {
            if(i == dictNames->end() -1) {
                sql.append("'%1'");
            } else {
                sql.append("'%1',");
            }
            sql = sql.arg(*i);
        }
        sql.append(" ) ");
    }


    qDebug() << sql;

    QSqlQuery sql_query;
    if(!sql_query.exec(sql))
    {
        qDebug()<<sql_query.lastError();
    }
    else
    {
        QList<qint32>* toRemoveIds = new QList<qint32>();
        while(sql_query.next()) {
            qint32 id = sql_query.value(0).toInt();
            toRemoveIds->append(id);
        }
        if(toRemoveIds->size() > 0) {

            QString sqlDict("DELETE FROM T_DICT");
            sqlDict.append(" WHERE ID in ( ");

            QString sqlIndex("DELETE FROM T_DICT_INDEX");
            sqlIndex.append(" WHERE DICT_ID in ( ");

            for (QList<qint32>::iterator i = toRemoveIds->begin(); i != toRemoveIds->end(); ++i) {
                if(i == toRemoveIds->end() -1) {
                    sqlDict.append("'%1'");
                    sqlIndex.append("'%1'");
                } else {
                    sqlDict.append("'%1',");
                    sqlIndex.append("'%1',");
                }
                sqlDict = sqlDict.arg(*i);
                sqlIndex = sqlIndex.arg(*i);
            }
            sqlDict.append(" ) ");
            sqlIndex.append(" ) ");

            qDebug() << sqlDict;
            qDebug() << sqlIndex;

            if(!sql_query.exec(sqlDict)){
                qDebug()<<sql_query.lastError();
                return;
            }

            if(!sql_query.exec(sqlIndex)){
                qDebug()<<sql_query.lastError();
                return;
            }
        }
    }
}
