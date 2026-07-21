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

    // Read the opening header block directly off Byte 0 of the host storage stream
    Superblock test_sb;
    disk.read(reinterpret_cast<char*>(&test_sb), sizeof(Superblock));
    disk.close();

    // Cross-examine the signature token payload fields
    if (test_sb.magic != MAGIC_SIGNATURE) {
        std::cerr << "❌ Signature violation: Invalid or corrupted virtual disk architecture." << std::endl;
        return false;
    }

    // Initialize state attributes upon verification
    sb_ = test_sb;
    active_disk_path_ = host_path;
    is_mounted_ = true;

    std::cout << "✓ [Disk Mounted] Environment verification passed. Live Clusters Tracked: "
              << sb_.total_clusters << std::endl;
    return true;
}

} // namespace Purity
