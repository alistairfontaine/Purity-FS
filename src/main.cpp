#include "vfs.hpp"
#include "shell.hpp"
#include <iostream>

int main() {
    std::cout << "🧬 ======================================================== 🧬" << std::endl;
    std::cout << "🔥 PURITY-FS: Bedrock Storage Engine & Isolated Shell Shell Core 🔥" << std::endl;
    std::cout << "🧬 ======================================================== 🧬" << std::endl;

    Purity::VirtualFilesystem fs;

    // Launch the interactive interpretation command parsing loop on bare metal
    Purity::launch_interactive_shell(fs);

    return 0;
}
