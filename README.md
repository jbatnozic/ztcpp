# ZTCpp
User-friendly C++ wrapper over [libzt](https://github.com/zerotier/libzt)'s C interface.

Also available as [Conan](https://conan.io/) package.

Latest version: `2.0.0`

**Note:** ZTCpp does not currently expose all features that libzt offers. Feel free to make a feature request
or a PR if you need something that's missing.

### Why ZTCpp?
While `libzt` is a great and capable tool, its C-based API is very low-level and thus not always the most comfortable to use.
In some places it can be confusing if you're not very familiar with the [Berkeley socket model](https://en.wikipedia.org/wiki/Berkeley_sockets).

# License
MIT (see [LICENSE](https://github.com/jbatnozic/ztcpp/blob/master/LICENSE)).
The license applies ONLY to files in this repository, and NOT to the library being wrapped (libzt).

# Instructions

## Getting the package
The package artifacts are not stored in any global repository, so you will have to build the package locally yourself.

To do that, download the repository, and in it run the following command from your terminal:

```
conan create . @<user>/<channel> --profile=<profile> --build=outdated -s compiler.cppstd=<std>
```

You can choose the `user` and `channel` arbitrarily, however, for compatibility with other Conan packages created by 
me, I recommend that you use user `jbatnozic` and channel `stable`.

For `profile`, use `default` or one of your own.

ZTCpp is supported for C++ standards C++17 and newer, so put at least `17` as `<std>` above. This setting can be skipped
if it is included in the profile you're using.

**IMPORTANT:** ZTCpp has a dependency on [libzt](https://github.com/jbatnozic/libzt-conan), so make sure to get that first!

## Consuming the package
Consumed through a conanfile (.txt or .py) like any other Conan package. Supports both `build-type`s (Debug and Release) as you'd expect.

The only supported options are `fPIC` (except on Windows) and `shared`. Both are boolean (True/False). Note: due to a defect in the CMake files, builds with `ztcpp:shared=False` will not work for now.

## Building without Conan
While building without Conan is not currently officially supported, it is possible, but requires some modifications to the CMake files:
remove all Conan-related stuff, and add appropriate commands so it can find libzt headers and libraries.

# Usage
Just `#include <ZTCpp.hpp>` and you're good to go. I also recommend making a namespace alias, for example `namespace zt = jbatnozic::ztcpp;`.

While all APIs have doc comments, you can also take a look at:
- [ZeroTier documentation](https://github.com/zerotier/libzt/blob/master/include/README.md), for information about ZeroTier semantics
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/), for information about socket semantics in general
- [Examples in this repository](https://github.com/jbatnozic/ztcpp/tree/master/Examples/Source), for examples of usage

# Also see
[libzt-conan](https://github.com/jbatnozic/libzt-conan), a Conan package for the ZeroTier SDK.