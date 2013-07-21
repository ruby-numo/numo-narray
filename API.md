# API of Next NArray
under development
## NArrayの前提
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
イテレータとそのループタイプを与えれば、内部でループルールの処理・配列生成を自動的にを行う

    // 最も内側のループごとに呼ばれるイテレータ関数
    #define cT cDFloat
    static void
    iter_dfloat_add(na_loop_t *const lp)
    {
        double *a = ...
        double *b = ...
        double *c = ...
        for (i=0; i<n; i++) {
           c[i] = a[i] + b[i];
        }
    }

    // Rubyメソッドに対応するC関数を定義
    static VALUE
    nary_dfloat_s_add(VALUE mod, VALUE a1, VALUE a2)
    {
        ndfunc_t *func;
        VALUE v;
        func = ndfunc_alloc(iter_dfloat_add, FULL_LOOP,
                            2, 1, cT, cT, cT);
        v = ndloop_do(func, 2, a1, a2);
        ndfunc_free(func);
        return v;
    }

    // RubyメソッドをC関数として定義
    void
    Init_nary_dfloat()
    {
        rb_define_method(cT, "+", nary_dfloat_add, 1);
    }

## 関数説明
### ndfunc_alloc関数
ndfunc_t 構造体をアロケートして初期化して返す

    ndfunc_t* ndfunc_alloc(na_iter_func_t func, int flag, int narg, int nres,
                           VALUE in_type1, ... VALUE out_type1, ...)

* func: イテレータ関数
* flag: イテレータのタイプをフラグで指定 イテレータ内でループを持つか、stride/indexが可能か、etc.
* narg: 引数として渡す入力NArrayの数
* nres: 結果として戻る出力NArrayの数
* 以降: narg個の入力データ型とnres個の出力データ型を与える
    * 入力データはここで指定した入力データ型にキャストされる。キャストしない場合はQnil
    * 出力データ型はここで指定した出力データ型でNArrayが作られる。
      Fixnumのときは、n番目の入力データ型と同じデータ型のNArrayが作られる

### ndfunc_t 構造体

    typedef struct NDFUNCTION {
        na_iter_func_t func; // user function
        unsigned int flag;   // what kind of loop user function supports
        int narg;            // # of arguments
        int nopt;            // # of options
        int nres;            // # of results
        ndfunc_arg_t *args;  // spec of arguments
        VALUE *opt_types;    // option types
    } ndfunc_t;

### ndfunc_arg_t 構造体

    typedef struct NDFUNC_ARG {
        VALUE type;    // argument types
        VALUE init;    // initial value
        int dim;       // # of dimension of argument handled by user function
        union {
            size_t shape[1];
            size_t *shape_p;
        } aux;         // shape
    } ndfunc_arg_t;

* 内側ループの次元とサイズをコントロールしたい場合は、dim と shape に値をセットする

### ndloop_do関数
イテレータ関数を呼んで多次元ループを行う

    ndloop_do(ndfunc_t *nf, int argc, ...)

* ndfunc_t 構造体に指定した戻り値を返す

### イテレータ関数
na_loop_t 構造体を引数として渡されて呼ばれる。

    iter_dfloat_add(na_loop_t *const lp)

### na_loop_t 構造体
引数の配列と、配列へのアクセス方法の情報を格納

    typedef struct NA_LOOP {
        int  narg;
        int  ndim;             // n of user dimention
        size_t *n;             // n of elements for each dim
        na_loop_args_t *args;  // for each arg
        na_loop_iter_t *iter;  // for each dim, each arg
        VALUE  info;
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

