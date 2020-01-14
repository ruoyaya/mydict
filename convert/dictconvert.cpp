#include "dictconvert.hpp"
#include "startdictconvert.hpp"


QString DictConvert::convert(QString filename) {
    if(filename == nullptr || filename.size() == 0) {
        return QString();
    }

    StartDictConvert convert;
    if(convert.validate(filename)) {
        QString dictId = convert.doConvert();
        if(dictId != nullptr && dictId.size() > 0) {
            return dictId;
        }
    }



    return QString();
}
