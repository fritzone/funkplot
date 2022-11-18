#ifndef INDEXEDACCESS_H
#define INDEXEDACCESS_H

#include <QString>

/**
 * @brief The IndexedAccess struct will be created upon accessing and indexed expression. Will be
 * evaluated at the RuntimeProvider level
 */
struct IndexedAccess
{
    QString indexedVariable;
    int index;
};

#endif // INDEXEDACCESS_H
