# 🌐 Web-Server

A simple HTTP/1.1 web server implemented in C++ from scratch as part of the **42 Network** curriculum. This project aims to deepen understanding of socket programming, networking, HTTP protocol, and system-level programming in a UNIX environment.

---

## 📚 Table of Contents

- [About](#about)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Examples](#examples)
- [Project Requirements](#project-requirements)
- [Credits](#credits)

---

## 📖 About

This project is a custom implementation of a web server inspired by real-world servers like Nginx or Apache. The goal was to learn:

- How HTTP works under the hood
- How to handle multiple simultaneous connections
- How to implement server configurations and routing
- How to properly manage resources and system calls

It adheres to the rules and constraints defined by the **webserv** project at **42**.

---

## ✨ Features

- Supports **HTTP/1.1**
- Handles `GET`, `POST`, and `DELETE` methods
- Virtual hosts based on configuration
- Custom error pages
- Autoindexing for directories
- File uploads via POST
- CGI support (PHP/Python/Bash)
- Non-blocking I/O using `epoll()`
- Basic multipart form-data handling
- Supports cookies and session management

---

## ⚙️ Installation

```bash
git clone https://github.com/Geesama02/Web-server
cd Web-server
make
```

> 💡 Requires a C++98-compliant compiler (e.g., `g++`) and a UNIX-based OS (macOS/Linux).

---

## 🚀 Usage

```bash
./webserv config.conf
```

The server will start using the configuration specified in the file. You can open your browser and navigate to:

```
http://localhost:8001
```

(Default port is configurable.)

---

## 🛠️ Configuration

The server is configured through a `.conf` file similar to Nginx.

---

## 🔍 Examples

- Serve static HTML files
- Run a CGI script via `/cgi-bin/example.py`
- Enable autoindexing for a specific route
- Handle file uploads via HTML form

---

## 📋 Project Requirements

✅ No use of external libraries for HTTP parsing  
✅ Only C++98 and POSIX system calls  
✅ One server process (no forking per request)  
✅ Handle multiple simultaneous connections  
✅ Graceful error handling and clean resource management

---

## 👥 Credits

Developed by:

- **[oait-laa](https://github.com/Geesama02)**
- **[maglagal](https://github.com/MAROUAN17)**

Project part of the **42 Network** curriculum.

---
