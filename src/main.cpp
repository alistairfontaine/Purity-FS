#include "vfs.hpp"
#include <iostream>

int main() {
    std::cout << "🧬 ========================================== 🧬" << std::endl;
    std::cout << "🔥 PURITY-FS: Bedrock Storage Engine Initialization 🔥" << std::endl;
    std::cout << "🧬 ========================================== 🧬" << std::endl;

    Purity::VirtualFilesystem fs;
    std::string custom_disk = "sandbox.purity";

    // 1. Trigger the binary Block Allocation Table formatting pipeline layout
    // Format a 512-cluster container file (2MB sovereign storage block space)
    if (!fs.format_virtual_disk(custom_disk, 512)) {
        std::cerr << "❌ System Boot Stopped: Critical formatting fault." << std::endl;
        return 1;
    }

    std::cout << std::endl;

    // 2. Cross-examine the signature headers by mounting the brand new container file
    if (!fs.mount_virtual_disk(custom_disk)) {
        std::cerr << "❌ System Boot Stopped: Environment verification signature mismatch." << std::endl;
        return 1;
    }

    std::cout << std::endl;
    std::cout << "🚀 [Status Update] Milestone 1 successfully operational on your disk sectors!" << std::endl;
    std::cout << "🧬 ========================================== 🧬" << std::endl;
    return 0;
}
