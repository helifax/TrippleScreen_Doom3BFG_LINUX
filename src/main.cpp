#include <iostream>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/user.h>
#include<malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string.h>
#include <string>
#include<include/TripleLinux.h>
#include<errno.h>
#include <unistd.h>



// Create the Class for DOOM 3 BFG FIX
class Doom3BG: public TripleLinux
{
 public:
  void enable()
    {
      // Returns the pif of the process to hook.
      m_executable = pidOf("Doom3-BFG-32bit");

      // Get the base address + address of free space where to write the codecave
      long baseAddress = getPidBaseAddress(m_executable);
      //long codeCaveHUD_address = getFreeAllocationSpace(m_executable);
      long codeCaveHUD_address = baseAddress + 0x8D8FC3;

      // Address offsets
      long offsetHUDEnable = 0x3835DE;
      long offsetNOPEnable = 0x3835F3;

      ////////////////////////////////////////////
      /*
        E9 00 00 00 00 - jmp _to_Code_Cave
        */

      char pokeHUDEnable[] = "\xE9\x00\x00\x00\x00";
      ////////////////////////////////////////////
      /*
       C7 40 28 9A 99 99 3F	      - mov [eax+28], ,3F 99 99 9A // 1.2 float
       A1 44 3F 92 08             - mov eax,[08923F44]
       E9 00 00 00 00             - jmp _to_next_instruction_after_insert
        */

      char codeCaveHUD[] = "\xC7\x40\x28\x9A\x99\x99\x3F\xA1\x44\x3F\x92\x08\xE9\x00\x00\x00\x00";
      ////////////////////////////////////////////
      /*
        90 90 90 90- nop nop nop nop
        */
      char codeCaveNOP[] = "\x90\x90\x90\x90";
      ////////////////////////////////////////////

      // Attach to the process
      ptrace(PTRACE_ATTACH, m_executable, NULL, NULL);
      wait(NULL);

      ////////////////////////////////////////////
      // Write the codecave in the process memory
      putData(m_executable, codeCaveHUD_address, codeCaveHUD, (sizeof(codeCaveHUD) - 1));
      ////////////////////////////////////////////

      // Calculate the jump after the codecave // A jump is always 5 bytes long
      long jumpAfter = ((baseAddress + offsetHUDEnable) + (sizeof(pokeHUDEnable) - 1)) - (codeCaveHUD_address + (sizeof(codeCaveHUD) - 1));
      ////////////////////////////////////////////
      // Write the jump after the codecave
      putData(m_executable, codeCaveHUD_address + (sizeof(codeCaveHUD) -1) - 0x04, getBytes(jumpAfter).c_str(), 0x04);
      ////////////////////////////////////////////

      ///////////////////////////////////////////////
      // Write the jump to the code-cave
      putData(m_executable, baseAddress + offsetHUDEnable, pokeHUDEnable, sizeof(pokeHUDEnable)-2);

      // Calculate the jump to the codecave // A jump is always 5 bytes long
      long jumpTo = (codeCaveHUD_address - (baseAddress + offsetHUDEnable + 0x05));
      ////////////////////////////////////////////
      // Write the jump to the codecave
      putData(m_executable, baseAddress + offsetHUDEnable + 0x01, getBytes(jumpTo).c_str(), 0x04);
      ////////////////////////////////////////////

      ///////////////////////////////////////////////
      // Write the NOP the code-cave
      putData(m_executable, baseAddress + offsetNOPEnable, codeCaveNOP, sizeof(codeCaveNOP)-1);

      ////////////////////////////////////////////
      //Write everything in memory
      memcpy(&oldregs, &regs, sizeof(regs));
      ////////////////////////////////////////////


      ptrace(PTRACE_CONT, m_executable, NULL, NULL);

      // Detach from the process after injection was done
      ptrace(PTRACE_DETACH, m_executable, NULL, NULL);

      printf("Pid number %d \n", m_executable);
      printf("Base Address %x \n", baseAddress);
      printf("Address to inject %x \n", codeCaveHUD_address);

    }
};

// Create the Doom3 Fix class
Doom3BG fix;

// Main function
int main(int argc, char** argv)
{
    fix.enable();
    return 0;

}
