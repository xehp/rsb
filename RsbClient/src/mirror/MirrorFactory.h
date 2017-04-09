/*
MirrorFactory.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef MirrorFactory_H
#define MirrorFactory_H

// TODO there is a better way: http://www.cplusplus.com/faq/sequences/arrays/sizeof-array/#cpp
#define SIZEOF_ARRAY( a ) (sizeof( a ) / sizeof( a[ 0 ] ))

#include "MirrorBase.h"

// Create a MirrorBase object from the type name.
MirrorBase *mirrorFactoryCreate(const char* typeName);



#endif




