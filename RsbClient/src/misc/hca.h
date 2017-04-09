/*
hca.h

Henriks Communication Api

This is a demo program you may use it free as long as you keep the history 
header and use it on your own responsibility.

It should work with both Windows and UNIX (such as Solaris & Linux).

History
940721 Created by Henrik Bjorkman 
000418 Small cleanup by Henrik
010718 Renamed to hsca. Henrik (www.eit.se)
010719 ported to unix again. Henrik 
010721 Both the client and server API merged into one file. Henrik
010721 Support for use in client or server. Henrik 
030313 Adapted for C++ also. Ragnar
030504 Improved comments to make code more understandable. Henrik
030508 Support for standard input and serial port. Henrik
050709 Improved comments and renamed some function to make things more clear. Henrik
*/

#ifndef HCA_H
#define HCA_H

#ifdef __cplusplus
extern "C"
{
#endif


#define HCA_NO_FD -1



/* 
This function should be called before any of the other hca function are used.
*/
int hca_init();


/* 
If program shall be a client then call this function to connect to the remote host.
It returns the client socket if successful or a negative value if not. 
*/
int hca_client(const char *hostname, int port);


/*
Call this function to read the data received on a socket. 
*/
int hca_read(int socket, char *buf, int len);

/*
Send data to a specific client 
*/
int hca_write(int socket, const char *buf, int len);

/* 
Close a connection. 
*/
int hca_close(int socket);

/* 
This is supposed to be called before program terminates to clean things up. 
*/
int hca_exit();

#ifdef __cplusplus
}
#endif

extern int hcaDebugCount;

class HcaSocket
{
private:
	enum {InitialReceiveBufferSize = 0x10000};

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	HcaSocket(const HcaSocket&);
	HcaSocket& operator=(const HcaSocket&);

public:
	HcaSocket();
	~HcaSocket();

	int connect(const char *hostname, int port);
	int send(const char* str);
	char* receive();
	void close();
	bool isOpen() const {return socket>=0;};

private:
	int socket;
	char *receiveBuffer;
	size_t receiveBufferSize;
	char* receivePtr;
	int receiveIndex;
};

#ifndef __EMSCRIPTEN__
extern HcaSocket hcaSocket; // This replaces WebSockest in the native build.
#endif


#endif
