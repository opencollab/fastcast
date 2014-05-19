#include <iostream>
#include <chrono>
//#include <typeinfo>

#include "fastcast.hxx"

struct A;
struct B;
struct C;
struct D;
struct E;
struct F;
struct G;

using Fcast = fastcast::fcast<A, uint64_t>;

struct A : public Fcast
{
    typedef fastcast::hierarchy<fastcast::root, fastcast::children<B, C>> fcast_hierarchy;

    A() { Fcast::set_id<A>(); }

    virtual int foo() { return 0; }
};

struct B : public A
{
    typedef fastcast::hierarchy<A, fastcast::children<D>> fcast_hierarchy;

    B() { Fcast::set_id<B>(); }

    virtual int foo() { return 1; }
};

struct C : public A
{
    typedef fastcast::hierarchy<A> fcast_hierarchy;

    C() { Fcast::set_id<C>(); }

    virtual int foo() { return 2; }
};

struct D : public B
{
    typedef fastcast::hierarchy<B, fastcast::children<E>> fcast_hierarchy;

    D() { Fcast::set_id<D>(); }

    virtual int foo() { return 3; }
};

struct E : public D
{
    typedef fastcast::hierarchy<D, fastcast::children<F>> fcast_hierarchy;

    E() { Fcast::set_id<E>(); }

    virtual int foo() { return 4; }
};

struct F : public E
{
    typedef fastcast::hierarchy<E, fastcast::children<G>> fcast_hierarchy;

    F() { Fcast::set_id<F>(); }

    virtual int foo() { return 5; }
};

struct G : public F
{
    typedef fastcast::hierarchy<F> fcast_hierarchy;

    G() { Fcast::set_id<G>(); }

    virtual int foo() { return 6; }
};

template<typename T>
int bench_dynamic_cast(unsigned long long N, A ** arr, unsigned int size)
{
    int s = 0;
    unsigned int j = 0;
    for (unsigned long long i = 0; i < N; i++)
    {
	T * x = dynamic_cast<T *>(arr[j]);
	if (x)
	{
	    s += x->foo();
	}
	if (j == size - 1)
	{
	    j = 0;
	}
	else
	{
	    j++;
	}
    }

    return s;
}

template<typename T>
int bench_fast_cast(unsigned long long N, A ** arr, unsigned int size)
{
    int s = 0;
    unsigned int j = 0;
    for (unsigned long long i = 0; i < N; i++)
    {
	T * x = Fcast::cast<T>(arr[j]);
	if (x)
	{
	    s += x->foo();
	}
	if (j == size - 1)
	{
	    j = 0;
	}
	else
	{
	    j++;
	}
    }

    return s;
}


int main(int argc, char ** argv)
{
    if (argc >= 2)
    {
	unsigned int type = std::atol(argv[1]);
	unsigned long long N = std::atol(argv[2]);
	int s = 0;
	A ** arr = new A *[1];
	arr[0] = new G;
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

	switch(type)
	{
	    case 0:
		s = bench_dynamic_cast<G>(N, arr, 1);
		break;
	    case 1:
		s = bench_fast_cast<G>(N, arr, 1);
		break;
	    case 2:
		s = bench_dynamic_cast<B>(N, arr, 1);
		break;
	    case 3:
		s = bench_fast_cast<B>(N, arr, 1);
		break;
	    case 4:
		s = bench_dynamic_cast<C>(N, arr, 1);
		break;
	    case 5:
		s = bench_fast_cast<C>(N, arr, 1);
		break;
	}
 	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	delete arr[0];
	delete[] arr;

	std::cout << (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() * 1e-6) << std::endl; 
    }    

    return 0;
}
