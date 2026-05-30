#include "minishell.h"

static intapply_heredoc(char *delimiter)
{
intpipefd[2];
char*line;

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
return (close(pipefd[0]), perror("dup2"), -1);
close(pipefd[0]);
return (0);
}

intredir_stdin_from_file(char *path)
{
intfd;

fd = open(path, O_RDONLY);
if (fd == -1)
return (perror(path), -1);
if (dup2(fd, STDIN_FILENO) == -1)
return (close(fd), perror("dup2"), -1);
close(fd);
return (0);
}

intredir_stdout_to_file(char *path)
{
intfd;

fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
if (fd == -1)
return (perror(path), -1);
if (dup2(fd, STDOUT_FILENO) == -1)
return (close(fd), perror("dup2"), -1);
close(fd);
return (0);
}

intredir_stdout_append_file(char *path)
{
intfd;

fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
if (fd == -1)
return (perror(path), -1);
if (dup2(fd, STDOUT_FILENO) == -1)
return (close(fd), perror("dup2"), -1);
close(fd);
return (0);
}

static intapply_single_redir(t_redir *redir)
{
if (redir->type == R_IN)
return (redir_stdin_from_file(redir->target));
if (redir->type == R_OUT)
return (redir_stdout_to_file(redir->target));
if (redir->type == R_APPEND)
return (redir_stdout_append_file(redir->target));
if (redir->type == R_HEREDOC)
return (apply_heredoc(redir->target));
return (0);
}

intapply_redirections(t_cmd *cmd)
{
t_redir*current;

current = cmd->redirs;
while (current)
{
if (apply_single_redir(current) == -1)
return (-1);
current = current->next;
}
return (0);
}
