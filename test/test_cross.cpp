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

using FcastA = fastcast::fcast<A, uint8_t>;
using FcastD = fastcast::fcast<D, uint8_t>;

struct A : public FcastA
{
    typedef fastcast::hierarchy<fastcast::root, fastcast::children<B, C>> fcast_hierarchy;
    A() { FcastA::set_id<A>(); }
};

struct B : public A
{
    typedef fastcast::hierarchy<A> fcast_hierarchy;
    B() { FcastA::set_id<B>(); }
};

struct C : public A
{
    typedef fastcast::hierarchy<A, fastcast::children<G>> fcast_hierarchy;
    C() { FcastA::set_id<C>(); }
};

struct D : public FcastD
{
    typedef fastcast::hierarchy<fastcast::root, fastcast::children<E, F>> fcast_hierarchy;
    D() { FcastD::set_id<D>(); }
};

struct E : public D
{
    typedef fastcast::hierarchy<D, fastcast::children<C>> fcast_hierarchy;
    E() { FcastD::set_id<E>(); }
};

struct F : public D
{
    typedef fastcast::hierarchy<D> fcast_hierarchy;
    F() { FcastD::set_id<F>(); }
};

struct G : public C, public E
{
    typedef fastcast::hierarchy<fastcast::parents<C, E>, fastcast::children<H>> fcast_hierarchy;

    G()
        {
            FcastA::set_id<G>();
            FcastD::set_id<G>();
        }
};

struct H : public G
{
    typedef fastcast::hierarchy<G> fcast_hierarchy;

    H()
        {
            FcastA::set_id<H>();
            FcastD::set_id<H>();
        }
};

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

    assert(FcastA::instanceof<B>(static_cast<A *>(&b)));
    assert(FcastA::instanceof<C>(static_cast<A *>(&c)));
    assert(FcastD::instanceof<E>(static_cast<D *>(&e)));
    assert(FcastD::instanceof<F>(static_cast<D *>(&f)));
    assert(FcastA::instanceof<G>(static_cast<A *>(&g)));
    assert(FcastD::instanceof<G>(static_cast<D *>(&g)));
    assert(FcastA::instanceof<G>(static_cast<A *>(&h)));
    assert(FcastD::instanceof<G>(static_cast<D *>(&h)));
    assert(FcastA::instanceof<H>(static_cast<A *>(&h)));
    assert(FcastD::instanceof<H>(static_cast<D *>(&h)));

    return 0;
}
