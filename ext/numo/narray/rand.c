#include "ruby.h"
#include "narray.h"
#include "SFMT.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <time.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

int n_bits(u_int64_t a)
{
    int i, x, xu, xl, n=5;
    u_int64_t m;

    if (a==0) return 0;
    //if (a<0) a=-a;

    x  = 1<<n;
    xu = 1<<(n+1);
    xl = 0;
    //printf("%3i, [%3i, %3i], %i\n", i, xu, xl, x);

    for (i=n; i>=0; i--) {
	m = ~((1<<(x-1))-1);
	if (m & a) {
	    xl = x;
	    x += 1<<(i-1);
	} else {
	    xu = x;
	    x -= 1<<(i-1);
	}
	//printf("%3i, [%3i, %3i], %i, 0x%lx, 0x%lx\n", i, xu, xl, x, m, m&a);
    }
    return xl;
}

void rand_norm(double *a)
{
    double x1, x2, w;
    do {
	x1 = to_res53(gen_rand64());
	x1 = x1*2-1;
	x2 = to_res53(gen_rand64());
	x2 = x2*2-1;
	w = x1 * x1 + x2 * x2;
    } while (w>=1);
    w = sqrt( (-2*log(w)) / w );
    a[0] = x1*w;
    a[1] = x2*w;
}

static u_int64_t
 random_seed()
{
    static int n = 0;
    struct timeval tv;

    gettimeofday(&tv, 0);
    return tv.tv_sec ^ tv.tv_usec ^ getpid() ^ n++;
}

static VALUE
 nary_s_srand(int argc, VALUE *argv, VALUE obj)
{
    VALUE vseed;
    u_int64_t seed;

    //rb_secure(4);
    if (rb_scan_args(argc, argv, "01", &vseed) == 0) {
        seed = random_seed();
    }
    else {
        seed = NUM2UINT64(vseed);
    }
    init_gen_rand(seed);

    return Qnil;
}

void
Init_nary_rand() {
    rb_define_singleton_method(cNArray, "srand", nary_s_srand, -1);
    init_gen_rand(0);
}
