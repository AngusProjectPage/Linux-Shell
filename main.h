//Find the user home directory from the environment
//Set current working directory to user home directory
//Save the current path
//Load history
//Load aliases
void loop_shell();
	void display();
	void read_parse();
//	While the command is a history invocation or alias then replace it with the
//	appropriate command from history or the aliased command respectively
//	If command is built-in invoke appropriate function
//	Else execute command as an external process
//End while
//Save history
//Save aliases
//Restore original path
//Exit

int start_fork();
