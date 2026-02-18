# FTP Client Implementation (TCP/IP – Application Layer)

## Overview

This project implements a fully functional FTP client in C using the POSIX/BSD sockets API.  
It establishes TCP connections to remote FTP servers, authenticates users, enters passive mode, and downloads files using the FTP protocol as defined in RFC 959.

The goal of the project was to gain a deep understanding of application-layer protocols, TCP/IP communication, and client-server interaction.

---

## Architecture

The client follows the standard FTP communication model:

- **Control Connection (TCP, port 21)**  
  Used for command exchange and server replies.

- **Data Connection (TCP, dynamic port via PASV)**  
  Used exclusively for file transfer.

This separation reflects the official FTP protocol design and required careful socket management.

---

## Implemented FTP Workflow

For a given URL such as:

```
ftp://user:password@host/path/file.txt
```

The client performs the following steps:

1. Parse the FTP URL (user, password, hostname, path, filename)
2. Resolve hostname to IP address (DNS resolution)
3. Establish TCP control connection to port 21
4. Receive and interpret the server’s `220` greeting
5. Send `USER` and `PASS` commands for authentication
6. Send `PASV` command and parse server response:
   - Extract `(h1,h2,h3,h4,p1,p2)`
   - Compute data port using: `port = p1 * 256 + p2`
7. Establish TCP data connection to the provided IP and port
8. Send `RETR` command to request file
9. Receive file data through the data socket
10. Close data connection
11. Send `QUIT` and close control connection

---

## Key Features

- Full FTP command handling (`USER`, `PASS`, `CWD`, `PASV`, `RETR`, `QUIT`)
- Passive mode implementation
- URL parsing with optional authentication
- DNS resolution
- FTP response code parsing (3-digit codes)
- Handling of multi-line server replies
- Robust error detection and reporting
- Clean connection termination

---

## Error Handling and Robustness

The client validates:

- Socket creation and connection errors
- Server reply codes (2xx, 3xx, 4xx, 5xx)
- Unexpected protocol states
- File I/O failures
- Network interruptions

If any step fails, the client exits gracefully with informative error messages.

---

## Repository Structure

- `clientTCP.c` — Entry point and main workflow
- `ftp.c` — FTP protocol implementation and command handling
- `getip.c` — DNS resolution utilities
- `aux.c`, `aux.h` — Helper functions (parsing, I/O, socket utilities)
- `Makefile` — Build configuration

---

## Technologies Used

- C programming language
- POSIX/BSD sockets API
- TCP/IP networking
- Linux environment
- Makefile build system

---

## Build Instructions

From the project root:

```bash
make
```

The executable will be generated in the project directory.

---

## Run Instructions

### Basic usage

```bash
./clientTCP ftp://<host>/<path>/<file>
```

### With authentication

```bash
./clientTCP ftp://<user>:<password>@<host>/<path>/<file>
```

Example:

```bash
./clientTCP ftp://ftp.up.pt/pub/file.txt
```

---

## Learning Outcomes

Through this project, the following concepts were explored in depth:

- Application-layer protocol implementation
- TCP client-server communication
- Control vs data channel separation
- FTP protocol state transitions
- Parsing structured protocol responses
- DNS resolution and socket management
- Robust network programming in C

---

## Possible Extensions

- Support for Active Mode (PORT)
- FTPS (TLS encryption)
- Resume interrupted downloads
- Interactive command-line interface
- Support for directory listing (LIST)

---

This project demonstrates practical understanding of Internet protocols, low-level network programming, and protocol-driven software design.
