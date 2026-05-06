#include "minishell.h"


void	prompt_loop(void)
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
		if (g_signal == SIGINT)
		{
			g_signal = 0;
			free(input);
			continue ;
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
			execute_command(cmd);
			free_cmd(cmd);
		}
		free(input);
	}
}
