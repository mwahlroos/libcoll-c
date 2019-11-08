libcoll -- a C Collections Library
===================================

``libcoll`` is a generic collections library for the C programming language.

This library aims to provide a simple and minimalistic API of generic
collections.

The library has been developed on Linux, although all of the code should be
compatible with at least other POSIX platforms, and probably any platform
that provides a C standard library.

Features
--------

* Currently supported collection types:
  - treemap, hashmap, linked list
* List types are double-linked, and with iterators
* Arbitrary pointer types accepted as values
* Arbitrary pointer types accepted as keys for map-style collections

Building
--------

To build a shared object (.so) of the library, run ``make``.

To also run included unit tests, run ``make runtests``.

Type safety
-----------

The library provides **no explicit type safety**.

All functions that take keys or values take any pointer types as arguments.
All functions that return values return a void pointer that must be explicitly
cast to the desired type.

Dependencies
------------

None, other than the C standard library.

Other requirements
------------------

At the moment, **C99 support is required** for compiling the library.

Efficiency
----------

Basic care has been taken to make operations on the collections algorithmically
efficient. However, other implementations, such as those based heavily on the C
preprocessor, may be more efficient.

Comparisons of keys and values are done by calling (potentially customized)
comparator functions. The overhead of function calls makes this probably slower
than preprocessor-based libraries.

Further notes
-------------

To be honest, this is a hobby project. If you find it useful or educational in
any way, cool, but it was written mostly to teach myself a bit more C and to
explore the challenges of designing APIs without automatic memory management.

The library is a work in progress, and may see both API/ABI changes and added
collection types in the future.
