# RCOM Lab 2 — Data Link Layer Protocol

This project was developed for the **Computer Networks (RCOM)** course and focuses on the implementation and analysis of a **reliable data transmission protocol at the Data Link Layer**.

The main goal is to study how reliable communication can be achieved over an unreliable physical medium by implementing mechanisms such as framing, error detection, acknowledgements, and retransmissions.

---

## 📡 Project Overview

The project simulates a point-to-point communication system between two nodes, implementing a custom **Data Link Layer protocol** on top of a serial connection.

The system ensures reliable data transfer despite possible transmission errors by handling:

- Frame delimitation
- Error detection
- Flow control
- Retransmissions
- Connection setup and termination

---

## ⚙️ Key Features

- **Custom framing protocol**  
  Data is encapsulated into frames with headers and control fields.

- **Error detection**  
  Uses error detection mechanisms to identify corrupted frames.

- **Stop-and-Wait ARQ**  
  Ensures reliable delivery through acknowledgements and retransmissions.

- **Timeout handling**  
  Frames are retransmitted if acknowledgements are not received within a time limit.

- **Connection management**  
  Includes connection establishment and termination procedures.

---

## 📁 Repository Structure

- `clientTCP.c` — Main program (FTP client entry point)
- `ftp.c` — FTP protocol logic (control connection commands, passive mode, download)
- `getip.c` — Hostname to IP resolution utilities
- `aux.c`, `aux.h` — Helper functions (I/O, parsing, socket helpers, etc.)
- `instructions.md` — Lab instructions / notes
- `Makefile` — Build script

---

## ⚙️ What the Program Does

For an input URL like:

- `ftp://ftp.example.com/pub/file.txt`
- `ftp://user:password@ftp.example.com/pub/file.txt`

the program:

1. **Parses the FTP URL** (user, password, host, path, filename)
2. **Resolves the server address** (DNS → IP)
3. Opens a **TCP control connection** to the FTP server (port 21)
4. Authenticates using:
   - Provided `user:password`, or
   - Anonymous login if none is provided
5. Requests **passive mode (PASV)** and opens the **TCP data connection**
6. Sends `RETR` to retrieve the file and **downloads it locally**
7. Closes connections cleanly

---

## 🛠️ Technologies Used

- **C programming language**
- **POSIX serial communication**
- **Linux environment**
- **Makefile-based build system**

---

## 🔧 Build Instructions

From the project root, compile the project using:

```bash
make

## ▶️ Run Instructions

### Basic usage

```bash
./clientTCP ftp://<host>/<path>/<file>
```

Example:

```bash
./clientTCP ftp://ftp.up.pt/pub/file.txt
```

---

### With authentication

```bash
./clientTCP ftp://<user>:<password>@<host>/<path>/<file>
```

Example:

```bash
./clientTCP ftp://user:password@ftp.example.com/pub/file.txt
```

---

### Output

- The requested file is downloaded and saved in the current directory using the original filename.
- The program prints FTP server replies and progress information to the terminal.

---

### Notes

- The client establishes a **TCP control connection** to port 21 and a separate **data connection** using **passive mode (PASV)**.
- Some FTP servers may restrict anonymous access or block certain passive-mode ports depending on their configuration.
