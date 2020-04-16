/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2020 Danny Petschke
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see http://www.gnu.org/licenses/.
**
*****************************************************************************
**
**  @author: Danny Petschke
**  @contact: danny.petschke@uni-wuerzburg.de
**
*****************************************************************************/

#include "defines.h"

void d_log(const char* format, ...)
{
    char buf[DMAX_LOG_LENGTH];

    va_list args;
    va_start(args, format);

    vsnprintf (buf, DMAX_LOG_LENGTH-3, format, args);
    strcat(buf, "\n");

    printf("DLib: %s", buf);
    fflush(stdout);

    va_end(args);
}

void d_errorLog(const char *file, int line, const char* format, ...)
{
    char buf[DMAX_LOG_LENGTH];

    va_list args;
    va_start(args, format);

    vsnprintf (buf, DMAX_LOG_LENGTH-3, format, args);
    strcat(buf, "\n");

    printf("file: %s line: %d DLib: %s", file, line, buf);
    fflush(stdout);

    va_end(args);
}
