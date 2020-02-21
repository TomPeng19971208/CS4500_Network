//
// Created by Jiawen Liu on 2/12/20.
//

#ifndef CS4500_A4_BOOLCOLUMN_H
#define CS4500_A4_BOOLCOLUMN_H

#pragma once;
#include <stdlib.h>
#include <cstdarg>
#include "column.h"

/*************************************************************************
 * BoolColumn::
 * Holds int values.
 */
class BoolColumn : public Column {
public:
    BoolColumn();
    BoolColumn(int n, ...);
    bool get(size_t idx);
    BoolColumn* as_bool();
    /** Set value at idx. An out of bound idx is undefined.  */
    void set(size_t idx, bool val);
    size_t size();
    void push_back(bool val);
    char get_type();
};

BoolColumn::BoolColumn() {
    Column::col = new Array();
}

BoolColumn::BoolColumn(int n,  ...) {
    Column::col = new Array();
    bool val;
    va_list vl;
    va_start(vl, n);
    for(int i = 0; i < n; i++){
        val = va_arg(vl, bool);
        BoolWrapper* valWrap = new BoolWrapper(val);
        col->add(valWrap);
    }
    va_end(vl);
}

bool BoolColumn::get(size_t idx) {
    BoolWrapper* valWrap= dynamic_cast<BoolWrapper*> (col->get(idx));
    if(valWrap) {
        return valWrap->data;
    }
    else {
        std:: cerr << "cast failed\n";
        exit(1);
    }
}

BoolColumn* BoolColumn::as_bool() {
    return this;
}

void BoolColumn::set(size_t idx, bool val) {
    BoolWrapper* valWrap = new BoolWrapper(val);
    col->replace(idx, valWrap);
}

size_t BoolColumn::size() {
    return col->get_size();
}

void BoolColumn::push_back(bool val) {
    BoolWrapper* valWrap = new BoolWrapper(val);
    col->add(valWrap);
}

char BoolColumn::get_type() {
    return 'F';
}

#endif //CS4500_A4_BoolColumn_H
