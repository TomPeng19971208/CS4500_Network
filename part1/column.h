//
// Created by Jiawen Liu on 2/12/20.
//

#ifndef CS4500_A4_COLUMN_H
#define CS4500_A4_COLUMN_H

#pragma once
#include "object.h"
#include "string.h"
#include "array/array.h"
#include "array/wrapper.h"




/**************************************************************************
 * Column ::
 * Represents one column of a data frame which holds values of a single type.
 * This abstract class defines methods overriden in subclasses. There is
 * one subclass per element type. Columns are mutable, equality is pointer
 * equality. */

class IntColumn;
class BoolColumn;
class FloatColumn;
class StringColumn;

class Column : public Object {
public:
    Array* col;
public:

    /** Type converters: Return same column under its actual type, or
     *  nullptr if of the wrong type.
     **/
    virtual IntColumn* as_int();
    virtual BoolColumn*  as_bool();
    virtual FloatColumn* as_float();
    virtual StringColumn* as_string();

    /** Type appropriate push_back methods. Calling the wrong method is
      * undefined behavior. **/
    virtual void push_back(int val);
    virtual void push_back(bool val);
    virtual void push_back(float val);
    virtual void push_back(String* val);

    /** Returns the number of elements in the column.
     *
    **/
    virtual size_t size();

    /** Return the type of this column as a char: 'S', 'B', 'I' and 'F'.
     *
    **/
    virtual char get_type();
  };

IntColumn* Column::as_int() {return nullptr;}
BoolColumn* Column::as_bool() {return nullptr;}
FloatColumn* Column::as_float() {return nullptr;}
StringColumn* Column::as_string() {return nullptr;}

void Column::push_back(int val) {return;}
void Column::push_back(bool val) {return;}
void Column::push_back(float val) {return;}
void Column::push_back(String* val) {return;}

size_t Column::size() {return 0;}

char Column::get_type() {return ' ';}


#endif //CS4500_A4_COLUMN_H
