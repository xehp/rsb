/*
YukigassenBlockRoom.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef YUKIGASSENTOPROOM_H_
#define YUKIGASSENTOPROOM_H_


class YukigassenTopRoom: public YukigassenBlockRoom
{
public:

	YukigassenTopRoom();
	virtual ~YukigassenTopRoom();

	virtual const char* getTypeName() const {return "YukigassenTopRoom";};

};

#endif /* YUKIGASSENTOPROOM_H_ */
