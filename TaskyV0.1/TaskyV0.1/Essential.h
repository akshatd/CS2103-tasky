#ifndef _ESSENTIAL_H_
#define _ESSENTIAL_H_

/*
*This class is mainly for definition of statuc code, it also includes some command header file
*names.
*
*Main Author: Akshat, Kai Wen, Joan, Junchao
*/

#include <vector>
#include <string>

using namespace std;

enum STATUS_CODE_SET_OVERALL{
	OVERALL_SUCCESS,
	OVERALL_WARNING,
	OVERALL_ERROR
};

enum STATUS_CODE_SET_SUCCESS{
	SUCCESS_ADD = OVERALL_ERROR+1,

	SUCCESS_REMOVE,

	SUCCESS_DISPLAY,

	SUCCESS_UPDATE,

	SUCCESS_RESHCEDULE,

	SUCCESS_MARK,

	SUCCESS_SEARCH,

	SUCCESS_LOAD,

	SUCCESS_SAVE,

	SUCCESS_UNDO,

	SUCCESS_RECORD,

	SUCCESS_REDO
};

enum STATUS_CODE_SET_WARNING{
	WARNING_ADD_CLASH = SUCCESS_REDO+1,
	
	WARNING_ADD_DUPLICATE,

	WARNING_UPDATE_SAME,
	
	WARNING_UPDATE_CLASH,

	WARNING_SEARCH_NO_RESULT,

	WARNING_DISPLAY_NO_RESULT,

	WARNING_MARK_NO_CHANGE,

	WARNING_UNDO_NO_TASKS,

	WARNING_WRONG_INPUT
};

enum STATUS_CODE_SET_ERROR{
	ERROR_ADD = WARNING_WRONG_INPUT+1,

	ERROR_REMOVE,

	ERROR_DISPLAY,

	ERROR_UPDATE,

	ERROR_SEARCH,

	ERROR_MARK,

	ERROR_UNDO,

	ERROR_RECORD,

	ERROR_LOAD_OPENFILE,

	ERROR_SAVE_SAVEFILE
};

enum STATUS_CODE_SET_PROPMT{
	PROMPT_REMOVE_CHOOSE = ERROR_SAVE_SAVEFILE+1,

	PROMPT_RENAME_CHOOSE,

	PROMPT_RESCHEDULE_CHOOSE,

	PROMPT_MARK_CHOOSE
};

#endif
