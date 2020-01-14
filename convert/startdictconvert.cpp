#include "startdictconvert.hpp"
#include <QString>
#include <QDir>
#include <QDebug>
#include <iostream>
#ifdef Q_OS_WIN32
#include <QtZlib/zlib.h>
#include <winsock.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <zlib.h>
#endif
#include <QCoreApplication>

StartDictConvert::StartDictConvert()
{

}

bool StartDictConvert::validate(QString filename) {

    if(filename == nullptr || !filename.endsWith(".ifo")) {
        return false;
    }

    QFileInfo ifoFileInfo(filename);
    if (!ifoFileInfo.exists()) {
        return false;
    }


    QFile infoFile(filename);
    if(!infoFile.open(QFile::ReadWrite)) {
        return false;
    }

    QByteArray firstLine = infoFile.readLine();
    if(firstLine.size() == 0 || !firstLine.startsWith("StarDict")) {
        return false;
    }


    QString path = ifoFileInfo.absolutePath();
    QString fileName = ifoFileInfo.fileName().replace(".ifo","");

    m_dictId = fileName;
    m_infoFile = path + "/" + fileName + ".ifo";
    m_idxFile = path + "/" + fileName + ".idx";
    m_dzFile = path + "/" + fileName + ".dict";

    QDir currentDir = QCoreApplication::applicationDirPath();
#ifdef Q_OS_OSX
    currentDir.cdUp();
#endif

    QString dictName(fileName);
    QString dictDataHome(currentDir.path() + "/data/dictionaries");
    QString dictDataPath(dictDataHome + "/" + dictName + ".dictionary");

    QDir dictDataDir(dictDataPath);
    if(!dictDataDir.exists()) {
        dictDataDir.mkpath(".");
    }

    m_targetInfoFile = dictDataPath + "/" + dictName + ".info";
    m_targetIndexFile = dictDataPath + "/" + dictName + ".index";
    m_targetDictFile = dictDataPath + "/" + dictName + ".dict";

    return true;
}


QString StartDictConvert::doConvert() {


    QString errMsg;
    qDebug() << m_targetInfoFile;
    if(m_infoFile.size() > 0 && !convertInfo(errMsg)) {
        qDebug() << errMsg;
        return QString();
    }

    qDebug() << m_targetIndexFile;
    if(m_idxFile.size() > 0 && !convertIndex(errMsg)) {
        qDebug() << errMsg;
        return QString();
    }

    qDebug() << m_targetDictFile;
    if(m_dzFile.size() > 0 && !convertDict(errMsg)) {
        qDebug() << errMsg;
        return QString();
    }

    return m_dictId;
}

bool StartDictConvert::convertInfo(QString& errMsg) {

    if(m_infoFile.size() == 0) {
        errMsg = "startdict info file is empty!";
        return false;
    }

    if(m_targetInfoFile.size() == 0) {
        errMsg = "target info file is empty!";
        return false;
    }

    QFile infoFile(m_infoFile);
    QFile targetInfoFile(m_targetInfoFile);

    if(targetInfoFile.exists()) {
        return true;
    }

    if (infoFile.open(QFile::ReadOnly) && targetInfoFile.open(QFile::WriteOnly)) {

        char buf[1024];
        while(true) {
            qint64 lineLength = infoFile.readLine(buf, sizeof(buf));
            if (lineLength != -1) {
                if(lineLength > 0) {
                    QString line(buf);
                    if(line.startsWith("StarDict")) {
                        targetInfoFile.write("MyDict Info File\n");
                    } else if(line.startsWith("version=")) {
                        targetInfoFile.write("version=1.0.0\n");
                    } else if(line.startsWith("author=")) {
                        targetInfoFile.write("author=WuJing\n");
                    } else if(line.startsWith("email=")) {
                        targetInfoFile.write("email=wujing0510@gmail.com\n");
                    } else {
                        targetInfoFile.write(buf);
                    }
                }

            } else {
                break;
            }
        }

        infoFile.close();
        targetInfoFile.close();
    }

    return true;
}

bool StartDictConvert::convertIndex(QString& errMsg) {
    if(m_idxFile.size() == 0) {
        errMsg = "startdict index file is empty!";
        return false;
    }

    if(m_targetIndexFile.size() == 0) {
        errMsg = "target index file is empty!";
        return false;
    }

    QFile targetIndexFile(m_targetIndexFile);
    if(targetIndexFile.exists()) {
        return true;
    }

    FILE* targetFile = fopen(m_targetIndexFile.toStdString().c_str(), "w");
    if(!targetFile) {
        return false;
    }


    gzFile stardictIdx = gzopen( m_idxFile.toStdString().c_str(), "rb");
    if ( !stardictIdx ) {
        //        std::cerr << "exCantReadFile[" << fileName << "]" << std::endl;
        return false;
    }

    std::vector< char > image;

    for( ; ; )
    {
        size_t oldSize = image.size();

        image.resize( oldSize + 65536 );

        int rd = gzread( stardictIdx, &image.front() + oldSize, 65536 );

        if ( rd < 0 )
        {
            gzclose( stardictIdx );
            //            std::cerr << "exCantReadFile[" << fileName << "]" << std::endl;
            return false;
        }

        if ( rd != 65536 )
        {
            image.resize( oldSize + rd + 1 );
            break;
        }
    }
    gzclose( stardictIdx );

    // We append one zero byte to catch runaway string at the end, if any

    image.back() = 0;

    // Now parse it
    bool isSynFile = false;

    for( char const * ptr = &image.front(); ptr != &image.back(); )
    {
        size_t wordLen = strlen( ptr );

        if ( ptr + wordLen + 1 + ( isSynFile ? sizeof( uint32_t ) :
                                   sizeof( uint32_t ) * 2 ) >
             &image.back() )
        {
            //            std::cerr << "Warning: sudden end of file [" << fileName << "]" << std::endl;
            break;
        }

        char const * word = ptr;

        ptr += wordLen + 1;

        if( strstr( word, "&#" ) )
        {
            // Decode some html-coded symbols in headword
            std::string unescapedWord = word;
            strncpy( const_cast<char *>(word), unescapedWord.c_str(), wordLen );
            wordLen = strlen( word );
        }

        uint32_t articleOffset = 0;
        uint32_t articleSize = 0;

        if ( !isSynFile )
        {
            // We're processing the .idx file

            memcpy( &articleOffset, ptr, sizeof( uint32_t ) );
            ptr += sizeof( uint32_t );
            memcpy( &articleSize, ptr, sizeof( uint32_t ) );
            ptr += sizeof( uint32_t );

            articleOffset = ntohl( articleOffset );
            articleSize = ntohl( articleSize );

            // Create an entry for the article in the chunked storage

            //        offset = chunks.startNewBlock();

            //        if ( articleOffsets )
            //          articleOffsets->push_back( offset );

            //        chunks.addToBlock( &articleOffset, sizeof( uint32_t ) );
            //        chunks.addToBlock( &articleSize, sizeof( uint32_t ) );
            //        chunks.addToBlock( word, wordLen + 1 );
        }
        else
        {

            // Some StarDict dictionaries are in fact badly converted Babylon ones.
            // They contain a lot of superfluous slashed entries with dollar signs.
            // We try to filter them out here, since those entries become much more
            // apparent in GoldenDict than they were in StarDict because of
            // punctuation folding. Hopefully there are not a whole lot of valid
            // synonyms which really start from slash and contain dollar signs, or
            // end with dollar and contain slashes.
            if ( *word == '/' )
            {
                if ( strchr( word, '$' ) )
                    continue; // Skip this entry
            }
            else
                if ( wordLen && word[ wordLen - 1 ] == '$' )
                {
                    if ( strchr( word, '/' ) )
                        continue; // Skip this entry
                }
        }

        // Insert new entry into an index
        qDebug() << "word=" << word << ",articleOffset=" << articleOffset << ",articleSize=" << articleSize;
        fprintf(targetFile, "%d,%d,%s\n", articleOffset, articleSize, word);
        //        DictEntry* entry = new DictEntry(word, articleOffset, articleSize);
        //        m_entryList.push_back(entry);
    }

    fclose(targetFile);

    return true;
}

bool StartDictConvert::convertDict(QString& errMsg) {


    if(m_dzFile.size() == 0) {
        errMsg = "startdict index file is empty!";
        return false;
    }

    QFile dzFile(m_dzFile);
    if(!dzFile.exists()) {
        errMsg = "DZ file is not exist!";
        return true;
    }

    if(m_targetDictFile.size() == 0) {
        errMsg = "target index file is empty!";
        return false;
    }

    QFile targetIndexFile(m_targetDictFile);
    if(targetIndexFile.exists()) {
        return true;
    }


    QFile::copy(m_dzFile, m_targetDictFile);

    return true;
}

