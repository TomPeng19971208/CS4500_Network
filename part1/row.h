//
// Created by Jiawen Liu on 2/12/20.
//

#ifndef CS4500_A4_ROW_H
#define CS4500_A4_ROW_H

#pragma once;
#include "object.h"
#include "schema.h"
#include "fielder.h"
#include "array/wrapper.h"

// LANGUAGE: CwC
/*************************************************************************
 * Row::
 *
 * This class represents a single row of data constructed according to a
 * dataframe's schema. The purpose of this class is to make it easier to add
 * read/write complete rows. Internally a dataframe hold data in columns.
 * Rows have pointer equality.
 */
class Row : public Object {
    int length;
    size_t idx;
    Array* data;
    char* types;
public:

    /** Build a row following a schema. */
    Row(Schema& scm) {
        length = scm.width();
        data = new Array();
        types = new char[length];
        idx = scm.length();
        for(int i=0; i<length; i++) {
            types[i] = scm.col_type(i);
        }
    }

    ~Row() {
        delete data;
        delete [] types;
    }

    /** Setters: set the given column with the given value. Setting a column with
      * a value of the wrong type is undefined. */
    void set(size_t col, int val) {
        assert(types[col] == 'I');
        data->add(new IntWrapper(val));
    }

    void set(size_t col, float val) {
        assert(types[col] == 'F');
        data->add(new FloatWrapper(val));
    }

    void set(size_t col, bool val) {
        assert(types[col] == 'B');
        data->add(new BoolWrapper(val));
    }

    /** The string is external. */
    void set(size_t col, String* val) {
        assert(types[col] == 'S');
        data->add(val);
    }

    /** Set/get the index of this row (ie. its position in the dataframe. This is
     *  only used for informational purposes, unused otherwise */
    void set_idx(size_t idx) {
        this->idx = idx;
    }

    size_t get_idx() {
        return idx;
    }

    /** Getters: get the value at the given column. If the column is not
      * of the requested type, the result is undefined. */
    int get_int(size_t col) {
        assert(types[col] == 'I' && data->get(col) != nullptr && col<length);
        return reinterpret_cast<IntWrapper*>(data->get(col)) -> data;
    }

    bool get_bool(size_t col) {
        assert(types[col] == 'B' && data->get(col) != nullptr && col<length);
        return reinterpret_cast<BoolWrapper*>(data->get(col)) -> data;
    }

    float get_float(size_t col) {
        assert(types[col] == 'F' && data->get(col) != nullptr && col<length);
        return reinterpret_cast<FloatWrapper*>(data->get(col)) -> data;
    }

    String* get_string(size_t col) {
        assert(types[col] == 'S' && data->get(col) != nullptr && col<length);
        return reinterpret_cast<String*>(data->get(col));
    }

    /** Number of fields in the row. */
    size_t width() {
        return length;
    }

    /** Type of the field at the given position. An idx >= width is  undefined. */
    char col_type(size_t idx) {
        assert(idx < length);
        return types[idx];
    }

    /** Given a Fielder, visit every field of this row. The first argument is
      * index of the row in the dataframe.
      * Calling this method before the row's fields have been set is undefined. */
    void visit(size_t idx, Fielder& f) {
        for(int i=0; i<length; i++) {
            switch(types[i]) {
                case 'I':
                    f.accept(get_int(i));
                    break;
                case 'F':
                    f.accept(get_float(i));
                    break;
                case 'B':
                    f.accept(get_bool(i));
                    break;
                case 'S':
                    f.accept(get_string(i));
                    break;
                default:
                    continue;
            }
        }
    }
};


/*******************************************************************************
 *  Rower::
 *  An interface for iterating through each row of a data frame. The intent
 *  is that this class should subclassed and the accept() method be given
 *  a meaningful implementation. Rowers can be cloned for parallel execution.
 */
class Rower : public Object {
public:
    Rower(){}
    Rower(Rower* other){}
public:
    /** This method is called once per row. The row object is on loan agit add:q:qnd
        should not be retained as it is likely going to be reused in the next
        call. The return value is used in filters to indicate that a row
        should be kept. */
    virtual bool accept(Row& r){}

    /** Once traversal of the data frame is complete the rowers that were
        split off will be joined.  There will be one join per split. The
        original object will be the last to be called join on. The join method
        is reponsible for cleaning up memory. */
    virtual void join_delete(Rower* other){}

    virtual Rower* clone(){}
};

class SumRower : public Rower {
    int sum = 0;
public:
    SumRower(){}
    SumRower(SumRower* other){
        sum = other->sum;
    }
    ~SumRower(){}
public:
    /** This method is called once per row. The row object is on loan agit add:q:qnd
        should not be retained as it is likely going to be reused in the next
        call. The return value is used in filters to indicate that a row
        should be kept. */
    bool accept(Row& r) {
        sum += r.get_int(0);
        return true;
    }

    /** Once traversal of the data frame is complete the rowers that were
        split off will be joined.  There will be one join per split. The
        original object will be the last to be called join on. The join method
        is reponsible for cleaning up memory. */
    void join_delete(Rower* other) {
        reinterpret_cast<SumRower*>(other)->sum += sum;
    }

    SumRower* clone() {
        return new SumRower(this);
    }
};


class FibRower : public Rower {
    int result = 0;
public:
    FibRower(){}
    FibRower(FibRower* other){
        result = other->result;
    }
    ~FibRower(){}
public:
    /** This method is called once per row. The row object is on loan agit add:q:qnd
        should not be retained as it is likely going to be reused in the next
        call. The return value is used in filters to indicate that a row
        should be kept. */
    bool accept(Row& r) {
        result += fib(r.get_int(0));
        return true;
    }

    int fib(int input) {
        if (input < 2) {return input;}
        return input + fib(input -1) + fib(input -2);
    }

    /** Once traversal of the data frame is complete the rowers that were
        split off will be joined.  There will be one join per split. The
        original object will be the last to be called join on. The join method
        is reponsible for cleaning up memory. */
    void join_delete(Rower* other) {
        reinterpret_cast<FibRower*>(other)->result += result;
    }

    FibRower* clone() {
        return new FibRower(this);
    }
};
#endif //CS4500_A4_ROW_H