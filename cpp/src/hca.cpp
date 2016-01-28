/*
hca.c

Copyright (C) 2015 Henrik Björkman www.eit.se/hb

License: You may use, copy, modify and redistribute this code as long as you keep the History below and add state there if you did any changes.

Henriks communication API

This is a demo program you may use it free as long as you keep the history 
header and use it on your own responsibility.

It should work with both Windows and UNIX (such as Solaris & Linux).

History
941111 Created by Henrik Bjorkman 
000418 Ported to win32s by Henrik and Bengt Englund 
010718 Renamed to hca. Henrik (www.eit.se)
010719 ported to unix again. Henrik 
010721 Both the client and server API merged into one file. Henrik
030508 Support for standard input and serial port. Henrik
050709 Improved comments and renamed some function to make things more clear. Henrik
*/

#ifndef __EMSCRIPTEN__


#include <stdio.h>
#include <ctype.h>
#include "hca.h"



/* Macro for debug printouts */
#define D(x)


#if 0
/* To write a buffer in hex */
static void HexPrint(unsigned char *ptr, int len)
{
  D(printf("HexPrint   %2d: ",len);)
  if (len>30) len=30;
  while(len-->0)
  {
    printf("%02x",*ptr++); 
  }
  printf("\n");
}

/* To write all graphic characters in a buffer  */
static void GraphPrint(char *ptr, int len)
{
  
  D(printf("GraphPrint %2d: ",len);)
  if (len>64) len=64;
  while(len-->0)
  {  
    if (isgraph(*ptr)) {putc(*ptr,stdout);} else {putc('.',stdout);}
    ptr++;
  }
  printf("\n");
}
#endif


/****************************************************************************/
#ifdef WIN32
/****************************************************************************/

#include <conio.h>

/*

When linking for windows perhaps wsock32.lib needs to be added to project.

*/

#include <io.h>
#include <sys/types.h>
#include <winsock.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>






/*****************************************************************************/

/* The following variables and functions are only to be used internally. */


/* Global data for the communication */

static unsigned int hca_width=FD_SETSIZE;
static int hca_initiated=0;
static int std_input_fd=HCA_NO_FD;

static void hca_start_up()
{
  WORD wVersionRequested;
  WSADATA wsaData;
  int err;

  wVersionRequested = MAKEWORD( 2, 0 );
 
  err = WSAStartup( wVersionRequested, &wsaData );
  if ( err != 0 ) {
    /* Tell the user that we couldn't find a usable */
    /* WinSock DLL.                                  */
    printf("WSAStartup error code %d %d \n",err, WSAGetLastError());
    return;
  }
 
  /* Confirm that the WinSock DLL supports 2.0.*/
  /* Note that if the DLL supports versions greater    */
  /* than 2.0 in addition to 2.0, it will still return */
  /* 2.0 in wVersion since that is the version we      */
  /* requested.                                        */
  if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 0 ) {
    /* Tell the user that we couldn't find a usable */
    /* WinSock DLL.                                  */
    printf("WinSock DLL error (perhaps to old)\n");
    WSACleanup( );
    return; 
  }
}








/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Our own signal handler. It does nothing but count the signals */
/* and print an error message */
static int hca_signal_count=0;
static void hca_signal_hander(int i)
{
  hca_signal_count++; 
  fprintf(stderr,"hca: signal %d %d\n",hca_signal_count, i);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* The following functions are to be used by other functions. */




/* 
Initiates global data and sets up the communication. 
This function must be called before uning any of the others.
*/
int hca_init()
{
  if (!hca_initiated)
  {
    D(printf("hca_init\n");)

    /* Check the name of the computer */

    /* initiate global data */

    hca_start_up();



    /* Registrating our own handler for the some signals. */
    /* Otherwise the program might terminate if they happen. */
    /*signal( SIGPOLL, hca_signal_hander );*/
    /*signal( SIGPIPE, hca_signal_hander );*/
    /*signal( SIGURG, hca_signal_hander );*/
    hca_initiated=1;
  }
  return(0);
}



// An alternative to the current implementation below would be
// to use CreateFile to create a handle to console input (CONIN$). 





/* Create a socket and bind it to the server port number */
int hca_client(const char *hostname, int port)
{
  char ownhostname[32+1]={0};
  struct hostent *hp=NULL;
  struct sockaddr_in server_address;
  SOCKET s;
  int err;
  long cmd=FIONBIO;
  long arg=0;

  hca_init();

  /* Get name of the own host if no hostname was given.  */
  if (hostname==NULL) 
  {
    gethostname(ownhostname,32);ownhostname[32]=0;hostname=ownhostname;
  }

  printf("port %d host %s\n",port,hostname);

  /* Get host address for host with name as in hostname */
  hp=gethostbyname(hostname);
  if (hp==NULL) 
  {
    fprintf(stderr,"Unknown host %s\n",hostname);
    return -1;
  }


  /* Store address data in an address struct to be used by connect */
  server_address.sin_family=AF_INET;
  server_address.sin_port=htons(port); // dont forget byte order (htons).
  memcpy((char*)&server_address.sin_addr, (char*)hp->h_addr, hp->h_length);
  D(printf("Address family %d port %d \n",server_address.sin_family,server_address.sin_port);)



  /* Create a new socket  SOCK_STREAM*/
  s=socket(AF_INET ,SOCK_STREAM , 0 /*PF_INET*/);
  if ((s==INVALID_SOCKET) || (s<0))
  {
    printf("socket error code %d %d \n",s, WSAGetLastError());
    return -1;
  }
  D(printf("hca: New socket %d created\n",s);)


  /* Connect socket to server */
  err=connect(s,(struct sockaddr*)&server_address, sizeof(server_address));
  if (err!=0)
  {
    printf("connect error code %d %d \n",err, WSAGetLastError());
    return -1;
  }






  /* Set socket to non blocking mode. */
  err=ioctlsocket(s, cmd, &arg);
  if (err!=0)
  {
    printf("ioctlsocket %d %ld %ld %d %d\n", (int)s, cmd, arg, err, WSAGetLastError());
  }
  



  return(s);
}




/* To deactivate the communication, This is not nessesary. */
/* But on win32s it is nessesary! */
int hca_close(int fd)
{
  D(printf("hca_close socket %d\n",fd);)

  closesocket(fd);

  return(0);
}


/* This is supposed to be called before program terminates to clean things up. */
int hca_exit()
{
  unsigned int i;

  WSACleanup();
  hca_initiated=0;
  return(0);
}




/* This funktion sends data on a socket. */
/* Pretty much same as standard write */
int hca_write(int fd, const char *buf, int len)
{
  int i=0;
  int bytesWritten;

  D(printf("hca_write %d \n",fd);)



  bytesWritten=send(fd,buf,len,0);


  if (bytesWritten!=len)
  {
    fprintf(stderr,"send error %d %d\n", len ,bytesWritten);
  }
  return(bytesWritten);
}



/* 
To read data that has arrived on a socket. 
Ungefdr som vanlig read plus en felutskrift om det blev fel.

Return codes:
>0 The number of characters read.
 0 no data from this client.
-1 The client has probably disconnected.
-2 Other error.

*/
int hca_read(int fd,char *buf, int len)
{
  D(printf("hca_read: socket %d\n",fd);)


  len=recv(fd,buf,len,0);

  D(printf("read len %d\n",len);)

  if (len<0) // Error or no data
  {
    int err=WSAGetLastError();
    if (err==WSAEWOULDBLOCK)
    {
    	// There was no data
    	return(0);
    }

    fprintf(stderr,"hca: socket %d error %d\n",fd,err);
    D(if (errno==EBADF) {printf("EBADF\n");})
    return(-2);
  }
   
  if (len==0) // Klienten har lagt av tror jag
  {
    D(printf("Client on socket %d disconnected\n",fd);)
    return(-1);
  }

  if (len>0) /* inkommande data */
  {
    D(printf("Data recieved on socket %2d: \n",fd);) 
    D(GraphPrint(buf,len);)
    D(HexPrint(buf,len);)
  }
  return(len);
}





/****************************************************************************/
#else
/****************************************************************************/

/* This code for an unix target using Berkeley style sockets. */


/* 
IP server socket interface by Henrik Bjorkman 1994-11-23

For more information see an exampel server on page 4-6 in:
  Programmer's Guide: Networking Interfaces ISBN 0-13-020645-8
And:
  Internetworking with TCP/IP Volume 1; Principles, protocolls and architecture.
  Douglas E. Comer
  ISBN 0-13-474321-0
And ofcourse the man pages in unix.

Serial port programming is from:
  http://www.linux.org/docs/ldp/howto/Serial-Programming-HOWTO/x115.html

*/

#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
//#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
//#include <termios.h>
#include <assert.h>


/*****************************************************************************/

/* The following variables and functions are only to be used internaly. */

/* Global data for the communication */
static int hca_initiated=0;






/* Accept a new socket for communication with a new client. */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Our own signal handler. It does nothing but count the signals */
/* and print an error message */
int hca_signal_count=0;
void hca_signal_handler(int i)
{
  hca_signal_count++; 
  fprintf(stderr,"hca: ignored signal %d\n",i);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* The following functions are to be used by other functions. */

/* 
This function should be called before any of the other.
Initiates global data and sets up the communication. 
This function must be called before uning any of the others.
Returns zero.
After calling this call "hca_server" or "hca_client".
*/
int hca_init()
{
  if (!hca_initiated)
  {
    D(printf("hca_init:\n");)

    /* initiate global data */


    /* Registrating our own handler for the some signals. */
    /* Otherwise the program might terminate if they happen. */
    /*signal( SIGPOLL, hca_signal_handler );*/
    signal( SIGPIPE, hca_signal_handler );
    signal( SIGURG, hca_signal_handler );

    hca_initiated=1;
  }
  return(0);
}






/* 
Initiates global data and sets up the communication as a client.
*/
int hca_client(const char *hostname, int port)
{
  int s; 
  char ownhostname[64+1]={0};
  struct hostent *hp=NULL;
  struct sockaddr_in server_address;
  int err;

  hca_init();

  D(printf("hca_client: port %d\n",port);)

  /* Get name of the own host if no hostname was given.  */
  if (hostname==NULL) 
  {
    gethostname(ownhostname,sizeof(ownhostname)-1);
    ownhostname[sizeof(ownhostname)-1]=0;
    hostname=ownhostname;
  }

  D(printf("port %d host %s\n",port,hostname);)

  /* Get host address for host with name as in hostname */
  hp=gethostbyname(hostname);
  if (hp==NULL) 
  {
    fprintf(stderr,"Unknown host %s\n",hostname);
    return -1;
  }


  /* Store address data in an address struct, to be used by connect */
  server_address.sin_family=AF_INET;
  server_address.sin_port=htons(port);
  memcpy((char*)&server_address.sin_addr, (char*)hp->h_addr, hp->h_length);
  D(printf("Address family %d port %d \n",server_address.sin_family,server_address.sin_port);)



  /* Create a new socket  SOCK_STREAM*/
  s=socket(AF_INET ,SOCK_STREAM , 0 /*PF_INET*/);
  if (s<0)
  {
    perror("socket");
    return -1;
  }
  D(printf("hca: New socket %d created\n",s);)


  printf("hca: connecting to %s:%d\n", hostname, port);

  /* Connect socket to server */
  err=connect(s,(struct sockaddr*)&server_address, sizeof(server_address));
  if (err!=0)
  {
    perror("connect");
    return -1;
  }


  /* Set socket to non blocking mode */
  err=fcntl(s, F_SETFL,O_NDELAY);
  if (err) 
  {
    fprintf(stderr, "hsa: socket %d fcntl returned error code %d\n", s, err);
  }

  return(s);
}


/*
This is supposed to be called before program terminates to clean things up. 
*/
int hca_exit()
{
  hca_initiated=0;

  return(0);
}


/* 
To close a socket.
*/
int hca_close(int s)
{
  D(printf("hca_close: Closing %d\n",s);)

  close(s);


  return(0);
}

/* 
This funktion sends data on a socket.
Same as standard write but a message is printed in case of some error. 
*/
int hca_write(int fd, const char *buf, int len)
{
  D(printf("hca_write: %d %d\n",fd, len);)

  if (write(fd,buf,len)!=len)
  {
    perror("write");
    fprintf(stderr,"write error\n");
  }
  return(len);
}




/* 
To read data that has arrived on a socket. 
Ungefdr som vanlig read plus en felutskrift om det blev fel.

Return codes:
>0 The number of characters read.
 0 no data.
-1 The client has probably disconnected.
-2 Other error.

*/
int hca_read(int fd,char *buf, int len)
{
  D(printf("hca_read: socket %d\n",fd);)

  len=read(fd,buf,len);

  D(printf("read len %d\n",len);)

  if (len<0) /* Fel? */
  {
    if (errno==EWOULDBLOCK)
    {
      // There was not data
      return(0);
    }
    else
    {
      // Something wrong
      perror("read");
      fprintf(stderr,"hca_read: socket %d error %d\n",fd,errno);
      return(-2);
    }
  }
   
  if (len==0) // Klienten har lagt av tror jag
  {
    return(-1);
  }

  if (len>0) // inkommande data
  {
    D(printf("Data recieved on socket %2d: \n",fd);) 
    D(GraphPrint(buf,len);)
    D(HexPrint(buf,len);)
  }
  return(len);
}


/****************************************************************************/
#endif
/****************************************************************************/


HcaSocket::HcaSocket() :
	socket(-1),
	receiveBuffer(NULL),
	receiveBufferSize(InitialReceiveBufferSize),
	receivePtr(NULL),
	receiveIndex(0)
{
	hca_init();
}


HcaSocket::~HcaSocket()
{
	close();
}

// Returns 0 if OK
int HcaSocket::connect(const char *hostname, int port)
{
	if (socket<0)
	{
		socket = hca_client(hostname, port);
		if (socket>=0)
		{
			receiveBuffer = (char*)malloc(receiveBufferSize); // Free is to be done in close which shall be called from destructor.
			assert(receiveBuffer!=NULL);
			receivePtr = receiveBuffer;
			receiveIndex = 0;
			return 0;
		}
	}
	return -1;
}



// To send one line at a time ('\n' is appended).
// TODO: There is no handling of "would block", or is it in blocking mode? Have not examined this yet.
int HcaSocket::send(const char* str)
{
	hca_write(socket, str, strlen(str));
	hca_write(socket, "\n", 1);
	return 0;
}



// Receives one line at a time (separated by '\n').
// Caller must use/copy the received string before calling HcaSocket again.
char* HcaSocket::receive()
{
	// If receive buffer is almost at the end (well half way to the end) we need to move data to beginning (if possible)
	if (receiveIndex > (receiveBufferSize/2))
	//if (receiveIndex > 10)
	{
		const int nUnusedBytes = (receivePtr - receiveBuffer);
		if (nUnusedBytes != 0)
		{
			int nBytesToMove = receiveIndex - nUnusedBytes;
			char *toPtr = receiveBuffer;
			char *fromPtr = receivePtr;
			while(nBytesToMove > 0)
			{
				// This is not tested, but it must work.
				*toPtr=*fromPtr;
				++toPtr;
				++fromPtr;
				--nBytesToMove;
			}
			*toPtr=0;
			receiveIndex -= nUnusedBytes;
			receivePtr = receiveBuffer;
		}
	}

	const int n = hca_read(socket, receiveBuffer+receiveIndex, (receiveBufferSize-1)-receiveIndex);


	if (n>0)
	{
		receiveIndex += n;

		receiveBuffer[receiveIndex]=0;

		// Is there an '\n' terminated string in our receive buffer?
	}
	else if (n<0)
	{
		// Something is wrong
		printf("HcaSocket: n<0\n");
		close();
		return NULL;
	}

	char * end = strchr(receivePtr, '\n');

	if (end!=NULL)
	{
		char * prev = receivePtr;
		//const char* tmp = prev;
		// change the '\n' to zero.
		*end = 0;
		receivePtr = end + 1;
		return prev;
	}
	else if (receiveIndex>=(receiveBufferSize-1))
	{
		printf("Very long input string? %d %d\n", receiveIndex, (int)receiveBufferSize);
		// We need to make receive buffer bigger.

		const int nUnusedBytes = (receivePtr - receiveBuffer);
		char *tmp = (char*)malloc(receiveBufferSize*2); // Free is to be done in close which shall be called from destructor.
		assert(tmp!=NULL);
		memcpy(tmp, receiveBuffer+nUnusedBytes, receiveBufferSize-nUnusedBytes);
		receiveBufferSize *= 2;
		free(receiveBuffer);
		receiveBuffer=tmp;
		receivePtr = receiveBuffer;
		receiveIndex -= nUnusedBytes;
	}

	return NULL;
}

void HcaSocket::close()
{
	if (socket>=0)
	{
		hca_close(socket);
		socket = -1;
	}

	free(receiveBuffer);
	receiveBuffer = NULL;

	receivePtr = NULL;
	receiveIndex = 0;
}


#endif




/* end of file */
