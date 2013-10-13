#include <assert.h>
#include "Processor.h"

const string Processor::EMPTY_STRING = "";
const string Processor::FROM_KEY_WORD = "from";
const string Processor::TO_KEY_WORD = "to";
const string Processor::BY_KEY_WORD = "by";
const string Processor::NEW_LINE_STRING = "\n";

const string Processor::ADD_TASK_SUCCESS = "Task added successfully: ";
const string Processor::ADD_TASK_FAILURE_DUPLICATE = "Failed! Task existed: ";
const string Processor::ADD_TASK_FAILURE_UNEXPECTED = "Failed! Unexpected error during task adding: ";
const string Processor::ADD_TASK_WARNING_CLASH = "Warning: this task clashes with existing ones";
const string Processor::DISPLAY_TASK_SUCCESS = "Task Display success!";
const string Processor::DISPLAY_TASK_FAILURE_UNEXPECTED = "Unexpected task failure";
const string Processor::WRONG_INPUT = "Wrong input";
const string Processor::UNEXPECTED_ERROR = "Unexpected error";
const string Processor::UPDATE_CHOOSE_TASK = "Enter the task number to update: ";
const string Processor::UPDATE_MESSAGE_WARNING_SAME = "Warning! The new data is the same as the old data";
const string Processor::UPDATE_MESSAGE_WARNING_CLASH = "Warning! The new data will make the task clash with the following:";
const string Processor::UPDATE_MESSAGE_FAILURE = "Error! Unable to update task";
const string Processor::REMOVE_CHOOSE_TASK = "Enter the task number to remove: ";
const string Processor::MARK_CHOOSE_TASK = "Enter the task number to mark: ";
const string Processor::NO_SUCH_TASK = "No such task!";
const string Processor::FILE_SAVE_SUCCESS = "File Saved successfully!";
const string Processor::FILE_SAVE_FAILURE = "File COULD NOT BE SAVED!! LOL GG";

const char Processor::SLASH = '/';
const char Processor::BACK_SLASH = '\\';
const char Processor::DOT = '.';
const char Processor::COLON = ':';
const char Processor::DASH = '-';
const char Processor::ZERO = '0';
const char Processor::NINE = '9';
const char Processor::SPACE = ' ';
const char Processor::NEW_LINE = '\n';

Processor::Processor(){
	_statusFlag=0;
	_wordsList = new vector<string>;
}

/*
* Purpose: Handle user input based on first keyword
* and call appropriate functions to handle
*
* Param: 
* command - first keyword of user input
*
* Returns: 
* formatted string of feedback and with user command (including task)
*/
int Processor::UImainProcessor(string input, string& message, vector<string>& list){
	input = _interpreter.toLowerCase(input);
	_wordsList->clear();
	breakIntoStringVectorBySpace(input, *_wordsList);
	string firstWord = _wordsList->at(0);
	int returnCode = -1;
	assert(_statusFlag >= 0 && _statusFlag < 5);
	while (firstWord != "exit"){
		switch (_statusFlag){
		case 0:
			if(firstWord == "add"){
				returnCode = addCommandProcessor();
			}else if(firstWord == "remove"){
				returnCode = removeCommandProcessor();
			}else if(firstWord == "display"){
				returnCode = displayCommandProcessor();
			}else if(firstWord == "update"){
				returnCode = renameCommandProcessor();
			}else if(firstWord == "reschedule"){
				returnCode = rescheduleCommandProcessor();
			}else if(firstWord == "mark"){
				returnCode = markCommandProcessor();
			}else if(firstWord == "search"){
				returnCode = searchCommandProcessor();
			}else{
				returnCode = otherCommandProcessor();
			}
			break;

		case 1:
			returnCode = removeCommandProcessor();
		case 2:
			returnCode = renameCommandProcessor();
		case 3:
			returnCode = rescheduleCommandProcessor();
		case 4:
			returnCode = markCommandProcessor();
		default:
			break;
		}

		return simpleReturn(returnCode, message, list);
	}
	return saveFile();
}

//level 1 abstraction
/*
* Purpose: Add tasks according to type
* 0 - Floating tasks
* 1 - Deadline tasks
* 2 - Normal tasks
*
* Returns: 
* Message if task is successfully added (or not with reason)
*/
int Processor::addCommandProcessor(){
	int type;
	string title, comment;
	BasicDateTime startingDateTime;
	BasicDateTime endingDateTime;

	string user_command = combineStringsWithSpaceOnVector(1, _wordsList->size()-1);
	_interpreter.interpretAdd(user_command, title, type, startingDateTime, endingDateTime, comment);

	switch (type){
	case 0:
		return addFloatingTask(title, comment);
		break;
	case 1:
		return addDeadlineTask(title, endingDateTime, comment);
		break;
	case 2:
		return addNormalTask(title, startingDateTime, endingDateTime, comment);
		break;
	default:
		return ERROR_ADD;
	}
}

/*
* Purpose: Remove task (from search results)
*
* Returns: 
* Task successfully removed; Ask user to choose task to remove; No such task found
*/
int Processor::removeCommandProcessor(){
	int operationStatus;
	if(_statusFlag == 1){
		unsigned int choice = _interpreter.stringToInt(_wordsList->at(1));
		if((choice <=_tempTaskList.size()) && choice > 0){
			operationStatus=_logic.remove(_tempTaskList[choice-1]);
		}
		_statusFlag = 0;
		return operationStatus;
	}else if(_statusFlag == 0){
		if(_wordsList->size()>1){
			string user_command=combineStringsWithSpaceOnVector(1, _wordsList->size()-1);
			vector<string> keywords;
			_interpreter.interpretRemove(user_command, _tempTitle);
			_tempTaskList.clear();
			breakIntoStringVectorBySpace(_tempTitle, keywords);
			_logic.searchKeywords(keywords, _tempTaskList);
			if (_tempTaskList.size() == 1){
				return _logic.remove(_tempTaskList[0]);

			}else if(!_tempTaskList.empty()){
				_statusFlag = 1;
				return PROMPT_REMOVE_CHOOSE;
			}else if(_tempTaskList.empty()){
				return WARNING_SEARCH_NO_RESULT;
			}
		}
	}
	return ERROR_REMOVE;
}

/*
* Purpose: Display to user type of task
*
* Returns: 
* Tasks of type; Unable to display
*/
int Processor::displayCommandProcessor(){
	string user_command;
	BasicDateTime start, end;
	bool status;
	user_command = combineStringsWithSpaceOnVector(1, _wordsList->size()-1);
	int return_code = _interpreter.interpretDisplay(user_command, start, end, status);
	if (return_code == 0){
		return _logic.displayAll(_tempTaskList);

	}else if (return_code == 1){
		return _logic.displayStatus(status, _tempTaskList);

	}else if(return_code == 2){
		return _logic.displayInRange(start, end, _tempTaskList);

	}else{
		return ERROR_DISPLAY;
	}

}

//pass the created task and the task in the vector at position
//create a new vector and pass in that for new clashes
int Processor::renameCommandProcessor(){
	int operationStatus;
	if(_statusFlag == 2){
		unsigned int choice = _interpreter.stringToInt(_wordsList->at(1));
		if((choice <=_tempTaskList.size()) && choice > 0 && _wordsList->size()==2){
			string title, comment;
			BasicDateTime startingDateTime;
			BasicDateTime endingDateTime;
			Task t = _tempTaskList[choice-1];
			vector<Task> clash;

			t.setTitle(_tempTitle);
			operationStatus = _logic.update(_tempTaskList[choice-1], t, clash);
			_tempTaskList = clash;
		}

		_statusFlag = 0;
		return operationStatus;

	}else if(_statusFlag == 0){
		if(_wordsList->size()>1){
			string user_command=combineStringsWithSpaceOnVector(1, _wordsList->size()-1);
			string oldTitle;
			vector<string> keywords;
			vector<Task> clash;
			_interpreter.interpretRename(user_command, oldTitle, _tempTitle);
			_tempTaskList.clear();
			breakIntoStringVectorBySpace(oldTitle, keywords);
			_logic.searchKeywords(keywords, _tempTaskList);
			if (_tempTaskList.size() == 1){
				Task t = _tempTaskList[0];
				t.setTitle(_tempTitle);
				return _logic.update(_tempTaskList[0], t, clash);

			}else if(!_tempTaskList.empty()){
				_statusFlag = 2;
				return PROMPT_RENAME_CHOOSE;
			}else if(_tempTaskList.empty()){
				return WARNING_SEARCH_NO_RESULT;
			}
		}
	}
	return ERROR_UPDATE;
}

int Processor::rescheduleCommandProcessor(){
	int operationStatus;
	if(_statusFlag == 3){
		unsigned int choice = _interpreter.stringToInt(_wordsList->at(1));
		if((choice <=_tempTaskList.size()) && choice > 0 && _wordsList->size()==2){
			string title, comment;
			BasicDateTime startingDateTime;
			BasicDateTime endingDateTime;
			Task t = _tempTaskList[choice-1];
			vector<Task> clash;

			t.setStartDate(_tempStart);
			t.setEndDate(_tempEnd);
			t.setType(_tempType);
			operationStatus = _logic.update(_tempTaskList[choice-1], t, clash);
			_tempTaskList = clash;
		}

		_statusFlag = 0;
		return operationStatus;

	}else if(_statusFlag == 0){
		if(_wordsList->size()>1){
			vector<string> keywords;
			string user_command=combineStringsWithSpaceOnVector(1, _wordsList->size()-1);

			//problem in extractDateTimeForReschdule, pos2+4 should be pos2+7
			_interpreter.interpretReschedule(user_command, _tempTitle, _tempType, _tempStart, _tempEnd);
			_tempTaskList.clear();
			breakIntoStringVectorBySpace(_tempTitle, keywords);
			_logic.searchKeywords(keywords, _tempTaskList);

			if (_tempTaskList.size() == 1){
				Task t = _tempTaskList[0];
				vector<Task> clash;
				t.setStartDate(_tempStart);
				t.setEndDate(_tempEnd);
				t.setType(_tempType);
				return _logic.update(_tempTaskList[0], t, clash);

			}else if(!_tempTaskList.empty()){
				_statusFlag = 3;
				return PROMPT_RESCHEDULE_CHOOSE;
			}else if(_tempTaskList.empty()){
				return WARNING_SEARCH_NO_RESULT;
			}
		}
	}
	return ERROR_UPDATE;
}

/*
* Purpose: Mark Task as Done/Pending (from search results)
*
* Returns: 
* Task is marked; Ask user to choose task to mark from results; No such task found
*/
int Processor::markCommandProcessor(){
	int operationStatus;
	if(_statusFlag == 4){
		//stringtointvec doenst seem to be working
		vector<int> choice = _interpreter.stringToIntVec(_wordsList->at(1));

		if(choiceIsValid(choice)){
			for (unsigned int i = 0; i < choice.size(); i++){
				operationStatus=_logic.mark(_tempStatus, _tempTaskList[choice[i]]);
			}
		}
		_statusFlag = 0;
		return operationStatus;
	}else if(_statusFlag == 0){
		if(_wordsList->size()>1){
			vector<string> keywords;
			string user_command=combineStringsWithSpaceOnVector(1, _wordsList->size()-1);
			_interpreter.interpretMark(user_command, _tempTitle, _tempStatus);
			_tempTaskList.clear();
			breakIntoStringVectorBySpace(_tempTitle, keywords);
			_logic.searchKeywords(keywords, _tempTaskList);
			if (_tempTaskList.size() == 1){
				return _logic.mark(_tempStatus, _tempTaskList[0]);
			}	
			else if(!_tempTaskList.empty()){
				_statusFlag = 4;
				return PROMPT_MARK_CHOOSE;
			}else if(_tempTaskList.empty()){
				return WARNING_SEARCH_NO_RESULT;
			}
		}
	}
	return ERROR_MARK;
}


int Processor::searchCommandProcessor(){
	vector<string> keywords;
	string user_command;
	BasicDateTime start, end;
	int operationStatus;

	user_command = combineStringsWithSpaceOnVector(1, _wordsList->size()-1);
	_tempTaskList.clear();
	breakIntoStringVectorBySpace(_tempTitle, keywords);
	int return_code = _interpreter.interpretSearch(user_command, keywords, start, end);

	if (return_code == 0){
		return _logic.searchKeywords(keywords, _tempTaskList);

	}else if(return_code == 2){
		return _logic.searchKeywordsInRange(keywords, _tempTaskList, start, end);

	}else{
		return ERROR_SEARCH;
	}

}


int Processor::otherCommandProcessor(){
	return WARNING_WRONG_INPUT;
}

int Processor::saveFile(){
	vector<Task> allTasks;
	vector<string> allTasksString;
	_logic.displayAll(allTasks);
	for (unsigned int i = 0; i < allTasks.size(); i++){
		allTasksString.push_back(taskToString(allTasks[i]));
	}
	return _fileProcessing.save(allTasksString);
}

int Processor::simpleReturn(int returnCode, string&message, vector<string>& list){

}

bool Processor::choiceIsValid(vector<int> choice){
	if(choice.empty()){
		return false;
	}else{
		for (unsigned int i = 0; i < choice.size(); i++){
			unsigned int curr_choice = choice[i];
			if(curr_choice>=_tempTaskList.size() || choice[i]<1){
				return false;
			}
		}
		return true;
	}
}
/*
* Purpose: To get feedback to tell user is command is succesful/unsuccessful
*
* Param: 
* statusReturnedFromLogic - feedback from Logic code 
*
* Returns: 
* formatted string of feedback and with user command (including task)
*/
int Processor::simpleReturn(int returnCode, string& message, vector<string>& list){
	
	switch (returnCode){
	case STATUS_CODE_SET_SUCCESS::SUCCESS_ADD:
		message = 
		return combineStatusMsgWithFeedback(ADD_TASK_SUCCESS);
		break;
	case STATUS_CODE_SET_SUCCESS::SUCCESS_REMOVE:
		return combineStatusMsgWithFeedback(ADD_TASK_FAILURE_DUPLICATE);
		break;
	case STATUS_CODESTATUS_CODE_SET_SUCCESS_SET::SUCCESS_DISPLAY:
		return combineStatusMsgWithFeedback(ADD_TASK_WARNING_CLASH);
		break;
	case STATUS_CODE_SET_SUCCESS::SUCCESS_UPDATE:
		return combineStatusMsgWithFeedback(ADD_TASK_FAILURE_UNEXPECTED);
		break;
	default:
		return ADD_TASK_FAILURE_UNEXPECTED;
		break;
	}
}

/*
* Purpose: Add task of type Floating tasks 
* puts the successfully created task into _tempTaskList
*
* Param: 
* title - name of task
* comment - additional description
*/
int Processor::addFloatingTask(string title, string comment){
	BasicDateTime dt1, dt2;
	Task t;
	t = Task(title, dt1, dt2, 2, false, comment);
	_tempTaskList.clear();
	return _logic.add(t, _tempTaskList);
}

/*
* Purpose: Add task of type Dealine tasks 
* puts the successfully created task into _tempTaskList
*
* Param: 
* title - name of task
* dt - deadline of task in DateTime format
* comment - additional description
*/
int Processor::addDeadlineTask(string title, BasicDateTime dt, string comment){
	BasicDateTime dt1;
	Task t;
	t = Task(title, dt1, dt, 2, false, comment);
	_tempTaskList.clear();
	return _logic.add(t, _tempTaskList);
}

/*
* Purpose: Add task of type Normal tasks 
* puts the successfully created task into _tempTaskList
*
* Param: 
* title - name of task
* dt1 - starting time of task in DateTime format
* dt2 - ending time of task in DateTime format
* comment - additional description
*/
int Processor::addNormalTask(string title, BasicDateTime dt1, BasicDateTime dt2, string comment){
	Task t;
	t = Task(title, dt1, dt2, 2, false, comment);
	_tempTaskList.clear();
	return _logic.add(t, _tempTaskList);
}
/*
* Purpose: Separates the user input word by word into vector _wordsList
*
* Param: longStr - user input
*
* Returns: success
*/
int Processor::breakIntoStringVectorBySpace(string longStr, vector<string>& outputVector){
	stringstream ss(longStr);
	string tempStr;
	bool noSpace = true;
	while (std::getline(ss, tempStr, SPACE)){
		outputVector.push_back(tempStr);
		noSpace = false;
	}

	if (noSpace){
		outputVector.push_back(longStr);
	}
	return 0;
}

/*
* Purpose:Formats the user input which was previously separated
* into vector into a string again
*
* Param: start - beginning of word to be combined in vector;
*		 end - end of word to be combined in vector
*
* Returns: formatted string of user input
*/
string Processor::combineStringsWithSpaceOnVector(int start, int end){
	string result=_wordsList->at(start);
	for (int i=start+1;i<=end;i++){
		if(_wordsList->at(i)!=""){
			result=result+" "+_wordsList->at(i);
		}
	}
	return removeLeadingSpaces(result);
}

/*
* Purpose:Formats all the task between start to end into 1 string
*
* Param: start - beginning of task to be combined in vector;
*		 end - end of task to be combined in vector
*
* Returns: formatted task details
*/
string Processor::combineStringsWithNewLineOnVector(int start, int end){
	string result=taskToString(_tempTaskList.at(start));
	for (int i=start+1;i<end;i++){
		result=combineStringsWithNewLine(result, taskToString(_tempTaskList.at(i)));
	}
	return result;
}

/*
* Purpose:Formats 2 strings separated by a new line character
*
* Returns: formatted string
*/
string Processor::combineStringsWithNewLine(string str1, string str2){
	return str1+NEW_LINE_STRING+str2;
}

/*
* Purpose: Checks the type of the task,
* and print the details of task according to type 
*
* Param: t- task with details
*
* Returns: formatted task details
*/
string Processor::taskToString(Task t){
	if(t.getType() == 0)
	{
		return printFloatingTask(t);
	}else if(t.getType() == 1){
		return printDeadlineTask(t);
	}else if(t.getType() == 2){
		return printTimedTask(t);
	}else{
		return UNEXPECTED_ERROR;
	}
}

/*
* Purpose:Formats the details task into a string to be printed
*
* Param: t- task with details
*
* Returns: formatted task details
*/
string Processor::printFloatingTask(Task t){
	string result;
	result="title: "+t.getTitle();
	result=combineStringsWithNewLine(result, "Status: ");
	result+= printStatus(t.getDone());
	result=combineStringsWithNewLine(result, "Type: ");
	result+="Floating task";
	return result;
}

/*
* Purpose:Formats the details task into a string to be printed
*
* Param: t- task with details
*
* Returns: formatted task details
*/
string Processor::printDeadlineTask(Task t){
	string result;
	result="title: "+t.getTitle();
	result=combineStringsWithNewLine(result, "Status: ");
	result+= printStatus(t.getDone());
	result=combineStringsWithNewLine(result, "Type: ");
	result+="Deadline task";
	result=combineStringsWithNewLine(result, "Deadline: ");
	result+= t.getEnd().getDateTimeString();
	return result;
}

/*
* Purpose:Formats the details task into a string to be printed
*
* Param: t- task with details
*
* Returns: formatted task details
*/
string Processor::printTimedTask(Task t){
	string result;
	result="title: "+t.getTitle();
	result=combineStringsWithNewLine(result, "Status: ");
	result+= printStatus(t.getDone());
	result=combineStringsWithNewLine(result, "Type: ");
	result+="Timed task";
	result=combineStringsWithNewLine(result, "Start Date: ");
	result+= t.getStart().getDateTimeString();
	result=combineStringsWithNewLine(result, "End Date: ");
	result+= t.getEnd().getDateTimeString();
	return result;
}

/*
* Purpose:Retrieves status of task
*
* Param: status - true - done; false - pending
*
* Returns: string with status of task
*/
string Processor::printStatus(bool status){
	if(status){
		return "done";
	}else{
		return "pending";
	}
}

/*
* Purpose: trim spaces ahead of the string
*
* Param: str- string to be transformed
*
* Returns: string without spaces
*/
string Processor::removeLeadingSpaces(string str){
	int num=str.find_first_not_of(SPACE);
	return str.substr(num);
}

/*
* Purpose: combines the status message of adding task to task
*(for display and search)
*
* Param: str- status message
*
* Returns: formatted status message of adding task
*/
string Processor::taskVecToStringVec(vector<string>& stringTasks){
	int size=_tempTaskList.size();
	stringTasks.push_back(taskToString(_tempTaskList.at(0)));
	for (int i=1;i<size;i++){
		stringTasks.push_back(taskToString(_tempTaskList.at(i)));
	}

	return feedback;
}

Processor::~Processor(){
	delete _wordsList;
}
