/**
 * This program tries to consume all stack memory and on stack memory shortage 
 * signal handler is raised that prints the used stack size (at least manually 
 * allocated by means of alloca() call). 
 *
 * Man pages on alloca() mention undefined behavior on memory allocation failure
 * and hence this may rely on undefined behavior. :-)
 */

#include <iostream>
#include <cerrno>
#include <csignal>
#include <alloca.h>

static volatile long long sStackSize = sizeof sStackSize;

char gSignalStack[ SIGSTKSZ ];       //! Large enough to cover the usual requirements. 
char gSignalStackMin[ MINSIGSTKSZ ]; //! Minimum size required to execute a signal handler.

void segment_fault(int sig, siginfo_t *siginfo, void* context)
{
    printf("Signal: %d\n", sig);
    printf("Allocated on stack: %lld\n", sStackSize);
    fflush(stdout);

    // Hit stop condition and boil out. 
    exit(0);
}

int main(int argc, char* argv[])
{
    // Install a signal handler via sigaction() system call.
    // std::signal could not be used as need to specify to 
    // employ alternate stack.
    struct sigaction sigact; 
    memset(&sigact, '\0', sizeof sigact);
    // Execute on alternate signal stack and set signal handler that takes
    // three arguments.
    sigact.sa_flags = SA_ONSTACK | SA_SIGINFO;
    sigact.sa_sigaction = &segment_fault; 
    if (sigaction(SIGSEGV, &sigact, NULL) == 0) {
        std::cout << "Set sigaction for SIGSEGV signal.\n";
    }
    else {
        std::cerr << "sigaction failed with error: " << std::strerror(errno) << "\n";
    }

    std::cout << "Minimum signal stack size: " << MINSIGSTKSZ << "\n";
    std::cout << "Usual signal stack size:   " << SIGSTKSZ << "\n";
    std::cout << "\n";

    stack_t signal_stack_alt, signal_stack_cur;
    signal_stack_alt.ss_sp    = static_cast<void*>(&gSignalStack);
    signal_stack_alt.ss_size  = sizeof gSignalStack; 
    signal_stack_alt.ss_flags = 0;
    
    if (sigaltstack(&signal_stack_alt, &signal_stack_cur) == 0) {
        std::cout << "Set alternate signal stack.\n";
        std::cout << "-- Alternate signal stack [CURRENT] --\n";
        std::cout << "Flags: " 
                  << ((signal_stack_cur.ss_flags == SS_DISABLE) ? "DISABLED" : "ONSTACK" ) << "\n";
        std::cout << "Size: "  
                  << signal_stack_cur.ss_size << "\n";
        std::cout << "\n";
    }
    else {
        std::cerr << "sigaltstack failed with error: " << std::strerror(errno) << "\n";
    }

    // Retrieve and output current resource limits
    struct rlimit current;
    if (getrlimit(RLIMIT_STACK, &current) == 0)
    {
        std::cout << "-- RLIMIT_STACK --\n";
        if (current.rlim_cur == RLIM_INFINITY) {
            std::cout << "INFINITY denotes no limit.\n";
        }
        else {
            std::cout << "soft_limit: " << current.rlim_cur
                      << ", " << (current.rlim_cur + 1023) / 1024 << "\n";
            std::cout << "hard_limit: " << current.rlim_max
                      << ", " << (current.rlim_max + 1023) / 1024 << "\n";
        }
        std::cout << std::endl;
    }
    volatile char* data = nullptr;
    // Pull out memory from stack until signal received. :-)
    while (true) {
        data = static_cast<char*>(alloca(256));
    
        // There is no error indication if the stack frame cannot be extended.
        // (However, after a failed allocation, the program is likely to receive
        // a SIGSEGV signal if it attempts to access the unallocated space.)
        // Attempt to write to that location
        data[0] = 037;  // Year 2015 in octal: 03737
        data[1] = 037;
        data[3] = 00;
        data[255] = 00;
        sStackSize += 256;
    }
    // Expected to exit from sigaction handler.
    return 1;
}
