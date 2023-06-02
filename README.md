# lserializing

[![CI](https://github.com/benthevining/lserializing/actions/workflows/ci.yml/badge.svg)](https://github.com/benthevining/lserializing/actions/workflows/ci.yml) [![Build docs](https://github.com/benthevining/lserializing/actions/workflows/docs.yml/badge.svg)](https://github.com/benthevining/lserializing/actions/workflows/docs.yml) [![pre-commit.ci status](https://results.pre-commit.ci/badge/github/benthevining/lserializing/main.svg)](https://results.pre-commit.ci/latest/github/benthevining/lserializing/main)

C++ serializing library

## Features

This library provides a format agnostic way to represent structured data, and classes to convert this data
structure to and from various representations, such as JSON, XML, etc.

### Supported serialization formats

* JSON
* XML
* YAML
* TOML
* INI

Contributing additional serialization format backends is highly valued, please open PRs if you would like
additional formats supported!

## Portability

This library is tested on Mac, Windows, and Linux (with GCC, Clang, and MSVC), as well as cross-compiled
for iOS, tvOS, watchOS, and Emscripten (WebAssembly).

## Building

This library is built with CMake. CMake presets are provided for each of the toolchains we target. This
library supports being added to other CMake projects via `find_package()`, `FetchContent`, or a plain
`add_subdirectory()`. In all cases, you should link against the target `limes::lserializing` and include
the main header this way:
```cpp
#include <lserializing/lserializing.h>
```
All of this library's headers can be individually included, but including `lserializing.h` is the easiest
way to bring in the entire library.

## Links

[CDash testing dashboard](https://my.cdash.org/index.php?project=lserializing)

[Documentation](https://benthevining.github.io/lserializing/)
