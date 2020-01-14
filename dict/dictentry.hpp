#ifndef DICTENTRY_HPP
#define DICTENTRY_HPP

#include <QObject>
#include <QString>
#include "dictinfo.hpp"

class DictEntry
{
public:
    DictEntry();
    DictEntry(qint32 dictId, QString& key, quint32 offset, quint32 size);
    ~DictEntry();

    qint32 dictId;
    QString key;
    quint32 offset;
    quint32 size;
};

#endif // DICTENTRY_HPP
