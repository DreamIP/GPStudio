#ifndef MODEL_IO_H
#define MODEL_IO_H

#include "gpstudio_lib_common.h"

#include "model_block.h"

class GPSTUDIO_LIB_EXPORT ModelIO : public ModelBlock
{
public:
    ModelIO();
    virtual ~ModelIO();

    QString type() const;

public:
    static ModelIO *fromNodeGenerated(const QDomElement &domElement, ModelIO *io=NULL);
};

#endif // MODEL_IO_H
