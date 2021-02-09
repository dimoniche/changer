#include <includes.h>
#include "app_serv.h"
#include "ftp_client.h"
#include <string.h>
#include "time.h"
#include "host_app.h"


/**
 * @brief Removes all trailing whitespace from a string
 * @param[in,out] s Pointer to a NULL-terminated character string
 **/
void strRemoveTrailingSpace(char *s)
{
   char *end;

   // Search for the first whitespace to remove
   // at the end of the string
   for (end = NULL; *s != '\0'; s++)
   {
      if (*s != ' ')
         end = NULL;
      else if (!end)
         end = s;
   }

   // Trim whitespace from the end
   if (end) *end = '\0';
}


/**
 * @brief Convert a binary IPv4 address to dot-decimal notation
 * @param[in] ipAddr Binary representation of the IPv4 address
 * @param[out] str NULL-terminated string representing the IPv4 address
 * @return Pointer to the formatted string
 **/
char *ipv4AddrToString(uint32_t ipAddr, char *str)
{
   uint8_t *p;
   static char buffer[16];

   // The str parameter is optional
   if (!str) str = buffer;

   // Cast the address to byte array
   p = (uint8_t *) &ipAddr;
   // Format IPv4 address
   sprintf(str, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);

   // Return a pointer to the formatted string
   return str;
}

/**
 * @brief Establish a connection with the specified FTP server
 * @param[in] context Pointer to the FTP client context
 * @param[in] interface Underlying network interface (optional parameter)
 * @param[in] serverAddr IP address of the FTP server
 * @param[in] serverPort Port number
 * @param[in] flags Connection options
 * @return Error code
 **/

int ftpConnect(FtpClientContext *context, uint32_t *serverAddr, uint16_t serverPort, uint32_t flags)
{
    int error;
    uint32_t replyCode;
    NET_ERR err;

    // Invalid context?
    if (context == NULL)
        return ERROR_INVALID_PARAMETER;

    // Clear context
    memset(context, 0, sizeof(FtpClientContext));

    // Save the IP address of the FTP server
    context->serverAddr = *serverAddr;

    // Use passive mode?
    if (flags & FTP_PASSIVE_MODE)
        context->passiveMode = 1;
    else
        context->passiveMode = 0;
      
    // Open control socket
    context->controlSocket = HostConnectSocket(context->serverAddr, serverPort, FTP_CLIENT_DEFAULT_TIMEOUT, &err);
    if ((err != NET_SOCK_ERR_NONE) || (context->controlSocket < 0))
    {
        return ERROR_OPEN_FAILED;
    }

    // Wait for the connection greeting reply
    error = ftpSendCommand(context, NULL, &replyCode);
    // Any communication error to report?
    if (error >= 0)
    {
        // Check FTP response code
        if (!FTP_REPLY_CODE_2YZ(replyCode))
            error = ERROR_UNEXPECTED_RESPONSE;
    }
    
    // Any error to report?
    if (error < 0) 
    {
        // Clean up side effects
        NetSock_Close(context->controlSocket, &err);
        context->controlSocket = -1;
    }

   // Return status code
   return error;
}


/**
 * @brief Login to the FTP server using the provided username and password
 * @param[in] context Pointer to the FTP client context
 * @param[in] username The username to login under
 * @param[in] password The password to use
 * @param[in] account Account name
 * @return Error code
 **/
int ftpLogin(FtpClientContext *context, const char *username, const char *password, const char *account)
{
    int error;
    uint32_t replyCode;

    // Invalid context?
    if (context == NULL)
        return ERROR_INVALID_PARAMETER;

    // Format the USER command
    sprintf((char*)context->buffer, "USER %s\r\n", username);

    // Send the command to the server
    error = ftpSendCommand(context, (char const*)context->buffer, &replyCode);
    // Any error to report?
    if (error) return error;

    // Check FTP response code
    if (FTP_REPLY_CODE_2YZ(replyCode))
        return 0;
    else if(!FTP_REPLY_CODE_3YZ(replyCode))
        return ERROR_UNEXPECTED_RESPONSE;

    // Format the PASS command
    sprintf((char*)context->buffer, "PASS %s\r\n", password);

    // Send the command to the server
    error = ftpSendCommand(context, (char const*)context->buffer, &replyCode);
    // Any error to report?
    if (error) return error;

    // Check FTP response code
    if(FTP_REPLY_CODE_2YZ(replyCode))
        return 0;
    else if(!FTP_REPLY_CODE_3YZ(replyCode))
        return ERROR_UNEXPECTED_RESPONSE;

    // Format the ACCT command
    sprintf((char*)context->buffer, "ACCT %s\r\n", account);

    // Send the command to the server
    error = ftpSendCommand(context, (char const*)context->buffer, &replyCode);
    // Any error to report?
    if (error) return error;

    // Check FTP response code
    if (!FTP_REPLY_CODE_2YZ(replyCode))
        return ERROR_UNEXPECTED_RESPONSE;

    // Successful processing
    return 0;
}

/**
 * @brief Set the port to be used in data connection
 * @param[in] context Pointer to the FTP client context
 * @param[in] ipAddr Host address
 * @param[in] port Port number
 * @return Error code
 **/
int ftpSetPort(FtpClientContext *context, const uint32_t *ipAddr, uint16_t port)
{
    int error;
    uint32_t replyCode;
    char *p;

    // Invalid context?
    if (context == NULL)
        return ERROR_INVALID_PARAMETER;

#if 1
    // IPv4 FTP client?
    if (1)//(ipAddr->length == sizeof(Ipv4Addr))
    {
        // Format the PORT command
        strcpy((char*)context->buffer, "PORT ");

        // Append host address
        ipv4AddrToString((uint32_t)ipAddr, (char*)context->buffer + 5);

        // Parse the resulting string
        for (p = (char*)context->buffer; *p != '\0'; p++)
        {
            // Change dots to commas
            if (*p == '.') *p = ',';
        }

        // Append port number
        sprintf(p, "%d,%d\r\n", (port >> 8), (port & 0xFF));
    }
    else
#endif
    // Invalid IP address?
    {
        // Report an error
        return ERROR_INVALID_ADDRESS;
    }

    // Send the command to the server
    error = ftpSendCommand(context, (char const*)context->buffer, &replyCode);
    // Any error to report?
    if (error) return error;

    // Check FTP response code
    if (!FTP_REPLY_CODE_2YZ(replyCode))
        return ERROR_UNEXPECTED_RESPONSE;

    // Successful processing
    return 0;
}

/**
 * @brief Enter passive mode
 * @param[in] context Pointer to the FTP client context
 * @param[out] port The port number the server is listening on
 * @return Error code
 **/
int ftpSetPassiveMode(FtpClientContext *context, uint16_t *port)
{
    int error;
    uint32_t replyCode;
    char delimiter;
    char *p;

    // Invalid context?
    if (context == NULL)
        return ERROR_INVALID_PARAMETER;

    #if 1 //(IPV4_SUPPORT == ENABLED)
    // IPv4 FTP server?
    if (1)//(context->serverAddr.length == sizeof(Ipv4Addr))
    {
        // Send the command to the server
        error = ftpSendCommand(context, "PASV\r\n", &replyCode);
        // Any error to report?
        if (error) return error;

        // Check FTP response code
        if (!FTP_REPLY_CODE_2YZ(replyCode))
            return ERROR_UNEXPECTED_RESPONSE;

        // Delimiter character
        delimiter = ',';

        // Retrieve the low byte of the port number
        p = strrchr((char const*)context->buffer, delimiter);
        // Failed to parse the response?
        if (!p) return ERROR_INVALID_SYNTAX;

        // Convert the resulting string
        *port = atoi(p + 1);
        // Split the string
        *p = '\0';

        // Retrieve the high byte of the port number
        p = strrchr((char const*)context->buffer, delimiter);
        // Failed to parse the response?
        if (!p) return ERROR_INVALID_SYNTAX;

        // Convert the resulting string
        *port |= atoi(p + 1) << 8;
    }
    else
    #endif
    //Invalid IP address?
    {
        // Report an error
        return ERROR_INVALID_ADDRESS;
    }

    // Successful processing
    return FTP_NO_ERROR;
}


/**
 * @brief Set representation type
 * @param[in] context Pointer to the FTP client context
 * @param[in] type Single character identifying the desired type
 * @return Error code
 **/
int ftpSetType(FtpClientContext *context, char type)
{
    int error;
    uint32_t replyCode;

    // Invalid context?
    if (context == NULL)
        return ERROR_INVALID_PARAMETER;

    // Format the TYPE command
    sprintf((char*)context->buffer, "TYPE %c\r\n", type);

    // Send the command to the server
    error = ftpSendCommand(context, (char const*)context->buffer, &replyCode);
    // Any error to report?
    if (error) return error;

    // Check FTP response code
    if(!FTP_REPLY_CODE_2YZ(replyCode))
        return ERROR_UNEXPECTED_RESPONSE;

    // Successful processing
    return FTP_NO_ERROR;
}

/**
 * @brief Get the working directory from the FTP server
 * @param[in] context Pointer to the FTP client context
 * @param[out] path Output buffer where to store the current directory
 * @param[in] size Size of the output buffer
 * @return Error code
 **/
int ftpGetWorkingDir(FtpClientContext *context, char *path, uint32_t size)
{
    int error;
    uint32_t length;
    uint32_t replyCode;
    char *p;

    //Invalid context?
    if (context == NULL)
        return ERROR_INVALID_PARAMETER;
    // Check parameters
    if (path == NULL || size == 0)
        return ERROR_INVALID_PARAMETER;

    // Send the command to the server
    error = ftpSendCommand(context, "PWD\r\n", &replyCode);
    // Any error to report?
    if (error) return error;

    // Check FTP response code
    if (!FTP_REPLY_CODE_2YZ(replyCode))
        return ERROR_UNEXPECTED_RESPONSE;

    // Search for the last double quote
    p = strrchr((char*)context->buffer, '\"');
    // Failed to parse the response?
    if (!p) return ERROR_INVALID_SYNTAX;

    // Split the string
    *p = '\0';

    // Search for the first double quote
    p = strchr((char const*)context->buffer, '\"');
    // Failed to parse the response?
    if (!p) return ERROR_INVALID_SYNTAX;

    // Retrieve the length of the working directory
    length = strlen(p + 1);
    // Limit the number of characters to copy
    if (length > size - 1)
    {
        length = size - 1;
    }
    
    // Copy the string
    strncpy(path, p + 1, length);
    // Properly terminate the string with a NULL character
    path[length] = '\0';

    // Successful processing
    return 0;
}

/**
 * @brief Open a file for reading, writing, or appending
 * @param[in] context Pointer to the FTP client context
 * @param[in] path Path to the file to be be opened
 * @param[in] flags Access mode
 * @return Error code
 **/
int ftpOpenFile(FtpClientContext *context, const char *path, uint32_t flags)
{
    int error;
    uint32_t replyCode;
    uint32_t ipAddr;
    uint16_t port;
    NET_ERR err;

    // Invalid context?
    if (context == NULL)
        return ERROR_INVALID_PARAMETER;

    context->dataSocket = NetSock_Open(NET_SOCK_ADDR_FAMILY_IP_V4, NET_SOCK_TYPE_STREAM, NET_SOCK_PROTOCOL_TCP, &err);
    if ((err != NET_SOCK_ERR_NONE) || (context->dataSocket < 0))
    {
        return ERROR_OPEN_FAILED;
    }

    // Start of exception handling block
    do
    {
        //Set representation type
        if (flags & FTP_TEXT_TYPE)
        {
            // Use ASCII type
            error = ftpSetType(context, 'A');
            // Any error to report?
            if(error) break;
        }
        else
        {
            // Use image type
            error = ftpSetType(context, 'I');
            // Any error to report?
            if(error) break;
        }
    
        // Check transfer mode
        if (!context->passiveMode)
        {
            NET_SOCK_ADDR_IP    server_sock_addr_ip;
            
            Mem_Clr((void*)&server_sock_addr_ip, (CPU_SIZE_T)sizeof(server_sock_addr_ip));
            server_sock_addr_ip.Family = NET_SOCK_ADDR_FAMILY_IP_V4;
            server_sock_addr_ip.Addr       = NET_UTIL_HOST_TO_NET_32(NET_SOCK_ADDR_IP_WILD_CARD);
            server_sock_addr_ip.Port       = NET_UTIL_HOST_TO_NET_16(FTP_DATA_PORT);
            
            NetSock_Bind(context->dataSocket, (NET_SOCK_ADDR *)&server_sock_addr_ip, (NET_SOCK_ADDR_LEN)NET_SOCK_ADDR_SIZE, (NET_ERR *)&err);
            if (err != NET_SOCK_ERR_NONE) 
            {
                NetSock_Close(context->dataSocket, &err);
                context->dataSocket = -1;
                return -1;
            }
        
            NetSock_Listen(context->dataSocket, 1, &err);
            if (err != NET_SOCK_ERR_NONE) 
            {
                NetSock_Close(context->dataSocket, &err);
                context->dataSocket = -1;
                return -1;
            }

            ipAddr = server_sock_addr_ip.Addr;
            port = FTP_DATA_PORT;
            
            // Set the port to be used in data connection
            error = ftpSetPort(context, &ipAddr, port);
            // Any error to report?
            if (error) break;
        }
        else
        {
            NET_SOCK_ADDR_IP    server_sock_addr_ip;
            CPU_INT32U time_stamp;
                
            // Enter passive mode
            error = ftpSetPassiveMode(context, &port);
            // Any error to report?
            if (error) break;
    
            memset(&server_sock_addr_ip, 0, sizeof(server_sock_addr_ip));
            server_sock_addr_ip.Family = NET_SOCK_ADDR_FAMILY_IP_V4;
            server_sock_addr_ip.Addr = NET_UTIL_HOST_TO_NET_32(context->serverAddr);
            server_sock_addr_ip.Port = NET_UTIL_HOST_TO_NET_16(port);
        
            time_stamp = OSTimeGet();
            NetSock_Conn((NET_SOCK_ID)context->dataSocket, (NET_SOCK_ADDR *)&server_sock_addr_ip, (NET_SOCK_ADDR_LEN)sizeof(server_sock_addr_ip), &err);
            while (NetSock_IsConn((NET_SOCK_ID)context->dataSocket, &err) != DEF_YES)
            {
                if ((err != NET_SOCK_ERR_CONN_IN_PROGRESS) && (err != NET_SOCK_ERR_NONE))
                {
                    break;
                }
                if (OSTimeGet() - time_stamp > FTP_CLIENT_DEFAULT_TIMEOUT)
                {
                    error = -1;
                    break;
                }
                OSTimeDly(2);
            }
        
            if ((err != NET_SOCK_ERR_NONE) || (error))
            {
                break;
            }
        }
    
        // Format the command
        if (flags & FTP_FOR_WRITING)
            sprintf((char*)context->buffer, "STOR %s\r\n", path);
        else if(flags & FTP_FOR_APPENDING)
            sprintf((char*)context->buffer, "APPE %s\r\n", path);
        else
            sprintf((char*)context->buffer, "RETR %s\r\n", path);
    
        // Send the command to the server
        error = ftpSendCommand(context, (char const*)context->buffer, &replyCode);
        // Any error to report?
        if (error) break;
    
        // Check FTP response code
        if (!FTP_REPLY_CODE_1YZ(replyCode))
        {
            // Report an error
            error = ERROR_UNEXPECTED_RESPONSE;
            break;
        }
    
        // Check transfer mode
        if (!context->passiveMode)
        {
            NET_SOCK_ADDR_IP    client_sock_addr_ip;
            NET_SOCK_ADDR_LEN   client_sock_addr_ip_size = sizeof(client_sock_addr_ip);
            NET_SOCK_ID         newSockID;
            CPU_BOOLEAN         attempt_conn;
            CPU_INT32U time_stamp;

            time_stamp = OSTimeGet();
            do 
            {
                if (NetNIC_ConnStatusGet() != DEF_ON)
                {
                    err = NET_SOCK_ERR_NONE_AVAIL;
                    break;
                }
                
                if (OSTimeGet() - time_stamp > FTP_CLIENT_DEFAULT_TIMEOUT)
                {
                    err = NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT;
                    break;
                }
    
                OSTimeDly(2);
                
                newSockID = NetSock_Accept((NET_SOCK_ID )context->dataSocket, (NET_SOCK_ADDR *)&client_sock_addr_ip, (NET_SOCK_ADDR_LEN *)&client_sock_addr_ip_size, (NET_ERR *)&err);
                switch (err) 
                {
                    case NET_SOCK_ERR_NONE:
                        attempt_conn = DEF_NO;
                        break;
                    case NET_ERR_INIT_INCOMPLETE:
                    case NET_SOCK_ERR_NULL_PTR:
                    case NET_SOCK_ERR_NONE_AVAIL:
                    case NET_SOCK_ERR_CONN_ACCEPT_Q_NONE_AVAIL:
                    case NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT:
                    case NET_OS_ERR_LOCK:
                        attempt_conn = DEF_YES;
                        break;
                    default:
                        attempt_conn = DEF_NO;
                        break;
                }
            } while (attempt_conn == DEF_YES);

            // No connection request?
            if ((newSockID < 0) || (err != NET_SOCK_ERR_NONE))
            {
                // Report an error
                error = -1;
                break;
            }
    
            // Close the listening socket
            NetSock_Close(context->dataSocket, &err);
            context->dataSocket = newSockID;
        }

    // End of exception handling block
    } while(0);

    // Any error to report?
    if (error)
    {
        // Clean up side effects
        NetSock_Close(context->dataSocket, &err);
        context->dataSocket = -1;
    }

    // Return status code
    return error;
}

/**
 * @brief Write to a remote file
 * @param[in] context Pointer to the FTP client context
 * @param[in] data Pointer to a buffer containing the data to be written
 * @param[in] length Number of data bytes to write
 * @param[in] flags Set of flags that influences the behavior of this function
 * @return Error code
 **/
int ftpWriteFile(FtpClientContext *context, const void *data, uint32_t length, uint32_t flags)
{
    // Invalid context?
    if (context == NULL)
        return ERROR_INVALID_PARAMETER;

    OSTimeDly(10);
    if (HostWriteDataTimeout(context->dataSocket, (char*)data, length, FTP_CLIENT_WRITE_TIMEOUT) != length)
    {
        return -1;
    }
    OSTimeDly(20);

    // Transmit data to the FTP server
    return FTP_NO_ERROR;
}


/**
 * @brief Close file
 * @param[in] context Pointer to the FTP client context
 * @return Error code
 **/
int ftpCloseFile(FtpClientContext *context)
{
    NET_ERR err;
    int error;
    uint32_t replyCode;

    // Invalid context?
    if (context == NULL)
        return ERROR_INVALID_PARAMETER;

    // Close the data socket
    NetSock_Close(context->dataSocket, &err);
    context->dataSocket = -1;

    // Check the transfer status
    error = ftpSendCommand(context, NULL, &replyCode);
    // Any error to report?
    if (error) return error;

    // Check FTP response code
    if (!FTP_REPLY_CODE_2YZ(replyCode))
        return ERROR_UNEXPECTED_RESPONSE;

    // Successful processing
    return FTP_NO_ERROR;
}

/**
 * @brief Delete a file
 * @param[in] context Pointer to the FTP client context
 * @param[in] path Path to the file to be be deleted
 * @return Error code
 **/
int ftpDeleteFile(FtpClientContext *context, const char *path)
{
   int error;
   uint32_t replyCode;

   //Invalid context?
   if(context == NULL)
      return ERROR_INVALID_PARAMETER;

   //Format the DELE command
   sprintf((char*)context->buffer, "DELE %s\r\n", path);

   //Send the command to the server
   error = ftpSendCommand(context, (char const*)context->buffer, &replyCode);
   //Any error to report?
   if(error) return error;

   //Check FTP response code
   if(!FTP_REPLY_CODE_2YZ(replyCode))
      return ERROR_UNEXPECTED_RESPONSE;

   //Successful processing
   return 0;
}

/**
 * @brief Close the connection with the FTP server
 * @param[in] context Pointer to the FTP client context
 * @return Error code
 **/
int ftpClose(FtpClientContext *context)
{
    NET_ERR err;
    
    // Invalid context?
    if (context == NULL)
    {
        return ERROR_INVALID_PARAMETER;
    }
    
    // Close data socket
    if (context->dataSocket >= 0)
    {
        NetSock_Close(context->dataSocket, &err);
        context->dataSocket = -1;
    }

    // Close control socket
    if (context->controlSocket)
    {
        NetSock_Close(context->controlSocket, &err);
        context->controlSocket = -1;
    }

    // Successful processing
    return 0;
}


/**
 * @brief Send FTP command and wait for a reply
 * @param[in] context Pointer to the FTP client context
 * @param[in] command Command line
 * @param[out] replyCode Response code from the FTP server
 * @return Error code
 **/
int ftpSendCommand(FtpClientContext *context, const char *command, uint32_t *replyCode)
{
    int length;
    char *p;

    // Any command line to send?
    if (command)
    {
        if (HostWriteDataTimeout(context->controlSocket, (char*)command, strlen(command), FTP_CLIENT_WRITE_TIMEOUT) != strlen(command))
        {
            return -1;
        }
    }

    // Multiline replies are allowed for any command
    while (1)
    {
        // Wait for a response from the server
        NET_ERR err;
        length = HostReadData(context->controlSocket, (char *)context->buffer, FTP_CLIENT_BUFFER_SIZE - 1, FTP_CLIENT_DEFAULT_TIMEOUT, &err);
        if (length <= 0)
        {
            return ERROR_EMPTY_RECEIVE;
        }
        
        // Point to the beginning of the buffer
        p = (char*)context->buffer;
        // Properly terminate the string with a NULL character
        p[length] = '\0';

        // Remove trailing whitespace from the response
        strRemoveTrailingSpace(p);

        // Check the length of the response
        if (strlen(p) >= 3)
        {
            // All replies begin with a three digit numeric code
            if (isdigit(p[0]) && isdigit(p[1]) && isdigit(p[2]))
            {
                // A space character follows the response code for the last line
                if (p[3] == ' ' || p[3] == '\0')
                {
                    // Get the server response code
                    *replyCode = strtoul(p, NULL, 10);
                    // Exit immediately
                    break;
                }
            }
        } 
    }

    // Successful processing
    return 0;
}


/**
 * @brief Change the current working directory of the FTP session
 * @param[in] context Pointer to the FTP client context
 * @param[in] path The new current working directory
 * @return Error code
 **/
int ftpChangeWorkingDir(FtpClientContext *context, const char *path)
{
   int error;
   uint32_t replyCode;

   //Invalid context?
   if(context == NULL)
      return ERROR_INVALID_PARAMETER;

   //Format the CWD command
   sprintf((char*)context->buffer, "CWD %s\r\n", path);

   //Send the command to the server
   error = ftpSendCommand(context, (char*)context->buffer, &replyCode);
   //Any error to report?
   if(error) return error;

   //Check FTP response code
   if(!FTP_REPLY_CODE_2YZ(replyCode))
      return ERROR_UNEXPECTED_RESPONSE;

   //Successful processing
   return 0;
}


/**
 * @brief Create a new directory
 * @param[in] context Pointer to the FTP client context
 * @param[in] path The name of the new directory
 * @return Error code
 **/
int ftpMakeDir(FtpClientContext *context, const char *path)
{
   int error;
   uint32_t replyCode;

   //Invalid context?
   if(context == NULL)
      return ERROR_INVALID_PARAMETER;

   //Format the MKD command
   sprintf((char*)context->buffer, "MKD %s\r\n", path);

   //Send the command to the server
   error = ftpSendCommand(context, (char*)context->buffer, &replyCode);
   //Any error to report?
   if(error) return error;

   //Check FTP response code
   if(!FTP_REPLY_CODE_2YZ(replyCode))
      return ERROR_UNEXPECTED_RESPONSE;

   //Successful processing
   return 0;
}

/**
 * @brief Change the current working directory to the parent directory
 * @param[in] context Pointer to the FTP client context
 * @return Error code
 **/
int ftpChangeToParentDir(FtpClientContext *context)
{
   int error;
   uint32_t replyCode;

   //Invalid context?
   if(context == NULL)
      return ERROR_INVALID_PARAMETER;

   //Send the command to the server
   error = ftpSendCommand(context, "CDUP\r\n", &replyCode);
   //Any error to report?
   if(error) return error;

   //Check FTP response code
   if(!FTP_REPLY_CODE_2YZ(replyCode))
      return ERROR_UNEXPECTED_RESPONSE;

   //Successful processing
   return 0;
}
