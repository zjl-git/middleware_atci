#ifndef __ATCI_H__
#define __ATCI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "atci_ports.h"
#include "atci_config.h"

#define ATCI_HASH_TABLE_ROW                                 (37)
#define ATCI_HASH_TABLE_SPAN                                (5)
#define ATCI_MAX_CMD_NAME_LEN                               (2*ATCI_HASH_TABLE_SPAN)
#define ATCI_MAX_CMD_HEAD_LEN                               (ATCI_MAX_CMD_NAME_LEN+3)

typedef enum {
    ATCI_STATUS_REGISTRATION_FAILURE = -2,                  /*Failed to register the AT command handler table. */
    ATCI_STATUS_ERROR = -1,                                 /*An error occurred during the function call. */
    ATCI_STATUS_OK = 0                                      /*No error occurred during the function call. */
} atci_status;

typedef enum {
    ATCI_CMD_MODE_READ,                                     /*Read mode command, such as "AT+CMD?".*/
    ATCI_CMD_MODE_ACTIVE,                                   /*Active mode command, such as "AT+CMD".*/
    ATCI_CMD_MODE_EXECUTION,                                /*Execute mode command, such as "AT+CMD=<op>".*/
    ATCI_CMD_MODE_TESTING,                                  /*Test mode command, such as "AT+CMD=?".*/
    ATCI_CMD_MODE_INVALID                                   /*The input command doesn't belong to any of the four types.*/
} atci_cmd_mode;

typedef enum {
    ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR = 0x00000002,      /*Auto append "\r\n" at the end of the response string. */
    ATCI_RESPONSE_FLAG_URC_FORMAT = 0x00000010,             /*The URC notification flag. */
    ATCI_RESPONSE_FLAG_QUOTED_WITH_LF_CR = 0x00000020,      /*Auto append "\r\n" at the begining and end of the response string. */
    ATCI_RESPONSE_FLAG_APPEND_OK = 0x00000040,              /*Auto append "OK\r\n" at the end of the response string. */
    ATCI_RESPONSE_FLAG_APPEND_ERROR = 0x00000080            /*Auto append "ERROR\r\n" at the end of the response string. */
} atci_response_flag;

/*msg*/
typedef enum {
    ATCI_MSG_ID_IDLE = 0,
    ATCI_MSG_ID_MUX_DATA_READY,
    ATCI_MSG_ID_SEND_RSP_NOTIFY,
    ATCI_MSG_ID_SEND_URC_NOTIFY,
    ATCI_MSG_ID_MAX,
} atci_msg_id;

typedef struct {
    atci_msg_id     msg_id;
    uint8_t         *msg_data;
    uint16_t        msg_len;
    atci_port       port;
} atci_general_msg;

/*parse*/
typedef struct {
    uint8_t         *string_ptr;
    uint32_t        string_len;
    uint32_t        name_len;                           /*AT command name length. ex. In "AT+EXAMPLE=1,2,3", name_len = 10 (not include = symbol) */
    uint32_t        parse_pos;                          /*parse_pos means the length after detecting AT command mode */
    atci_cmd_mode   mode;
    atci_port       port;

    uint32_t        hash_value1;
    uint32_t        hash_value2;
} atci_parse_param;

/*resp*/
typedef struct {
    uint8_t     response_buf[ATCI_UART_TX_BUFFER_SIZE];         /*The response data buffer*/
    uint16_t    response_len;                                  /*The actual data length of response_buf*/
    uint32_t    response_flag;                                 /*For more information, please refer to #atci_response_flag_t*/
    atci_port   response_port;
} atci_response;

/*handler*/
typedef struct {
    char            *string_ptr;
    uint32_t        string_len;
    uint32_t        name_len;
    uint32_t        parse_pos;
    atci_cmd_mode   mode;
    atci_port       port;
} atci_cmd_param;

typedef void (*at_cmd_handler_fp)(atci_cmd_param *cmd_param);
typedef struct {
    char                *command_head;
    at_cmd_handler_fp   command_handler;
    uint32_t            hash_value1;
    uint32_t            hash_value2;
} atci_cmd_handler_item;

typedef struct {
    atci_cmd_handler_item *item_table;
    uint32_t item_table_size;
} atci_cmd_handler_table;


/**/
typedef void (*atci_general_msg_handler)(atci_general_msg *msg);
typedef struct {
    uint8_t inited;
    uint32_t msg_queue;
    atci_general_msg_handler msg_handler[ATCI_MSG_ID_MAX];
} atci_ctrl;

extern atci_ctrl g_atci_ctrl;

void atci_send_msg(atci_general_msg *msg);

void atci_receive_callback(atci_port port, uint8_t *data, uint16_t data_len, uint8_t from_isr);

void atci_send_response(atci_response *response);

void atci_init(void);


#ifdef __cplusplus
}
#endif

#endif 
