#include "minishell.h"

extern char	**environ;

static int	count_cmds(t_cmd *cmd)
{
	int	count;

	count = 0;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}

static void	set_last_status(t_shell *shell, int status)
{
	if (WIFEXITED(status))
		shell->last_exit = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		shell->last_exit = 128 + WTERMSIG(status);
}

static void	wait_pipeline(pid_t *pids, int count, pid_t last_pid, t_shell *shell)
{
	int	i;
	int	status;

	i = 0;
	while (i < count)
	{
		if (waitpid(pids[i], &status, 0) > 0 && pids[i] == last_pid)
			set_last_status(shell, status);
		i++;
	}
}

int	have_next_pipe(t_cmd *cmd)
{
	return (cmd && cmd->next != NULL);
}

static void	exec_pipeline_child(t_cmd *cmd, t_shell *shell, int fd_in,
		int pipefd[2], int has_next)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (fd_in != -1)
		dup2(fd_in, STDIN_FILENO);
	if (has_next)
		dup2(pipefd[1], STDOUT_FILENO);
	if (fd_in != -1)
		close(fd_in);
	if (has_next)
	{
		close(pipefd[0]);
		close(pipefd[1]);
	}
	if (apply_redirections(cmd) == -1)
		exit(1);
	if (!cmd->args || !cmd->args[0])
		exit(0);
	environ = shell->env;
	execvp(cmd->args[0], cmd->args);
	executor_exit_exec_error(cmd->args[0]);
}

void	execute_pipeline(t_cmd *cmd, t_shell *shell)
{
	int		pipefd[2];
	int		fd_in;
	int		has_next;
	int		count;
	int		idx;
	pid_t	pid;
	pid_t	last_pid;
	pid_t	*pids;
	t_cmd	*current_cmd;

	if (!cmd)
		return ;
	count = count_cmds(cmd);
	pids = malloc(sizeof(pid_t) * count);
	if (!pids)
		return ((void)(shell->last_exit = 1));
	idx = 0;
	last_pid = -1;
	fd_in = -1;
	current_cmd = cmd;
	while (current_cmd)
	{
		if (executor_expand_args(current_cmd, shell) == -1)
		{
			shell->last_exit = 1;
			break ;
		}
		has_next = have_next_pipe(current_cmd);
		if (has_next && pipe(pipefd) == -1)
		{
			perror("pipe");
			shell->last_exit = 1;
			break ;
		}
		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			shell->last_exit = 1;
			if (has_next)
			{
				close(pipefd[0]);
				close(pipefd[1]);
			}
			break ;
		}
		if (pid == 0)
			exec_pipeline_child(current_cmd, shell, fd_in, pipefd, has_next);
		pids[idx++] = pid;
		last_pid = pid;
		if (fd_in != -1)
			close(fd_in);
		if (has_next)
		{
			close(pipefd[1]);
			fd_in = pipefd[0];
		}
		else
			fd_in = -1;
		current_cmd = current_cmd->next;
	}
	if (fd_in != -1)
		close(fd_in);
	if (idx > 0)
		wait_pipeline(pids, idx, last_pid, shell);
	free(pids);
}