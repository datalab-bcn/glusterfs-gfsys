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

#ifndef __SYS_EVENT_H__
#define __SYS_EVENT_H__

typedef void (* sys_event_notify_f)(void);

typedef struct _sys_event_client
{
    struct _sys_event_client * next;
    sys_event_notify_f         notify;
} sys_event_client_t;

typedef struct _sys_event
{
    sys_event_client_t *  clients;
    sys_event_client_t ** plast_client;
} sys_event_t;

#define SYS_EVENT_DECLARE(_name) \
    extern sys_event_t _name

#define SYS_EVENT_DEFINE(_name) \
    sys_event_t _name = \
    { \
        .clients = NULL, \
        .plast_client = &_name.clients \
    }

#define SYS_EVENT_CLIENT_DEFINE(_name) \
    static sys_event_client_t _name = \
    { \
        .next = NULL, \
        .notify = NULL \
    }

void sys_event_attach(sys_event_t * event, sys_event_client_t * client);
void sys_event_enable(sys_event_client_t * client, sys_event_notify_f callback);
void sys_event_disable(sys_event_client_t * client);
void sys_event_fire(sys_event_t * event);

#endif /* __SYS_EVENT_H__ */
