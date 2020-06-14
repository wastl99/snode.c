#ifndef MANAGEDWRITER_H
#define MANAGEDWRITER_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#include "Manager.h"
#include "Writer.h"


class ManagedWriter : public Manager<Writer> {
public:
    int dispatch(const fd_set& fdSet, int count) override;
};

#endif // MANAGEDWRITER_H
