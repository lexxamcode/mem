//
// Created by oleksiy on 30.04.23.
//
#include <iostream>
#include <cstdlib>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>

#include <sys/ptrace.h>
#include <sys/wait.h>
#include <cstddef>

#include "transformations.h"

/// The structure represents a line of\n
/// whole content of /proc/pid/maps file\n
struct Mapping
{
    unsigned long start_address;
    unsigned long end_address;
    std::string permissions;
};
/// \name Memory
/// \details The class Represents memory of the process:\n
/// It has it's own pid, file descriptor of proc/pid/mem file\n
/// And the contents of the /proc/pid/maps file\n
class Memory
{
private:
    /// Pid of the process
    pid_t _pid;
    /// file descriptor of /proc/pid/mem
    int _mem;
    /// Content of /proc/pid/maps - line by line
    std::vector<Mapping> _mappings;
public:
    Memory(): _pid(0), _mem(-1) { }
    explicit Memory(size_t pid) : _pid(pid), _mem(-1) { }

    /// Get pid for current process
    void set_pid(pid_t pid)
    {
       _pid = pid;
    }

    /// Get pid of current process\n
    /// Returns pid_t _pid;
    pid_t get_pid() const
    {
        return _pid;
    }

    /// Get vector of memory section of current process
    std::vector<Mapping> get_mappings() const
    {
        return _mappings;
    }

    /// \name read_maps()
    /// \details Reads /proc/pid/maps file into the vector\n
    /// of memory sections - _mappings
    /// \returns 0 in case of success\n -1 in case of error
    int read_maps()
    {
        _mappings.clear();

        std::string maps = std::string("/proc/") + std::to_string(_pid) + std::string("/maps");

        // Open maps file for getting process mappings
        std::ifstream maps_file(maps);

        // Parse maps file
        if (maps_file.is_open())
        {
            std::string line;
            while (std::getline(maps_file, line))
            {
                // Get split string
                std::vector<std::string> words_in_line = string_split(line, " ");

                // Get start and end addresses as strings
                std::string start_address_str = string_split(words_in_line[0], "-")[0];
                std::string end_address_str = string_split(words_in_line[0], "-")[1];

                // Parse them to unsigned values
                unsigned long start_address = std::stoul(start_address_str, nullptr, 16);
                unsigned long end_address = std::stoul(end_address_str, nullptr, 16);

                // Get permissions as string
                std::string permissions = words_in_line[1];

                Mapping new_mapping{start_address, end_address, permissions};
                _mappings.push_back(new_mapping);
            }
            maps_file.close();
            return 0;
        }
        else
        {
            std::cerr << "Can not open maps file" << std::endl;
            return -1;
        }
    }

    /// \name
    /// find_addresses(type value)
    /// \details
    /// Finds all addresses containing value
    /// \returns
    /// Vector of all addresses containing value or empty vector
    template <class type>
    std::vector<unsigned long> find_addresses(const type& value)
    {
        std::vector<unsigned long> addresses;
        char bytes[sizeof value];
        std::copy(static_cast<const char*>(static_cast<const void*>(&value)),
                  static_cast<const char*>(static_cast<const void*>(&value)) + sizeof value,
                  bytes);

        // Open mem file for using process memory
        std::string mem = std::string("/proc/") + std::to_string(_pid) + std::string("/mem");
        _mem =  open(mem.c_str(), O_RDWR);
        if (_mem == -1)
        {
            std::cerr << "Error while opening mem file: " << errno << std::endl;
            return addresses;
        }

        if (ptrace(PTRACE_ATTACH, _pid, 0, 0) == -1)
        {
            std::cerr << "Can not attach to the process" << std::endl;
            close(_mem);
            return addresses;
        }

        read_maps();

        waitpid(_pid, nullptr, 0);

        for (auto &it: _mappings)
        {
            if (it.permissions.find('w') != std::string::npos)
            {
                if (sizeof(value) <= it.end_address - it.start_address)
                {
                    for (unsigned long address = it.start_address; address < it.end_address; address++)
                    {
                        char found_bytes[sizeof(value)];

                        lseek(_mem, address, SEEK_SET);

                        if (pread(_mem, found_bytes, sizeof(value), address) != -1)
                        {
                            if (memcmp(found_bytes, bytes, sizeof(value)) == 0)
                                addresses.push_back(address);
                        }
                    }
                }
            }
        }

        ptrace(PTRACE_DETACH, _pid, 0, 0);
        close(_mem);
        return addresses;
    }

    /// \name
    /// find_addresses(char* value)
    /// \details
    /// Finds all addresses containing string value
    /// \returns
    /// Vector of all addresses containing string value or empty vector
    std::vector<unsigned long> find_addresses(char* value)
    {
        std::vector<unsigned long> addresses;
        size_t length = strlen(value);
        char bytes[length + 1];

        std::wcout << value << std::endl;
        std::cout << sizeof bytes << std::endl;

        for (int i = 0; i < length; i++)
        {
            bytes[i] = static_cast<int>(value[i]);
            std::cout << std::hex << static_cast<int>(bytes[i]) << " ";
        }
        bytes[length] = 0;
        std::cout << std::endl;

        // Open mem file for using process memory
        std::string mem = std::string("/proc/") + std::to_string(_pid) + std::string("/mem");
        _mem =  open(mem.c_str(), O_RDWR);
        if (_mem == -1)
        {
            std::cerr << "Error while opening mem file: " << errno << std::endl;
            return addresses;
        }

        if (ptrace(PTRACE_ATTACH, _pid, 0, 0) == -1)
        {
            std::cerr << "Can not attach to the process" << std::endl;
            close(_mem);
            return addresses;
        }

        read_maps();

        waitpid(_pid, nullptr, 0);

        for (auto &it: _mappings)
        {
            if (sizeof(value) <= it.end_address - it.start_address)
            {
                for (unsigned long address = it.start_address; address < it.end_address; address++) {
                    char found_bytes[length + 1];
                    memset(found_bytes, 0, length + 1);

                    lseek(_mem, address, SEEK_SET);

                    if (pread(_mem, found_bytes, length + 1, address) != -1) {
                        if (memcmp(found_bytes, bytes, length + 1) == 0)
                            addresses.push_back(address);
                    }
                }
            }
        }

        ptrace(PTRACE_DETACH, _pid, 0, 0);
        close(_mem);
        return addresses;
    }

    template <class type>
    std::vector<unsigned long> find_addresses(const type& value, const std::vector<unsigned long>& prev_addresses)
    {
        std::vector<unsigned long> addresses;
        char bytes[sizeof value];
        std::copy(static_cast<const char*>(static_cast<const void*>(&value)),
                  static_cast<const char*>(static_cast<const void*>(&value)) + sizeof value,
                  bytes);

        // Open mem file for using process memory
        std::string mem = std::string("/proc/") + std::to_string(_pid) + std::string("/mem");
        _mem =  open(mem.c_str(), O_RDWR);
        if (_mem == -1)
        {
            std::cerr << "Error while opening mem file: " << errno << std::endl;
            return addresses;
        }

        if (ptrace(PTRACE_ATTACH, _pid, 0, 0) == -1)
        {
            std::cerr << "Can not attach to the process" << std::endl;
            close(_mem);
            return addresses;
        }

        read_maps();

        waitpid(_pid, nullptr, 0);
        for (auto& address: prev_addresses)
        {
            char found_bytes[sizeof(value)];

            lseek(_mem, address, SEEK_SET);

            if (pread(_mem, found_bytes, sizeof(value), address) != -1)
            {
                if (memcmp(found_bytes, bytes, sizeof(value)) == 0)
                    addresses.push_back(address);
            }
        }

        ptrace(PTRACE_DETACH, _pid, 0, 0);
        close(_mem);
        return addresses;
    }

    std::vector<unsigned long> find_addresses(char* value, const std::vector<unsigned long> prev_addresses)
    {
        std::vector<unsigned long> addresses;
        size_t length = strlen(value);
        char bytes[length + 1];

        std::wcout << value << std::endl;
        std::cout << sizeof bytes << std::endl;

        for (int i = 0; i < length; i++)
        {
            bytes[i] = static_cast<int>(value[i]);
            std::cout << std::hex << static_cast<int>(bytes[i]) << " ";
        }
        bytes[length] = 0;
        std::cout << std::endl;

        // Open mem file for using process memory
        std::string mem = std::string("/proc/") + std::to_string(_pid) + std::string("/mem");
        _mem =  open(mem.c_str(), O_RDWR);
        if (_mem == -1)
        {
            std::cerr << "Error while opening mem file: " << errno << std::endl;
            return addresses;
        }

        if (ptrace(PTRACE_ATTACH, _pid, 0, 0) == -1)
        {
            std::cerr << "Can not attach to the process" << std::endl;
            close(_mem);
            return addresses;
        }

        read_maps();

        waitpid(_pid, nullptr, 0);
        for (auto& address: prev_addresses)
        {
            char found_bytes[length + 1];
            memset(found_bytes, 0, length + 1);

            lseek(_mem, address, SEEK_SET);

            if (pread(_mem, found_bytes, length + 1, address) != -1) {
                if (memcmp(found_bytes, bytes, length + 1) == 0)
                    addresses.push_back(address);
            }
        }

        ptrace(PTRACE_DETACH, _pid, 0, 0);
        close(_mem);
        return addresses;
    }

    /// \name
    /// write_value(type value, off_t address)
    /// \details
    /// Writes value to a process memory ad given address
    /// \returns
    /// 0 in case of success\n
    /// -1 in case of error
    template<class type>
    int write_value(type value, off_t address)
    {
        // Open mem file for using process memory
        std::string mem = std::string("/proc/") + std::to_string(_pid) + std::string("/mem");
        _mem =  open(mem.c_str(), O_RDWR);
        if (_mem == -1)
        {
            std::cerr << "Error while opening mem file: " << errno << std::endl;
            return -1;
        }

        if (ptrace(PTRACE_ATTACH, _pid, 0, 0) == -1)
        {
            std::cerr << "Can not connect to the process, errcode = " << errno << std::endl;
            close(_mem);
            return -1;
        }

        waitpid(_pid, nullptr, 0);

        lseek(_mem, address, SEEK_SET);
        if (write (_mem, &value, sizeof(value)) == -1)
        {
            std::cerr << "Error while writing, errcode = " << errno << std::endl;
            close(_mem);
            return -1;
        }

        ptrace(PTRACE_DETACH, _pid, 0, 0);
        close(_mem);
        return 0;
    }

    /// \name
    /// write_value(char* value, size_t size, off_t address)
    /// \details
    /// Writes char* to a process memory ad given address
    /// \returns
    /// 0 in case of success\n
    /// -1 in case of error
    int write_value(const char* value, size_t size, off_t address)
    {
        // Open mem file for using process memory
        std::string mem = std::string("/proc/") + std::to_string(_pid) + std::string("/mem");
        _mem =  open(mem.c_str(), O_RDWR);
        if (_mem == -1)
        {
            std::cerr << "Error while opening mem file: " << errno << std::endl;
            return -1;
        }

        if (ptrace(PTRACE_ATTACH, _pid, 0, 0) == -1)
        {
            std::cerr << "Can not connect to the process, errcode = " << errno << std::endl;
            close(_mem);
            return -1;
        }

        waitpid(_pid, nullptr, 0);

        lseek(_mem, address, SEEK_SET);
        if (write (_mem, value, size) == -1)
        {
            std::cerr << "Error while writing, errcode = " << errno << std::endl;
            close(_mem);
            return -1;
        }

        ptrace(PTRACE_DETACH, _pid, 0, 0);
        close(_mem);
        return 0;
    }

    /// \name
    /// read_value(off_t address, size_t bytes)
    /// \details
    /// Reads bytes from the process memory at given address
    /// \returns
    /// Bytes array in case of success\n nullptr in case of error
    char* read_value(off_t address, size_t bytes)
    {
        // Open mem file for using process memory
        std::string mem = std::string("/proc/") + std::to_string(_pid) + std::string("/mem");
        _mem =  open(mem.c_str(), O_RDWR);
        if (_mem == -1)
        {
            std::cerr << "Error while opening mem file: " << errno << std::endl;
            return nullptr;
        }

        if (ptrace(PTRACE_ATTACH, _pid, 0, 0) == -1)
        {
            std::cerr << "Can not connect to the process, errcode = " << errno << std::endl;
            close(_mem);
            return nullptr;
        }

        waitpid(_pid, nullptr, 0);

        lseek(_mem, address, SEEK_SET);

        char* buffer = new char[bytes];
        if (read(_mem, buffer, bytes) == -1) {
            std::cerr << "Error while reading, errcode = " << errno << std::endl;
            close(_mem);
            delete[] buffer;
            return nullptr;
        }

        ptrace(PTRACE_DETACH, _pid, 0, 0);
        close(_mem);
        return buffer;
    }
};
