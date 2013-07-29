# API of Next NArray
under development

## NArray演算の仕様

### NArray Types
    #define NARRAY_DATA_T 0x1       // データを保持する。contiguousアクセスのみ可能。
    #define NARRAY_VIEW_T 0x2       // データを保持しない。他のNArrayを参照。stride/index アクセスが可能
    #define NARRAY_FILEMAP_T 0x3    // ファイルマップ（TBI）

### Loop Rule
    method( a[nz,1,nz], b[nz,ny,1] ) => c[nz,ny,nx]
* サイズ1の次元は、1エレメントを繰り返し参照される。外積のようなもの
* すべての演算・メソッドについて同様のルールが適用

### Inplace
    a.inplace + b => a に結果が保存される

## NArrayメソッド定義の例

単純なループ演算を行うイテレータ関数を定義し、
ndfunc_alloc関数で ndfunc_t構造体に登録し、
ndfunc_do関数で多次元ループ処理を行う。
配列のキャスト、出力配列の準備、および多次元ループ処理については、内部で自動的に行う。

    #define cT cDFloat
    // 最も内側のループごとに呼ばれるイテレータ関数
    static void
    iter_dfloat_add(na_loop_t *const lp)
    {
        size_t  n = lp->n[0];
        double *a = (double*)(lp->args[0].ptr + lp->iter[0].pos);
        double *b = (double*)(lp->args[1].ptr + lp->iter[1].pos);
        double *c = (double*)(lp->args[2].ptr + lp->iter[2].pos);
        size_t  i;

        for (i=0; i<n; i++) {
           c[i] = a[i] + b[i];
        }
    }

    // Rubyメソッドに対応するC関数を定義
    static VALUE
    nary_dfloat_add_self(VALUE self, VALUE other)
    {
        ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
        ndfunc_arg_out_t aout[1] = {{cT,0}};
        ndfunc_t ndf = { iter_dfloat_add, STRIDE_LOOP, 2, 1, ain, aout };

        return na_ndloop(&ndf, 2, self, other);
    }

    static VALUE
    nary_dfloat_add(VALUE self, VALUE other)
    {
        VALUE klass, v;
        klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
        if (klass==cT) {
            return nary_dfloat_add_self(self, other);
        } else {
            v = rb_funcall(klass, id_cast, 1, self);
            return rb_funcall(v, '+', 1, other);
        }
    }

    // RubyメソッドをC関数として定義
    void
    Init_nary_dfloat()
    {
        rb_define_method(cT, "+", nary_dfloat_add, 1);
    }


## 関数説明

### ndfunc_t 構造体

ループのスペックを記録する構造体。na_ndloop 関数に渡す。
malloc で確保すると例外が起きると回収されないので、スタックで確保する。

    typedef struct NDFUNCTION {
        na_iter_func_t func;    // user function
        unsigned int flag;      // what kind of loop user function supports
        int nin;                // # of arguments
        int nout;               // # of results
        ndfunc_arg_in_t *ain;   // spec of input arguments
        ndfunc_arg_out_t *aout; // spec of output result
    } ndfunc_t;

* func: イテレータ関数
* flag: イテレータのタイプをフラグで指定

            #define NDF_CONTIGUOUS_LOOP     (1<<0) // x[i]
            #define NDF_STRIDE_LOOP         (1<<1) // *(x+stride*i)
            #define NDF_INDEX_LOOP          (1<<2) // *(x+idx[i])
            #define NDF_KEEP_DIM            (1<<3)
            #define NDF_ACCEPT_SWAP         (1<<4)
            #define NDF_HAS_MARK_DIM        (1<<5)
            (#define NDF_INPLACE)
            #define NDF_FULL_LOOP (NDF_CONTIGUOUS_LOOP|NDF_STRIDE_LOOP|NDF_INDEX_LOOP)

* nin: 引数として渡す入力NArrayの数
* nout: 結果として戻る出力NArrayの数

### ndfunc_arg_in_t 構造体

入力引数の引数のタイプとユーザ次元を指定する。

    typedef struct NDF_ARG_IN {
        VALUE   type;    // argument types
        int     dim;     // # of dimension of argument handled by user function
    } ndfunc_arg_in_t;

* type が Qnil のとき、キャストは行われない。
* type が NArray型クラスのとき、その型へキャストが行われる。
* type が Symbol のとき、オプション変数とみなされる。次のいずれか
    * :option のとき、loop->option に代入。
    * :init のとき、dim番目の出力変数の初期化に用いられる。
    * :reduce のとき、リデュース次元のビットを立てたFixnumを代入。
    * :loop_opt 内部使用

### ndfunc_arg_out_t 構造体

結果をストアするnarrayのタイプとユーザ次元を指定する。

typedef struct NDF_ARG_OUT {
    VALUE   type;    // argument types
    int     dim;     // # of dimension of argument handled by user function
    size_t *shape;
} ndfunc_arg_out_t;

* type が i (Fixnum) のとき、入力引数のi番目の型を使用する。

* イテレータ関数がループをサポートする場合、次のいずれかの方法で指示。
    * flag にループ可能であることを指定。
    * dim を 1 以上にセットする。さらに出力配列の場合は shape に配列サイズをセットする。

### na_ndloop関数
多次元ループのメイン処理を行う。イテレータ関数を呼んで多次元ループを行う。

    VALUE na_ndloop(ndfunc_t *nf, int argc, ...)
    VALUE na_ndloop2(ndfunc_t *nf, VALUE args)
    VALUE na_ndloop3(ndfunc_t *nf, void *opt_ptr, int argc, ...)
    VALUE na_ndloop4(ndfunc_t *nf, void *opt_ptr, VALUE args)

* 戻り値は、ndfunc_t 構造体で指定した戻り値を返す

### イテレータ関数
配列情報を格納した na_loop_t 構造体へのポインタが引数として渡される。

    iter_dfloat_add(na_loop_t *const lp)

### na_loop_t 構造体
引数の配列と、配列へのアクセス方法の情報を格納する。

    typedef struct NA_LOOP {
        int  narg;
        int  ndim;             // n of user dimention
        size_t *n;             // n of elements for each dim
        na_loop_args_t *args;  // for each arg
        na_loop_iter_t *iter;  // for each dim, each arg
        VALUE  option;
        void  *opt_ptr;
    } na_loop_t;

    typedef struct NA_LOOP_ARGS {
        VALUE    value;
        ssize_t  elmsz;
        char    *ptr;
    } na_loop_args_t;

    typedef struct NA_LOOP_ITER {
        ssize_t    pos;
        ssize_t    step;
        size_t    *idx;
    } na_loop_iter_t;

