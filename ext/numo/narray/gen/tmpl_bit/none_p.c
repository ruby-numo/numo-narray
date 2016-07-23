VALUE
numo_bit_none_p(int argc, VALUE *argv, VALUE self)
{
    VALUE v;

    v = numo_bit_any_p(argc,argv,self);

    if (v==Qtrue) {
        return Qfalse;
    } else if (v==Qfalse) {
        return Qtrue;
    }
    return numo_bit_not(v);
}
