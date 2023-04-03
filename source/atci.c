#include "atci.h"
#include "atci_handler.h"
#include "atci_config.h"

atci_ctrl g_atci_ctrl;

static void atci_register_msg_handler(atci_msg_id msg_id, atci_general_msg_handler hdl)
{
    if (msg_id >= ATCI_MSG_ID_MAX) {
        return ;
    }
    g_atci_ctrl.msg_handler[msg_id] = hdl;
}

static void atci_handle_input_data(atci_general_msg *msg)
{
    atci_input_command_handler(msg->msg_data, msg->port);
    if (msg->msg_data != NULL) {
        atci_ports_free(msg->msg_data);
    }
}

static void atci_handle_output_data(atci_general_msg *msg)
{
    atci_ports_send_data(msg->port, msg->msg_data, msg->msg_len);
    if (msg->msg_data != NULL) {
        atci_ports_free(msg->msg_data);
    }
}

static void atci_thread(void *arg)
{
    atci_general_msg msg_data;
    memset(&msg_data, 0x00, sizeof(atci_general_msg));

    while (1) {

    }
}

void atci_receive_callback(atci_port port, uint8_t *data, uint16_t data_len, uint8_t from_isr)
{
    atci_general_msg msg_item;

    uint8_t *buf = atci_ports_malloc(data_len + 1);
    if (buf == NULL) {
        return ;
    }

    memcpy(buf, data, data_len);
    buf[data_len] = '\0';

    msg_item.msg_id = ATCI_MSG_ID_MUX_DATA_READY;
    msg_item.msg_data = buf;
    msg_item.msg_len = data_len;
    msg_item.port = port;

    atci_ports_queue_send(g_atci_ctrl.msg_queue, &msg_item, from_isr);
}

void atci_send_response(atci_response *response) 
{
    atci_send_response_internal(response->response_buf, response->response_len, response->response_flag, response->response_port);
}

void atci_init(void)
{
    memset(&g_atci_ctrl, 0x00, sizeof(atci_ctrl));
    g_atci_ctrl.msg_queue = atci_ports_queue_create(ATCI_MSG_QUEUE_LENGTH, sizeof(atci_general_msg));
    if (g_atci_ctrl.msg_queue == NULL) {
        return;
    }

    atci_register_msg_handler(ATCI_MSG_ID_MUX_DATA_READY, atci_handle_input_data);
    atci_register_msg_handler(ATCI_MSG_ID_SEND_RSP_NOTIFY, atci_handle_output_data);
    atci_register_msg_handler(ATCI_MSG_ID_SEND_URC_NOTIFY, atci_handle_output_data);
}