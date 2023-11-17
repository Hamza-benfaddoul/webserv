#include <stdio.h>
    #include <dirent.h>

    int main () {
        struct dirent *pDirent;
        DIR *pDir;


        // Ensure we can open directory.

        pDir = opendir ("config");
        if (pDir == NULL) {
            printf ("Cannot open directory");
            return 1;
        }

        // Process each entry.

        while ((pDirent = readdir(pDir)) != NULL) {
            printf ("[%s]\n", pDirent->d_name);
        }

        // Close directory and exit.

        closedir (pDir);
        return 0;
    }