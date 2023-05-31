#include "memlib/Memory.h"

void get_type()
{
    std::cout << "(c) char" << std::endl;
    std::cout << "(i) int" << std::endl;
    std::cout << "(f) float" << std::endl;
    std::cout << "(d) double" << std::endl;
    std::cout << "(s) string" << std:: endl;
}

int menu(Memory memory)
{
    char char_value;
    int int_value;
    float float_value;
    double double_value;
    std::string string_value;

    std::vector<unsigned long> addresses;

    while(true)
    {
        addresses.clear();
        system("clear");

        std::cout << std::dec << "PID: " << memory.get_pid() << std::endl;
        std::cout << "(s) Set pid" << std::endl;
        std::cout << "(f) Find value" << std:: endl;
        std::cout << "(c) Change value" << std::endl;
        std::cout << "(q) Quit" << std::endl;
        std::cout << "=========================" << std::endl;

        int choise = getchar();

        switch (choise)
        {
            case 's':
            {
                pid_t pid;
                std::cin >> pid;
                memory.set_pid(pid);
                break;
            }
            case 'f':
            {
                get_type();
                char type;
                std::cin >> type;
                std::cout << "Enter value: ";
                switch(type)
                {
                    case 'c': { std::cin >> char_value; addresses = memory.find_addresses(char_value); break; }
                    case 'i': { std::cin >> int_value; addresses = memory.find_addresses(int_value); break; }
                    case 'f': { std::cin >> float_value; addresses = memory.find_addresses(float_value); break; }
                    case 'd': { std::cin >> double_value; addresses = memory.find_addresses(double_value); break; }
                    case 's': {
                        std::cin >> string_value;

                        char buffer[string_value.length()];
                        memcpy(buffer, string_value.data(), string_value.length());
                        addresses = memory.find_addresses(buffer);
                        break;
                    }
                    default: break;
                }

                for (auto& it: addresses)
                    std::cout << std::hex << it << std::endl;

                std::string dummy;
                std::cin >> dummy;

                break;
            }
            case 'c':
            {
                get_type();
                char type;
                std::cin >> type;
                std::string address;
                std::cout << "Enter value and then address: ";
                switch(type)
                {
                    case 'c': { std::cin >> char_value; std::cin >> address; memory.write_value(char_value, std::stoul(address, 0, 16)); break; }
                    case 'i': { std::cin >> int_value; std::cin >> address; memory.write_value(int_value, std::stoul(address, 0, 16)); break; }
                    case 'f': { std::cin >> float_value; std::cin >> address; memory.write_value(float_value, std::stoul(address, 0, 16)); break; }
                    case 'd': { std::cin >> double_value; std::cin >> address; memory.write_value(double_value, std::stoul(address, 0, 16)); break; }
                    case 's': { std::cin >> string_value; std::cin >> address; memory.write_value(string_value.c_str(), string_value.size(), std::stoul(address, 0, 16)); break; }
                    default: break;
                }
                break;
            }
            case 'q':
            {
                return 0;
            }
            default: break;
        }
    }
}

int main()
{
    Memory memory;
    menu(memory);

    return 0;
}