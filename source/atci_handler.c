#include "atci.h"
#include "atci_handler.h"

#define ATCI_INVALID_COMMAND_HASH_VALUE         (0xffff)
#define ATCI_IS_LOWER(alpha_char)               (((alpha_char >= ATCI_char_a) && (alpha_char <= ATCI_char_z) ) ?  1 : 0)
#define ATCI_IS_UPPER(alpha_char)               (((alpha_char >= ATCI_CHAR_A) && (alpha_char <= ATCI_CHAR_Z) ) ? 1 : 0)
#define ATCI_IS_HEX_ALPHA(alpha_char)           (((alpha_char >= ATCI_CHAR_A) && (alpha_char <= ATCI_CHAR_F) ) ? 1 : 0)
#define ATCI_IS_NUMBER(alpha_char)              (((alpha_char >= ATCI_CHAR_0) && (alpha_char <= ATCI_CHAR_9) ) ? 1 : 0)
#define ATCI_IS_ALPHA(alpha_char)               ((ATCI_IS_UPPER(alpha_char) || ATCI_IS_LOWER(alpha_char) ) ? 1 : 0)

#define ATCI_SET_OUTPUT_PARAM_STRING(s, ptr, len, port, flag)      \
    {                                                              \
        memcpy((void*)s.response_buf, (uint8_t*)ptr, len);         \
        s.response_len = (uint32_t)(len);                          \
        s.response_flag = (uint32_t)(flag);                        \
        s.response_port = port;                                    \
    }


static atci_cmd_handler_table g_atci_general_handler_tables[ATCI_MAX_GNENERAL_TABLE_NUM] = {{0}};
uint32_t g_atci_registered_table_number = 0;

static uint16_t atci_caculate_hash_value(uint8_t *at_name, uint32_t *hash_value1, uint32_t *hash_value2) 
{
    uint16_t i = 0, counter = 0;
    char ascii_char = 0;
    uint32_t value1 = 0;
    uint32_t value2 = 0;

    (*hash_value1) = ATCI_INVALID_COMMAND_HASH_VALUE;
    (*hash_value2) = ATCI_INVALID_COMMAND_HASH_VALUE;
    if ((at_name[0] == ATCI_CHAR_A && at_name[1] == ATCI_CHAR_T) || (at_name[0] == ATCI_char_a && at_name[1] == ATCI_char_t)) {
        /*only support 'AT+XX' 'AT#XX'*/
        if (at_name[2] == ATCI_CHAR_PLUS || at_name[2] == ATCI_CHAR_POUND) {
            /*caculate hash value after ("AT+" or "AT#") until entering ('=' or '?' or CR/LF/NULL)*/
            i = 3;
            counter = 3;
            while((at_name[i] != ATCI_EQUAL) && (at_name[i] != ATCI_QUESTION_MARK) && (at_name[i] != ATCI_CR_C) && 
                  (at_name[i] != ATCI_LF_C) && (at_name[i] != '\0')) {
                if (ATCI_IS_UPPER(at_name[i])) {
                    ascii_char = at_name[i] - ATCI_CHAR_A;
                } else if (ATCI_IS_LOWER(at_name[i])) {
                    ascii_char = at_name[i] - ATCI_char_a;
                } else if (ATCI_IS_NUMBER(at_name[i])) {
                    ascii_char = at_name[i] - ATCI_CHAR_0;
                }

                if (counter < (ATCI_HASH_TABLE_SPAN + 3)) {
                    value1 = value1 * (ATCI_HASH_TABLE_ROW + 1) + (ascii_char + 1); /* 0 ~ 4*/
                } else if (counter < (ATCI_MAX_CMD_NAME_LEN + 3)) {
                    value2 = value2 * (ATCI_HASH_TABLE_ROW + 1) + (ascii_char + 1); /* 5 ~ 9*/
                }
                i++;
                counter++;
                
            }
        }
    }
    (*hash_value1) = value1;
    (*hash_value2) = value2;
    return counter;
}

static atci_status atci_dispatch_general_handler(atci_parse_param *parse_cmd)
{
    atci_status ret = ATCI_STATUS_ERROR;
    uint32_t item_table_size, i, j;
    atci_cmd_handler_item *handler_item = NULL;

    for (i = 0; i < g_atci_registered_table_number; i++) {
        item_table_size = g_atci_general_handler_tables[i].item_table_size;

        if (ret = ATCI_STATUS_OK) {
            break;
        }

        for (j = 0; j < item_table_size; j++) {
            handler_item = &(g_atci_general_handler_tables[i].item_table[j]);
            if (parse_cmd->hash_value1 == handler_item->hash_value1 && parse_cmd->hash_value2 == handler_item->hash_value2 &&
                !strncmp((char *)parse_cmd->string_ptr, handler_item->command_head, parse_cmd->name_len)) {
                ret = ATCI_STATUS_OK;
                break;
            }
            handler_item = NULL;
        }
    }

    /*executing at command handler*/
    if (handler_item != NULL && ret == ATCI_STATUS_OK) {
        handler_item->command_handler((atci_cmd_param*)parse_cmd);
    }
    return ret;
}

static atci_status atci_parse_extend_cmd(atci_parse_param *parse_cmd)
{
    uint32_t name_len = 0;
    uint8_t *at_name = parse_cmd->string_ptr;

    name_len = atci_caculate_hash_value(at_name, &(parse_cmd->hash_value1), &(parse_cmd->hash_value2));
    parse_cmd->name_len = name_len;
    parse_cmd->parse_pos = name_len;

    if (name_len == 0) {
        return ATCI_STATUS_ERROR;
    } else {
        return ATCI_STATUS_OK;
    }
}

static atci_status atci_parse_basic_cmd(atci_parse_param *parse_cmd)
{
    uint32_t name_len = 2;                  /*AT command begins "AT"*/
    uint8_t  *string_ptr = parse_cmd->string_ptr;
    uint32_t length = parse_cmd->string_len;

    while (name_len < length) {
        if ((string_ptr[name_len]  == ATCI_CR_C) || (string_ptr[name_len]  == ATCI_LF_C || (string_ptr[name_len]  == '\0'))) {
            break;
        }
        name_len++;
    }
    parse_cmd->name_len = name_len;
    parse_cmd->parse_pos = name_len;
    return ATCI_STATUS_OK;
}

static atci_status atci_parse_find_mode(atci_parse_param *parse_cmd)
{
    atci_cmd_mode mode = ATCI_CMD_MODE_INVALID;
    uint16_t index = parse_cmd->name_len;
    char *string_ptr = (char *)parse_cmd->string_ptr;

    if (string_ptr[index] == ATCI_QUESTION_MARK) {
        index++;
        if (string_ptr[index] == ATCI_CR_C || string_ptr[index] == ATCI_LF_C || string_ptr[index] == '\0') {
            mode = ATCI_CMD_MODE_READ;
        }
    } else if (string_ptr[index] == ATCI_EQUAL) {
        index++;
        if (string_ptr[index] == ATCI_QUESTION_MARK) {
            index++;
            if (string_ptr[index] == ATCI_CR_C || string_ptr[index] == ATCI_LF_C || string_ptr[index] == '\0') {
                mode = ATCI_CMD_MODE_TESTING;
            }
        } else {
            mode = ATCI_CMD_MODE_EXECUTION;
        }
    } else if (string_ptr[index] == ATCI_CR_C || string_ptr[index] == ATCI_LF_C || string_ptr[index] == '\0') {
        mode = ATCI_CMD_MODE_ACTIVE;
    }
    parse_cmd->parse_pos = index;
    parse_cmd->mode = mode;
    return ATCI_STATUS_OK;
}

static atci_status atci_parse_at_cmd(atci_parse_param *parse_cmd)
{
    atci_status ret;
    char *string_ptr = (char *)parse_cmd->string_ptr;
    uint32_t length = parse_cmd->string_len;
    uint16_t index = 0;

    /*AT<CR><LF> is the shortest length*/
    if (length < 4) {
        return ATCI_STATUS_ERROR;
    }

    /*skip not AT*/
    while (index < length - 1) {
        if (string_ptr[index] == ATCI_CHAR_A || string_ptr[index] == ATCI_char_a) {
            if (string_ptr[index + 1] == ATCI_CHAR_T || string_ptr[index + 1] == ATCI_char_t) {
                break;
            }
        }
        index += 1;
    }

    if (index >= length - 1) {
        return ATCI_STATUS_ERROR;
    }
    index += 2;

    /*Calculate has value and extend AT command: AT+, AT#, AT%, AT*, AT^, AT$*/
    if (string_ptr[index] == ATCI_CHAR_PLUS || string_ptr[index] == ATCI_CHAR_POUND || string_ptr[index] == ATCI_CHAR_PERCENT ||
        string_ptr[index] == ATCI_CHAR_STAR || string_ptr[index] == ATCI_HAT || string_ptr[index] == ATCI_MONEY) {
        ret = atci_parse_extend_cmd(parse_cmd);
        atci_parse_find_mode(parse_cmd);
    } else {
        parse_cmd->parse_pos = index;
        ret = atci_parse_basic_cmd(parse_cmd);
    }
    return ret;
}

atci_status atci_input_command_handler(uint8_t *cmd_msg, atci_port port)
{
    atci_status ret = ATCI_STATUS_ERROR;
    atci_parse_param parse_cmd;
    atci_response response = {{0}};
    if (cmd_msg == NULL) {
        return ret;
    }

    parse_cmd.port = port;
    parse_cmd.string_ptr = cmd_msg;
    parse_cmd.string_len = strlen((char *)cmd_msg);

    if (parse_cmd.string_ptr[0] == ATCI_CHAR_A || parse_cmd.string_ptr[0] == ATCI_char_a &&
        parse_cmd.string_ptr[1] == ATCI_CHAR_T || parse_cmd.string_ptr[1] == ATCI_char_t &&
        parse_cmd.string_ptr[1] == ATCI_CR_C || parse_cmd.string_ptr[1] == ATCI_LF_C|| parse_cmd.string_ptr[1] == '\0') {
        
        ATCI_SET_OUTPUT_PARAM_STRING(response, "OK", strlen("OK"), port, ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR);
        atci_send_response(&response);
    }

    /*change lowercase letters to uppercase letters*/
    for (uint32_t i = 0; i < parse_cmd.string_len; i++) {
        if (parse_cmd.string_ptr[i] == ATCI_EQUAL) {
            break;
        }

        if (parse_cmd.string_ptr[i] >= 'a' && parse_cmd.string_ptr[i] <= 'z') {
            parse_cmd.string_ptr[i] -= 32;
        }
    }

    /*Parse AT command and calculate hash value*/
    ret = atci_parse_at_cmd(&parse_cmd);
    if (ret != ATCI_STATUS_ERROR) {
        ret = atci_dispatch_general_handler(&parse_cmd);
    }

    if (ret != ATCI_STATUS_OK) {
        ATCI_SET_OUTPUT_PARAM_STRING(response, "ERROR", strlen("ERROR"), port, ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR);
        atci_send_response(&response);
    }
    return ret;
}

static uint16_t atci_process_tail_len(uint32_t flag)
{
    uint16_t tail_len = 0;

    if (ATCI_RESPONSE_FLAG_APPEND_OK == (flag & ATCI_RESPONSE_FLAG_APPEND_OK)) {
        /*append 'O','K',CR,LF*/
        tail_len += 4;
    }
    if (ATCI_RESPONSE_FLAG_APPEND_ERROR == (flag & ATCI_RESPONSE_FLAG_APPEND_ERROR)) {
        /*append 'E','R','R','O','R',CR,LF*/
        tail_len += 7;
    }
    if (ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR == (flag & ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR)) {
        /*APPEND BACK CR LF*/
        tail_len += 2;
    }
    if (ATCI_RESPONSE_FLAG_QUOTED_WITH_LF_CR == (flag & ATCI_RESPONSE_FLAG_QUOTED_WITH_LF_CR)) {
        /*APPEND FRONT CR LF*/
        tail_len += 2;
        /*APPEND BACK CR LF*/
        tail_len += 2;
    }

    return tail_len;
}

static uint16_t atci_process_flag(uint8_t *str_ptr, uint16_t str_len_ptr, uint16_t str_max_len, uint32_t flag)
{
    uint16_t str_len = str_len_ptr;
    int32_t i;

    if (ATCI_RESPONSE_FLAG_APPEND_OK == (flag & ATCI_RESPONSE_FLAG_APPEND_OK)) {
        /*append 'O', 'K', CR, LF*/
        if (str_len + 4 > str_max_len) {
            return ATCI_STATUS_ERROR;
        }
        str_ptr[str_len] = ATCI_CHAR_O;
        str_ptr[str_len + 1] = ATCI_CHAR_K;
        str_ptr[str_len + 2] = ATCI_CR_C;
        str_ptr[str_len + 3] = ATCI_LF_C;
        str_len += 4;
    }
    if (ATCI_RESPONSE_FLAG_APPEND_ERROR == (flag & ATCI_RESPONSE_FLAG_APPEND_ERROR)) {
        /*append 'E','R','R','O','R', CR, LF*/
        if (str_len + 7 > str_max_len) {
            return ATCI_STATUS_ERROR;
        }
        str_ptr[str_len] = ATCI_CHAR_E;
        str_ptr[str_len + 1] = ATCI_CHAR_R;
        str_ptr[str_len + 2] = ATCI_CHAR_R;
        str_ptr[str_len + 3] = ATCI_CHAR_O;
        str_ptr[str_len + 4] = ATCI_CHAR_R;
        str_ptr[str_len + 5] = ATCI_CR_C;
        str_ptr[str_len + 6] = ATCI_LF_C;
        str_len += 7;
    }
    if (ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR == (flag & ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR)) {
        if (str_len + 2 > str_max_len) {
            return ATCI_STATUS_ERROR;
        }
        /*APPEND BACK CR LF*/
        str_ptr[str_len] = ATCI_CR_C;
        str_ptr[str_len + 1] = ATCI_LF_C;
        str_len += 2;
    }
    if (ATCI_RESPONSE_FLAG_QUOTED_WITH_LF_CR == (flag & ATCI_RESPONSE_FLAG_QUOTED_WITH_LF_CR)) {
        if (str_len + 4 > str_max_len) {
            return ATCI_STATUS_ERROR;
        }

        /*left shift 2 position*/
        for (i = str_len - 1; i >= 0; i--) {
            str_ptr[i + 2] = str_ptr[i];
        }
        /*APPEND FRONT CR LF*/
        str_ptr[0] = ATCI_CR_C;
        str_ptr[1] = ATCI_LF_C;
        str_len += 2;
        /*APPEND BACK CR LF*/
        str_ptr[str_len] = ATCI_CR_C;
        str_ptr[str_len + 1] = ATCI_LF_C;
        str_len += 2;
    }

    /*add null terminal at end of string buffer*/
    if (str_max_len > (str_len)) {
        str_ptr[str_len] = ATCI_END_OF_STRING_CHAR;
    }
    return str_len;
}


atci_status atci_send_response_internal(const uint8_t *rsp, uint16_t rsp_len, uint32_t rsp_flag, atci_port port)
{
    uint16_t total_len;
    atci_general_msg msg;

    if (g_atci_ctrl.inited == false) {
        return ATCI_STATUS_ERROR;
    }
    total_len = atci_process_tail_len(rsp_flag);
    total_len += rsp_len;

    msg.msg_data = (uint8_t *)atci_ports_malloc(total_len);
    if (msg.msg_data == NULL) {
        return ATCI_STATUS_ERROR;
    }

    msg.port = port;
    msg.msg_len = total_len;
    memcpy(msg.msg_data, rsp, rsp_len);
    atci_process_flag(msg.msg_data, rsp_len, total_len, rsp_flag);

    msg.msg_id = ATCI_MSG_ID_SEND_RSP_NOTIFY;
    atci_send_msg(&msg);

    return ATCI_STATUS_OK;
}