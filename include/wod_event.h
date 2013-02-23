/*
 * wodEv.h
 *
 *  Created on: 2012-10-23
 *      Author: goalworld
 */

#ifndef _WOD_WODEV_H_
#define _WOD_WODEV_H_

#include <stddef.h>
enum {
	WV_NONE=0X00,
	WV_IO_READ=0X01,
	WV_IO_WRITE=0X02
};
enum{
	WV_ROK = 0
};
enum{
	WV_POLL_EPOLL,
	WV_POLL_SELECT,
	WV_POLL_POLL,
	WV_POLL_KQUEUE
};
struct wod_event_loop;

typedef void (*wod_event_io_fn)(struct wod_event_loop *loop,void * nv,int mask);
typedef int (*wod_event_time_fn)(struct wod_event_loop *loop,void * nv);
typedef int (*wod_event_userdef_fn)(struct wod_event_loop *loop,void * nv);

struct wod_event_loop * wod_event_loop_new(int set_size,int type);
void wod_event_loop_delete(struct wod_event_loop *);
void wod_event_loop_once(struct wod_event_loop *);
void wod_event_loop_loop(struct wod_event_loop *);
void wod_event_loop_stop(struct wod_event_loop *);

int wod_event_io_add(struct wod_event_loop *,int fd,int event,wod_event_io_fn cb,void *cbArg);
void wod_event_io_remove(struct wod_event_loop *,int id,int event);
int wod_event_io_wait(int fd,int mask,long long timeout);
int wod_event_time_add(struct wod_event_loop *,long long usec,wod_event_time_fn cb,void *cbArg);
void wod_event_time_remove(struct wod_event_loop *,int id);
int wod_event_userdef_add(struct wod_event_loop *,wod_event_userdef_fn cb,void *cbArg);
void wod_event_userdef_remove(struct wod_event_loop *,int id);
long long wod_event_time();
void wod_event_sleep(long long usec);
#endif /* GODEV_H_ */
