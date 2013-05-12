/// \file TripleLinux.h
///
/// \author Octavian Mihai Vasilovici
/// \version 1.0
/// \date 15/04/2013
/// Revision History :
///

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>

#ifndef TRIPLELINUX_H
#define TRIPLELINUX_H

class TripleLinux
{

public:

    /// @brief default ctor
    //TripleLinux();

    /// @brief default dtor
    //~TripleLinux();

    /// @brief Stores the original bytes
    void getData(
                pid_t _PID,
                long _address,
                char *_string,
                int _length);

    /// @brief Write the codecave bytes in the memory space
    void putData(
                pid_t _PID,
                long _address,
                const char *_string,
                int _length
                );

    /// @brief Return the proccess number for the set proccess name
    int pidOf(char *_processName);

    /// @brief Return the base address of the process
    long getPidBaseAddress(pid_t _PID);

    /// @brief Returns the first available memory address for code allocation
    long getFreeAllocationSpace(pid_t _PID);

    /// @brief Returns the bytes from the number
    std::string getBytes(long _longNumber);

    /// @brief Virtual function where what/how/when the bytes will be written in the process target space
    /// @brief This function will be implemented per plugin
    /// @brief Each plugin will have a class derived from TripleLinux class
    virtual void enable();

    /// @brief Virtual function that restores the original code in the target process.
    virtual void disable();

    /// @brief Virtual function that will write information in the target process at a specific interval.
    virtual void continual();

    static void* doit(void *PTR)
    {
        ((TripleLinux*) PTR)->continual(); // segmentation fault on virtual functions
        return NULL;
    }


protected:
    /// @brief Traced Process PID
    pid_t m_executable;
    /// @brief Traced Process registers before and after modification
    struct user_regs_struct oldregs, regs;
};

#endif // TRIPLELINUX_H
