#include "minishell.h"

static intget_exit_sign(char *s, int *i, int *neg, unsigned long long *lim)
{
*i = 0;
*neg = 0;
*lim = (unsigned long long)LLONG_MAX;
if (s[*i] == '+' || s[*i] == '-')
{
if (s[*i] == '-')
{
*neg = 1;
*lim = (unsigned long long)LLONG_MAX + 1;
}
(*i)++;
}
return (s[*i] != '\0');
}

static intparse_exit_code(char *s, unsigned char *code)
{
unsigned long longvalue;
unsigned long longlimit;
intnegative;
inti;

if (!get_exit_sign(s, &i, &negative, &limit))
return (0);
value = 0;
while (s[i])
{
if (!ft_isdigit(s[i]))
return (0);
if (value > (limit - (s[i] - '0')) / 10)
return (0);
value = value * 10 + (s[i] - '0');
i++;
}
if (negative)
*code = (unsigned char)(0 - value);
else
*code = (unsigned char)value;
return (1);
}

voidbuiltin_exit(t_shell *shell, t_cmd *cmd)
{
unsigned charcode;

if (cmd->argc == 1)
exit(shell->last_exit);
if (!parse_exit_code(cmd->args[1], &code))
{
ft_putstr_fd("minishell: exit: ", 2);
ft_putstr_fd(cmd->args[1], 2);
ft_putstr_fd(": numeric argument required\n", 2);
exit(2);
}
if (cmd->argc > 2)
{
ft_putstr_fd("minishell: exit: too many arguments\n", 2);
shell->last_exit = 1;
return ;
}
exit(code);
}
