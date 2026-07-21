# Purity-FS — Software Engineering Release Changelog

All core development tiers are fully complete, verified, stable, and frozen on the master branch tracks with zero outstanding errors or compiler flags.

---

## ✅ v1.0.0 Production Release (Current Stable Build) — 100% COMPLETE
- **Linked-List Cluster Chaining:** Completely overhauled the Block Allocation Table (BAT) layer to track cluster list node pointer chains, allowing files to expand seamlessly across infinite non-contiguous sectors.
- **Inter-Op Data Ingress/Egress Bridges:** Programmed robust host system binary file interaction utilities (`import` and `export`) to pass files directly back and forth between Linux and the `.purity` image container.
- **Nested Path Isolation Navigation Shell:** Expanded the shell loop prompt to support dynamic folder creation (`mkdir`) and traversal navigation (`cd`) using a runtime path prefix filter matrix.
- **Stack Smashing Protection:** Refined file reading streams to ingest metadata straight into pre-allocated memory block array buffers, completely fortifying the system registers against stack overflow crashes.

---

## ✅ v0.2.0 Core Architecture Milestone — 100% COMPLETE
- **In-Memory Directory Inodes:** Built real-time cache vector arrays to read and hold active packed file information structures in RAM for high-speed tracking on boot.
- **Symmetric Bit-Wise Mask Cryptography:** Integrated a zero-dependency bit-shifting XOR mask scrambler derived from the string `"PURITYFS"` to encrypt data on-the-fly.
- **Unix-Style Directory Listings:** Deployed the interactive directory printer tool (`ls`) to read and output structured layout variables like size, name, and timestamps.

---

## ✅ v0.1.0 Storage Bedrock Milestone — 100% COMPLETE
- **Packed Superblock Signature Header:** Designed packed binary structures containing the magic token verification key `0x50555254` to act as our format signature.
- **Binary Format Automation Loop:** Programmed sector file allocation sweeps to write clean empty formats across raw hard disk storage blocks.
- **Stream Descriptor Mount Engine:** Engineered input file validation streams to read, evaluate, and verify `.purity` container headers safely.
