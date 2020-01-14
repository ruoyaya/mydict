#include "dict.hpp"
#include "dictentry.hpp"
#include "dictinfo.hpp"
#include "dictdatabase.hpp"
#include "convert/dictconvert.hpp"
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

Dict::Dict():
    m_database(nullptr),
    m_dataFileMap(nullptr),
    m_dictInfoMap(nullptr),
    m_dictInfos(nullptr)
{

    QDir currentDir = QCoreApplication::applicationDirPath();
#ifdef Q_OS_OSX
    currentDir.cdUp();
#endif
    this->m_dataHome = currentDir.path() + "/data";
    this->m_result = nullptr;


    QDir dataDir(this->m_dataHome);
    if(!dataDir.exists()) {
        dataDir.mkpath(".");
    }


    QString dbFileName(this->m_dataHome + "/dictionary.db");
    m_database = new DictDatabase(dbFileName);

    initDictInfoList();
}

void Dict::reloadDict() {

    QDir dictDir(this->m_dataHome + "/dictionaries");
    if(!dictDir.exists()) {
        dictDir.mkpath(".");
    }

    QStringList subDirs = dictDir.entryList(QDir::Filter::Dirs);
    QList<QString>* dictNames = new QList<QString>();
    if(subDirs.size() > 0) {
        for (QStringList::iterator dirIndex = subDirs.begin(); dirIndex != subDirs.end(); dirIndex++) {
            if(dirIndex->endsWith(".dictionary")) {
                QString fullName = *dirIndex;
                QString dictName = fullName.left(fullName.size() - 11);
                addDict2Database(dictName, dictDir.path() + "/" + fullName);
                dictNames->append(dictName);
            }
        }
    }

    m_database->updateDict(dictNames);

    clearDictInfo();
    initDictInfoList();
}

Dict::~Dict() {

    if(this->m_result && this->m_result->size() > 0) {
        for (QList<DictEntry*>::iterator i = this->m_result->begin(); i != this->m_result->end(); ++i) {
            delete *i;
        }
        delete this->m_result;
        this->m_result = nullptr;
    }

    clearDictInfo();
}



QList<DictEntry*>* Dict::search(QString word, QList<qint32>* dictId) {
    if(this->m_result && this->m_result->size() > 0) {
        for (QList<DictEntry*>::iterator i = this->m_result->begin(); i != this->m_result->end(); ++i) {
            delete *i;
        }
        delete this->m_result;
        this->m_result = nullptr;
    }

    this->m_result = m_database->search(word, dictId);
    return this->m_result;
}

QString Dict::loadArticle(QString word) {

    QString articleText;

    if(this->m_dataFileMap && this->m_dataFileMap->size() > 0
            && this->m_result && this->m_result->size() > 0) {
        for (QList<DictEntry*>::iterator i = this->m_result->begin(); i != this->m_result->end() ; ++i) {
            DictEntry * entry = *i;
            QString key = entry->key.trimmed().toLower();
            if(key.compare(word) == 0 && this->m_dataFileMap->contains(entry->dictId)) {
                QFile* dataFile = this->m_dataFileMap->value(entry->dictId);

                QString entryText;
                loadArticle(entry, dataFile, entryText);

                if(entryText.size() > 0) {
                    articleText.append("<h2>" + entry->key.trimmed() + "</h2>");
                    if(m_dictInfoMap->contains(entry->dictId)) {
                        articleText.append("<h4>《" + m_dictInfoMap->value(entry->dictId)->bookname().trimmed() + "》</h4>");
                    }

                    entryText.replace("\"","&quot;");
                    entryText.replace("&","&amp;");
//                    entryText.replace("<","&lt;");
//                    entryText.replace(">","&gt;");
                    entryText.replace(" ","&nbsp;");
                    entryText.replace("\n","</p><p>");

                    articleText.append("<p>" + entryText + "</p>");
                    articleText.append("<hr/>");
                }

            }

        }

    }

//    qDebug() << articleText;
    return articleText;
}

void Dict::loadArticle(const DictEntry* entry, QFile* dataFile, QString& articleText) {
    if(!dataFile) {
        return;
    }
    if(!dataFile->isOpen()) {
        if(!dataFile->open(QFile::ReadOnly)) {
            return;
        }
    }

    quint32 idxitem_offset = entry->offset;
    quint32 idxitem_size = entry->size;

    dataFile->seek(idxitem_offset);
    articleText.push_back(dataFile->read(idxitem_size));
}


void Dict::convertDict(QString fileName) {
    if(fileName != nullptr && fileName.size() > 0) {
        QString dictName = DictConvert::convert(fileName);
        if(dictName.size() > 0) {
            addDict2Database(dictName);
        }
    }
}

void Dict::addDict2Database(QString& dictName){
    QDir dictDir(this->m_dataHome + "/dictionaries/" + dictName + ".dictionary");
    if(dictDir.exists()) {
        addDict2Database(dictName, dictDir.path());
    }
}

void Dict::addDict2Database(QString dictName, QString dictPath) {

    QDir dictDir(dictPath);
    QString dictInfoFile;
    QString dictIndexFile;
    QString dictDataFile;

    QStringList dictInfoFiles = dictDir.entryList(QDir::Filter::Files);
    if(dictInfoFiles.size() > 0) {
        for (QStringList::iterator i = dictInfoFiles.begin(); i != dictInfoFiles.end(); i++) {
            QString fileName = (*i);
            if(fileName.endsWith(".dict")) {
                dictDataFile = fileName;
            } else if(fileName.endsWith(".index")) {
                dictIndexFile = fileName;
            } else if(fileName.endsWith(".info")) {
                dictInfoFile = fileName;
            }
        }
    }

    if(dictInfoFile.size() > 0 && dictIndexFile.size() > 0 && dictDataFile.size() > 0) {

        if(!m_database->dictExists(dictName)) {
            DictInfo dictInfo(dictDir.path() + "/" + dictInfoFile);
            dictInfo.setDictName(dictName);
            dictInfo.setDictPath(dictPath);
            dictInfo.setInfoFile(dictInfoFile);
            dictInfo.setIndexFile(dictIndexFile);
            dictInfo.setDataFile(dictDataFile);
            m_database->addDict(dictInfo);
        }

        int dictId = m_database->getDictId(dictName);
        if(dictId < 0) {
            return;
        }

        if(!m_database->dictIndexExists(dictId)) {
            QFile indexFile(dictDir.path() + "/" + dictIndexFile);
            if (!indexFile.open(QFile::ReadOnly)) {
//                qDebug() << dictIndexFile << " open error!";
            }
            quint16 buffSize = 1024;
            char* buff = new char[buffSize];
            QString line;
            QList<DictEntry*> entryList;
            while (indexFile.readLine(buff, buffSize) != -1) {
                line.clear();
                line.append(buff);
                if(line.size() > 0 && line.contains(",")) {
                    QStringList strList =  line.split(",");
                    if(strList.size() == 3) {
                        quint32 articleOffset = strList.takeFirst().toUInt();
                        quint32 articleSize = strList.takeFirst().toUInt();
                        QString word = strList.takeFirst();
                        word = word.replace("\n", "");
                        entryList.append(new DictEntry(dictId, word, articleOffset, articleSize));
                    }
                }
            }
            if(entryList.size() > 0) {
                m_database->addDictIndex(entryList);

                for (QList<DictEntry*>::iterator index = entryList.begin(); index != entryList.end(); ++index) {
                    delete *index;
                }
            }

        }
    }
}

void Dict::initDictInfoList(){
    if(!this->m_dictInfos) {
        m_dictInfos = m_database->getAllDictInfo();
        if(m_dictInfos != nullptr && m_dictInfos->size() > 0) {
            this->m_dataFileMap = new QMap<qint32,QFile*>();
            this->m_dictInfoMap = new QMap<qint32,DictInfo*>();
            for (QList<DictInfo*>::iterator index = m_dictInfos->begin(); index != m_dictInfos->end(); ++index) {
                DictInfo* info =  *index;
                QFile* dataFile = new QFile(info->dictPath() +"/"+ info->dataFile());
                if(dataFile->exists()) {
                    this->m_dataFileMap->insert(info->id(), dataFile);
                }
                this->m_dictInfoMap->insert(info->id(), info);
            }
        }
    }
}

QList<DictInfo*>* Dict::getDictInfoList(){
    if(!this->m_dictInfos) {
        initDictInfoList();
    }

    return m_dictInfos;
}


void Dict::clearDictInfo(){
    if(this->m_dataFileMap) {
        if(this->m_dataFileMap->size() > 0) {
            for (QMap<qint32,QFile*>::iterator i = this->m_dataFileMap->begin(); i != this->m_dataFileMap->end(); ++i) {
                QFile* file = i.value();
                if(file->isOpen()) {
                    file->close();
                }
                delete file;
            }
        }
        delete this->m_dataFileMap;
        m_dataFileMap = nullptr;
    }

    if(this->m_dictInfoMap) {
        if(this->m_dictInfoMap->size() > 0) {
            for (QMap<qint32,DictInfo*>::iterator i = this->m_dictInfoMap->begin(); i != this->m_dictInfoMap->end(); ++i) {
                delete (i.value());
            }
        }
        delete this->m_dictInfoMap;
        m_dictInfoMap = nullptr;
    }
    if(m_dictInfos) {
        delete m_dictInfos;
        m_dictInfos = nullptr;
    }
}
