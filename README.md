# stnc - Student Network Communication and Performance Testing Tool

## Overview

`stnc` is a versatile command-line tool for network communication and performance testing. It supports two modes:
- **Chat Mode:** Enables two-way communication over TCP.
- **Performance Test Mode:** Benchmarks various communication styles, including IPv4/IPv6, UNIX domain sockets, mmap, and pipes.

The tool is designed for efficient communication using the `poll/select` API.

## Features

### Chat Mode
- Simultaneous sending and receiving of messages over a TCP connection.
- Simple command-line interface for client and server communication.

### Performance Test Mode
- Generates a 100MB data chunk and calculates its checksum.
- Transmits data over various communication styles:
  - **IPv4:** TCP/UDP
  - **IPv6:** TCP/UDP
  - **Unix Domain Sockets:** Stream/Datagram
  - **mmap:** Memory-mapped file
  - **pipe:** Named pipe
- Measures and reports transmission time in milliseconds.

### Command Usage
- Displays usage instructions and handles invalid/missing parameters gracefully.
- Supports quiet mode for performance testing.

## Getting Started

### Prerequisites
- A Linux environment (Ubuntu or other distributions).
- GCC compiler.

### Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/username/stnc-tool.git
   cd stnc-tool
   ```

2. Build the project:
   ```bash
   make
   ```

### Usage

#### Chat Mode
- **Server:**
  ```bash
  ./stnc -s PORT
  ```
- **Client:**
  ```bash
  ./stnc -c IP PORT
  ```

#### Performance Test Mode
- **Server:**
  ```bash
  ./stnc -s PORT -p [-q]
  ```
  - `-p`: Enables performance testing.
  - `-q`: Quiet mode (only test results are printed).

- **Client:**
  ```bash
  ./stnc -c IP PORT -p <type> <param>
  ```
  - `<type>`: Communication style (e.g., ipv4, ipv6, uds, mmap, pipe).
  - `<param>`: Specific parameter (e.g., tcp/udp or file name).

#### Example Commands
- IPv4 TCP performance test:
  ```bash
  ./stnc -c 127.0.0.1 8080 -p ipv4 tcp
  ```

- UNIX Domain Socket stream test:
  ```bash
  ./stnc -c /tmp/socket 8080 -p uds stream
  ```

## Project Structure

- `stnc.c`: Main program implementation.
- `Makefile`: Build script with `make` and `make clean` targets.
- `README.md`: Documentation.

## Notes

- Ensure the server is running before initiating the client.
- Use Beej's Guide to IPC for reference during implementation.
- The tool is designed without threads for simplicity and compatibility.

## Contributing

Contributions are welcome! Fork the repository, create a branch, and submit a pull request.

## Acknowledgments

This project was developed as part of an academic assignment to practice advanced C++ programming concepts.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Author

**Halel Itzhaki**

For any questions or suggestions, please feel free to contact me.
