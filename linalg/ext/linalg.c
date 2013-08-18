extern void Init_nary_dfloat_linalg();
extern void Init_nary_sfloat_linalg();
extern void Init_nary_dcomplex_linalg();
extern void Init_nary_scomplex_linalg();

void Init_linalg() {
    Init_nary_dfloat_linalg();
    Init_nary_sfloat_linalg();
    Init_nary_dcomplex_linalg();
    Init_nary_scomplex_linalg();
}
