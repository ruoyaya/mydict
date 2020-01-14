#include "dictinfo.hpp"
#include <QFile>
#include <QDebug>
#include <QString>

DictInfo::DictInfo(const QString& infoFileName)
{
    this->m_infoFile = infoFileName;

    QFile infoFile(m_infoFile);
    if(!infoFile.open(QFile::ReadOnly)) {
        qDebug() << "file[" << m_infoFile << "] cannot open!";
        return;
    }

    QString line;
    while(!infoFile.atEnd()) {
        line.clear();
        line.push_back(infoFile.readLine());

        if(line.size() == 0) {
            continue;
        } else if(line.startsWith("version=")) {
            line = line.replace("version=", "");
            this->m_version = line;
        }  else if(line.startsWith("bookname=")) {
            line = line.replace("bookname=", "");
            this->m_bookname= line;
        }  else if(line.startsWith("wordcount=")) {
            line = line.replace("wordcount=", "");
            this->m_wordcount = line.toInt();
        }  else if(line.startsWith("synwordcount=")) {
            line = line.replace("synwordcount=", "");
            this->m_synwordcount = line.toInt();
        }  else if(line.startsWith("idxfilesize=")) {
            line = line.replace("idxfilesize=", "");
            this->m_idxfilesize = line.toInt();
        }  else if(line.startsWith("idxoffsetbits=")) {
            line = line.replace("idxoffsetbits=", "");
            this->m_idxoffsetbits = line.toInt();
        }  else if(line.startsWith("sametypesequence=")) {
            line = line.replace("sametypesequence=", "");
            this->m_sametypesequence= line;
        }  else if(line.startsWith("dicttype=")) {
            line = line.replace("dicttype=", "");
            this->m_dicttype= line;
        }  else if(line.startsWith("description=")) {
            line = line.replace("description=", "");
            this->m_description= line;
        }  else if(line.startsWith("copyright=")) {
            line = line.replace("copyright=", "");
            this->m_copyright= line;
        }  else if(line.startsWith("author=")) {
            line = line.replace("author=", "");
            this->m_author= line;
        }  else if(line.startsWith("email=")) {
            line = line.replace("email=", "");
            this->m_email= line;
        }  else if(line.startsWith("website=")) {
            line = line.replace("website=", "");
            this->m_website= line;
        } else if(line.startsWith("date=")) {
            line = line.replace("date=", "");
            this->m_date= line;
        }
    }

    infoFile.close();
}

DictInfo::DictInfo(qint32 id, qint32 wordcount, qint32 synwordcount,
                   qint32 idxfilesize, qint32 idxoffsetbits, QString& version,
                   QString& bookname, QString& author, QString& email,
                   QString& website, QString& date, QString& copyright,
                   QString& description, QString& sametypesequence, QString& dicttype,
                   QString& dictName, QString& dictPath, QString& infoFile,
                   QString& indexFile, QString& dataFile, QString& memo) {
    this->m_id = id;
    this->m_wordcount = wordcount;
    this->m_synwordcount = synwordcount;
    this->m_idxfilesize = idxfilesize;
    this->m_idxoffsetbits = idxoffsetbits;
    this->m_version = version;
    this->m_bookname = bookname;
    this->m_author = author;
    this->m_email = email;
    this->m_website = website;
    this->m_date = date;
    this->m_copyright = copyright;
    this->m_description = description;
    this->m_sametypesequence = sametypesequence;
    this->m_dicttype = dicttype;
    this->m_dictName = dictName;
    this->m_dictPath = dictPath;
    this->m_infoFile = infoFile;
    this->m_indexFile = indexFile;
    this->m_dataFile = dataFile;
    this->m_memo = memo;
}
