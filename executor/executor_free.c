#include "minishell.h"

static void	free_redirections(t_redir *redirs)
{
	t_redir	*next;

	while (redirs)
	{
		next = redirs->next;
		free(redirs->target);
		free(redirs);
		redirs = next;
	}
}

void	free_cmd(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;
	i = 0;
	while (cmd->args && cmd->args[i])
		free(cmd->args[i++]);
	free_redirections(cmd->redirs);
	free(cmd->args);
	free(cmd->quoted);
	free(cmd->infile);
	free(cmd->outfile);
	free(cmd->heredoc_delim);
	free(cmd);
}

void	free_cmd_list(t_cmd *cmd)
{
	t_cmd	*next;

	while (cmd)
	{
		next = cmd->next;
		free_cmd(cmd);
		cmd = next;
	}
}
