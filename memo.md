- 処理の途中で raise された場合、処理が戻ってこない場合がある。
- その場合、 malloc で確保したメモリーが開放されない可能性があり、メモリーリークになる。
- メモリーリークを防ぐには、次のいずれかの方法を採る。
    - スタックにメモリを確保する
    - ruby object としてラップする

現状のコード

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

    typedef struct NDFUNCTION {
        na_iter_func_t func; // user function
        unsigned int flag;   // what kind of loop user function supports
        int narg;            // # of arguments
        int nopt;            // # of options
        int nres;            // # of results
        ndfunc_arg_t *args;  // spec of arguments
        VALUE *opt_types;    // option types
    } ndfunc_t;

    typedef struct NDFUNC_ARG {
        VALUE type;    // argument types
        VALUE init;    // initial value
        int dim;       // # of dimension of argument handled by user function
        union {
            size_t shape[1];
            size_t *shape_p;
        } aux;         // shape
    } ndfunc_arg_t;


ndfunc_t をスタックで確保するには

    // Rubyメソッドに対応するC関数を定義
    static VALUE
    nary_dfloat_s_add(VALUE mod, VALUE a1, VALUE a2)
    {
        ndfunc_t nf;
        VALUE v;
        VALUE inp[2] = {cT,cT};
        VALUE out[1] = {cT};
        option_t option;

        ndfunc_arg_t inp[2] = {{cT,0,NULL},{cT,0,NULL}};
        ndfunc_arg_t out[1] = {{cT,0,NULL}};

        ndfunc_set( nf, itr, flag, inp, 2, out, 1 );
        nf.func = itr_func;
        nf.flag = flag;
        nf.inp = inp;
        nf.ninp = 2;
        nf.out = out;
        nf.nout = 1;
        nf.option = &option;

        func = ndfunc_alloc(iter_dfloat_add, NDF_CONTIGUOUS_LOOP,
                            2, 1, cT, cT, cT);
        v = ndloop_do(func, 2, a1, a2);
        ndfunc_free(func);
        return v;
    }

こうしたい：

    // Rubyメソッドに対応するC関数を定義
    static VALUE
    nary_dfloat_s_add(VALUE mod, int argc, VALUE *argv)
    {
        nanika_t opts;
        ndfunc_arg_t args[3] = {{cT,0,NULL},{cT,0,NULL},{INT2FIX(3),0,NULL}};
        ndfunc_t ndf = { init_func, itr_func, flags, 2, 1, args, &opts };

        return na_ndloop(&ndf, argc, argv);
    }



    // Rubyメソッドに対応するC関数を定義
    static VALUE
    nary_dfloat_s_add(VALUE mod, int argc, VALUE *argv)
    {
        nanika_t opts;
        ndfunc_arg_t argt[3] = {{cT,0,NULL},{cT,0,NULL},{INT2FIX(3),0,NULL}};
        ndfunc_t ndf = { init_func, itr_func, flags, 2, 1, argt, &opts };

        return na_vndloop(&ndf, argc, argv);
    }

