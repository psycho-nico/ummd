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
#include <string.h>

#include "core/targets_priv.h"

#include "util/list.h"
#include "util/log.h"
#include "util/mem.h"
#include "util/url.h"

static my_list_t my_targets;

#define MY_TARGET_REGISTER(x) { \
	extern my_target_impl_t my_target_##x; \
	my_target_register(&my_target_##x); \
}

static my_target_impl_t *my_target_impl_find(my_target_conf_t *conf)
{
	my_target_impl_t *impl;
	my_node_t *node;
	char *impl_name;
	char *conf_type;
	char url_prot[10];
	char url_host[255];
	char buf[255];

	impl_name = "file";
	my_url_split(
		url_prot, sizeof(url_prot),
		NULL, 0, /* auth */
		url_host, sizeof(url_host),
		NULL,    /* port */
		NULL, 0, /* path */
		conf->url
	);
	if (strlen(url_host) > 0) {
		if (strlen(url_prot) == 0) {
			strncpy(url_prot, "udp", sizeof(url_prot));
		}
		conf_type = conf->type;
		if (!conf_type) {
			conf_type = "client";
		}
		snprintf(buf, sizeof(buf), "%s-%s", url_prot, conf_type);
		impl_name = buf;
	} else {
		if (strlen(url_prot) > 0) {
			impl_name = url_prot;
		}
	}

	for (node = my_targets.head; node; node = node->next) {
		impl = MY_TARGET_IMPL(node->data);
		if (strcmp(impl->name, impl_name) == 0) {
			return impl;
		}
	}

	my_log(MY_LOG_ERROR, "core/target: unknown type '%s' for target #%d '%s'", impl_name, conf->index, conf->name);

	return NULL;
}


my_target_t *my_target_priv_create(my_target_conf_t *conf, size_t size)
{
	my_target_t *target;

	target = my_mem_alloc(size);
	if (!target) {
		goto _MY_ERR_alloc;
	}

	target->conf = conf;

	target->iports = my_list_create();
	if (!target->iports) {
		goto _MY_ERR_create_iports;
	}
	
	return target;

	my_list_destroy(target->iports);
_MY_ERR_create_iports:
	my_mem_free(target);
_MY_ERR_alloc:
	return NULL;
}

void my_target_priv_destroy(my_target_t *target)
{
	my_list_destroy(target->iports);
	my_mem_free(target);
}


my_target_t *my_target_create(my_target_conf_t *conf)
{
	my_target_t *target;
	my_target_impl_t *impl;

	impl = my_target_impl_find(conf);
	if (!impl) {
		return NULL;
	}

	target = impl->create(conf);
	if (!target) {
		my_log(MY_LOG_ERROR, "core/target: error creating target #%d '%s'", conf->index, conf->name);
		return NULL;
	}

	MY_TARGET_GET_IMPL(target) = impl;

	return target;
}

void my_target_destroy(my_target_t *target)
{
	MY_TARGET_GET_IMPL(target)->destroy(target);
}

static int my_target_create_fn(void *data, void *user, int flags)
{
	my_core_t *core = MY_CORE(user);
	my_target_t *target;
	my_target_conf_t *conf = MY_TARGET_CONF(data);

	target = my_target_create(conf);
	if (target) {
		my_list_enqueue(core->targets, target);
		target->core = core;
	}

	return 0;
}

int my_target_create_all(my_core_t *core, my_conf_t *conf)
{
	MY_DEBUG("core/target: creating all targets");
	return my_list_iter(conf->targets, my_target_create_fn, core);
}


int my_target_destroy_all(my_core_t *core)
{
	my_target_t *target;

	MY_DEBUG("core/target: destroying all targets");
	while (target = my_list_dequeue(core->targets)) {
		MY_TARGET_GET_IMPL(target)->destroy(target);
	}
}


static int my_target_open_fn(void *data, void *user, int flags)
{
	my_target_t *target = MY_TARGET(data);

	MY_TARGET_GET_IMPL(target)->open(target);

	return 0;
}

int my_target_open_all(my_core_t *core)
{
	MY_DEBUG("core/target: opening all targets");
	return my_list_iter(core->targets, my_target_open_fn, core);
}


static int my_target_close_fn(void *data, void *user, int flags)
{
	my_target_t *target = MY_TARGET(data);

	MY_TARGET_GET_IMPL(target)->close(target);

	return 0;
}

int my_target_close_all(my_core_t *core)
{
	MY_DEBUG("core/target: closing all targets");
	return my_list_iter(core->targets, my_target_close_fn, core);
}


void my_target_register(my_core_t *core, my_target_impl_t *target)
{
	my_list_enqueue(&my_targets, target);
}

void my_target_register_all(void)
{
/*
	MY_TARGET_REGISTER(file);
	MY_TARGET_REGISTER(sock);
	MY_TARGET_REGISTER(http_client);
	MY_TARGET_REGISTER(rtp_client);
	MY_TARGET_REGISTER(udp_client);
*/
}

#ifdef MY_DEBUGGING

static int my_target_dump_fn(void *data, void *user, int flags)
{
	my_target_impl_t *impl = MY_TARGET_IMPL(data);

	MY_DEBUG("\t{");
	MY_DEBUG("\t\tname=\"%s\";", impl->name);
	MY_DEBUG("\t\tdescription=\"%s\";", impl->desc);
	MY_DEBUG("\t}%s", flags & MY_LIST_ITER_FLAG_LAST ? "" : ",");

	return 0;
}

void my_target_dump_all(void)
{
	MY_DEBUG("# registered targets");
	MY_DEBUG("targets = (");
	my_list_iter(&my_targets, my_target_dump_fn, NULL);
	MY_DEBUG(");");

}

#endif /* MY_DEBUGGING */
