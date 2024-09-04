#ifndef _HOST_APP_H_
#define _HOST_APP_H_


#define HOST_SOCKET_DEFAULT_TIMEOUT   1000


extern int host_conn_ctr_all;
extern int host_conn_ctr_ok;


extern void InitHostApp();
extern int HostCheckIpDevice(CPU_INT32U ip_addr, CPU_INT16U port, CPU_INT32U timeout);
extern int HostWriteParam(CPU_INT32U ip_addr, CPU_INT16U port, char* param_str, char* param_val, CPU_INT32U timeout);
extern int HostWritePulses(CPU_INT32U ip_addr, CPU_INT32U count, CPU_INT32U len_ms);

#if uC_TCPIP_MODULE > 0
extern NET_SOCK_ID HostConnectSocket(CPU_INT32U ip_addr, CPU_INT16U port, CPU_INT32U timeout, NET_ERR* err);
extern int HostWriteDataTimeout(NET_SOCK_ID sock, char* str, int len, CPU_INT32U timeout);
extern int HostReadData(NET_SOCK_ID sock, char *str, CPU_INT32U maxlen, CPU_INT32U timeout, NET_ERR *err);
#endif

#endif //_HOST_APP_H_
