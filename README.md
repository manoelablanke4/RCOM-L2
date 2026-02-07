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

## 🧱 Project Structure

- `src/` – Source code implementing the protocol logic  
- `include/` – Header files  
- `docs/` – Project documentation and reports (if applicable)  
- `Makefile` – Build configuration  

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
