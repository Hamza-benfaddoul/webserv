#include "../includes/configParser.hpp"

int main (int ac, char **av)
{
    if (ac > 0)
    {
        try {
            if (av[1])
                configParser webserv(av[1]);
            else
                configParser webserv;
        } catch (const std::exception &e) {
            std:: cout << "\e[0;31m" << e.what() << "\033[0m"<< std::endl;
        }
    }
    return 0;
}
