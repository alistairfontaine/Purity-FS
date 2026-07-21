#ifndef SHELL_HPP
#define SHELL_HPP

#include "vfs.hpp"

namespace Purity {

/**
 * 💻 THE OFFICIAL INTERACTIVE TERMINAL SHELL LOOP 💻
 * Hooks into active standard I/O streams to spin an isolated CLI environment.
 * Maps user token arguments straight onto native filesystem functions.
 */
void launch_interactive_shell(VirtualFilesystem& fs);

} // namespace Purity

#endif // SHELL_HPP
