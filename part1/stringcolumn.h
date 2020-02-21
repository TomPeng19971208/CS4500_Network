//
// Created by Jiawen Liu on 2/12/20.
//

#ifndef CS4500_A4_STRINGCOLUMN_H
#define CS4500_A4_STRINGCOLUMN_H

#pragma once
#include <stdlib.h>
#include <cstdarg>
#include "column.h"

/*************************************************************************
 * StringColumn::
 * Holds string pointers. The strings are external.  Nullptr is a valid
 * value.
 */
class StringColumn : public Column {
 public:
  StringColumn();
  StringColumn(int n, ...);
  StringColumn* as_string();
  /** Returns the string at idx; undefined on invalid idx.*/
  String* get(size_t idx);
  /** Out of bound idx is undefined. */
  void set(size_t idx, String* val);
  size_t size();
  void push_back(String* val);
  char get_type();
};

StringColumn::StringColumn() {
  Column::col = new Array();
}

StringColumn::StringColumn(int n, ...) {
  Column::col = new Array();
  char* val;
  va_list vl;
  va_start(vl, n);
  for(int i = 0; i < n; i++){
    val = va_arg(vl, char*);
    String* valWrap = new String(val);
    col->add(valWrap);
  }
  va_end(vl);
}

String* StringColumn::get(size_t idx) {
  String* valWrap= dynamic_cast<String*> (col->get(idx));
  if(valWrap) {
    return valWrap;
  }
  else {
    std:: cerr << "cast failed\n";
    exit(1);
  }
}

StringColumn* StringColumn::as_string() {
  return this;
}

void StringColumn::set(size_t idx, String* val) {
  col->replace(idx, val);
}

size_t StringColumn::size() {
  return col->get_size();
}

void StringColumn::push_back(String* val) {
  col->add(val);
}

char StringColumn::get_type() {
  return 'S';
}

#endif //CS4500_A4_STRINGCOLUMN_H
