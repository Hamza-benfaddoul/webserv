#include <stdio.h>
#include <dirent.h>
#include <vector>
#include <iostream>

int main () {
    // struct dirent *pDirent;
    // DIR *pDir;
    std::vector<int> arr;
    for (int i = 0; i < __LONG_MAX__; i++)
    {
        std::cout << i << std::endl;
        arr.push_back(i);
    }

    // // Ensure we can open directory.

    // pDir = opendir ("config");
    // if (pDir == NULL) {
    //     printf ("Cannot open directory");
    //     return 1;
    // }

    // // Process each entry.

    // while ((pDirent = readdir(pDir)) != NULL) {
    //     printf ("[%s]\n", pDirent->d_name);
    // }

    // // Close directory and exit.

    // closedir (pDir);
    return 0;
}