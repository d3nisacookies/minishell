#include "minishell.h"


void	prompt_loop(t_shell *shell)
{
	char *input;
	t_cmd *cmd;

	while (1)
	{
		input = readline("$> ");
		if (input == NULL)
		{
			write(1, "\n", 1);
			break ;
		}
		if (strlen(input) == 0)
		{
			free(input);
			continue ;
		}
		add_history(input);
		cmd = parse_command(input);
		if (cmd != NULL)
		{
			execute_command(cmd, shell);
			free_cmd(cmd);
		}
		free(input);
	}
}
