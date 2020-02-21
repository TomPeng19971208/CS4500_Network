//
// Created by Jiawen Liu on 2/12/20.
//

#ifndef CS4500_A4_FIELDER_H
#define CS4500_A4_FIELDER_H

#pragma once;
#include "object.h"
#include "string.h"

/*****************************************************************************
 * Fielder::
 * A field vistor invoked by Row.
 */
class Fielder : public Object {
public:

    /** Called before visiting a row, the argument is the row offset in the
      dataframe. */
    virtual void start(size_t r);

    /** Called for fields of the argument's type with the value of the field. */
    virtual void accept(bool b);
    virtual void accept(float f);
    virtual void accept(int i);
    virtual void accept(String* s);

    /** Called when all fields have been seen. */
    virtual void done();
};

#endif //CS4500_A4_FIELDER_H
