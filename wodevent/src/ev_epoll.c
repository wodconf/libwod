/*
 * gev_epoll.c
 *
 *  Created on: 2012-10-23
 *      Author: goalworld
 */

#include "ev_inner.h"
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
struct epoll_info{
	int epFd;
};

static int 
epollNew(struct wvLoop * loop,int flag)
{
	struct epoll_info * pInfo = malloc(sizeof(struct epoll_info));
	if((pInfo->epFd = epoll_create1(EPOLL_CLOEXEC)) < 0){
		free(pInfo);
		return -errno;
	}
	loop->pollorData = pInfo;
	return WV_ROK;
}
static void 
epollDel(struct wvLoop *loop)
{
	struct epoll_info * pInfo = (struct epoll_info *)(loop->pollorData);
	close( pInfo->epFd );
	free(loop->pollorData);
}
static int 
epollAdd(struct wvLoop *loop, int fd, int mask)
{
	struct epoll_info * pInfo = (struct epoll_info *)(loop->pollorData);
	struct epoll_event epEv;
	epEv.data.fd = fd;
	mask |=loop->files[fd].event;
	epEv.events = (mask & WV_IO_READ) ? EPOLLIN:0 |(mask & WV_IO_WRITE) ? EPOLLOUT:0 ;
	int ret = -1;
	if(epEv.events != 0){
		//epEv.events |= EPOLLET
		
		if(loop->files[fd].event == WV_NONE){
			ret = epoll_ctl(pInfo->epFd,EPOLL_CTL_ADD,fd,&epEv);
			if( ret < 0 && errno == EEXIST){
				ret = epoll_ctl(pInfo->epFd,EPOLL_CTL_MOD,fd,&epEv);
			}
		}else{
			ret = epoll_ctl(pInfo->epFd,EPOLL_CTL_MOD,fd,&epEv);
		}
	}
	return ret >=0 ? WV_ROK : -errno ;
}

static int 
epollRemove(struct wvLoop *loop ,int fd, int mask)
{
	struct epoll_info * pInfo = (struct epoll_info *)(loop->pollorData);
	struct epoll_event epEv;
	epEv.data.fd = fd;
	mask =(loop->files[fd].event & (~mask));
	epEv.events = (mask & WV_IO_READ) ? EPOLLIN:0 |(mask & WV_IO_WRITE) ? EPOLLOUT:0 ;
	int ret;
	if(mask == WV_NONE){
		return epoll_ctl(pInfo->epFd,EPOLL_CTL_DEL,fd,&epEv);
	}else{
		return epoll_ctl(pInfo->epFd,EPOLL_CTL_MOD,fd,&epEv);
	}
}

static int 
epollPoll(struct wvLoop *loop,double timeOut)
{
	struct epoll_info * pInfo = (struct epoll_info *)(loop->pollorData);
	int ret;
	struct epoll_event epEvs[loop->set_size];
	while( ( ret = epoll_wait(pInfo->epFd,epEvs,loop->set_size,timeOut*1E3) ) && ret < 0 && errno == EINTR);
	if(ret > 0){
		int tmp = ret;
		struct epoll_event * epEv = epEvs ;
		int * pPendFd = loop->pendFds;
		for(;tmp>0;tmp--){
			loop->files[epEv->data.fd].revent = epEv->events & ( EPOLLIN | EPOLLHUP | EPOLLERR ) ? WV_IO_READ:0
											|( epEv->events & ( EPOLLIN | EPOLLHUP | EPOLLERR ) ) ? WV_IO_WRITE:0;
			epEv++;
			*(pPendFd++) = epEv->data.fd;
		}
	}
	return ret;
}
