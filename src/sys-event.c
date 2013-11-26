/*
  Copyright (c) 2012-2013 DataLab, S.L. <http://www.datalab.es>

  This file is part of the gfsys library for GlusterFS.

  The gfsys library for GlusterFS is free software: you can redistribute
  it and/or modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  The gfsys library for GlusterFS is distributed in the hope that it will
  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the gfsys library for GlusterFS. If not, see
  <http://www.gnu.org/licenses/>.
*/

#include "gfsys.h"

void sys_event_attach(sys_event_t * event, sys_event_client_t * client)
{
    sys_event_client_t ** plast;

    client->notify = NULL;
    client->next = NULL;
    plast = atomic_xchg(&event->plast_client, client, memory_order_seq_cst);
    atomic_store(plast, client, memory_order_release);
}

void sys_event_enable(sys_event_client_t * client, sys_event_notify_f notify)
{
    atomic_store(&client->notify, notify, memory_order_release);
}

void sys_event_disable(sys_event_client_t * client)
{
    atomic_store(&client->notify, NULL, memory_order_release);
}

void sys_event_fire(sys_event_t * event)
{
    sys_event_client_t *client, ** pclient, ** plast;

    plast = atomic_load(&event->plast_client, memory_order_acquire);
    pclient = &event->clients;
    while (pclient != plast)
    {
        do
        {
            client = atomic_load(pclient, memory_order_acquire);
            if (unlikely(client == NULL))
            {
                pthread_yield();
            }
        } while (unlikely(client == NULL));

        if (client->notify != NULL)
        {
            client->notify();
        }

        pclient = &client->next;
    }
}
