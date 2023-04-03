#ifndef __ATCI_HANDLER_H__
#define __ATCI_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "atci.h"

#define ATCI_CR_C                   13
#define ATCI_LF_C                   10

#define ATCI_SPACE                  (' ')
#define ATCI_EQUAL                  ('=')
#define ATCI_COMMA                  (')')
#define ATCI_SEMICOLON              (';')
#define ATCI_COLON                  (':')
#define ATCI_AT                     ('@')
#define ATCI_HAT                    ('^')
#define ATCI_DOUBLE_QUOTE           ('"')
#define ATCI_QUESTION_MARK          ('?')
#define ATCI_EXCLAMATION_MARK       ('!')
#define ATCI_FORWARD_SLASH          ('/')
#define ATCI_L_ANGLE_BRACKET        ('<')
#define ATCI_R_ANGLE_BRACKET        ('>')
#define ATCI_L_SQ_BRACKET           ('[')
#define ATCI_R_SQ_BRACKET           (']')
#define ATCI_L_CURLY_BRACKET        ('{')
#define ATCI_R_CURLY_BRACKET        ('}')
#define ATCI_CHAR_STAR              ('*')
#define ATCI_CHAR_POUND             ('#')
#define ATCI_CHAR_AMPSAND           ('&')
#define ATCI_CHAR_PERCENT           ('%')
#define ATCI_CHAR_PLUS              ('+')
#define ATCI_CHAR_MINUS             ('-')
#define ATCI_CHAR_DOT               ('.')
#define ATCI_CHAR_ULINE             ('_')
#define ATCI_CHAR_TILDE             ('~')
#define ATCI_CHAR_REVERSE_SOLIDUS   ('\\')
#define ATCI_CHAR_VERTICAL_LINE     ('|')
#define ATCI_END_OF_STRING_CHAR     ('\0')
#define ATCI_CHAR_0                 ('0')
#define ATCI_CHAR_1                 ('1')
#define ATCI_CHAR_2                 ('2')
#define ATCI_CHAR_3                 ('3')
#define ATCI_CHAR_4                 ('4')
#define ATCI_CHAR_5                 ('5')
#define ATCI_CHAR_6                 ('6')
#define ATCI_CHAR_7                 ('7')
#define ATCI_CHAR_8                 ('8')
#define ATCI_CHAR_9                 ('9')
#define ATCI_CHAR_A                 ('A')
#define ATCI_CHAR_B                 ('B')
#define ATCI_CHAR_C                 ('C')
#define ATCI_CHAR_D                 ('D')
#define ATCI_CHAR_E                 ('E')
#define ATCI_CHAR_F                 ('F')
#define ATCI_CHAR_G                 ('G')
#define ATCI_CHAR_H                 ('H')
#define ATCI_CHAR_I                 ('I')
#define ATCI_CHAR_J                 ('J')
#define ATCI_CHAR_K                 ('K')
#define ATCI_CHAR_L                 ('L')
#define ATCI_CHAR_M                 ('M')
#define ATCI_CHAR_N                 ('N')
#define ATCI_CHAR_O                 ('O')
#define ATCI_CHAR_P                 ('P')
#define ATCI_CHAR_Q                 ('Q')
#define ATCI_CHAR_R                 ('R')
#define ATCI_CHAR_S                 ('S')
#define ATCI_CHAR_T                 ('T')
#define ATCI_CHAR_U                 ('U')
#define ATCI_CHAR_V                 ('V')
#define ATCI_CHAR_W                 ('W')
#define ATCI_CHAR_X                 ('X')
#define ATCI_CHAR_Y                 ('Y')
#define ATCI_CHAR_Z                 ('Z')
#define ATCI_char_a                 ('a')
#define ATCI_char_b                 ('b')
#define ATCI_char_c                 ('c')
#define ATCI_char_d                 ('d')
#define ATCI_char_e                 ('e')
#define ATCI_char_f                 ('f')
#define ATCI_char_g                 ('g')
#define ATCI_char_h                 ('h')
#define ATCI_char_i                 ('i')
#define ATCI_char_j                 ('j')
#define ATCI_char_k                 ('k')
#define ATCI_char_l                 ('l')
#define ATCI_char_m                 ('m')
#define ATCI_char_n                 ('n')
#define ATCI_char_o                 ('o')
#define ATCI_char_p                 ('p')
#define ATCI_char_q                 ('q')
#define ATCI_char_r                 ('r')
#define ATCI_char_s                 ('s')
#define ATCI_char_t                 ('t')
#define ATCI_char_u                 ('u')
#define ATCI_char_v                 ('v')
#define ATCI_char_w                 ('w')
#define ATCI_char_x                 ('x')
#define ATCI_char_y                 ('y')
#define ATCI_char_z                 ('z')
#define ATCI_R_BRACKET              (')')
#define ATCI_L_BRACKET              ('(')
#define ATCI_MONEY                  ('$')


atci_status atci_input_command_handler(uint8_t *cmd_msg, atci_port port);
atci_status atci_send_response_internal(const uint8_t *rsp, uint16_t rsp_len, uint32_t rsp_flag, atci_port port);

#ifdef __cplusplus
}
#endif

#endif 
