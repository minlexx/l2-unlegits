#ifndef H_NET_HOOK_DEFS
#define H_NET_HOOK_DEFS

//
// struct member alignment must be == 1 !!!!!
//
#ifdef _MSC_VER // MS Visual C
#pragma pack ( push, 1 )
struct jmp_push_ret
{
	unsigned char instr_push;  //здесь будет код инструкции push
	unsigned int  push_arg;    //аргумент push
	unsigned char instr_ret;   //здесь будет код инструкции ret
};

struct jmp_jmp
{
	unsigned char instr_jmp; // здесь будет инструкция JMP
	unsigned int  jmp_arg;    // аргумент JMP
};

#pragma pack ( pop )
#endif

#ifdef __GNUC__ // mingw32
#warning __GNUC__: deal with structure member alignment!
struct jmp_push_ret
{
	unsigned char instr_push __attribute__ (( __packed__ ));    //здесь будет код инструкции push
	unsigned int  push_arg   __attribute__ (( __packed__ ));    //аргумент push
	unsigned char instr_ret  __attribute__ (( __packed__ ));    //здесь будет код инструкции ret
};

struct jmp_jmp
{
	unsigned char instr_jmp  __attribute__ (( __packed__ ));    // здесь будет инструкция JMP
	unsigned int  jmp_arg    __attribute__ (( __packed__ ));    // аргумент JMP
};

#endif




// because we do not include winsock2.h, we need to declare some types
// rename them not to conflict with those from winsock2.h
typedef struct my_in_addr {
        union {
                struct { unsigned char  s_b1, s_b2, s_b3, s_b4; } S_un_b;
                struct { unsigned short s_w1, s_w2; } S_un_w;
                unsigned int S_addr;
        } S_un;
#ifndef s_addr
#define s_addr  S_un.S_addr /* can be used for most tcp & ip code */
#endif
} MY_IN_ADDR, *PMY_IN_ADDR;

// address family
#define AF_INET         2               // internetwork: UDP, TCP, etc.
//
// Socket types.
//
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3
#define SOCK_RDM        4
#define SOCK_SEQPACKET  5

// sockaddr_in
typedef struct my_sockaddr_in
{
#if(_WIN32_WINNT < 0x0600)
	short   sin_family;    
#else //(_WIN32_WINNT < 0x0600)
	ADDRESS_FAMILY sin_family;
#endif //(_WIN32_WINNT < 0x0600)
	unsigned short sin_port;
	MY_IN_ADDR sin_addr;
	unsigned char sin_zero[8];
} MY_SOCKADDR_IN, *PMY_SOCKADDR_IN;


#endif /* H_NET_HOOK_DEFS */
