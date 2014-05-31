# FASTCAST

Fastcast provides a fast alternative to dynamic_cast.
Each class in the hierarchy has an integer identifier. This identifier is built to make instanceof operation very fast.
Cross casts are supported.

# Usage:

Fastcast is easy to use:

1. include *fastcast.hxx*

2. the root class (say A) must inherit from ```fastcast::fcast<A, uint64_t>``` (or an other integer type)

3. each class in the hierarchy having children must typedef a *fcast_hierarchy*:

   ```
   // typedef for the root class
   typedef fastcast::hierarchy<fastcast::root, fastcast::children<B, C>> fcast_hierarchy;
   
   // typedef for a class other than root
   // The first template parameter gives the parent
   // and the second define the children
   typedef fastcast::hierarchy<A, fastcast::children<D>> fcast_hierarchy;

   // here there is no child
   typedef fastcast::hierarchy<D> fcast_hierarchy;
   ```

4. Each constructor must set the identifier for the class (have a look at test/test.cpp):

   ```
   struct B : public A
   {
       typedef fastcast::hierarchy<A, fastcast::children<D>> fcast_hierarchy;
   
       B() { fastcast::fcast<A, uint32_t>::set_id<B>(); }
   };
   ```

5. Cross casts are possible in using fastcast::parents (have a look at test/test_cross.cpp):

   ```
   struct G : public C, public E
   {
       typedef fastcast::hierarchy<fastcast::parents<C, E>, fastcast::children<H>> fcast_hierarchy;

       G()
           {
               FcastA::set_id<G>();
               FcastD::set_id<G>();
           }
   };
   ```

6. the file test.cpp could be compiled in using:

   ```
   clang++ -Wall -std=c++11 -oout test/test.cpp -I. && ./out
   ```

   or

   ```
   g++ -Wall -std=c++11 -oout test/test.cpp -I. && ./out
   ```