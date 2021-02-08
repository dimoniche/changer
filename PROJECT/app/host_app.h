#ifndef _HOST_APP_H_
#define _HOST_APP_H_


#define HOST_SOCKET_DEFAULT_TIMEOUT   1000


extern int host_conn_ctr_all;
extern int host_conn_ctr_ok;


extern void InitHostApp();
extern int HostCheckIpDevice(CPU_INT32U ip_addr, CPU_INT16U port, CPU_INT32U timeout);
extern int HostWriteParam(CPU_INT32U ip_addr, CPU_INT16U port, char* param_str, char* param_val, CPU_INT32U timeout);
extern int HostWritePulses(CPU_INT32U ip_addr, CPU_INT32U count, CPU_INT32U len_ms);

#endif //_HOST_APP_H_
