#include "minishell.h"

extern char	**environ;

static void	exit_exec_error(char *cmd_name)
{
    if (errno == ENOENT)
    {
        ft_putstr_fd(cmd_name, 2);
        ft_putstr_fd(": command not found\n", 2);
        exit(127);
    }
    if (errno == EACCES)
    {
        ft_putstr_fd(cmd_name, 2);
        ft_putstr_fd(": Permission denied\n", 2);
        exit(126);
    }
    perror(cmd_name);
    exit(1);
}

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

int have_next_pipe(t_cmd *cmd)
{
    return (cmd && cmd->next != NULL);
}

void execute_pipeline(t_cmd *cmd, t_shell *shell)
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
    {
        shell->last_exit = 1;
        return ;
    }
    idx = 0;
    last_pid = -1;
    fd_in = -1;
    current_cmd = cmd;
    while (current_cmd)
    {
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
            if (apply_redirections(current_cmd) == -1)
                exit(1);
            environ = shell->env;
            execvp(current_cmd->args[0], current_cmd->args);
            exit_exec_error(current_cmd->args[0]);
        }
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