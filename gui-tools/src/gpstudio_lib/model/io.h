#ifndef IO_H
#define IO_H

#include "gpstudio_lib_common.h"

#include <QString>

#include "block.h"

class GPSTUDIO_LIB_EXPORT IO : public Block
{
public:
    IO();
    virtual ~IO();

    QString type() const;
};

#endif // IO_H
