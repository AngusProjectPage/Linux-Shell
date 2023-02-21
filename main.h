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
	int start_fork();
//Save history
//Save aliases
//Restore original path
//Exit


