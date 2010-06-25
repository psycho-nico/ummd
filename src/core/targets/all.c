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

#include <stdlib.h>

#include "core/targets.h"

#include "util/list.h"
#include "util/log.h"

#define MY_CORE_TARGET_REGISTER(c,x) { \
	extern my_core_target_t my_core_target_##x; \
	my_core_target_register((c), &my_core_target_##x); \
}

void my_core_target_register(my_core_t *core, my_core_target_t *target)
{
	my_list_queue(core->targets, target);
}

void my_core_target_register_all(my_core_t *core)
{
/*
	MY_CORE_TARGET_REGISTER(core, file);
	MY_CORE_TARGET_REGISTER(core, sock);
	MY_CORE_TARGET_REGISTER(core, net_http_client);
	MY_CORE_TARGET_REGISTER(core, net_rtp_client);
	MY_CORE_TARGET_REGISTER(core, net_udp_client);
*/
}

#ifdef MY_DEBUGGING

static int my_core_target_dump(void *data, void *user, int flags)
{
	my_core_target_t *target = (my_core_target_t *)data;

	MY_DEBUG("\t{");
	MY_DEBUG("\t\tname=\"%s\";", target->name);
	MY_DEBUG("\t\tdescription=\"%s\";", target->desc);
	MY_DEBUG("\t}%s", flags & MY_LIST_ITER_FLAG_LAST ? "" : ",");

	return 0;
}

void my_core_target_dump_all(my_core_t *core)
{
	MY_DEBUG("# registered targets");
	MY_DEBUG("targets = (");
	my_list_iter(core->targets, my_core_target_dump, NULL);
	MY_DEBUG(");");

}

#endif /* MY_DEBUGGING */
