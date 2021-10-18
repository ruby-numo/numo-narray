static size_t
<%=type_name%>_memsize(const void* ptr)
{
    size_t size = sizeof(narray_data_t);
    const narray_data_t *na = (const narray_data_t*)ptr;

    assert(na->base.type == NARRAY_DATA_T);

    if (na->ptr != NULL) {
  <% if is_bit %>
        size += ((na->base.size-1)/8/sizeof(BIT_DIGIT)+1)*sizeof(BIT_DIGIT);
  <% else %>
        size += na->base.size * sizeof(dtype);
  <% end %>
    }
    if (na->base.size > 0) {
        if (na->base.shape != NULL && na->base.shape != &(na->base.size)) {
            size += sizeof(size_t) * na->base.ndim;
        }
    }
    return size;
}

static void
<%=type_name%>_free(void* ptr)
{
    narray_data_t *na = (narray_data_t*)ptr;

    assert(na->base.type == NARRAY_DATA_T);

    if (na->ptr != NULL) {
        if (na->owned) {
            xfree(na->ptr);
        }
        na->ptr = NULL;
    }
    if (na->base.size > 0) {
        if (na->base.shape != NULL && na->base.shape != &(na->base.size)) {
            xfree(na->base.shape);
            na->base.shape = NULL;
        }
    }
    xfree(na);
}

static narray_type_info_t <%=type_name%>_info = {
  <% if is_bit %>
    1,             // element_bits
    0,             // element_bytes
    1,             // element_stride (in bits)
  <% else %>
    0,             // element_bits
    sizeof(dtype), // element_bytes
    sizeof(dtype), // element_stride (in bytes)
  <% end %>
};

<% if is_object %>
static void
<%=type_name%>_gc_mark(void *ptr)
{
    size_t n, i;
    VALUE *a;
    narray_data_t *na = ptr;

    if (na->ptr) {
        a = (VALUE*)(na->ptr);
        n = na->base.size;
        for (i=0; i<n; i++) {
            rb_gc_mark(a[i]);
        }
    }
}

static const rb_data_type_t <%=type_name%>_data_type = {
    "<%=full_class_name%>",
    {<%=type_name%>_gc_mark, <%=type_name%>_free, <%=type_name%>_memsize,},
    &na_data_type,
    &<%=type_name%>_info,
    0, // flags
};

<% else %>

static const rb_data_type_t <%=type_name%>_data_type = {
    "<%=full_class_name%>",
    {0, <%=type_name%>_free, <%=type_name%>_memsize,},
    &na_data_type,
    &<%=type_name%>_info,
    RUBY_TYPED_FROZEN_SHAREABLE, // flags
};

<% end %>

static VALUE
<%=c_func(0)%>(VALUE klass)
{
    narray_data_t *na = ALLOC(narray_data_t);

    na->base.ndim = 0;
    na->base.type = NARRAY_DATA_T;
    na->base.flag[0] = NA_FL0_INIT;
    na->base.flag[1] = NA_FL1_INIT;
    na->base.size = 0;
    na->base.shape = NULL;
    na->base.reduce = INT2FIX(0);
    na->ptr = NULL;
    na->owned = FALSE;
    return TypedData_Wrap_Struct(klass, &<%=type_name%>_data_type, (void*)na);
}
