#include "minishell.h"

static int apply_heredoc(t_cmd *cmd)
{
	int pipefd[2];
	char *line;

	if (pipe(pipefd) == -1)
		return (-1);
	while(1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, cmd->heredoc_delim) == 0)
		{
			free(line);
			break ;
		}
		ft_putstr_fd(line, pipefd[1]);
		ft_putchar_fd('\n', pipefd[1]);
		free(line);
	}
	close(pipefd[1]);
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		close(pipefd[0]);
		return (-1);
	}
	close(pipefd[0]);
	return (0);
}

// take the output of a command and send it to the file instead of terminal
int	crocodile(t_cmd *cmd)
{
	int	fd;

	fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror(cmd->outfile);
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

// take the output of a command and send it to the file and append instead of terminal
int	double_crocodile(t_cmd *cmd)
{
	int	fd;
        

	fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		perror("open");
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	// execve(cmd->path, cmd->args, env);
	// perror("execve");
    return (0);
}

// take a file as the stdin and redirect it to the command
int	reverse_crocodile(t_cmd *cmd)
{
	int	fd;
        

	fd = open(cmd->infile, O_RDONLY);
	if (fd == -1)
	{
		perror("open");
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	// execve(cmd->path, cmd->args, env);
	// perror("execve");

    return 0;
}


int	apply_redirections(t_cmd *cmd)
{
	if (cmd->infile)
	{
		if (reverse_crocodile(cmd) == -1)
			return (-1);
	}
	if (cmd->outfile)
	{
		if (cmd->append)
		{
			if (double_crocodile(cmd) == -1)
				return (-1);
		}
		else
		{
			if (crocodile(cmd) == -1)
				return (-1);
		}
	}
	return (0);
}