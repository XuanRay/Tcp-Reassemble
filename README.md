# TCP-Reassemble
## note: 
基于libpcap实现的一个解析pcap以及重组TCP的程序。

## Usage
```shell
    sudo chmod +x build.sh

    ./build.sh

    ./TCP-Reassembler absolutePath
```
Makefile in src directory.

By executing **Usage** code, can obtain a pcaps directory, which stores uni-flow in **pcap** format, and reqts directory, which stores tcp reassemble contents in **txt** format.

## Dependency
```c
    libpcap
```