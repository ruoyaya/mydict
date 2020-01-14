#ifndef DICTINFO_HPP
#define DICTINFO_HPP

#include <stdlib.h>
#include <QString>

class DictInfo
{
public:
    DictInfo(const QString& infoFileName);
    DictInfo(qint32 id, qint32 wordcount, qint32 synwordcount,
             qint32 idxfilesize, qint32 idxoffsetbits, QString& version,
             QString& bookname, QString& author, QString& email,
             QString& website, QString& date, QString& copyright,
             QString& description, QString& sametypesequence, QString& dicttype,
             QString& dictName, QString& dictPath, QString& infoFile,
             QString& indexFile, QString& dataFile, QString& memo);

    inline void setId(const qint32 id) {this->m_id = id; }
    inline qint32 id() {return this->m_id; }
    inline void setWordcount(const qint32 wordcount) {this->m_wordcount = wordcount; }
    inline qint32 wordcount() {return this->m_wordcount; }
    inline void setSynwordcount(const qint32 synwordcount) {this->m_synwordcount = synwordcount; }
    inline qint32 synwordcount() {return this->m_synwordcount; }
    inline void setIdxfilesize(const qint32 idxfilesize) {this->m_idxfilesize = idxfilesize; }
    inline qint32 idxfilesize() {return this->m_idxfilesize; }
    inline void setIdxoffsetbits(const qint32 idxoffsetbits) {this->m_idxoffsetbits = idxoffsetbits; }
    inline qint32 idxoffsetbits() {return this->m_idxoffsetbits; }
    inline void setVersion(const QString& version) {this->m_version = version; }
    inline QString version() {return this->m_version; }
    inline void setBookname(const QString& bookname) {this->m_bookname = bookname; }
    inline QString bookname() {return this->m_bookname; }
    inline void setAuthor(const QString& author) {this->m_author = author; }
    inline QString author() {return this->m_author; }
    inline void setEmail(const QString& email) {this->m_email = email; }
    inline QString email() {return this->m_email; }
    inline void setWebsite(const QString& website) {this->m_website = website; }
    inline QString website() {return this->m_website; }
    inline void setDate(const QString& date) {this->m_date = date; }
    inline QString date() {return this->m_date; }
    inline void setCopyright(const QString& copyright) {this->m_copyright = copyright; }
    inline QString copyright() {return this->m_copyright; }
    inline void setDescription(const QString& description) {this->m_description = description; }
    inline QString description() {return this->m_description; }
    inline void setSametypesequence(const QString& sametypesequence) {this->m_sametypesequence = sametypesequence; }
    inline QString sametypesequence() {return this->m_sametypesequence; }
    inline void setDicttype(const QString& dicttype) {this->m_dicttype = dicttype; }
    inline QString dicttype() {return this->m_dicttype; }
    inline void setDictName(const QString& dictName) {this->m_dictName = dictName; }
    inline QString dictName() {return this->m_dictName; }
    inline void setDictPath(const QString& dictPath) {this->m_dictPath = dictPath; }
    inline QString dictPath() {return this->m_dictPath; }
    inline void setInfoFile(const QString& infoFile) {this->m_infoFile = infoFile; }
    inline QString infoFile() {return this->m_infoFile; }
    inline void setIndexFile(const QString& indexFile) {this->m_indexFile = indexFile; }
    inline QString indexFile() {return this->m_indexFile; }
    inline void setDataFile(const QString& dataFile) {this->m_dataFile = dataFile; }
    inline QString dataFile() {return this->m_dataFile; }
    inline void setMemo(const QString& memo) {this->m_memo = memo; }
    inline QString memo() {return this->m_memo; }

private:
    qint32 m_id;
    qint32 m_wordcount;
    qint32 m_synwordcount;
    qint32 m_idxfilesize;
    qint32 m_idxoffsetbits;
    QString m_version;
    QString m_bookname;
    QString m_author;
    QString m_email;
    QString m_website;
    QString m_date;
    QString m_copyright;
    QString m_description;
    QString m_sametypesequence;
    QString m_dicttype;

    QString m_dictName; /* langdao-ce-gb */
    QString m_dictPath; /* /xxx/data/dictionaries/langdao-ce-gb.dictionary */
    QString m_infoFile; /* langdao-ce-gb.info */
    QString m_indexFile; /* langdao-ce-gb.index */
    QString m_dataFile; /* langdao-ce-gb.dict */
    QString m_memo;

};

#endif // DICTINFO_HPP
