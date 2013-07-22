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

    // 最も内側のループごとに呼ばれるイテレータ関数
    #define cT cDFloat
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
    nary_dfloat_s_add(VALUE mod, VALUE a1, VALUE a2)
    {
        ndfunc_t *func;
        VALUE v;

        func = ndfunc_alloc(iter_dfloat_add, NDF_CONTIGUOUS_LOOP,
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
ndfunc_alloc関数は、ndfunc_t 構造体をアロケート・初期化して返す

    ndfunc_t* ndfunc_alloc(na_iter_func_t func, int flag, int narg, int nres,
       　　　　　　　　　　　　VALUE in_type1, ... VALUE out_type1, ...)

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

* narg: 引数として渡す入力NArrayの数
* nres: 結果として戻る出力NArrayの数
* 以降: narg個の入力データ型とnres個の出力データ型を与える
    * 入力データはここで指定した入力データ型にキャストされる。キャストしない場合はQnil。
    * 出力データ型はここで指定した出力データ型でNArrayが作られる。
      Fixnumのときは、n番目の引数と同じデータ型のNArrayが作られる。
      （todo: Arrayのとき、例えば [0,1] のときは、0番目と1番目の引数の型からUPCASTする。）

### ndfunc_t 構造体
ndfunc_t 構造体は、ndfunc_alloc関数を用いてアロケートする。

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

* ndfunc_alloc関数で初期化されたときは、ユーザー次元 dim が 0 にセットされている。
* イテレータ関数がループをサポートする場合、次のいずれかの方法で指示。
    * flag にループ可能であることを指定。
    * dim を 1 以上にセットする。さらに出力配列の場合は shape に配列サイズをセットする。
* 内側ループの次元とサイズをコントロールしたい場合は、dim と shape に値をセットする。

### ndloop_do関数
多次元ループのメイン処理を行う。イテレータ関数を呼んで多次元ループを行う。

    ndloop_do(ndfunc_t *nf, int argc, ...)

* 戻り値は、ndfunc_t 構造体で指定した戻り値を返す

### イテレータ関数
配列情報を格納した na_loop_t 構造体へのポインタが引数として渡される。

    iter_dfloat_add(na_loop_t *const lp)

### na_loop_t 構造体
引数の配列と、配列へのアクセス方法の情報を格納する。

    typedef struct NA_LOOP {
        int  narg;             // nf->narg + nf->nres
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

