/*
RsbInvItem.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/


#include "WordReader.h"

#include "RsbInvItem.h"


RsbInvItem::RsbInvItem() :
	RsbString(),
	numberOf(0)
{

}

RsbInvItem::~RsbInvItem()
{}


void RsbInvItem::readSelf(WordReader *wr)
{
	RsbString::readSelf(wr);
	numberOf = wr->readLong();
}



