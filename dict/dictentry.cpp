#include "dictentry.hpp"

DictEntry::DictEntry()
{
    this->dictId = 0;
    this->key = nullptr;
    this->offset = 0;
    this->size = 0;
}

DictEntry::DictEntry(qint32 dictId, QString& pkey, quint32 poffset, quint32 psize)
{
    this->dictId = dictId;
    this->key = pkey;
    this->offset = poffset;
    this->size = psize;
}


DictEntry::~DictEntry() {
}
