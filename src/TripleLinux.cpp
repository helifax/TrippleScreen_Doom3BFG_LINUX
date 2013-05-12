#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <include/TripleLinux.h>

const int long_size = sizeof(long);

void TripleLinux::getData(
                         pid_t _PID,
                         long _address,
                         char *_string,
                         int _length)
{
    char *laddr;
    int i, j;
    union u
    {
        long val;
        char chars[long_size];
     }data;

     i = 0;
     j = _length / long_size;
     laddr = _string;

     while(i < j)
     {
         data.val = ptrace(PTRACE_PEEKDATA, _PID, _address + i * 4, NULL);
         memcpy(laddr, data.chars, long_size);
         ++i;
         laddr += long_size;
     }

     j = _length % long_size;

     if(j != 0)
     {
        data.val = ptrace(PTRACE_PEEKDATA, _PID, _address + i * 4, NULL);
        memcpy(laddr, data.chars, j);
     }

     _string[_length] = '\0';
}
//----------------------------------------------------------------------------------------------------------------------

void TripleLinux::putData(
                          pid_t _PID,
                          long _address,
                          const char *_string,
                          int _length
                          )
{

    const char *laddr;
    int i, j;
    union u
    {
        long val;
        char chars[long_size];
     }data;

     i = 0;
     j = _length / long_size;
     laddr = _string;

     while(i < j)
     {
         memcpy(data.chars, laddr, long_size);
         ptrace(PTRACE_POKEDATA, _PID, _address + i * 4, data.val);
         ++i;
         laddr += long_size;
     }

     j = _length % long_size;

     if(j != 0)
     {
         memcpy(data.chars, laddr, j);
         ptrace(PTRACE_POKEDATA, _PID, _address + i * 4, data.val);
     }

}
//----------------------------------------------------------------------------------------------------------------------

int TripleLinux::pidOf(char *_processName)
{
    char line[20]; //line containing the output of the cli command
    std::string command = "pidof ";
    command.append(_processName);

    FILE *cmd = popen(command.c_str(), "r"); //issue the command as read only
    fgets(line, 20, cmd);
    pid_t pid = strtoul(line, NULL, 10); //retrieve the PID number

    pclose(cmd);
    return pid;
}
//----------------------------------------------------------------------------------------------------------------------

long TripleLinux::getPidBaseAddress(pid_t _PID)
{
    FILE *fp;
    char filename[30];
    char line[85];
    long base_address;
    char str[20];

    sprintf(filename, "/proc/%d/maps", _PID);
    fp = fopen(filename, "r");
    if(fp == NULL)
        exit(1); //if file doesn't exist exit

    fgets(line, 9, fp);
    sscanf(line, "%lx-%*lx %*s %*s %s", &base_address, str, str, str, str); //retrieve the base address of the current process
    fclose(fp); //close the map file

    return base_address; // return the address
}
//----------------------------------------------------------------------------------------------------------------------

std::string TripleLinux::getBytes(long _longNumber)
{
    char byte_value[sizeof(_longNumber)];
    std::copy(static_cast<const char*>(static_cast<const void*>(&_longNumber)),
              static_cast<const char*>(static_cast<const void*>(&_longNumber)) + sizeof _longNumber,
              byte_value);

    std::string byte_array = byte_value;
    return byte_array;
}
//----------------------------------------------------------------------------------------------------------------------

long TripleLinux::getFreeAllocationSpace(pid_t _PID)
{
    FILE *fp;
    char filename[30];
    char line[85];
    long free_addr;
    char str[20];

    sprintf(filename, "/proc/%d/maps", _PID);
    fp = fopen(filename, "r");
    if(fp == NULL)
        exit(1); //if file doesn't exist exit

    while(fgets(line, 85, fp) != NULL)
    {
        sscanf(line, "%lx-%*lx %*s %*s %s", &free_addr, str, str, str, str); //retrieve the first free location available
        if(strcmp(str, "00:00") == 0)
            break;
    }
    fclose(fp); //close the map file

    return free_addr; // return the address
}
//----------------------------------------------------------------------------------------------------------------------

// Void definition of virtual enabled
void TripleLinux::enable(){}
//----------------------------------------------------------------------------------------------------------------------

// Void definition of virtual disable
void TripleLinux::disable(){}
//----------------------------------------------------------------------------------------------------------------------

// Void definition of virtual continual
void TripleLinux::continual(){}
//----------------------------------------------------------------------------------------------------------------------

