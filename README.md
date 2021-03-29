# luacpp
Lua CPP wrapper

## Build Status
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/100ac66e05684bc4b4ceb26781fb9166)](https://app.codacy.com/gh/jordanvrtanoski/luacpp?utm_source=github.com&utm_medium=referral&utm_content=jordanvrtanoski/luacpp&utm_campaign=Badge_Grade_Settings)
[![<ORG_NAME>](https://circleci.com/gh/jordanvrtanoski/luacpp/tree/main.svg?style=shield)](https://circleci.com/gh/jordanvrtanoski/luacpp)
[![Coverage Status](https://coveralls.io/repos/github/jordanvrtanoski/luacpp/badge.svg?branch=main)](https://coveralls.io/github/jordanvrtanoski/luacpp?branch=main)
[![CodeQL](https://github.com/jordanvrtanoski/luacpp/actions/workflows/codeql-analysis.yml/badge.svg?branch=main)](https://github.com/jordanvrtanoski/luacpp/actions/workflows/codeql-analysis.yml)

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
