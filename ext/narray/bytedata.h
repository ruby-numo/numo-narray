/*
  bytedata.h
  Binary Array Class for Ruby
    (C) Copyright 2007 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#ifndef BYTEDATA_H
#define BYTEDATA_H

void *rb_bd_pointer_for_read _((VALUE));
void *rb_bd_pointer_for_write _((VALUE));
VALUE rb_bd_new _((size_t));

EXTERN VALUE cByteData;

#define IsByteData(obj) (rb_obj_is_kind_of(obj,cByteData)==Qtrue)

#endif /* ifndef BYTEDATA_H */
