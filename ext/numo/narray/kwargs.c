/**********************************************************************

  Function to extract Keyword argument for ruby-2.1.x
  Copied from class.c in ruby-2.4.2

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/
#include <ruby.h>
#define rb_hash_tbl_raw(hash) rb_hash_tbl(hash)

/* from internal.h */
struct RBasicRaw {
    VALUE flags;
    VALUE klass;
};

#define RBASIC_SET_CLASS(obj, cls)     do { \
    VALUE _obj_ = (obj); \
    RB_OBJ_WRITE(_obj_, &((struct RBasicRaw *)(_obj_))->klass, cls); \
} while (0)

/* from class.c */
VALUE
rb_keyword_error_new(const char *error, VALUE keys)
{
    const char *msg = "";
    VALUE error_message;

    if (RARRAY_LEN(keys) == 1) {
	keys = RARRAY_AREF(keys, 0);
    }
    else {
	keys = rb_ary_join(keys, rb_usascii_str_new2(", "));
	msg = "s";
    }

    error_message = rb_sprintf("%s keyword%s: %"PRIsVALUE, error, msg, keys);

    return rb_exc_new_str(rb_eArgError, error_message);
}

NORETURN(static void rb_keyword_error(const char *error, VALUE keys));
static void
rb_keyword_error(const char *error, VALUE keys)
{
    rb_exc_raise(rb_keyword_error_new(error, keys));
}

NORETURN(static void unknown_keyword_error(VALUE hash, const ID *table, int keywords));
static void
unknown_keyword_error(VALUE hash, const ID *table, int keywords)
{
    st_table *tbl = rb_hash_tbl_raw(hash);
    VALUE keys;
    int i;
    for (i = 0; i < keywords; i++) {
	st_data_t key = ID2SYM(table[i]);
	st_delete(tbl, &key, NULL);
    }
    keys = rb_funcallv(hash, rb_intern("keys"), 0, 0);
    if (!RB_TYPE_P(keys, T_ARRAY)) rb_raise(rb_eArgError, "unknown keyword");
    rb_keyword_error("unknown", keys);
}

static int
separate_symbol(st_data_t key, st_data_t value, st_data_t arg)
{
    VALUE *kwdhash = (VALUE *)arg;

    if (!SYMBOL_P(key)) kwdhash++;
    if (!*kwdhash) *kwdhash = rb_hash_new();
    rb_hash_aset(*kwdhash, (VALUE)key, (VALUE)value);
    return ST_CONTINUE;
}

VALUE
rb_extract_keywords(VALUE *orighash)
{
    VALUE parthash[2] = {0, 0};
    VALUE hash = *orighash;

    if (RHASH_EMPTY_P(hash)) {
	*orighash = 0;
	return hash;
    }
    st_foreach(rb_hash_tbl_raw(hash), separate_symbol, (st_data_t)&parthash);
    *orighash = parthash[1];
    if (parthash[1] && RBASIC_CLASS(hash) != rb_cHash) {
	RBASIC_SET_CLASS(parthash[1], RBASIC_CLASS(hash));
    }
    return parthash[0];
}

int
rb_get_kwargs(VALUE keyword_hash, const ID *table, int required, int optional, VALUE *values)
{
    int i = 0, j;
    int rest = 0;
    VALUE missing = Qnil;
    st_data_t key;

#define extract_kwarg(keyword, val) \
    (key = (st_data_t)(keyword), values ? \
     st_delete(rb_hash_tbl_raw(keyword_hash), &key, (val)) : \
     st_lookup(rb_hash_tbl_raw(keyword_hash), key, (val)))

    if (NIL_P(keyword_hash)) keyword_hash = 0;

    if (optional < 0) {
	rest = 1;
	optional = -1-optional;
    }
    if (values) {
	for (j = 0; j < required + optional; j++) {
	    values[j] = Qundef;
	}
    }
    if (required) {
	for (; i < required; i++) {
	    VALUE keyword = ID2SYM(table[i]);
	    if (keyword_hash) {
		st_data_t val;
		if (extract_kwarg(keyword, &val)) {
		    if (values) values[i] = (VALUE)val;
		    continue;
		}
	    }
	    if (NIL_P(missing)) missing = rb_ary_tmp_new(1);
	    rb_ary_push(missing, keyword);
	}
	if (!NIL_P(missing)) {
	    rb_keyword_error("missing", missing);
	}
    }
    j = i;
    if (optional && keyword_hash) {
	for (i = 0; i < optional; i++) {
	    st_data_t val;
	    if (extract_kwarg(ID2SYM(table[required+i]), &val)) {
		if (values) values[required+i] = (VALUE)val;
		j++;
	    }
	}
    }
    if (!rest && keyword_hash) {
	if (RHASH_SIZE(keyword_hash) > (unsigned int)(values ? 0 : j)) {
	    unknown_keyword_error(keyword_hash, table, required+optional);
	}
    }
    return j;
#undef extract_kwarg
}
