#include "vfs.hpp"
#include <fstream>
#include <iostream>
#include <ctime>
#include <cstring>

namespace Purity {

VirtualFilesystem::VirtualFilesystem() : is_mounted_(false), current_working_directory_("/") {
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
    Inode entries_buffer[MAX_DIR_ENTRIES];
    disk.read(reinterpret_cast<char*>(entries_buffer), MAX_DIR_ENTRIES * sizeof(Inode));

    for (uint32_t i = 0; i < MAX_DIR_ENTRIES; ++i) {
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
    for (uint32_t i = 0; i < MAX_DIR_ENTRIES; ++i) {
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

    // Reject before allocating anything if the fixed-size root directory is full.
    // Otherwise we'd consume clusters and return true, but write_back_metadata only
    // persists the first MAX_DIR_ENTRIES inodes, silently losing the file (and leaking
    // its clusters) on the next mount.
    if (inode_table_.size() >= MAX_DIR_ENTRIES) {
        std::cerr << "\xE2\x9D\x8C Directory Full: root directory is limited to "
                  << MAX_DIR_ENTRIES << " entries." << std::endl;
        return false;
    }

    // Calculate how many 4KB clusters this file actually needs to occupy
    size_t bytes_left = data.size();
    uint32_t clusters_needed = (bytes_left + CLUSTER_SIZE - 1) / CLUSTER_SIZE;
    if (clusters_needed == 0) clusters_needed = 1; // Always allocate at least 1 block

    std::vector<uint32_t> allocated_chain;

    // 1. Scan the active BAT map to secure all required cluster slots upfront
    for (uint32_t i = 0; i < sb_.total_clusters; ++i) {
        if (bat_cache_[i] == 0xFFFFFFFF) {
            allocated_chain.push_back(i);
            if (allocated_chain.size() == clusters_needed) break;
        }
    }

    if (allocated_chain.size() < clusters_needed) {
        std::cerr << "❌ Spatial Exception: Not enough free cluster chains available inside .purity disk." << std::endl;
        return false;
    }

    // 2. Set up our Inode record parameters pointing to the first block in the chain
    Inode fresh_file;
    fresh_file.inode_id = inode_table_.size() + 1;
    std::strncpy(fresh_file.filename, filename.c_str(), MAX_FILENAME_LEN - 1);
fresh_file.filename[MAX_FILENAME_LEN - 1] = '\0';
    fresh_file.size_bytes = data.size();
    fresh_file.first_cluster_index = allocated_chain[0];
    fresh_file.is_directory = 0;
    fresh_file.created_at = std::time(nullptr);

    // 3. Open our binary disk stream to partition out the data blocks
    std::ofstream disk(active_disk_path_, std::ios::binary | std::ios::in | std::ios::out);
    if (!disk.is_open()) return false;

    const char* data_ptr = data.data();

    // 4. Slice data across the chained clusters and update the local BAT index
    for (size_t i = 0; i < allocated_chain.size(); ++i) {
        uint32_t curr_cluster = allocated_chain[i];

        // Map the linked-list pointers inside the Block Allocation Table
        if (i == allocated_chain.size() - 1) {
            bat_cache_[curr_cluster] = 0xCCCCCCFF; // EOF tracking byte marker token
        } else {
            bat_cache_[curr_cluster] = allocated_chain[i + 1]; // Points to next sequential sector block
        }

        // Compute the absolute position of the target sector cluster block on disk
        uint32_t cluster_file_position = sb_.root_dir_offset + (32 * sizeof(Inode)) + (curr_cluster * CLUSTER_SIZE);
        disk.seekp(cluster_file_position, std::ios::beg);

        size_t write_size = (bytes_left > CLUSTER_SIZE) ? CLUSTER_SIZE : bytes_left;
        if (write_size > 0) {
            std::vector<char> encrypted_block(data_ptr, data_ptr + write_size);
            scramble_buffer(encrypted_block.data(), encrypted_block.size());
            disk.write(encrypted_block.data(), write_size);
            data_ptr += write_size;
            bytes_left -= write_size;
        }
    }
    disk.close();

    inode_table_.push_back(fresh_file);
    return write_back_metadata();
}

void VirtualFilesystem::scramble_buffer(char* buffer, size_t size) const {
    for (size_t i = 0; i < size; ++i) {
        buffer[i] ^= CRYPTO_KEY[i % 8];
    }
}

void VirtualFilesystem::list_directory() const {
    std::cout << "\n📂 [Purity Index View] Contents of directory: " << current_working_directory_ << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "INODE\tTYPE\tSIZE (BYTES)\tCREATED\t\tNAME" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    uint32_t listed_count = 0;
    for (const auto& entry : inode_table_) {
        std::string full_name(entry.filename);

        // 🔒 DIRECTORY PREFIX FILTER: Determine if item sits inside active path tree
        bool matches_dir = false;
        if (current_working_directory_ == "/") {
            // Root checks: file cannot contain nested forward slashes
            matches_dir = (full_name.find_first_of('/') == std::string::npos);
        } else {
            // Nested checks: item must append directly onto the active path prefix string
            std::string prefix = current_working_directory_.substr(1) + "/";
            if (full_name.find(prefix) == 0) {
                std::string remainder = full_name.substr(prefix.length());
                matches_dir = (remainder.find_first_of('/') == std::string::npos);
            }
        }

        if (matches_dir) {
            listed_count++;
            std::string type_str = entry.is_directory ? "DIR " : "FILE";

            // Clean filename text string mapping to remove upper path components during viewing
            std::string clean_display_name = full_name;
            size_t last_slash = full_name.find_last_of('/');
            if (last_slash != std::string::npos) {
                clean_display_name = full_name.substr(last_slash + 1);
            }

            std::cout << entry.inode_id << "\t"
                      << type_str << "\t"
                      << entry.size_bytes << "\t\t"
                      << entry.created_at << "\t"
                      << clean_display_name << std::endl;
        }
    }

    if (listed_count == 0) {
        std::cout << "[Empty folder directory container]" << std::endl;
    }
    std::cout << "--------------------------------------------------------" << std::endl;
}


bool VirtualFilesystem::read_file(const std::string& filename, std::vector<char>& out_data) {
    if (!is_mounted_) return false;

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

    std::ifstream disk(active_disk_path_, std::ios::binary);
    if (!disk.is_open()) return false;

    out_data.clear();
    out_data.reserve(target_inode->size_bytes);

    uint32_t curr_cluster = target_inode->first_cluster_index;
    size_t total_bytes_to_read = target_inode->size_bytes;

    // 🔒 TRAVERSE THE LINKED-LIST BAT CHAIN SECURELY ON READ LOOPS
    while (curr_cluster != 0xCCCCCCFF && curr_cluster < bat_cache_.size() && total_bytes_to_read > 0) {
        uint32_t cluster_file_position = sb_.root_dir_offset + (32 * sizeof(Inode)) + (curr_cluster * CLUSTER_SIZE);
        disk.seekg(cluster_file_position, std::ios::beg);

        size_t read_size = (total_bytes_to_read > CLUSTER_SIZE) ? CLUSTER_SIZE : total_bytes_to_read;
        std::vector<char> cluster_buffer(read_size);

        disk.read(cluster_buffer.data(), read_size);
        scramble_buffer(cluster_buffer.data(), cluster_buffer.size());

        out_data.insert(out_data.end(), cluster_buffer.begin(), cluster_buffer.end());

        total_bytes_to_read -= read_size;
        curr_cluster = bat_cache_[curr_cluster]; // Jump to next node pointer block index
    }

    disk.close();
    return true;
}

/**
 * 📥 FEATURE 2: NATIVE INTER-OP IMPORT BRIDGE 📥
 * Extracts raw binary bytes off a real file sitting on your linux disk,
 * and pushes the payload straight through our cryptographically chained VFS sectors.
 */
bool VirtualFilesystem::import_host_file(const std::string& host_source_path, const std::string& vfs_dest_name) {
    std::ifstream host_file(host_source_path, std::ios::binary | std::ios::ate);
    if (!host_file.is_open()) {
        std::cerr << "❌ Import error: Failed to reach host operating file target." << std::endl;
        return false;
    }

    std::streamsize size = host_file.tellg();
    host_file.seekg(0, std::ios::beg);

    std::vector<char> file_buffer(size);
    if (size > 0) {
        host_file.read(file_buffer.data(), size);
    }
    host_file.close();

    std::cout << "📥 [Import Bridge] Slicing and writing " << size << " bytes into VFS container..." << std::endl;
    return create_file(vfs_dest_name, file_buffer);
}

/**
 * 📤 FEATURE 2: NATIVE INTER-OP EXPORT BRIDGE 📤
 * Traverses our virtual block chains, unscrambles the bytes on the fly,
 * and writes a real, fully accessible file back onto your local PC hard drive.
 */
bool VirtualFilesystem::export_to_host(const std::string& vfs_source_name, const std::string& host_dest_path) {
    std::vector<char> data_buffer;
    if (!read_file(vfs_source_name, data_buffer)) {
        std::cerr << "❌ Export error: Target file extraction failed inside VFS." << std::endl;
        return false;
    }

    std::ofstream host_file(host_dest_path, std::ios::binary | std::ios::trunc);
    if (!host_file.is_open()) {
        std::cerr << "❌ Export error: Failed to instantiate target file container on host drive." << std::endl;
        return false;
    }

    if (!data_buffer.empty()) {
        host_file.write(data_buffer.data(), data_buffer.size());
    }
    host_file.close();

    std::cout << "📤 [Export Bridge] Extraction success! File exported straight onto your machine disk layout at: " << host_dest_path << std::endl;
    return true;
}

/**
 * 📂 FEATURE 3: VIRTUAL DIRECTORY ALLOCATION ALLOCATOR 📂
 * Registers a new packed directory record mapping inside the Inode metadata tree.
 */
bool VirtualFilesystem::create_directory(const std::string& dirname) {
    if (!is_mounted_ || dirname.empty()) return false;

    // Build absolute path string token dependencies based on the active path layer
    std::string absolute_dir_path = (current_working_directory_ == "/")
        ? dirname
        : (current_working_directory_.substr(1) + "/" + dirname);

    if (absolute_dir_path.length() >= MAX_FILENAME_LEN) return false;

    // Same fixed-directory capacity guard as create_file (avoid silent loss on remount).
    if (inode_table_.size() >= MAX_DIR_ENTRIES) {
        std::cerr << "\xE2\x9D\x8C Directory Full: root directory is limited to "
                  << MAX_DIR_ENTRIES << " entries." << std::endl;
        return false;
    }

    int32_t cluster_idx = find_free_cluster();
    if (cluster_idx == -1) return false;

    Inode fresh_dir;
    fresh_dir.inode_id = inode_table_.size() + 1;
    std::strncpy(fresh_dir.filename, absolute_dir_path.c_str(), MAX_FILENAME_LEN - 1);
fresh_dir.filename[MAX_FILENAME_LEN - 1] = '\0';
    fresh_dir.size_bytes = 0; // Folder directory metadata structures occupy 0 file data bytes
    fresh_dir.first_cluster_index = cluster_idx;
    fresh_dir.is_directory = 1; // 🔒 DISCRIMINATOR KEY FLAG MARKED AS DIRECTORY LAYER
    fresh_dir.created_at = std::time(nullptr);

    // Secure sector mapping tracks inside the active block table map cache
    bat_cache_[cluster_idx] = 0xCCCCCCFF;

    inode_table_.push_back(fresh_dir);
    return write_back_metadata();
}

/**
 * 🚶 FEATURE 3: DYNAMIC PATH TRAVERSER NAVIGATION ENGINE 🚶
 * Updates the runtime state parameters to jump across virtual directory levels.
 */
bool VirtualFilesystem::change_directory(const std::string& target_path) {
    if (!is_mounted_) return false;

    if (target_path == "..") {
        if (current_working_directory_ == "/") return true; // Already anchored at system root
        size_t last_slash = current_working_directory_.find_last_of('/');
        if (last_slash == 0) {
            current_working_directory_ = "/";
        } else {
            current_working_directory_ = current_working_directory_.substr(0, last_slash);
        }
        return true;
    }

    if (target_path == "/") {
        current_working_directory_ = "/";
        return true;
    }

    // Traverse into a nested folder sector block
    std::string look_for = (current_working_directory_ == "/")
        ? target_path
        : (current_working_directory_.substr(1) + "/" + target_path);

    // Cross-examine records to prove that this folder directory genuinely exists on-chain
    bool found = false;
    for (const auto& entry : inode_table_) {
        if (std::string(entry.filename) == look_for && entry.is_directory == 1) {
            found = true;
            break;
        }
    }

    if (!found) {
        std::cerr << "❌ Path Exception: Directory folder not found: " << target_path << std::endl;
        return false;
    }

    current_working_directory_ = (current_working_directory_ == "/") ? ("/" + target_path) : (current_working_directory_ + "/" + target_path);
    return true;
}

} // namespace Purity

