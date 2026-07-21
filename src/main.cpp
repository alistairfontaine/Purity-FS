#include "vfs.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "🧬 ========================================== 🧬" << std::endl;
    std::cout << "🔥 PURITY-FS: Bedrock Storage Engine Initialization 🔥" << std::endl;
    std::cout << "🧬 ========================================== 🧬" << std::endl;

    Purity::VirtualFilesystem fs;
    std::string custom_disk = "sandbox.purity";

    // 1. Format a clean 512-cluster custom container file format disk image
    if (!fs.format_virtual_disk(custom_disk, 512)) {
        std::cerr << "❌ System Boot Stopped: Critical formatting fault." << std::endl;
        return 1;
    }

    std::cout << std::endl;

    // 2. Mount the brand new container file to boot the tracking table state
    if (!fs.mount_virtual_disk(custom_disk)) {
        std::cerr << "❌ System Boot Stopped: Environment verification signature mismatch." << std::endl;
        return 1;
    }

    std::cout << std::endl;

    // 3. Create a real virtual file inside our independent binary cluster matrix
    std::string test_data = "Purity Filesystem bedrock data text packet string payload. Dependency-free storage achieved.";
    std::vector<char> byte_payload(test_data.begin(), test_data.end());

    std::cout << "📝 [Data Ingress] Registering virtual file 'genesis.txt' into .purity allocation sectors..." << std::endl;
    if (fs.create_file("genesis.txt", byte_payload)) {
        std::cout << "✓ File successfully written and serialized on-chain!" << std::endl;
    } else {
        std::cerr << "❌ Data Allocation Error: File registration aborted." << std::endl;
    }

    // 4. Print out the active virtual index tree directory schema map
    fs.list_directory();

    std::cout << std::endl;
    std::cout << "🚀 [Status Update] Milestone 2 fully operational on your local machine!" << std::endl;
    std::cout << "🧬 ========================================== 🧬" << std::endl;
    return 0;
}
