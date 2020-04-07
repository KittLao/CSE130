/* 
 * This file is derived from source code for the Pintos
 * instructional operating system which is itself derived
 * from the Nachos instructional operating system. The 
 * Nachos copyright notice is reproduced in full below. 
 *
 * Copyright (C) 1992-1996 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose, without fee, and
 * without written agreement is hereby granted, provided that the
 * above copyright notice and the following two paragraphs appear
 * in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE
 * AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
 * MODIFICATIONS.
 *
 * Modifications Copyright (C) 2017-2018 David C. Harrison. 
 * All rights reserved.
 */

#include <stdio.h>
#include <syscall-nr.h>
#include <list.h>

#include "devices/shutdown.h"
#include "devices/input.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "filesys/inode.h"
#include "filesys/directory.h"
#include "threads/palloc.h"
#include "threads/malloc.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/syscall.h"
#include "userprog/process.h"
#include "userprog/umem.h"

#include "threads/lock.h"


//../utils/pintos -v -k -T 10 --qemu  --filesys-size=2 -p build/tests/userprog/create-empty -a create-empty -- -q  -f run create-empty
//../utils/pintos -v -k -T 10 --qemu  --filesys-size=2 -p build/tests/userprog/create-long -a create-long -- -q  -f run create-long
//../utils/pintos -v -k -T 10 --qemu  --filesys-size=2 -p build/tests/userprog/create-normal -a create-normal -- -q  -f run create-normal
//../utils/pintos -v -k -T 10 --qemu  --filesys-size=2 -p build/tests/userprog/open-missing -a open-missing -- -q  -f run open-missing
//../utils/pintos -v -k -T 10 --qemu  --filesys-size=2 -p build/tests/userprog/open-normal -a open-normal -p ../tests/userprog/sample.txt -a sample.txt -- -q  -f run open-normal
//../utils/pintos -v -k -T 10 --qemu  --filesys-size=2 -p build/tests/userprog/open-twice -a open-twice -p ../tests/userprog/sample.txt -a sample.txt -- -q  -f run open-twice
//../utils/pintos -v -k -T 10 --qemu  --filesys-size=2 -p build/tests/userprog/read-normal -a read-normal -p ../tests/userprog/sample.txt -a sample.txt -- -q  -f run read-normal
//../utils/pintos -v -k -T 10 --qemu  --filesys-size=2 -p build/tests/userprog/close-normal -a close-normal -- -q  -f run close-normal
//../utils/pintos -v -k -T 10 --qemu  --filesys-size=2 -p build/tests/userprog/exec-once -a exec-once -p build/tests/userprog/child-simple -a child-simple -- -q  -f run exec-once

static void syscall_handler(struct intr_frame *);

static void write_handler(struct intr_frame *);
static void exit_handler(struct intr_frame *);
static void create_handler(struct intr_frame *);
static void open_handler(struct intr_frame *);
static void read_handler(struct intr_frame *);
static void close_handler(struct intr_frame *);
static void file_size_handler(struct intr_frame *);
static void exec_handler(struct intr_frame *);
static void wait_handler(struct intr_frame *);

struct lock filesys_lock;

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&filesys_lock);
}

static void
syscall_handler(struct intr_frame *f)
{
  int syscall;
  ASSERT( sizeof(syscall) == 4 ); // assuming x86

  // The system call number is in the 32-bit word at the caller's stack pointer.
  umem_read(f->esp, &syscall, sizeof(syscall));

  // Store the stack pointer esp, which is needed in the page fault handler.
  // Do NOT remove this line
  thread_current()->current_esp = f->esp;

  // printf("%s%d\n", "--System call: ", syscall);
  // printf("%s%d\n", "--System call read: ", SYS_READ);
  switch (syscall) {
  case SYS_HALT: 
    shutdown_power_off();
    break;

  case SYS_EXIT: 
    exit_handler(f);
    break;
      
  case SYS_WRITE: 
    write_handler(f);
    break;

  case SYS_CREATE:
    create_handler(f);
    break;

  case SYS_OPEN:
    open_handler(f);
    break;

  case SYS_READ:
    read_handler(f);
    break;

  case SYS_FILESIZE:
    file_size_handler(f);
    break;

  case SYS_CLOSE:
    close_handler(f);
    break;

  case SYS_EXEC:
    exec_handler(f);
    break;

  case SYS_WAIT:
    wait_handler(f);
    break;

  default:
    printf("[ERROR] system call %d is unimplemented!\n", syscall);
    thread_exit();
    break;
  }
}

/****************** System Call Implementations ********************/

void sys_exit(int status) 
{
  printf("%s: exit(%d)\n", thread_current()->name, status);
  thread_exit();
}

static void exit_handler(struct intr_frame *f) 
{
  int exitcode;
  umem_read(f->esp + 4, &exitcode, sizeof(exitcode));

  sys_exit(exitcode);
}

/*
 * BUFFER+0 and BUFFER+size should be valid user adresses
 */
static uint32_t sys_write(int fd, const void *buffer, unsigned size)
{
  lock_acquire(&filesys_lock);
  umem_check((const uint8_t*) buffer);
  umem_check((const uint8_t*) buffer + size - 1);

  int ret = -1;

  if (fd == 1) { // write to stdout
    putbuf(buffer, size);
    ret = size;
  }

  if(fd >= 2) {
    struct thread *curr_t = thread_current();
    struct file *file = curr_t->files[fd - 2];
    ret = file_write(file, buffer, size);
  }
  lock_release(&filesys_lock);
  return (uint32_t) ret;
}

static bool sys_create(char *fname, unsigned size) {
  lock_acquire(&filesys_lock);
  int file_size = strlen(fname);
  bool success = false;
  if(file_size > 0 && file_size < 14) {
    success = filesys_create(fname, size, false);
  }
  lock_release(&filesys_lock);
  return success;
}

static int sys_open(char *fname) {
  lock_acquire(&filesys_lock);
  int fd = 2;
  struct file *file = filesys_open(fname);
  if(file == NULL) {
    fd = -1;
  } else {
    struct thread *curr_t = thread_current();
    fd += curr_t->num_files_open;
    curr_t->files[curr_t->num_files_open] = file;
    curr_t->num_files_open++;
  }
  lock_release(&filesys_lock);
  return fd;
}

static off_t sys_read(int fd, void *buffer, unsigned size) {
  lock_acquire(&filesys_lock);
  umem_check((const uint8_t*) buffer);
  umem_check((const uint8_t*) buffer + size - 1);

  off_t bytes_read = 0;
  if(fd < 2) {
    bytes_read -1;
  } else {
    struct thread *curr_t = thread_current();
    int file_index = fd - 2;
    struct file *file = curr_t->files[file_index];
    bytes_read = file_read(file ,buffer, size);
  }
  lock_release(&filesys_lock);
  return bytes_read;
}

static int sys_file_size(int fd) {
  if(fd < 2) return -1;
  struct thread *curr_t = thread_current();
  int file_index = fd - 2;
  struct file *file = curr_t->files[file_index];
  return file_length(file);
}

static void sys_close(int fd) {
  if(fd < 2) return;
  lock_acquire(&filesys_lock);
  struct thread *curr_t = thread_current();
  int file_index = fd - 2;
  struct file *file = curr_t->files[file_index];
  file_close(file);
  lock_release(&filesys_lock);
}

static tid_t sys_exec(char *cmdline) {
  lock_acquire(&filesys_lock);
  tid_t tid = process_execute(cmdline);
  lock_release(&filesys_lock);
  return tid;
}

static int sys_wait(tid_t tid) {
  lock_acquire(&filesys_lock);
  int status = process_wait(tid);
  lock_release(&filesys_lock);
  return status;
}

static void write_handler(struct intr_frame *f)
{
    int fd;
    const void *buffer;
    unsigned size;

    umem_read(f->esp + 4, &fd, sizeof(fd));
    umem_read(f->esp + 8, &buffer, sizeof(buffer));
    umem_read(f->esp + 12, &size, sizeof(size));

    f->eax = sys_write(fd, buffer, size);
}

static void create_handler(struct intr_frame *f) {
  const char* fname;
  unsigned size;

  umem_read(f->esp + 4, &fname, sizeof(fname));
  umem_read(f->esp + 8, &size, sizeof(size));

  f->eax = sys_create(fname, size);
}

static void open_handler(struct intr_frame *f) {
  const char* fname;

  umem_read(f->esp + 4, &fname, sizeof(fname));

  f->eax = sys_open(fname);
}

static void file_size_handler(struct intr_frame *f) {
  int fd;

  umem_read(f->esp + 4, &fd, sizeof(fd));

  f->eax = sys_file_size(fd);
}

static void read_handler(struct intr_frame *f) {
  int fd;
  const void *buffer;
  unsigned size;

  umem_read(f->esp + 4, &fd, sizeof(fd));
  umem_read(f->esp + 8, &buffer, sizeof(buffer));
  umem_read(f->esp + 12, &size, sizeof(size));

  f->eax = sys_read(fd, buffer, size);
}


static void close_handler(struct intr_frame *f) {
  int fd;
  umem_read(f->esp + 4, &fd, sizeof(fd));
  sys_close(fd);
}

static void exec_handler(struct intr_frame *f) {
  const char *cmdline;
  umem_read(f->esp + 4, &cmdline, sizeof(cmdline));
  thread_current()->called_by_exec = true;
  f->eax = sys_exec(cmdline);
}

static void wait_handler(struct intr_frame *f) {
  tid_t tid;
  umem_read(f->esp + 4, &tid, sizeof(tid));
  f->eax = sys_wait(tid);
}










