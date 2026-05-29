#include "minishell.h"

char*trim_spaces(char *s)
{
intstart;
intend;

if (!s)
return (NULL);
start = 0;
while (s[start] && parser_is_space(s[start]))
start++;
end = ft_strlen(s);
while (end > start && parser_is_space(s[end - 1]))
end--;
s[end] = '\0';
return (s + start);
}

voidfree_cmd_list(t_cmd *cmd)
{
t_cmd*next;

while (cmd)
{
next = cmd->next;
free_cmd(cmd);
cmd = next;
}
}

intis_trailing_empty_segment(char **segments, int index)
{
if (index == 0)
return (0);
if (segments[index + 1] != NULL)
return (0);
return (1);
}

intends_with_pipe(char *segment)
{
intend;

end = ft_strlen(segment);
while (end > 0 && parser_is_space(segment[end - 1]))
end--;
if (end == 0)
return (0);
return (segment[end - 1] == '|');
}

intstarts_with_pipe(char *segment)
{
intstart;

start = 0;
while (segment[start] && parser_is_space(segment[start]))
start++;
return (segment[start] == '|');
}
