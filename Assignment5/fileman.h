/*********************************************************************
 *
 * Copyright (C) 2020 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/

#include <stddef.h>

/*
 * Read at most SIZE bytes from FNAME starting at FOFFSET into BUF starting 
 * at BOFFSET.
 *
 * RETURN number of bytes read from FNAME into BUFF, -1 on error.
 */
int fileman_read(char *fname, size_t foffset, char *buf, size_t boffset, size_t size); 

/*
 * Create FILE and Write SIZE bytes from BUF starting at BOFFSET into FILE 
 * starting at FOFFSET.
 * 
 * RETURN number of bytes from BUFF written to FNAME, -1 on error or if FILE 
 * already exists
 */
int fileman_write(char *fname, size_t foffset, char *buf, size_t boffset, size_t size); 

/*
 * Append SIZE bytes from BUF to existing FILE.
 * 
 * RETURN number of bytes from BUFF appended to FNAME, -1 on error or if FILE 
 * does not exist
 */
int fileman_append(char *fname, char *buf, size_t size); 

/*
 * Copy existing file FSRC to new file FDEST.
 * 
 * RETURN number of bytes from FSRC written to FDEST, -1 on error, or if FSRC does 
 * not exists, or if SDEST already exists
 */
int fileman_copy(char *fsrc, char *fdest); 

/*
 * Print a hierachival directory view starting at DNAME to file descriptor FD as 
 * shown in an example below where DNAME == 'data.dir'
 *
 *   data.dir
 *       blbcbuvjjko
 *           lgvoz
 *               jfwbv
 *                   jqlbbb
 *                   yfgwpvax
 *           tcx
 *               jbjfwbv
 *                   demvlgq
 *                   us
 *               zss
 *                   jfwbv
 *                       ahfamnz
 *       vkhqmgwsgd
 *           agmugje
 *               surxeb
 *                   dyjxfseur
 *                   wy
 *           tcx
 */
void fileman_dir(int fd, char *dname); 

/*
 * Print a hierachival directory tree view starting at DNAME to file descriptor 
 * FD as shown in an example below where DNAME == 'data.dir'
 *
 *   data.dir
 *   ├── blbcbuvjjko
 *   │   ├── lgvoz
 *   │   │   └── jfwbv
 *   │   │       ├── jqlbbb
 *   │   │       └── yfgwpvax
 *   │   └── tcx
 *   │       ├── jbjfwbv
 *   │       │   ├── demvlgq
 *   │       │   └── us
 *   │       └── zss
 *   │           └── jfwbv
 *   │               └── ahfamnz
 *   └── vkhqmgwsgd
 *       ├── agmugje
 *       │   └── surxeb
 *       │       ├── dyjxfseur
 *       │       └── wy
 *       └── tcx
 */
void fileman_tree(int fd, char *dname); 
