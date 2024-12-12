#undef _GNU_SOURCE
#define _GNU_SOURCE
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "input.h"
#include "rc.h"

static int useevent = 1;
static int eventfd = -1;

rcvar_t event_exports[] =
{
	RCV_BOOL("event", &useevent, "enable event"),
	RCV_END
};

static const int axes[4] =
{
	K_JOYUP, K_JOYDOWN, K_JOYLEFT, K_JOYRIGHT,
};


void event_init()
{
	if (!useevent) return;

	eventfd = open("/dev/input/event0", O_RDONLY);
	int flags = fcntl(eventfd, F_GETFL, 0);
	fcntl(eventfd, F_SETFL, flags | O_NONBLOCK);
}

void event_close()
{
	close(eventfd);
}

void event_poll()
{
	struct input_event readevent;
	event_t ev;
	
	if (eventfd < 0) return;

	while (read(eventfd,&readevent,sizeof(struct input_event)) == sizeof(struct input_event))
	{
		if (readevent.type == EV_KEY) {
			ev.type = readevent.value==2 ? EV_PRESS : EV_RELEASE;

            if(readevent.code == 12 || readevent.code == 13 || readevent.code == 14 || readevent.code == 15){ 
				ev.code = K_JOY0 + (readevent.code-12);
				ev_postevent(&ev);
			}
			if(readevent.code == 16 || readevent.code == 17 || readevent.code == 18 || readevent.code == 19){
				ev.code = axes[readevent.code-16];
				ev_postevent(&ev);
			}
		}
	}
}

