#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H

#include "Singleton.h"

class ErrorManager : public Singleton<ErrorManager>
{
public:
    void reportError();
};

#endif // ERRORMANAGER_H
