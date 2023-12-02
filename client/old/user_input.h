#ifndef CSG_CLIENT_USERINPUT_H
#define CSG_CLIENT_USERINPUT_H

/* Normally, you would poll user input via Window's Procedures.
 * But since this is designed to be an interactive text console
 * I decided to poll the "Win32 Console".
 *
 * This is extracted to separate all the pieces
 */

#define MAX_RECORD_SIZE 512
#define MAX_USERINPUT 16

#include "types/input.h"

#include <windows.h>
#include <stdint.h>

typedef enum {
	E_USERINPUT_SUCCESS = 0,

	E_USERINPUT_FAIL_NULL_INPUT_INIT,
	E_USERINPUT_FAIL_NULL_OUTPUT_DATA,
	E_USERINPUT_FAIL_NULL_USER_INPUT_BUFFER,
	E_USERINPUT_FAIL_NULL_TMP_RECORD_BUFFER,
	E_USERINPUT_FAIL_SIZE_USER_INPUT_BUFFER_NOT_CORRECT,
	E_USERINPUT_FAIL_SIZE_TMP_RECORD_BUFFER_NOT_CORRECT,
	E_USERINPUT_FAIL_GET_STD_HANDLE,
	E_USERINPUT_FAIL_PEEK_CONSOLE_INPUT,
} ERR_USERINPUT;

typedef struct {
	void *bufUserInput;
	void *bufRecord;
	size_t szUserInput;
	size_t szRecord;
} UserInputInit, *P_UserInputInit;

typedef struct {
	P_UserBuffer pUserInput;
	PINPUT_RECORD pTmpRecord;
} UserInputData, *P_UserInputData;

static ERR_USERINPUT
userInputInit(P_UserInputInit pInitData,
              P_UserInputData pUserInputData);

static ERR_USERINPUT
userInputUpdate(P_UserInputData pUserInputData);

static int
user_input_isvk(P_UserBuffer input);

static int
user_input_isascii(P_UserBuffer input);

static int
user_input_up(P_UserBuffer input);

static int
user_input_down(P_UserBuffer input);

static int
user_input_left(P_UserBuffer input);

static int
user_input_right(P_UserBuffer input);

static int
user_input_enter(P_UserBuffer input);

static int
user_input_backspace(P_UserBuffer input);

static int
user_input_delete(P_UserBuffer input);

static int
user_input_escape(P_UserBuffer input);

static int
user_input_type(P_UserBuffer input, char *buffer);

#endif // CSG_CLIENT_USERINPUT_H

