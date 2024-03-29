/*
 * libhfs - library for reading and writing Macintosh HFS volumes
 * Copyright (C) 1996-1998 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id$
 */

#ifdef CP_NOT_USED
int os_open(void **, const char *, int);
#endif
int os_callback_open(void **priv, oscallback func, void* cookie);
int os_close(void **);

#ifdef CP_NOT_USED
int os_same(void **, const char *);
#endif

unsigned long os_seek(void **, unsigned long);
unsigned long os_read(void **, void *, unsigned long);
unsigned long os_write(void **, const void *, unsigned long);
