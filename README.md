# luacpp
Lua CPP wrapper

## Build Status
[![<ORG_NAME>](https://circleci.com/gh/jordanvrtanoski/luacpp/tree/main.svg?style=shield)](https://circleci.com/gh/jordanvrtanoski/luacpp)
[![Coverage Status](https://coveralls.io/repos/github/jordanvrtanoski/luacpp/badge.svg?branch=main)](https://coveralls.io/github/jordanvrtanoski/luacpp?branch=main)
[![CodeQL](https://github.com/jordanvrtanoski/luacpp/actions/workflows/codeql-analysis.yml/badge.svg?branch=main)](https://github.com/jordanvrtanoski/luacpp/actions/workflows/codeql-analysis.yml)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/0f2b745e91164b2d8a573891ce1db457)](https://www.codacy.com/gh/jordanvrtanoski/luacpp/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=jordanvrtanoski/luacpp&amp;utm_campaign=Badge_Grade)

## Installing

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../Source
make -j 4
```

## Building documents

```
sudo apt-get install texlive-latex-base texlive-latex-recommended texlive-latex-extra graphviz

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../Source
make doc_doxygen

cd build/doc_doxygen/latex
make pdf
```

## Testing the memory management

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../Source
make -j 4

valgrind -v --run-cxx-freeres=yes --run-libc-freeres=yes ./hello
```

## Running the unit test

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../Source

make -j 4
make test 
```

## Running the coverage test

```
apt install gcovr
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Coverage ../Source

make -j 4
make coverage-cli
make coverage-html
```
