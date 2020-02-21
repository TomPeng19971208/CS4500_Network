//
// Created by Jiawen Liu on 2/12/20.
//

#ifndef CS4500_A4_FLOATCOLUMN_H
#define CS4500_A4_FLOATCOLUMN_H

#pragma once;
#include <stdlib.h>
#include <cstdarg>
#include "column.h"

/*************************************************************************
 * BoolColumn::
 * Holds int values.
 */
class FloatColumn : public Column {
public:
    FloatColumn();
    FloatColumn(int n, ...);
    float get(size_t idx);
    FloatColumn* as_float();
    /** Set value at idx. An out of bound idx is undefined.  */
    void set(size_t idx, float val);
    size_t size();
    void push_back(float val);
    char get_type();
};

FloatColumn::FloatColumn() {
    Column::col = new Array();
}

FloatColumn::FloatColumn(int n, ...) {
    Column::col = new Array();
    float val;
    va_list vl;
    va_start(vl, n);
    for(int i = 0; i < n; i++){
        val = va_arg(vl, float);
        FloatWrapper* valWrap = new FloatWrapper(val);
        col->add(valWrap);
    }
    va_end(vl);
}

float FloatColumn::get(size_t idx) {
    FloatWrapper* valWrap= dynamic_cast<FloatWrapper*> (col->get(idx));
    if(valWrap != nullptr) {
        return valWrap->data;
    }
    else {
        std:: cerr << "cast failed\n";
        exit(1);
    }
}

FloatColumn* FloatColumn::as_float() {
    return this;
}

void FloatColumn::set(size_t idx, float val) {
    FloatWrapper* valWrap = new FloatWrapper(val);
    col->replace(idx, valWrap);
}

size_t FloatColumn::size() {
    return col->get_size();
}

void FloatColumn::push_back(float val) {
    FloatWrapper* valWrap = new FloatWrapper(val);
    col->add(valWrap);
}

char FloatColumn::get_type() {
    return 'F';
}

#endif //CS4500_A4_FLOATCOLUMN_H
