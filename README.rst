libcoll -- a C Collections Library
===================================

``libcoll`` is a generic collections library for the C programming language.

This library aims to provide a simple and minimalistic API of generic
collections.

The library has been developed on Linux, although all of the code should be
compatible with other POSIX platforms.

Features
--------

The library currently supports the following collection types:

* treemap
* hashmap
* linked list (doubly-linked, with iterators)
* vector

General features:

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

The library itself has no external dependencies beyond the C standard library
and POSIX.

A variant of ``make`` is needed for building from source. At least GNU make
works; no other variants have been tested.

GCC is needed for compiling. Other C compilers should work but have not been
tested.

For building and running automated tests, the `Check`_ framework is required.

.. _Check: https://libcheck.github.io/check/

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

The library is a work in progress, and **neither the API nor the ABI are stable
at the moment**.
