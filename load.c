//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	www.roderickmackenzie.eu
//	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//
// This program is free software; you can redistribute it and/or modify it
// under the terms and conditions of the GNU General Public License,
// version 2, as published by the Free Software Foundation.
//
// This program is distributed in the hope it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "inp.h"
#include "util.h"
#include <sys/ioctl.h>
#include <net/if.h>
#include "inp.h"
#include <time.h>

struct node_struct nodes[100];
static int nnodes=0;

int send_node_load(int sock)
{
	double loadavg[3];
	char buf[LENGTH];

	getloadavg(loadavg, 3);

	sprintf(buf,"gpvdmload\n#load0\n%lf\n#load1\n%lf\n#load2\n%lf\n#ip\n%s\n#end",loadavg[0],loadavg[1],loadavg[2],get_my_ip());

	if(send_all(sock, buf, LENGTH,TRUE) < 0)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}


return 0;
}

int cmp_nodeload(int sock,char *revbuf)
{
	char exe_name[200];
	char dir_name[200];
	char buf[LENGTH];
	int cpus=0;

	int ret=0;
	if (cmpstr_min(revbuf,"gpvdmnodegetload")==0)
	{
		ret=send_node_load(sock);
		return ret;
	}
return -1;
}

int cmp_rxloadstats(int sock,char *revbuf)
{
	char ip[200];
	double load0=0.0;
	double load1=0.0;
	double load2=0.0;
	struct node_struct* node;
	if (cmpstr_min(revbuf,"gpvdmload")==0)
	{
		struct inp_file decode;
		//printf("revbuf='%s'\n",revbuf);
		inp_init(&decode);
		decode.data=revbuf;
		decode.fsize=strlen(revbuf);

		//printf("now doing inp\n");
		inp_search_double(&decode,&load0,"#load0");

		inp_search_double(&decode,&load1,"#load1");

		inp_search_double(&decode,&load2,"#load2");

		inp_search_string(&decode,ip,"#ip");

		//printf("ip=%s load=%lf\n",ip,load0);

		node=node_find(ip);
		if (node!=NULL)
		{
			node->load0=load0;
			node->alive=time(NULL);
		}
	}

return -1;
}

