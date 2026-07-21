#include "shell.hpp"
#include <iostream>
#include <sstream>
#include <vector>

namespace Purity {

void launch_interactive_shell(VirtualFilesystem& fs) {
    std::string line;
    std::cout << "\n🛡️ [Purity-FS Safe Shell Initialized] Type 'help' to review command matrices." << std::endl;

    while (true) {
        std::cout << "purity-vfs> ";
        if (!std::getline(std::cin, line)) break;

        if (line.empty()) continue;

        // Tokenize input string fragments cleanly using standard string stream arrays
        std::stringstream ss(line);
        std::string command;
        ss >> command;

        std::vector<std::string> args;
        std::string arg;
        while (ss >> arg) {
            args.push_back(arg);
        }

        // --- COMMAND ROUTING INDEX MATRIX ---
        if (command == "exit" || command == "quit") {
            std::cout << "👋 Exiting sovereign storage shell. Core offline." << std::endl;
            break;
        }
        else if (command == "help") {
            std::cout << "\n📋 Available Purity-FS Bare-Metal Primitives:" << std::endl;
            std::cout << "  format <disk_name> <clusters>  - Initializes a fresh encrypted storage file container" << std::endl;
            std::cout << "  mount  <disk_name>             - Boots a .purity filesystem into active memory cache" << std::endl;
            std::cout << "  ls                             - Lists all virtual file index entries inside root directory" << std::endl;
            std::cout << "  write  <filename> <text_data>  - Encrypts and allocates blocks on-the-fly to write content" << std::endl;
            std::cout << "  read   <filename>              - Slices and decrypts data blocks to output content" << std::endl;
            std::cout << "  exit / quit                    - Unmounts storage layers and safely shuts down shell\n" << std::endl;
        }
        else if (command == "format") {
            if (args.size() < 2) {
                std::cerr << "⚠️ Usage error: format <disk_name.purity> <desired_clusters>" << std::endl;
                continue;
            }
            uint32_t clusters = std::stoul(args[1]);
            fs.format_virtual_disk(args[0], clusters);
        }
        else if (command == "mount") {
            if (args.empty()) {
                std::cerr << "⚠️ Usage error: mount <disk_name.purity>" << std::endl;
                continue;
            }
            fs.mount_virtual_disk(args[0]);
        }
        else if (command == "ls") {
            fs.list_directory();
        }
        else if (command == "write") {
            if (args.size() < 2) {
                std::cerr << "⚠️ Usage error: write <filename> <string_data_payload>" << std::endl;
                continue;
            }
            std::string filename = args[0];

            // Re-aggregate remaining trailing tokens into a single text packet string array
            std::string text_payload = args[1];
            for (size_t i = 2; i < args.size(); ++i) {
                text_payload += " " + args[i];
            }

            std::vector<char> byte_buffer(text_payload.begin(), text_payload.end());
            if (fs.create_file(filename, byte_buffer)) {
                std::cout << "✓ Success: Encrypted file allocation committed securely to sector clusters!" << std::endl;
            }
        }
        else if (command == "read") {
            if (args.empty()) {
                std::cerr << "⚠️ Usage error: read <filename>" << std::endl;
                continue;
            }
            std::vector<char> retrieved_bytes;
            if (fs.read_file(args[0], retrieved_bytes)) {
                std::string decrypted_text(retrieved_bytes.begin(), retrieved_bytes.end());
                std::cout << "📖 [Decrypted Content Output]:\n" << decrypted_text << std::endl;
            }
        }
        else if (command == "import") {
            if (args.size() < 2) {
                std::cerr << "⚠️ Usage error: import <host_source_path> <vfs_destination_name>" << std::endl;
                continue;
            }
            fs.import_host_file(args[0], args[1]);
        }
        else if (command == "export") {
            if (args.size() < 2) {
                std::cerr << "⚠️ Usage error: export <vfs_source_name> <host_destination_path>" << std::endl;
                continue;
            }
            fs.export_to_host(args[0], args[1]);
        }
        else {
            std::cerr << "❌ Unknown primitive: Type 'help' to review command directory patterns." << std::endl;
        }
    }
}

} // namespace Purity
