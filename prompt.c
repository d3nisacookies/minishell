#include "minishell.h"

static void	free_cmd_list(t_cmd *cmd)
{
	t_cmd	*next;

	while (cmd)
	{
		next = cmd->next;
		free_cmd(cmd);
		cmd = next;
	}
}

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
			free_cmd_list(cmd);
		}
		else if (parser_get_status() != 0)
			shell->last_exit = parser_get_status();
		else
			shell->last_exit = 1;
		free(input);
	}
}
