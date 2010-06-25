/*
 *  ummd ( Micro MultiMedia Daemon )
 *
 *  Copyright (C) 2010 Nicolas Thill <nicolas.thill@gmail.com>
 */

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __MY_FILTERS_H
#define __MY_FILTERS_H

#include "autoconf.h"

#include "core.h"

typedef struct my_filter_conf my_filter_conf_t;
typedef struct my_filter_impl my_filter_impl_t;

struct my_filter_conf {
	int index;
	char *name;
	char *desc;
	char *type;
	char *arg;
};

struct my_filter_impl {
	int id;
	char *name;
	char *desc;
};


extern void my_filter_register(my_core_t *core, my_filter_impl_t *filter);
extern void my_filter_register_all(my_core_t *core);

#ifdef MY_DEBUGGING
extern void my_filter_dump_all(my_core_t *core);
#endif

#endif /* __MY_FILTERS_H */
