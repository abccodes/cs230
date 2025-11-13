##### Setup

- Docker installed (https://www.docker.com/products/docker-desktop)
- Git installed

##### Docker

- cd cs230/AFLplusplus
- docker run -ti -v $(pwd):/targets aflplusplus/aflplusplus:latest

##### Build and Run
- make clean
- make
- make install

- cd /targets

###### Compile with AFL++ for LLVM-IR (PCGUARD)
- afl-clang-fast program.c -o program

###### Run program
- ./program
