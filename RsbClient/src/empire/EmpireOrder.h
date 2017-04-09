/*
 * EmpireOrder.h
 *
 *  Created on: Feb 12, 2017
 *      Author: henrik
 */

#ifndef SRC_EmpireOrder_H_
#define SRC_EmpireOrder_H_

#include <RsbString.h>

class EmpireOrder: public RsbString {
public:
	EmpireOrder();
	virtual ~EmpireOrder();

	virtual const char* getTypeName() const {return "EmpireOrder";};

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	EmpireOrder(const EmpireOrder&);
	EmpireOrder& operator=(const EmpireOrder&);

};

#endif /* SRC_EMPIRE_EMPIREMONEY_H_ */
