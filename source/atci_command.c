#include "atci.h"
#include "atci_command.h"
#include "atci_handler.h"

static atci_status atci_cmd_handler_test(atci_cmd_param *parse_cmd)
{

    return ATCI_STATUS_OK;
}

static atci_cmd_handler_item g_atcmd_table[] = {
    {"AT+TEST", atci_cmd_handler_test, 0, 0},
}

static void atci_command_init(void)
{
    
}