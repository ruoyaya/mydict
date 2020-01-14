#ifndef STARTDICTCONVERT_HPP
#define STARTDICTCONVERT_HPP

#include "dictconvert.hpp"
#include <QString>


class StartDictConvert
{
public:
    StartDictConvert();
    bool validate(QString filename);
    QString doConvert();
    bool convertInfo(QString& errMsg);
    bool convertIndex(QString& errMsg);
    bool convertDict(QString& errMsg);
private:
    QString m_infoFile;
    QString m_idxFile;
    QString m_dzFile;
    QString m_targetInfoFile;
    QString m_targetIndexFile;
    QString m_targetDictFile;
    QString m_dictId;
};

#endif // STARTDICTCONVERT_HPP
