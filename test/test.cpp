// Copyright (c) 2014, Calixte DENIZET
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <cassert>

#include "fastcast.hxx"

struct A;
struct B;
struct C;
struct D;
struct E;
struct F;
struct G;
struct H;
struct I;
struct J;
struct K;
struct L;
struct M;
struct N;

using Fcast = fastcast::fcast<A, uint32_t>;

struct A : public Fcast
{
    typedef fastcast::hierarchy<fastcast::root, fastcast::children<B, C>> fcast_hierarchy;

    A() { Fcast::set_id<A>(); }
};

struct B : public A
{
    typedef fastcast::hierarchy<A, fastcast::children<D>> fcast_hierarchy;

    B() { Fcast::set_id<B>(); }
};

struct C : public A
{
    typedef fastcast::hierarchy<A> fcast_hierarchy;

    C() { Fcast::set_id<C>(); }
};

struct D : public B
{
    typedef fastcast::hierarchy<B, fastcast::children<E, F, G>> fcast_hierarchy;

    D() { Fcast::set_id<D>(); }
};

struct E : public D
{
    typedef fastcast::hierarchy<D, fastcast::children<H, I>> fcast_hierarchy;

    E() { Fcast::set_id<E>(); }
};

struct F : public D
{
    typedef fastcast::hierarchy<D> fcast_hierarchy;

    F() { Fcast::set_id<F>(); }
};

struct G : public D
{
    typedef fastcast::hierarchy<D, fastcast::children<J, K, L, M, N>> fcast_hierarchy;

    G() { Fcast::set_id<G>(); }
};

struct H : public E
{
    typedef fastcast::hierarchy<E> fcast_hierarchy;

    H() { Fcast::set_id<H>(); }
};

struct I : public E
{
    typedef fastcast::hierarchy<E> fcast_hierarchy;

    I() { Fcast::set_id<I>(); }
};

struct J : public G
{
    typedef fastcast::hierarchy<G> fcast_hierarchy;

    J() { Fcast::set_id<J>(); }
};

struct K : public G
{
    typedef fastcast::hierarchy<G> fcast_hierarchy;

    K() { Fcast::set_id<K>(); }
};

struct L : public G
{
    typedef fastcast::hierarchy<G> fcast_hierarchy;

    L() { Fcast::set_id<L>(); }
};

struct M : public G
{
    typedef fastcast::hierarchy<G> fcast_hierarchy;

    M() { Fcast::set_id<M>(); }
};

struct N : public G
{
    typedef fastcast::hierarchy<G> fcast_hierarchy;

    N() { Fcast::set_id<N>(); }
};

/*
  A->B,C
  B->D
  D->E,F,G
  E->H,I
  G->J,K,L,M,N
*/

int main()
{
    A a;
    B b;
    C c;
    D d;
    E e;
    F f;
    G g;
    H h;
    I i;
    J j;
    K k;
    L l;
    M m;
    N n;

    assert(a.Fcast::_fcast_id == 0b1);
    assert(b.Fcast::_fcast_id == 0b101);
    assert(c.Fcast::_fcast_id == 0b111);
    assert(d.Fcast::_fcast_id == 0b1101);
    assert(e.Fcast::_fcast_id == 0b1001101);
    assert(f.Fcast::_fcast_id == 0b1011101);
    assert(g.Fcast::_fcast_id == 0b1101101);
    assert(h.Fcast::_fcast_id == 0b101001101);
    assert(i.Fcast::_fcast_id == 0b111001101);
    assert(j.Fcast::_fcast_id == 0b10001101101);
    assert(k.Fcast::_fcast_id == 0b10011101101);
    assert(l.Fcast::_fcast_id == 0b10101101101);
    assert(m.Fcast::_fcast_id == 0b10111101101);
    assert(n.Fcast::_fcast_id == 0b11001101101);

    assert(Fcast::instanceof<A>(&a));
    assert(Fcast::instanceof<B>(&b));
    assert(Fcast::instanceof<C>(&c));
    assert(Fcast::instanceof<D>(&d));
    assert(Fcast::instanceof<M>(&m));
    assert(Fcast::instanceof<N>(&n));
    assert(Fcast::same<A>(&a));
    assert(Fcast::same<B>(&b));
    assert(Fcast::same<C>(&c));
    assert(Fcast::same<D>(&d));
    assert(Fcast::same<M>(&m));
    assert(Fcast::same<N>(&n));

    assert(Fcast::instanceof<B>(static_cast<A *>(&b)));
    assert(Fcast::instanceof<C>(static_cast<A *>(&c)));
    assert(Fcast::instanceof<D>(static_cast<A *>(&d)));
    assert(Fcast::instanceof<D>(static_cast<B *>(&d)));
    assert(Fcast::instanceof<D>(static_cast<B *>(&e)));
    assert(Fcast::instanceof<D>(static_cast<B *>(&f)));
    assert(Fcast::instanceof<D>(static_cast<B *>(&g)));
    assert(Fcast::instanceof<B>(static_cast<A *>(&e)));
    assert(Fcast::instanceof<B>(static_cast<A *>(&f)));
    assert(Fcast::instanceof<B>(static_cast<A *>(&g)));
    assert(!Fcast::instanceof<C>(static_cast<A *>(&e)));
    assert(!Fcast::instanceof<C>(static_cast<A *>(&f)));
    assert(!Fcast::instanceof<C>(static_cast<A *>(&g)));
    assert(Fcast::instanceof<B>(static_cast<D *>(&h)));
    assert(Fcast::instanceof<B>(static_cast<D *>(&i)));
    assert(Fcast::instanceof<E>(static_cast<A *>(&h)));
    assert(Fcast::instanceof<E>(static_cast<B *>(&i)));
    assert(Fcast::instanceof<H>(static_cast<A *>(&h)));
    assert(Fcast::instanceof<I>(static_cast<B *>(&i)));
    assert(Fcast::instanceof<D>(static_cast<A *>(&j)));
    assert(Fcast::instanceof<D>(static_cast<A *>(&k)));
    assert(Fcast::instanceof<D>(static_cast<A *>(&l)));
    assert(Fcast::instanceof<D>(static_cast<A *>(&m)));
    assert(Fcast::instanceof<D>(static_cast<A *>(&n)));
    assert(Fcast::instanceof<G>(static_cast<A *>(&j)));
    assert(Fcast::instanceof<G>(static_cast<A *>(&k)));
    assert(Fcast::instanceof<G>(static_cast<A *>(&l)));
    assert(Fcast::instanceof<G>(static_cast<A *>(&m)));
    assert(Fcast::instanceof<G>(static_cast<A *>(&n)));
    assert(!Fcast::instanceof<E>(static_cast<A *>(&j)));
    assert(!Fcast::instanceof<H>(static_cast<A *>(&k)));
    assert(!Fcast::instanceof<I>(static_cast<A *>(&l)));
    assert(!Fcast::instanceof<C>(static_cast<A *>(&m)));
    assert(!Fcast::instanceof<J>(static_cast<A *>(&n)));
    assert(!Fcast::instanceof<C>(static_cast<A *>(&h)));
    assert(!Fcast::instanceof<C>(static_cast<A *>(&i)));

    return 0;
}
