# samp-mongo

**samp-mongo** is an asynchronous MongoDB connector for San Andreas Multiplayer (SA:MP), written in C++17. It allows server developers to store and retrieve data using BSON documents without blocking the main server thread.

## ⚡ Features
* **Non-blocking I/O:** Database queries run on a separate thread pool.
* **Modern Driver:** Built using the official `mongocxx` driver.
* **Simple API:** Easy-to-use PAWN natives for connecting and querying.