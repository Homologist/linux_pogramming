/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Supplementary program for Chapter 8 */

/* t_getpwnam.c

   Demonstrate the use of getpwent() to retrieve records from the system
   password file.
*/
#include <pwd.h>
#include "tlpi_hdr.h"

struct passwd *
getpwnam(const char *name)
{
    struct passwd *pwd;
    while ((pwd = getpwent()) != NULL){
        if (strcmp(pwd->pw_name, name) == 0){
            break;
        }
    }

    endpwent();

    return pwd;
}

int
main(int argc, char *argv[1])
{
    printf("%s", getpwnam(argv[1])->pw_shell);
}