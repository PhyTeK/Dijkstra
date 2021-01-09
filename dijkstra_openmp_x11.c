#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>

Display *dis;
int screen;
Window win;
GC gc;
unsigned long int chartreuse;

/* here are our X routines declared! */
void init_x();
void close_x();
void redraw();
void get_colors();
void create_colormap();

int main () {
	XEvent event;		/* the XEvent declaration !!! */
	KeySym key;		/* a dealie-bob to handle KeyPress Events */	
	char text[255];		/* a char buffer for KeyPress Events */
	long int i=0;
	init_x();
	get_colors();
	/* look for events forever... */
	while(1) {		
		/* get the next event and stuff it into our event variable.
		   Note:  only events we set the mask for are detected!
		*/
		XNextEvent(dis, &event);
	
		if (event.type==Expose && event.xexpose.count==0) {
		/* the window was exposed redraw it! */
			redraw();
		}
		if (event.type==KeyPress&&
		    XLookupString(&event.xkey,text,255,&key,0)==1) {
		/* use the XLookupString routine to convert the invent
		   KeyPress data into regular text.  Weird but necessary...
		*/
			if (text[0]=='q') {
				close_x();
			}
			printf("You pressed the %c key!\n",text[0]);
		}
		if (event.type==ButtonPress) {
		/* tell where the mouse Button was Pressed */
			int x=event.xbutton.x,
			    y=event.xbutton.y;
			i=0;
			//strcpy(text,"##################");
			//XSetForeground(dis,gc,rand()%event.xbutton.y%255);
			XSetForeground(dis,gc,100);
			//XDrawString(dis,win,gc,x,y, text, strlen(text));
			//XDrawLine(dis,win,gc, 0,0, 1000,500);
			//XDrawArc(dis,win,gc,100,100, 400, 500, 300, 10000);
			//XDrawRectangle(dis, win, gc, 100, 100, 200, 400);
			//XFillRectangle(dis,win,gc, 300, 300, 30, 30);
			XSetForeground(dis,gc,chartreuse);
			XDrawArc(dis,win,gc,x,1, 17,17,0,23040);
			
			while(i<100000){
			  XDrawArc(dis,win,gc,rand()%1680,rand()%1050,5,5,0,36000);
			  i++;
			}
		}
	}
}

void init_x() {
/* get the colors black and white (see section for details) */        
	unsigned long black,white;

	dis=XOpenDisplay((char *)0);
   	screen=DefaultScreen(dis);
	black=BlackPixel(dis,screen),
	white=WhitePixel(dis, screen);
   	win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,	
		300, 300, 5,black, white);
	XSetStandardProperties(dis,win,"Dijkstra","Dijkstra",None,NULL,0,NULL);
	XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);
        gc=XCreateGC(dis, win, 0,0);        
	XSetBackground(dis,gc,white);
	XSetForeground(dis,gc,black);
	XClearWindow(dis, win);
	XMapRaised(dis, win);
};

void close_x() {
	XFreeGC(dis, gc);
	XDestroyWindow(dis,win);
	XCloseDisplay(dis);	
	exit(1);				
};

void redraw() {
	XClearWindow(dis, win);
};

void get_colors() {
	XColor tmp;

	XParseColor(dis, DefaultColormap(dis,screen), "salmon", &tmp);
	XAllocColor(dis,DefaultColormap(dis,screen),&tmp);
	chartreuse=tmp.pixel;
};
void create_colormap() {
	int i;
	Colormap cmap;
	XColor tmp[255];

	for(i=0;i<255;i++) {
		tmp[i].pixel=i;
		tmp[i].flags=DoRed|DoGreen|DoBlue;
		tmp[i].red=i*256;
		tmp[i].blue=i*256;
		tmp[i].green=i*256;
	}	
	cmap=XCreateColormap(dis,RootWindow(dis,screen),
		DefaultVisual(dis,screen),AllocAll);
	XStoreColors(dis, cmap, tmp,255);
	XSetWindowColormap(dis,win,cmap);
};
