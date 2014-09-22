#include "ruby.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//#define WIN32

#ifdef WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

#include "narray.h"

#define BD_TMPLOCK  FL_USER1
#define BD_ASSOC    FL_USER3
#define BD_RESIZE   FL_USER4

#define IS_BYTEDATA(obj) ((RDATA(obj)->dfree == (RUBY_DATA_FUNC)bd_free) || (RDATA(obj)->dfree == (RUBY_DATA_FUNC)mm_free))
#define ByteData(obj) get_bytedata(obj)
//#define ByteData(obj) check_bytedata(obj)
//#define ByteDataPtr0(obj) (ByteData(obj)->ptr)
//#define ByteDataPtr(obj) ((void*)ByteDataPtr0(obj))
//#define ByteDataPtrPos(obj,pos) ((void*)((char*)ByteDataPtr0(obj)+(pos)))

//#define BD_RESIZABLE(x) FL_TEST((x), BD_RESIZE)
//#define BD_TMPLOCK(x)   FL_TEST((x), BD_TMPLOCK)


struct ByteData {
    unsigned char *ptr;
    size_t len;
    /*
      union {
      long capa;
      VALUE shared;
      } aux;
    */
};
typedef struct ByteData bytedata_t;

#ifdef WIN32
struct MmapByteData {
    struct ByteData bd;
    HANDLE hFile;
    HANDLE hMap;
};
#else // UNIX mmap
struct MmapByteData {
    struct ByteData bd;
    int prot;
    int flag;
};
#endif

enum  {
    BD_MMAP_RO,
    BD_MMAP_RW
};

static void bd_free _((struct ByteData *));
VALUE bd_locktmp _((VALUE));
VALUE bd_unlocktmp _((VALUE));
VALUE bd_freeze _((VALUE));
VALUE bd_enable_resize _((VALUE));
VALUE bd_disable_resize _((VALUE));
static void mm_free _((struct MmapByteData *));

#if 0
VALUE bd_new(length);
VALUE bd_new(string);
frozen?
locked?
resizable?
taint?
#endif

VALUE mNum;
VALUE cByteData;
VALUE cMmapByteData;

static struct ByteData*
check_bytedata(self)
    VALUE self;
{
    Check_Type(self, T_DATA);
    if (!IS_BYTEDATA(self)) {
        rb_raise(rb_eTypeError, "wrong argument type %s (expected ByteData)",
                 rb_class2name(CLASS_OF(self)));
    }
    return DATA_PTR(self);
}

static struct ByteData*
get_bytedata(self)
    VALUE self;
{
    struct ByteData *ptr = check_bytedata(self);
    if (!ptr) {
        rb_raise(rb_eIOError, "uninitialized bytedata");
    }
    return ptr;
}

static void
bd_free(ptr)
  struct ByteData *ptr;
{
  if (ptr->ptr != NULL) {
    xfree(ptr->ptr);
  }
  xfree(ptr);
}


static VALUE
bd_s_allocate(klass)
  VALUE klass;
{
  struct ByteData *ptr = ALLOC(struct ByteData);

  ptr->ptr = NULL;
  ptr->len = 0;

  return Data_Wrap_Struct(klass, 0, bd_free, ptr);
}


static VALUE
bd_initialize(argc, argv, self)
  int argc;
  VALUE *argv;
  VALUE self;
{
    struct ByteData *bd;
    VALUE vlen;
    size_t len;

    Check_Type(self, T_DATA);
    bd = DATA_PTR(self);
    //rb_call_super(0, 0);
    rb_scan_args(argc, argv, "10", &vlen);
    bd->len = NUM2SIZE(vlen);
    bd->ptr = ALLOC_N(char,bd->len);
    return self;
}


static VALUE
bd_s_new(argc, argv, obj)
  int argc;
  VALUE *argv, obj;
{
  VALUE res = rb_funcall2(obj, rb_intern("allocate"), 0, 0);
  rb_obj_call_init(res, argc, argv);
  return res;
}


VALUE
rb_bd_new( size_t len )
{
    volatile VALUE obj;
    VALUE argv[1];

    obj = bd_s_allocate(cByteData);
    //printf("pass# b1! %x %d \n", obj, len );
    argv[0] = SIZE2NUM(len);
    //printf("pass# b2! %x %d \n", obj, len );
    bd_initialize( 1, argv, obj );
    //printf("pass# b3! %x %d \n", obj, len );
    return obj;
}



static VALUE
bd_length(bd)
    VALUE bd;
{
    return SIZE2NUM(ByteData(bd)->len);
}


VALUE
bd_freeze(bd)
    VALUE bd;
{
    return rb_obj_freeze(bd);
}


VALUE
bd_locktmp(bd)
    VALUE bd;
{
    if (FL_TEST(bd, BD_TMPLOCK)) {
        rb_raise(rb_eRuntimeError, "temporal locking already locked bytedata");
    }
    FL_SET(bd, BD_TMPLOCK);
    return bd;
}

VALUE
bd_unlocktmp(bd)
    VALUE bd;
{
    if (!FL_TEST(bd, BD_TMPLOCK)) {
        rb_raise(rb_eRuntimeError, "temporal unlocking already unlocked bytedata");
    }
    FL_UNSET(bd, BD_TMPLOCK);
    return bd;
}


VALUE
bd_enable_resize(bd)
    VALUE bd;
{
    FL_SET(bd, BD_RESIZE);
    return bd;
}


VALUE
bd_disable_resize(bd)
    VALUE bd;
{
    FL_UNSET(bd, BD_RESIZE);
    return bd;
}


void *rb_bd_pointer_for_read(VALUE obj)
{
    struct ByteData *bd = ByteData(obj);
    if (!OBJ_FROZEN(obj)) {
	if (!FL_TEST(obj, BD_TMPLOCK)) {
	    if (FL_TEST(obj, BD_RESIZE)) {
		rb_raise(rb_eRuntimeError,
			 "cant read unfrozen, unlocked, resizable bytedata.");
	    }
	}
    }
    //if (pos >= bd->len) {
    //	rb_raise(rb_eRuntimeError, "cant acces to this position.");
    //}
    return bd->ptr;
}


void *rb_bd_pointer_for_write(VALUE obj)
{
    struct ByteData* bd = ByteData(obj);
    if (OBJ_FROZEN(obj)) {
	rb_raise(rb_eRuntimeError, "cant write frozen bytedata.");
    }
    if (!FL_TEST(obj, BD_TMPLOCK)) {
	if (FL_TEST(obj, BD_RESIZE)) {
	    rb_raise(rb_eRuntimeError, "cant write unlocked, resizable bytedata.");
	}
    }
    //if (pos >= bd->len) {
    //	rb_raise(rb_eRuntimeError, "cant acces to this position.");
    //}
    return bd->ptr;
}



static VALUE
bd_to_str(self)
  VALUE self;
{
  struct ByteData *bd;

  Check_Type(self, T_DATA);
  bd = DATA_PTR(self);
  return rb_str_new(bd->ptr,bd->len);
}

static VALUE
bd_aref(self, index)
     VALUE self;
     VALUE index;
{
    struct ByteData *bd;

    Check_Type(self, T_DATA);
    bd = DATA_PTR(self);
    return INT2FIX(bd->ptr[FIX2INT(index)]);
}

static VALUE
bd_aset(self, index, val)
     VALUE self;
     VALUE index;
     VALUE val;
{
    struct ByteData *bd;

    Check_Type(self, T_DATA);
    bd = DATA_PTR(self);
    bd->ptr[FIX2INT(index)] = FIX2INT(val);
    return val;
}








// -----------------------------------------------------------------------
//#if 1

//typedef enum mmap_open_mode_ {
//    SA_MMAP_RO,
//    SA_MMAP_RW
//} SA_MMAP_MODE;


#ifdef WIN32

static void
mm_free(struct MmapByteData *mm)
{
  if (mm->bd.ptr != NULL) {
	UnmapViewOfFile(mm->bd.ptr);
	CloseHandle(mm->hMap);
	CloseHandle(mm->hFile);
      //if (munmap(ptr->bd.ptr, ptr->bd.len)) {
      //	  rb_raise(rb_eRuntimeError, "munmap failed");
      //}
      //printf("pass3\n");
      //xfree(ptr->bd.ptr);
  }
  xfree(mm);
}


static int
mm_open(struct MmapByteData *mm, const char *fname)
//sa_open_mmap_aux(SA_MMAP *mm, const char *fname, const SA_MMAP_MODE mode)
{
    unsigned long mode1;
    unsigned long mode2;
    unsigned long mode3;
    HANDLE hFile;
    HANDLE hMap;

    /* set parameters
    if (mode == SA_MMAP_RO) {
	mode1 = GENERIC_READ;
	mode2 = PAGE_READONLY;
	mode3 = FILE_MAP_READ;
    } else if (mode == SA_MMAP_RW) {
	mode1 = GENERIC_READ | GENERIC_WRITE;
	mode2 = PAGE_READWRITE;
	mode3 = FILE_MAP_ALL_ACCESS;
    } else {
	return 1;
    }
    */
    mode1 = GENERIC_READ | GENERIC_WRITE;
    mode2 = PAGE_READWRITE;
    mode3 = FILE_MAP_ALL_ACCESS;

    //mm->other = sa_malloc(sizeof(MMAP_HANDLES));
    //if (mm->other == NULL)
    //    return 1;

    hFile = CreateFile(fname, mode1, 0, NULL,
		       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
      rb_raise(rb_eIOError,"CreateFile error");
      return 1;
    }
    mm->bd.len = GetFileSize(hFile, NULL);

    hMap = CreateFileMapping(hFile, NULL,
			     mode2, 0, 0, NULL);
    if (hMap == NULL) {
      CloseHandle(hFile);
      rb_raise(rb_eIOError,"CreateFileMapping error");
      return 1;
    }

    mm->bd.ptr = MapViewOfFile(hMap, mode3, 0, 0, 0);
    if (mm->bd.ptr == NULL) {
	CloseHandle(hFile);
	CloseHandle(hMap);
	rb_raise(rb_eIOError,"MapViewOfFile error");
	return 1;
    }

    mm->hFile = hFile;
    mm->hMap = hMap;

    return 0;
}


static VALUE
mm_s_allocate(klass)
  VALUE klass;
{
    struct MmapByteData *ptr = ALLOC(struct MmapByteData);

    ptr->bd.ptr = NULL;
    ptr->bd.len = 0;
    ptr->hFile = 0;
    ptr->hMap = 0;

    return Data_Wrap_Struct(klass, 0, mm_free, ptr);
}


#else // UNIX mmap

static void
mm_free(struct MmapByteData *ptr)
{
  if (ptr->bd.ptr != NULL) {
      if (munmap(ptr->bd.ptr, ptr->bd.len)) {
	  rb_raise(rb_eRuntimeError, "munmap failed");
      }
      //printf("pass3\n");
      //xfree(ptr->bd.ptr);
  }
  xfree(ptr);
}

static int
mm_open(struct MmapByteData *mm, const char *fname)
{
    int fd;
    struct stat stat_buf;
    int flag;
    int prot;

    /* set parameters
    if (mode == SA_MMAP_RO) {
        flag = O_RDONLY;
        prot = PROT_READ;
    } else
    if (mode == SA_MMAP_RW) {
        flag = O_RDWR;
        prot = PROT_READ | PROT_WRITE;
    } else {
	rb_raise(rb_eIOError,"wrong mode");
        return 1;
    }
    */
    flag = O_RDWR;
    prot = PROT_READ | PROT_WRITE;

    /* open the addressed file */
    if ((fd = open(fname, flag)) == -1) {
	rb_raise(rb_eIOError,"open(2) error");
        return 1;
    }

    /* size of the file */
    if (fstat(fd, &stat_buf) != 0) {
        close(fd);
	rb_raise(rb_eIOError,"fstat(2) error");
        return 1;
    }

    mm->bd.len = stat_buf.st_size;
    mm->prot = prot;
    mm->flag = flag;


    /* ready to load the file */
    mm->bd.ptr = mmap((caddr_t)0, mm->bd.len, prot, MAP_SHARED, fd, 0);
    close(fd);

    if (mm->bd.ptr == (void *)-1) {
	rb_raise(rb_eIOError,"mmap(2) error");
        return 1;
    }
    return 0;
}

static VALUE
mm_s_allocate(klass)
  VALUE klass;
{
    struct MmapByteData *ptr = ALLOC(struct MmapByteData);

    ptr->bd.ptr = NULL;
    ptr->bd.len = 0;
    ptr->prot = 0;
    ptr->flag = 0;

    return Data_Wrap_Struct(klass, 0, mm_free, ptr);
}

#endif



static VALUE
mm_initialize(argc, argv, self)
     int argc;
     VALUE *argv;
     VALUE self;
{
    struct MmapByteData *mm;
    VALUE vfile;
    char *fname;

    Check_Type(self, T_DATA);
    mm = DATA_PTR(self);
    //rb_call_super(0, 0);
    rb_scan_args(argc, argv, "10", &vfile);
    fname = StringValueCStr(vfile);

    mm_open(mm, fname);

    return self;
}


void
Init_bytedata()
{
  mNum = rb_define_module("Num");
  cByteData = rb_define_class_under(mNum, "ByteData", rb_cData);
  cMmapByteData = rb_define_class_under(mNum, "MmapByteData", cByteData);

  rb_define_alloc_func(cByteData, bd_s_allocate);
  rb_define_method(cByteData, "initialize", bd_initialize, -1);
  rb_define_method(cByteData, "to_str", bd_to_str, 0);
  rb_define_method(cByteData, "[]", bd_aref, 1);
  rb_define_method(cByteData, "[]=", bd_aset, 2);
  rb_define_method(cByteData, "length", bd_length, 0);
  rb_define_method(cByteData, "size", bd_length, 0);

  rb_define_alloc_func(cMmapByteData, mm_s_allocate);
  rb_define_method(cMmapByteData, "initialize", mm_initialize, -1);
}


/*
= memory class

a = Memory.new

struct
{
 size_t size;
 void *ptr;
    union {
        long capa;
        VALUE shared;
    } aux;
}

mem = Memory.new(size)

mem.size
mem.empty?
mem.realloc
mem.to_s
mem.dump

mem.[]
mem.concat
mem.freeze
mem.clone
mem.dup
mem.==
mem.eql?
mem.hash

= MemMap class

mem = Memory.new("file", mode, offset, length)
mem = Memory.new(file_obj, mode, offset, size)

map_anonymous

?
sync
shared
File objectから作成？
*/
