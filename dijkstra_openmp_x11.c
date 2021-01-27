#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <omp.h>
#include <math.h>

# define NV 10
# define PNV 20
# define NTHREADS 4
# define MAXD 50
Display *dis;
int screen;
Screen *screenWindow;
int screenWidth=600,screenHeight=600;
Window win,*winp;
GC gc;

unsigned long salmon;

void main(int argc, char **argv );
int *dijkstra_distance(int ohd[NV][NV], int spath[NV][PNV],int *);
void find_nearest(int s, int e, int *,int spath[NV][PNV], int mind[NV], int connected[NV], int *d, 
		  int *v );
void init(int ohd[NV][NV],int *,int *);
void timestamp(void );
void update_mind(int s, int e, int mv, int *node_id, int connected[NV],
    int ohd[NV][NV], int spath[NV][PNV], int mind[NV] );

void init_x();
void close_x();
void redraw();
void get_colors();
void create_colormap();
void draw_path(int,int *,int *);
void test_path(int ohd[NV][NV],int mind[NV],int spath[NV][PNV]);



void main (int argc, char **argv) {
  XEvent event;
  XWindowAttributes watt;
  KeySym key;	
  char text[255];
  char str[12];
  unsigned int i,j,k,p,l,x,y;
  int screenWidth=1680;
  int screenHight=1050;
  int nodex[NV+1], nodey[NV+1];
  int i4_huge = INT_MAX;
  int *mind;
  int ohd[NV][NV];    // Distance matrix
  int spath[NV][PNV];  // Shortest path between nodes 0 and i
  int sidx=0;

  timestamp();
  init_x();
  get_colors();


  
  while(1) {		
    XNextEvent(dis, &event);

    if (event.type == Expose && event.xexpose.count == 0) {
      redraw();
    }
    if (event.type == KeyPress &&
	XLookupString(&event.xkey,text,255,&key,0) == 1) {
      if (text[0] == 'q') {
	close_x();
      }
    }
    if (event.type==ButtonPress) {
      //int x=event.xbutton.x,
      //    y=event.xbutton.y;
      //XDrawString(dis,win,gc,x,y, text, strlen(text));
      //XDrawLine(dis,win,gc, 0,0, 1000,500);
      //XDrawArc(dis,win,gc,100,100, 400, 500, 300, 10000);
      //XDrawRectangle(dis, win, gc, 100, 100, 200, 400);
      //XFillRectangle(dis,win,gc, 300, 300, 30, 30);
      XSetForeground(dis,gc,salmon);
      XGetWindowAttributes(dis, win, &watt);
      screenHeight = watt.height;
      screenWidth = watt.width;
      //printf("\t%dX%d\n",screenWidth,screenHeight);

      redraw();


      // Draw starting point

      nodex[0]=nodey[0]=0;
      XFillArc(dis,win,gc,0,0,10,10,0,64*360);

      sprintf(str, "%d", 0);
      XDrawString(dis,win,gc,5,20,str,strlen(str));

      // Initialize shortest path matrices
      for(i=0;i<NV;i++){
	for(j=0;j<PNV;j++){
	  spath[i][j] = 0;
	}
      }

      
      // Draw nodes and save nodes positions
      k=0;
      while(k<NV){
	k++;
	x = rand()%screenWidth;
	y = rand()%screenHeight;
	nodex[k] = x;
	nodey[k] = y;
				
	XFillArc(dis,win,gc,x,y,10,10,0,64*360);
	sprintf(str, "%d", k);
	XDrawString(dis,win,gc,x,y,str,strlen(str));

      }

      // Init OHD and print out matrix
      init(ohd,&nodex[0],&nodey[0]);

      fprintf(stdout, "\n" );
      fprintf(stdout, "  Distance matrix:\n" );
      fprintf(stdout, "\n" );
      for(i = 0; i < NV; i++ ) {
	for(j = 0; j < NV; j++ ) {
	  if(ohd[i][j] == i4_huge ) {
	    fprintf(stdout, "  Inf" );
	  }else{
	    fprintf(stdout, "%3d", ohd[i][j] );
	  }
	}
	fprintf(stdout, "\n" );
      }

      // Obtain minimum distance for all nodes
      mind = dijkstra_distance(ohd,spath,&sidx);

      
      
      timestamp();


      // End each path with last node

      for(i=0;i<NV;i++){
	if(1){
	  k = spath[i][0] + 1;
	  spath[i][k] = i;
	  spath[i][0] = k;
	}
      }



      // Test if sum of paths for each node is the same as in mind[NV]

      test_path(ohd,mind,spath);
	
      
      fprintf(stdout, "\n" );
      fprintf(stdout, "  Minimum distances from node 0:\n");
      fprintf(stdout, "\n" );
      for(i = 0; i < NV; i++){
	fprintf(stdout, "%3d%3d:\t", i, mind[i]);
	j=0;
	
	while(j<PNV){
	  
	  // Print out the minimum paths for each node
	  fprintf(stdout, "%4d",spath[i][j]);
	  j++;
	}
	fprintf(stdout,"\n");
      }


      // Draw path between nodes I and J
      draw_path(NV,&nodex[0],&nodey[0]);

      // Draw shortest path between first and last node
      // draw_path(...);
    }
  }

  free(mind);
  return;	
}

void init_x() {
       
  unsigned long black,white;

  dis = XOpenDisplay((char *)0);
  screen = DefaultScreen(dis);
  screenWindow = ScreenOfDisplay(dis, screen);
  black = BlackPixel(dis,screen);
  white = WhitePixel(dis, screen);
  win = XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,	
		      screenWidth, screenHeight, 5,black, white);
  // winp = &win;
  XSetStandardProperties(dis,win,"Dijkstra","Dijk",None,NULL,0,NULL);
  XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);
  gc = XCreateGC(dis, win, 0,0);        
  XSetBackground(dis,gc,white);
  XSetForeground(dis,gc,black);
  XClearWindow(dis, win);
  XMapRaised(dis, win);
}

void close_x() {
  XFreeGC(dis, gc);
  XDestroyWindow(dis,win);
  XCloseDisplay(dis);	
  exit(1);				
}

void redraw() {
  XClearWindow(dis, win);
}

void get_colors() {
  XColor tmp;

  XParseColor(dis, DefaultColormap(dis,screen), "salmon", &tmp);
  XAllocColor(dis,DefaultColormap(dis,screen),&tmp);
  salmon = tmp.pixel;
}

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
}

void timestamp(void)
{
# define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct tm *tm;
  time_t now;

  now = time(NULL);
  tm = localtime(&now);

  strftime(time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );

  printf("%s\n", time_buffer );

  return;
# undef TIME_SIZE
}

void init(int ohd[NV][NV],int *ndx,int *ndy)
{
  int i,j;
  double x,y;
  int i4_huge = INT_MAX;

  for(i = 0; i < NV; i++ ) {
    for(j = 0; j < NV; j++ ){
      if(i == j){
        ohd[i][i] = 0;
      }else{	
        ohd[i][j] = i4_huge;
      }
    }
  }
  
  // Initialize all consecutive nodes
  
  for (i = 0; i < NV ; i++){
    for(j = i; j < NV; j++){ 
      if(i==j) {
	ohd[i][i] = 0;
      }else{
	//x = (double)(ndx[i]-ndx[j]);
	//y = (double)(ndy[i]-ndy[j]);
	// ohd[j][i] = ohd[i][j] = (int)sqrt(x*x+y*y);
	ohd[j][i] = ohd[i][j] = rand()%MAXD ;
      }
    }
  }
  
  return;
}

int *dijkstra_distance(int ohd[NV][NV],int spath[NV][PNV],int *sidx)
/*
  Purpose:

  DIJKSTRA_DISTANCE uses Dijkstra's minimum distance algorithm.

  Discussion:

  We essentially build a tree.  We start with only node 0 connected
  to the tree, and this is indicated by setting CONNECTED[0] = 1.

  We initialize MIND[I] to the one step distance from node 0 to node I.
    
  Now we search among the unconnected nodes for the node MV whose minimum
  distance is smallest, and connect it to the tree.  For each remaining
  unconnected node I, we check to see whether the distance from 0 to MV
  to I is less than that recorded in MIND[I], and if so, we can reduce
  the distance.

  After NV-1 steps, we have connected all the nodes to 0, and computed
  the correct minimum distances.

  Parameters:

  Input, int OHD[NV][NV], the distance of the direct link between
  nodes I and J.

  Output, int *MIND[NV], the minimum distance from node 0 to each node.
*/
{
  int *connected;
  int i,j,k,l,p=0;
  int i4_huge = INT_MAX;
  int md;
  int *mind;
  int mv;
  int my_first;
  int my_id;
  int my_last;
  int my_md;
  int my_mv;
  int my_step;
  int nth;
  int node_id;

  *sidx = 0;
  
  connected = (int*) malloc(NV*sizeof(int));
  if(connected == NULL) close_x();
  
  connected[0] = 1;
  for(i = 1; i < NV; i++){
    connected[i] = 0;
  }

  mind = (int*) malloc(NV*sizeof(int));
  if(mind == NULL) close_x();

  for(i = 0; i < NV; i++ ){
    mind[i] = ohd[0][i];
  }

   # pragma omp parallel private(my_first, my_id, my_last, my_md, my_mv, my_step ) \
     shared(connected, md, mind, mv, nth, ohd,spath)
  {
    my_id = omp_get_thread_num();
    nth = omp_get_num_threads();
    //nth = NTHREADS;
    my_first = (my_id*NV)/nth;
    my_last  = ((my_id + 1 ) *NV )/nth - 1;

    # pragma omp single
    {
      printf("\n" );
      printf("  P%d: Parallel region begins with %d threads\n", my_id, nth );
      printf("\n" );
    }
    
    fprintf(stdout, "  P%d:  First=%d  Last=%d\n", my_id, my_first, my_last );
    
    for(my_step = 1; my_step < NV; my_step++ ) {

      for(l=0;l<NV;l++)
      	    spath[l][my_step+8] = mind[l];

      
      # pragma omp single 
      {
        md = i4_huge;
        mv = -1; 
      }
      /*
	Each thread finds the nearest unconnected node in its part of the graph.
	Some threads might have no unconnected nodes left.
      */
      
      find_nearest(my_first, my_last, &node_id, spath, mind, connected, &my_md, &my_mv );


      /*
	In order to determine the minimum of all the MY_MD's, we must insist
	that only one thread at a time execute this block!
      */

      # pragma omp critical

      {
        if(my_md < md)  
	  {
	    md = my_md;
	    mv = my_mv;

	  }
      }
      /*
	This barrier means that ALL threads have executed the critical
	block, and therefore MD and MV have the correct value.  Only then
	can we proceed.
      */

      # pragma omp barrier

      /*
	If MV is -1, then NO thread found an unconnected node, so we're done early. 
	OpenMP does not like to BREAK out of a parallel region, so we'll just have 
	to let the iteration run to the end, while we avoid doing any more updates.

	Otherwise, we connect the nearest node.
      */

      # pragma omp single 
      {
        if(mv != - 1 )
	  {
	    connected[mv] = 1;

	    printf("  P%d: Connecting node %d.\n", my_id, mv );

	  }
	
      }
      /*
	Again, we don't want any thread to proceed until the value of
	CONNECTED is updated.
      */
     # pragma omp barrier
      /*
	Now each thread should update its portion of the MIND vector,
	by checking to see whether the trip from 0 to MV plus the step
	from MV to a node is closer than the current record.
      */
      if(mv != -1 ){
	update_mind(my_first, my_last, mv, &node_id,connected, ohd, spath, mind );
	}
      /*
	Before starting the next step of the iteration, we need all threads 
	to complete the updating, so we set a BARRIER here.
      */
      #pragma omp barrier

      
      //p++;
      //printf("mystep=%d md=%d mv=%d\n", my_step,md,mv);
    } // end of for mystep
    /*
      Once all the nodes have been connected, we can exit.
    */
# pragma omp single
    {
      printf("\n" );
      printf("  P%d: Exiting parallel region.\n", my_id );
    }
  }

  free(connected );

  return mind;
}


void update_mind(int s, int e, int mv, int *node_id, int connected[NV], int ohd[NV][NV], int spath[NV][PNV], int mind[NV] )
/*
  Purpose:

  UPDATE_MIND updates the minimum distance vector.

  Discussion:

  We've just determined the minimum distance to node MV.

  For each unconnected node I in the range S to E,
  check whether the route from node 0 to MV to I is shorter
  than the currently known minimum distance.

  Parameters:

  Input, int S, E, the first and last nodes that are to be checked.

  Input, int MV, the node whose minimum distance to node 0
  has just been determined.

  Input, int CONNECTED[NV], is 1 for each connected node, whose 
  minimum distance to node 0 has been determined.

  Input, int OHD[NV][NV], the distance of the direct link between
  nodes I and J.

  Input/output, int MIND[NV], the currently computed minimum distances
  from node 0 to each node.  On output, the values for nodes S through
  E have been updated.
*/
{
  int i,j,k,p,l;
  int i4_huge = INT_MAX;
    
  for(i = s; i <= e; i++ ){
    if(!connected[i]){
      if(ohd[mv][i] < i4_huge){
	//printf("mind[%d]=%d, ",i,mind[i]);
	//if(i == 9) printf("\nold mind[%d]=%d new mind(%d)=%d\n",i,mind[i],mv,mind[mv] + ohd[mv][i]);


	    
	if(mind[mv] + ohd[mv][i] < mind[i]){

	  k = spath[i][0] + 1;
	  
	  spath[i][k] = mv;
	  spath[i][0] = k;

	  //if( i== 9) printf("\nspath[%d][%d]=%d",i,k,spath[i][k]);
	  mind[i] = mind[mv] + ohd[mv][i];



	}
	
      }

    }
    
  }
  
  return;
}

void find_nearest(int s, int e, int *node_id, int spath[NV][PNV], int mind[NV], int connected[NV], int *d, int *v )

/******************************************************************************/
/*
  Purpose:

  FIND_NEAREST finds the nearest unconnected node.

  Parameters:

  Input, int s, e the first and last nodes that are to be checked.

  Input, int mindD[NV], the currently computed minimum distance from
  node 0 to each node.

  Input, int connected[NV], is 1 for each connected node, whose 
  minimum distance to node 0 has been determined.

  Output, int *d, the distance from node 0 to the nearest unconnected 
  node in the range s to e.

  Output, int *v, the index of the nearest unconnected node in the range
  S to E.
*/
{
  int i,k,p;
  int i4_huge = INT_MAX;

  *d = i4_huge;
  *v = -1;
  
  for(i = s; i <= e; i++){

    if(!connected[i] && (mind[i] < *d)){
	  *d = mind[i];
	  *v = i;

	  /* printf("%d,",i); */
	  /* p = spath[i][0] + 10; */

	  /* spath[i][p] = i; */
	  
	  //fprintf(stdout,"connected mind[%d]=%d",i,mind[i]);
	  //k = (spath[i][0]);
	  // Collect first direct path only
	  /* if(mind[i] == 0 && k == 0){ */
	  /*   // Fix the direct move to a 0-distance node */
	  /*   spath[i][0] = 1; */
	  /*   spath[i][1] = i; */
	  /* } */
	  

    }
  }
  return;
}

void draw_path(int nmax,int *ndx,int *ndy){
  for(int n=1;n<= nmax;n++){
    XDrawLine(dis,win,gc, ndx[n-1],ndy[n-1],ndx[n],ndy[n]);
  }
  return;
}


void test_path(int ohd[NV][NV],int mind[NV],int spath[NV][PNV]){
  // Keep test result in the last column of spath[][]
  
  int i,j,k,l,m;
  int test;
  
  for(i=0;i<NV;i++){
    k = spath[i][1];
    test = 0;
    l = spath[i][0];
    j=1;
    m = 0;
    while(j <= l){
      k = spath[i][j];
      test += ohd[m][k];
      m = k;
      j++;
    }

    if(test == mind[i]){
      spath[i][PNV-1] = test;
    }else{
      spath[i][PNV-1] = test;
    }

  }

  return;
}

void short_path(){


  return;
}
