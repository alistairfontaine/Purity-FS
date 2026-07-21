# Purity-FS (v1.0.0 Stable Release)

<img src="assets/logo.png" alt="Purity-FS Logo" width="200" height="200" />

An advanced, high-performance, completely dependency-free Virtual Filesystem (VFS) and Cryptographic Storage Engine written from scratch in pure C++20.

The entire ecosystem compiles directly down to bare metal using only core C++ standard library headers (`<fstream>`, `<vector>`, `<memory>`, `<cstring>`), rendering it completely immune to third-party dependency breakage, licensing shifts, or hidden security vulnerabilities.

---

## ⚙️ Core Subsystem Architecture

*   **Sovereign File Container Allocation:** Encapsulates an entire virtual disk mapping state within a single native, packed `.purity` filesystem block image.
*   **Linked-List Cluster Chaining:** Dynamically splits large data arrays across non-contiguous 4096-byte hardware sectors, using an in-memory Block Allocation Table (BAT) pointer chain.
*   **Symmetric Bit-Wise Scrambling:** Hardens file bytes on-the-fly before hard disk sector writes using a custom, lightweight XOR bit mask matrix with zero computational overhead.
*   **Inter-Op Host Data Bridges:** Built-in streaming pipelines to smoothly `import` raw binary data from host operating system tracks and `export` them back onto physical local drive paths.
*   **Path-Aware Console Interpreter Shell:** An isolated terminal environment loop featuring dynamic path prefix filters to handle nested folder structures (`mkdir` and `cd`).

---

## 🛠️ Verification and Compilation Guide

### 📦 1. Clone the Sovereign Core Repository
```bash
git clone https://github.com
cd Purity-FS
```

### 🔨 2. Execute the Bare-Metal Build Script
```bash
make clean
make
```

### 🕹️ 3. Initialize the Safe Shell Environment
```bash
./purity-fs
```

---

## 💻 Native Terminal Prompt Command Reference

*   `format <disk_name.purity> <clusters>` - Partitions a clean static block memory array file.
*   `mount  <disk_name.purity>`            - Loads the custom system superblock signature into active memory tracking cache vectors.
*   `mkdir  <directory_name>`             - Allocates an isolated nested directory record inside the inode metadata tree.
*   `cd     <directory_name_or_..>`       - Traverses current working directory context paths dynamically.
*   `ls`                                  - Lists all files and sub-folders matching the active directory path prefix.
*   `write  <filename> <string_payload>`  - Encrypts and writes a string across chained sector blocks.
*   `read   <filename>`                   - Slices, decrypts, and outputs virtual file contents to the screen.
*   `import <host_path> <vfs_name>`       - Imports a physical file straight from the host OS partition into VFS sectors.
*   `export <vfs_name> <host_path>`       - Decrypts and exports a virtual file back onto your local computer disk.
*   `exit / quit`                         - Safely flushes volatile tables to disk bytes and terminates the shell.

---

## 📜 Open-Source Framework License
Purity-FS is fully open-source and free to distribute under the terms of the official MIT License contract structure.
