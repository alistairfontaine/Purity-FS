#ifndef VFS_HPP
#define VFS_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace Purity {

// Enforce strict sector cluster geometry constraints off bedrock storage principles
constexpr uint32_t CLUSTER_SIZE = 4096;        // 4KB static sector allocation blocks
constexpr uint32_t MAGIC_SIGNATURE = 0x50555254; // ASCII hex representation for "PURT" (.purity standard)
constexpr uint32_t MAX_FILENAME_LEN = 64;

#pragma pack(push, 1) // Force the compiler to pack structs with zero padding byte leakage
/**
 * 📦 THE OFFICIAL .PURITY IMMUTABLE SUPERBLOCK HEADER 📦
 * Positioned at absolute Byte 0 of every virtual storage container file.
 * Governs the entire spatial allocation profile of the disk container space.
 */
struct Superblock {
    uint32_t magic;             // Must match MAGIC_SIGNATURE (0x50555254)
    uint32_t total_clusters;    // Dynamic allocation bounding size limit
    uint32_t bat_offset;        // Byte pointer marker offset to Block Allocation Table
    uint32_t root_dir_offset;   // Byte pointer marker offset to directory tree root
    uint8_t  reserved[32];      // Immutable padding tracks for alignment stability
};

/**
 * 🗂️ VIRTUAL FILE METADATA INODE ENTRY RECORD 🗂️
 * Represents a compressed index file mapping record layout inside the filesystem database.
 */
struct Inode {
    uint32_t inode_id;                        // Unique entry identification descriptor key
    char     filename[MAX_FILENAME_LEN];      // Fixed length string bounds to avoid heap allocs
    uint32_t size_bytes;                      // True raw size tracking of encapsulated bytes
    uint32_t first_cluster_index;             // Leading block lookup index pointer for file reads
    uint8_t  is_directory;                    // Boolean type discriminator flag (1=Dir, 0=File)
    uint64_t created_at;                      // Unix creation execution timestamp logging
};
#pragma pack(pop)

// Functional interfaces for orchestrating the binary disk streams
class VirtualFilesystem {
public:
    VirtualFilesystem();
    ~VirtualFilesystem();

    // Milestone 1 Core Primitives
    bool format_virtual_disk(const std::string& host_path, uint32_t desired_clusters);
    bool mount_virtual_disk(const std::string& host_path);

    // 📂 Milestone 2 Core Primitives (In-Memory Directory Indexing)
    bool create_file(const std::string& filename, const std::vector<char>& data);
    void list_directory() const;

    // 🔑 Milestone 3 Core Primitives (Direct Cryptographic Decryption read)
    bool read_file(const std::string& filename, std::vector<char>& out_data);

    // 🚀 VERSION 1.0 PRODUCTION PRIMITIVES (Import/Export Bridges & Chaining)
    bool import_host_file(const std::string& host_source_path, const std::string& vfs_dest_name);
    bool export_to_host(const std::string& vfs_source_name, const std::string& host_dest_path);



private:
    Superblock sb_;
    std::string active_disk_path_;
    bool is_mounted_;

    // 🔒 HIGH-PERFORMANCE ENTRY MATRIX: Runtime RAM cache of virtual filesystem records
    std::vector<uint32_t> bat_cache_;
    std::vector<Inode> inode_table_;

    // 🔑 Bedrock Cryptographic Signature Key layout configuration bounds
    static constexpr uint8_t CRYPTO_KEY[8] = { 0x50, 0x55, 0x52, 0x49, 0x54, 0x59, 0x46, 0x53 }; // "PURITYFS" bit mask

    // Internal structural helper routines
    int32_t find_free_cluster() const;
    bool write_back_metadata();
    bool load_metadata_to_cache(std::ifstream& disk);
    void scramble_buffer(char* buffer, size_t size) const; // 🔒 FIXED: Dependency-free bit-wise obfuscation loop
};



} // namespace Purity

#endif // VFS_HPP
