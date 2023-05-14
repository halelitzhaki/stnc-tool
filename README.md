# OS_Assignment_3

Written by:
1. Halel Itzhaki - 322989674
2. Ori Ekshtein - 213562069

This assignment include the stnc program which implements a cmd chat tool and a network communication performance tester.

## Installation

To install OS_Assignment_3, run on terminal - 
```bash
git clone https://github.com/halelitzhaki/OS_Assignment_3.git
```

## Usage

Run on terminal the following command:
```bash
make all
```

To use the chat tool, run the following commands in separate terminal windows:

First:
./stnc -s [PORT]

Second:
./stnc -c [IP] [PORT]

For example:
First: 
```bash
./stnc -s 9090
```
Second:
```bash
./stnc -c 127.0.0.1 9090
```


To use the network communication performance tester, run the following commands in separate terminal windows:

First:
./stnc -s [PORT] [flag] [flag]

Second:
./stnc -c [IP] [PORT] [flag] [type] [param]

For example:
First: 
```bash
./stnc -s 9090 -p -q
```
Second:
```bash
./stnc -c 127.0.0.1 9090 -p ipv6 tcp
```
