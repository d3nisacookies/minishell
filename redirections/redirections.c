#include "minishell.h"

static int	apply_heredoc(char *delimiter)
{
	int		pipefd[2];
	char	*line;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
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
		perror("dup2");
		close(pipefd[0]);
		return (-1);
	}
	close(pipefd[0]);
	return (0);
}

static int	apply_input_file(char *path)
{
	int	fd;

	fd = open(path, O_RDONLY);
	if (fd == -1)
	{
		perror(path);
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

static int	apply_output_file(char *path, int append)
{
	int	fd;
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(path, flags, 0644);
	if (fd == -1)
	{
		perror(path);
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
	t_redir	*current;

	current = cmd->redirs;
	while (current)
	{
		if (current->type == R_IN)
		{
			if (apply_input_file(current->target) == -1)
				return (-1);
		}
		else if (current->type == R_OUT)
		{
			if (apply_output_file(current->target, 0) == -1)
				return (-1);
		}
		else if (current->type == R_APPEND)
		{
			if (apply_output_file(current->target, 1) == -1)
				return (-1);
		}
		else if (current->type == R_HEREDOC)
		{
			if (apply_heredoc(current->target) == -1)
				return (-1);
		}
		current = current->next;
	}
	return (0);
}