# FASTCAST

Fastcast provides a fast alternative to dynamic_cast.
Each class in the hierarchy has an integer identifier. This identifier is built to make instanceof operation very fast.

# Usage:

Fastcast is easy to use:

1. include *fastcast.hxx*

2. the root class (say A) must inherit from ```*fastcast::fcast<A, uint64_t>*``` (or an other integer type)

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

4. Each ctor must set the identifier for the class:

   ```
   struct B : public A
   {
       typedef fastcast::hierarchy<A, fastcast::children<D>> fcast_hierarchy;
   
       B() { fastcast::fcast<A, uint32_t>::set_id<B>(); }
   };
   ```

5. the file test.cpp could be compiled in using:

   ```
   clang++ -std=c++11 -oout test/test.cpp -I. && ./out
   ```

   or

   ```
   g++ -std=c++11 -oout test/test.cpp -I. && ./out
   ```