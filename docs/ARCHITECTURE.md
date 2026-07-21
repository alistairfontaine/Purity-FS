# Purity-FS Deep Technical Architecture Specification

This engineering manifest details the packed binary layout footprints, cluster pointer structures, and memory-safe mapping loops governing the Purity-FS v1.0.0 storage layer execution matrix.

---

## 1. Packed Binary Geometry Format (.purity File Standard)
Every sovereign virtual disk file anchors its baseline validation parameters starting directly at absolute Byte 0 using a highly compressed, zero-padded `Superblock` structure.

```cpp
#pragma pack(push, 1)
struct Superblock {
    uint32_t magic;             // Signature token field constant (0x50555254 or "PURT")
    uint32_t total_clusters;    // Total count threshold of allocated 4KB sectors
    uint32_t bat_offset;        // Byte location pointer offset to the Block Allocation Table
    uint32_t root_dir_offset;   // Byte location pointer offset to the entry mapping tree Inodes
    uint8_t  reserved[32];      // Hard alignment padding layer to guarantee cross-platform disk sector layout safety
};
#pragma pack(pop)
```

---

## 2. Linked-List Block Allocation Table (BAT) Chaining
Purity-FS removes flat spatial restrictions by designing the Block Allocation Table into a responsive cluster index pointer array vector.
*   **Free Sectors:** Marked uniformly with individual hex keys `0xFFFFFFFF`.
*   **Chained Blocks:** When a file demands space surpassing a standard 4KB sector size profile, the allocation loop registers multiple free indices, writing the exact address index of the next cluster block directly inside the active BAT pointer slot.
*   **End-Of-File (EOF):** The final block link in the sector chain receives the definitive `0xCCCCCCFF` marker token, cleanly terminating stream extraction loops on read cycles.

---

## 3. Symmetric Bit-Wise Mask Cryptography Subsystem
File data data packets undergo an on-the-fly, stream-level manipulation pass prior to serialization using an Exclusive OR (`^=`) byte logic loop.
*   **The Key Blueprint Matrix:** `0x50, 0x55, 0x52, 0x49, 0x54, 0x59, 0x46, 0x53` (ASCII hex values derived directly from `"PURITYFS"` string parameters).
*   **Symmetric Restoration:** Because bit-wise XOR functions are perfectly mathematical and symmetric, running a cryptographically masked buffer block back through the exact same loop structure instantly reverts the bit states back to their original human-readable configuration for free, completely bypassing the need for complex, heavy third-party SDK packages.

---

## 4. In-Memory Inode Trees & Path Prefix Isolation Filters
Directory structures support up to 32 explicit file allocations natively, with memory boundaries fully protected by isolated block array chunk buffers (`entries_buffer`) to eliminate stack overflow crashes entirely.

```cpp
struct Inode {
    uint32_t inode_id;                        // Unique entry pointer key index descriptor
    char     filename[64];                    // Fixed string parameter bounds (MAX_FILENAME_LEN)
    uint32_t size_bytes;                      // True raw size tracking of encapsulated bytes
    uint32_t first_cluster_index;             // Leading link index into the BAT sector array
    uint8_t  is_directory;                    // Type identifier flag (1 = Folder/DIR, 0 = File Data)
    uint64_t created_at;                      // Unix creation epoch execution timestamp logging
};
```

Instead of running slow nested directory tree sector scans on raw hardware blocks, navigation and nested folder tracking loops utilize a high-performance **In-Memory Path Prefix Filter** matrix.
When a sub-folder directory is instantiated, it appends its tracking string prefix natively onto the front of file names (e.g. `entries/document.txt`). Calling `ls` triggers a string lookup scanner that filters the cache memory tables to isolate and display *only* the records whose naming schemas directly match your active shell folder path context.
