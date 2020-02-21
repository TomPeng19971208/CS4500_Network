//
// Created by Jiawen Liu on 2/12/20.
//

#ifndef CS4500_A4_INTCOLUMN_H
#define CS4500_A4_INTCOLUMN_H

#pragma once
#include <stdlib.h>
#include <cstdarg>
#include "column.h"
#include "array/array.h"
#include "array/wrapper.h"

/*************************************************************************
 * IntColumn::
 * Holds int values.
 */
class IntColumn : public Column {
public:
    IntColumn();
    IntColumn(int n, ...);
    int get(size_t idx);
    IntColumn* as_int();
    /** Set value at idx. An out of bound idx is undefined.  */
    void set(size_t idx, int val);
    size_t size();
    void push_back(int val);
    char get_type();
};

IntColumn::IntColumn() {
    Column::col = new Array();
}

IntColumn::IntColumn(int n, ...) {
    Column::col = new Array();
    int val;
    va_list vl;
    va_start(vl, n);
    for(int i = 0; i < n; i++){
        val = va_arg(vl, int);
        IntWrapper* valWrap = new IntWrapper(val);
        col->add(valWrap);
    }
    va_end(vl);
}

int IntColumn::get(size_t idx) {
    IntWrapper* valWrap= dynamic_cast<IntWrapper*> (col->get(idx));
    if(valWrap) {
        return valWrap->data;
    }
    else {
        std:: cerr << "cast failed\n";
        exit(1);
    }
}

IntColumn* IntColumn::as_int() {
    return this;
}

void IntColumn::set(size_t idx, int val) {
    IntWrapper* valWrap = new IntWrapper(val);
    col->replace(idx, valWrap);
}

size_t IntColumn::size() {
    return col->get_size();
}

void IntColumn::push_back(int val) {
    IntWrapper* valWrap = new IntWrapper(val);
    col->add(valWrap);
}

char IntColumn::get_type() {
    return 'I';
}

#endif //CS4500_A4_INTCOLUMN_H
