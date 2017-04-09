/*
 * EmpireMoney.h
 *
 *  Created on: Feb 12, 2017
 *      Author: henrik
 */

#ifndef SRC_EmpireMoney_H_
#define SRC_EmpireMoney_H_

#include <RsbLong.h>

class EmpireMoney: public RsbLong {
public:
	EmpireMoney();
	virtual ~EmpireMoney();

	virtual const char* getTypeName() const {return "EmpireMoney";};

};

#endif /* SRC_EMPIRE_EMPIREMONEY_H_ */
