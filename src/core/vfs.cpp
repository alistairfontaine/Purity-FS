#include "vfs.hpp"
#include <fstream>
#include <iostream>
#include <ctime>
#include <cstring>

namespace Purity {

VirtualFilesystem::VirtualFilesystem() : is_mounted_(false) {
    std::memset(&sb_, 0, sizeof(Superblock));
}

VirtualFilesystem::~VirtualFilesystem() {
    // Graceful release loop
}

/**
 * 🔨 THE OFFICIAL .PURITY FORMAT AUTOMATION ROUTINE 🔨
 * Instantiates a clean contiguous storage block file on the host machine disk space.
 * Compiles a packed header and initializes empty sector cluster fields.
 */
bool VirtualFilesystem::format_virtual_disk(const std::string& host_path, uint32_t desired_clusters) {
    std::cout << "🎛️ [Format Subsystem] Initializing clean binary mapping cluster arrays..." << std::endl;

    if (desired_clusters == 0) {
        std::cerr << "❌ Format violation: Cluster allocation bounds must exceed 0." << std::endl;
        return false;
    }

    // Force binary file write access permissions
    std::ofstream disk(host_path, std::ios::binary | std::ios::trunc);
    if (!disk.is_open()) {
        std::cerr << "❌ I/O Exception: Failed to open or allocate host file: " << host_path << std::endl;
        return false;
    }

    // 1. Populate the packed system parameters inside the superblock structure
    Superblock fresh_sb;
    fresh_sb.magic = MAGIC_SIGNATURE; // "PURT" standard token verification signature
    fresh_sb.total_clusters = desired_clusters;
    fresh_sb.bat_offset = sizeof(Superblock); // Block Allocation Table sits right after the header

    // Calculate the sizing profile of the BAT allocation map array matrix
    // Each entry maps onto a 4-byte cluster pointer key index
    uint32_t bat_size_bytes = desired_clusters * sizeof(uint32_t);
    fresh_sb.root_dir_offset = fresh_sb.bat_offset + bat_size_bytes;
    std::memset(fresh_sb.reserved, 0, sizeof(fresh_sb.reserved));


    // 2. Commit the physical signature block bytes directly to the start of the host file
    disk.write(reinterpret_cast<const char*>(&fresh_sb), sizeof(Superblock));

    // 3. Serialize empty allocation entries across the Block Allocation Table
    uint32_t empty_marker = 0xFFFFFFFF; // Hex marker signaling a free spatial sector block
    for (uint32_t i = 0; i < desired_clusters; ++i) {
        disk.write(reinterpret_cast<const char*>(&empty_marker), sizeof(uint32_t));
    }

    // 4. Fill out the remaining requested sector blocks with raw zero-padding bytes to secure space
    char zero_sector[CLUSTER_SIZE];
    std::memset(zero_sector, 0, CLUSTER_SIZE);

    for (uint32_t i = 0; i < desired_clusters; ++i) {
        disk.write(zero_sector, CLUSTER_SIZE);
    }

    disk.close();
    std::cout << "✓ [.purity Formatted] Sovereign disk container deployed successfully! Total Clusters: "
              << desired_clusters << " (" << (desired_clusters * CLUSTER_SIZE) / 1024 << " KB allocated)" << std::endl;
    return true;
}

/**
 * 🗺️ VIRTUAL DISK MOUNT ENGINE 🗺️
 * Parses an existing .purity file container, validates its system architecture headers,
 * and boots the virtual file environment securely into active memory space.
 */
bool VirtualFilesystem::mount_virtual_disk(const std::string& host_path) {
    std::cout << "📡 [Mount Engine] Connecting to sovereign .purity file ledger..." << std::endl;

    std::ifstream disk(host_path, std::ios::binary);
    if (!disk.is_open()) {
        std::cerr << "❌ Connection error: Could not trace target disk container file." << std::endl;
        return false;
    }

    Superblock test_sb;
    disk.read(reinterpret_cast<char*>(&test_sb), sizeof(Superblock));

    if (test_sb.magic != MAGIC_SIGNATURE) {
        std::cerr << "❌ Signature violation: Invalid or corrupted virtual disk architecture." << std::endl;
        disk.close();
        return false;
    }

    sb_ = test_sb;
    active_disk_path_ = host_path;
    is_mounted_ = true;

    std::cout << "✓ [Disk Mounted] Environment verification passed. Live Clusters Tracked: "
              << sb_.total_clusters << std::endl;

    // 🔒 FIXED: Keep stream open for metadata reading, close it gracefully inside the loader
    bool success = load_metadata_to_cache(disk);
    disk.close();
    return success;
}

bool VirtualFilesystem::load_metadata_to_cache(std::ifstream& disk) {
    if (!disk.is_open()) return false;

    disk.seekg(sb_.bat_offset, std::ios::beg);
    bat_cache_.resize(sb_.total_clusters);
    disk.read(reinterpret_cast<char*>(bat_cache_.data()), sb_.total_clusters * sizeof(uint32_t));

    disk.seekg(sb_.root_dir_offset, std::ios::beg);

    inode_table_.clear();
    // 🔒 FIXED: Explicitly declare a clean array buffer size of 32 to avoid stack overflows
    Inode entries_buffer[32];
    disk.read(reinterpret_cast<char*>(entries_buffer), 32 * sizeof(Inode));

    for (int i = 0; i < 32; ++i) {
        if (entries_buffer[i].inode_id != 0 && entries_buffer[i].inode_id != 0xFFFFFFFF) {
            inode_table_.push_back(entries_buffer[i]);
        }
    }
    return true;
}

/**
 * 🛠️ SEARCH UTILITY BLOCK PRIMITIVE 🛠️
 * Scans the in-memory BAT array to find the first un-allocated virtual space cluster.
 */
int32_t VirtualFilesystem::find_free_cluster() const {
    for (uint32_t i = 0; i < bat_cache_.size(); ++i) {
        if (bat_cache_[i] == 0xFFFFFFFF) return i; // Found free cluster index marker
    }
    return -1; // Storage container fully exhausted
}

/**
 * 💾 HARD DRIVE SECTOR WRITE-BACK ENGINE 💾
 * Serializes active in-memory cache configurations directly back onto host disk bytes.
 */
bool VirtualFilesystem::write_back_metadata() {
    std::ofstream disk(active_disk_path_, std::ios::binary | std::ios::in | std::ios::out);
    if (!disk.is_open()) return false;

    // Flush updated structural BAT allocations back to the system data boundaries
    disk.seekp(sb_.bat_offset, std::ios::beg);
    disk.write(reinterpret_cast<const char*>(bat_cache_.data()), bat_cache_.size() * sizeof(uint32_t));

    // Flush virtual entry tree inodes table straight into the directory map
    disk.seekp(sb_.root_dir_offset, std::ios::beg);
    for (size_t i = 0; i < 32; ++i) {
        if (i < inode_table_.size()) {
            disk.write(reinterpret_cast<const char*>(&inode_table_[i]), sizeof(Inode));

        } else {
            char empty_inode[sizeof(Inode)];
            std::memset(empty_inode, 0xFF, sizeof(Inode));
            disk.write(empty_inode, sizeof(Inode));
        }
    }
    disk.close();
    return true;
}

/**
 * 📝 FILE REGISTRATION INGRESS PRIMITIVE 📝
 * Allocates sectors, binds a packed data entry tracking node, and serializes raw content.
 */
bool VirtualFilesystem::create_file(const std::string& filename, const std::vector<char>& data) {
    if (!is_mounted_) return false;
    if (filename.length() >= MAX_FILENAME_LEN) return false;

    int32_t cluster_idx = find_free_cluster();
    if (cluster_idx == -1) {
        std::cerr << "❌ Spatial Exception: Purity allocation arrays fully saturated." << std::endl;
        return false;
    }

    // 1. Pack fresh structural parameters directly inside our custom metadata tree Inode
    Inode fresh_file;
    fresh_file.inode_id = inode_table_.size() + 1;
    std::strncpy(fresh_file.filename, filename.c_str(), MAX_FILENAME_LEN);
    fresh_file.size_bytes = data.size();
    fresh_file.first_cluster_index = cluster_idx;
    fresh_file.is_directory = 0; // Type identifier: Explicit file data block
    fresh_file.created_at = std::time(nullptr);

    // 2. Mark this cluster sector as fully occupied inside the BAT map cache index
    bat_cache_[cluster_idx] = 0xCCCCCCFF; // End-Of-File (EOF) tracking byte marker standard

    // 3. Force stream manipulation write to push the actual contents into the virtual cluster partition
    std::ofstream disk(active_disk_path_, std::ios::binary | std::ios::in | std::ios::out);
    if (!disk.is_open()) return false;

    // Calculate the absolute file position byte index location of this virtual cluster
    uint32_t cluster_file_position = sb_.root_dir_offset + (32 * sizeof(Inode)) + (cluster_idx * CLUSTER_SIZE);
    disk.seekp(cluster_file_position, std::ios::beg);

    if (!data.empty()) {
        // 🔒 MILESTONE 3 PROTECTION: Scramble data bytes on the fly before serialization
        std::vector<char> encrypted_payload = data;
        scramble_buffer(encrypted_payload.data(), encrypted_payload.size());
        disk.write(encrypted_payload.data(), encrypted_payload.size());
    }
    disk.close();

    // 4. Update the state table caches and write the records back to file bytes
    inode_table_.push_back(fresh_file);
    return write_back_metadata();
}

/**
 * 🔑 DEEP BEDROCK CRYPTOGRAPHIC BIT-WISE MASK LOOP 🔑
 * Scrambles input bytes using an repeating XOR matrix pattern derived from "PURITYFS".
 * Running the exact same byte buffer back through this loop automatically decrypts it.
 */
void VirtualFilesystem::scramble_buffer(char* buffer, size_t size) const {
    for (size_t i = 0; i < size; ++i) {
        buffer[i] ^= CRYPTO_KEY[i % 8]; // Bit-wise XOR encryption transaction pass
    }
}

/**
 * 🖨️ INTERACTIVE DIRECTORY PRINTER 🖨️
 * Reads the virtual index map array to list tracked parameters in a clean Unix style format.
 */
void VirtualFilesystem::list_directory() const {
    std::cout << "\n📂 [Purity Index View] Contents of root directory:" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "INODE\tTYPE\tSIZE (BYTES)\tCREATED\t\tNAME" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    if (inode_table_.empty()) {
        std::cout << "[Empty directory index layout]" << std::endl;
        return;
    }

    for (const auto& entry : inode_table_) {
        std::string type_str = entry.is_directory ? "DIR " : "FILE";
        std::cout << entry.inode_id << "\t"
                  << type_str << "\t"
                  << entry.size_bytes << "\t\t"
                  << entry.created_at << "\t"
                  << entry.filename << std::endl;
    }
    std::cout << "--------------------------------------------------------" << std::endl;
}

/**
 * 🔑 CRYPTOGRAPHIC FILE READ & DECRYPTION PRIMITIVE 🔑
 * Locates an Inode records mapping snapshot, reads the raw cluster bytes off the host disk,
 * and passes the payload through the symmetric scramble loop to decrypt it for free.
 */
bool VirtualFilesystem::read_file(const std::string& filename, std::vector<char>& out_data) {
    if (!is_mounted_) return false;

    // 1. Locate the target filename inside the in-memory inode map table cache array
    const Inode* target_inode = nullptr;
    for (const auto& entry : inode_table_) {
        if (std::string(entry.filename) == filename) {
            target_inode = &entry;
            break;
        }
    }

    if (!target_inode) {
        std::cerr << "❌ File System Exception: Path target not found: " << filename << std::endl;
        return false;
    }

    // 2. Open a direct input file stream to slice the binary segments off your drive partition
    std::ifstream disk(active_disk_path_, std::ios::binary);
    if (!disk.is_open()) return false;

    // 3. Compute the exact absolute byte index location matching the file's primary allocation cluster
    uint32_t cluster_file_position = sb_.root_dir_offset + (32 * sizeof(Inode)) + (target_inode->first_cluster_index * CLUSTER_SIZE);
    disk.seekg(cluster_file_position, std::ios::beg);

    // 4. Resize our data reception array layout and pull the raw scrambled bytes off disk sectors
    out_data.resize(target_inode->size_bytes);
    if (target_inode->size_bytes > 0) {
        disk.read(out_data.data(), target_inode->size_bytes);

        // 🔒 MILSETONE 3 SYMMETRIC PASS: Run the scrambled bytes through the XOR mask loop to decrypt them completely
        scramble_buffer(out_data.data(), out_data.size());
    }
    disk.close();
    return true;
}

} // namespace Purity

