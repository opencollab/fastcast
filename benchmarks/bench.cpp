#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>

#include "fastcast.hxx"

struct A;
struct B;
struct C;
struct D;
struct E;
struct F;
struct G;

// We use volatile here to force to read the _fcast_id property
using Fcast = fastcast::fcast<A, volatile uint64_t>;

struct A : public Fcast
{
    typedef fastcast::hierarchy<fastcast::root, fastcast::children<B, C>> fcast_hierarchy;

    A() { Fcast::set_id<A>(); }

    virtual ~A() { }
    virtual int foo() { return 0; }
};

struct B : public A
{
    typedef fastcast::hierarchy<A, fastcast::children<D>> fcast_hierarchy;

    B() { Fcast::set_id<B>(); }

    virtual ~B() { }
    virtual int foo() { return 1; }
};

struct C : public A
{
    typedef fastcast::hierarchy<A> fcast_hierarchy;

    C() { Fcast::set_id<C>(); }

    virtual ~C() { }
    virtual int foo() { return 2; }
};

struct D : public B
{
    typedef fastcast::hierarchy<B, fastcast::children<E>> fcast_hierarchy;

    D() { Fcast::set_id<D>(); }

    virtual ~D() { }
    virtual int foo() { return 3; }
};

struct E : public D
{
    typedef fastcast::hierarchy<D, fastcast::children<F>> fcast_hierarchy;

    E() { Fcast::set_id<E>(); }

    virtual ~E() { }
    virtual int foo() { return 4; }
};

struct F : public E
{
    typedef fastcast::hierarchy<E, fastcast::children<G>> fcast_hierarchy;

    F() { Fcast::set_id<F>(); }

    virtual ~F() { }
    virtual int foo() { return 5; }
};

struct G : public F
{
    typedef fastcast::hierarchy<F> fcast_hierarchy;

    G() { Fcast::set_id<G>(); }

    virtual ~G() { }
    virtual int foo() { return 6; }
};

template<typename T>
int bench_dynamic_cast(unsigned long long N, A ** x)
{
    int s = 0;
    A * y = *x;

    for (unsigned long long i = 0; i < N; i++)
    {
        s += dynamic_cast<T *>(y) ? 0 : 1;
    }

    return s;
}

template<typename T>
int bench_fast_cast(unsigned long long N, A ** x)
{
    int s = 0;
    A * y = *x;

    for (unsigned long long i = 0; i < N; i++)
    {
        s += Fcast::cast<T>(y) ? 0 : 1;
    }

    return s;
}

unsigned long long bench(unsigned L, unsigned long long N, A ** x, int (*func)(unsigned long long, A **))
{
    std::vector<unsigned long long> times;
    for (unsigned int i = 0; i < L; i++)
    {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        func(N, x);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        times.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
    }

    unsigned long long m = 0;
    for (auto i : times)
    {
        m += i;
    }

    m /= L;
    double e = 0;
    for (auto i : times)
    {
        e += (m - i) * (m - i);
    }
    e /= L;
    e = sqrt(e);

    std::cout << L << " loops with " << N << " iterations: " << "mean=" << m << "(microsecs) and sigma=" << e << std::endl;

    return m;
}


int main(int argc, char ** argv)
{
    if (argc >= 2)
    {
        unsigned int L = std::atol(argv[1]);
        unsigned long long N = std::atol(argv[2]);
        A * x = new G;
        unsigned long long mean1, mean2;

        std::cout << "dynamic_cast<G*>((A*)G*):" << std::endl;
        mean1 = bench(L, N, &x, bench_dynamic_cast<G>);

        std::cout << "fastcast::cast<G>((A*)G*):" << std::endl;
        mean2 = bench(L, N, &x, bench_fast_cast<G>);

        std::cout << "fastcast is " << (((double)mean1) / mean2) << " times faster." << std::endl << std::endl;

        std::cout << "dynamic_cast<B*>((A*)G*):" << std::endl;
        mean1 = bench(L, N, &x, bench_dynamic_cast<B>);

        std::cout << "fastcast::cast<B>((A*)G*):" << std::endl;
        mean2 = bench(L, N, &x, bench_fast_cast<B>);

        std::cout << "fastcast is " << (((double)mean1) / mean2) << " times faster." << std::endl << std::endl;

        std::cout << "dynamic_cast<C*>((A*)G*):" << std::endl;
        mean1 = bench(L, N, &x, bench_dynamic_cast<C>);

        std::cout << "fastcast::cast<C>((A*)G*):" << std::endl;
        mean2 = bench(L, N, &x, bench_fast_cast<C>);

        std::cout << "fastcast is " << (((double)mean1) / mean2) << " times faster." << std::endl << std::endl;

        delete x;
    }

    return 0;
}
