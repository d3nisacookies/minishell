#include "minishell.h"

static int	get_word_len(char *s, int i)
{
	int	len;
	int	next;
	if (parser_is_redir_char(s[i]))
		return (parser_get_redir_len(s, i));
	len = 0;
	while (s[i] && !parser_is_space(s[i]) && !parser_is_redir_char(s[i]))
	{
		if (s[i] == '\'' || s[i] == '"')
		{
			next = parser_skip_quoted(s, i);
			if (next < 0)
				return (-1);
			len += next - i - 2;
			i = next;
		}
		else if (++len)
			i++;
	}
	return (len);
}

static int	fill_redirection_word(char *s, int *i, char *word)
{
	int	j;

	j = 0;
	word[j++] = s[*i];
	(*i)++;
	if (s[*i] == word[0])
		word[j++] = s[(*i)++];
	word[j] = '\0';
	return (0);
}

static int	copy_quoted_content(char *s, int *i, char *word, int *j)
{
	char	q;

	q = s[(*i)++];
	while (s[*i] && s[*i] != q)
		word[(*j)++] = s[(*i)++];
	if (!s[*i])
		return (-1);
	(*i)++;
	return (q);
}

static int	fill_word(char *s, int *i, char *word, int *was_quoted)
{
	int		j;

	j = 0;
	if (parser_is_redir_char(s[*i]))
		return (fill_redirection_word(s, i, word));
	while (s[*i] && !parser_is_space(s[*i]) && !parser_is_redir_char(s[*i]))
	{
		if (s[*i] == '\'' || s[*i] == '"')
		{
			*was_quoted = copy_quoted_content(s, i, word, &j);
			if (*was_quoted == -1)
				return (-1);
		}
		else
			word[j++] = s[(*i)++];
	}
	word[j] = '\0';
	return (0);
}

char	*parser_extract_word(char *s, int *i, int *was_quoted)
{
	char	*word;
	int		len;

	len = get_word_len(s, *i);
	if (len < 0)
		return (NULL);
	word = malloc(sizeof(char) * (len + 1));
	if (!word)
		return (NULL);
	*was_quoted = 0;
	if (fill_word(s, i, word, was_quoted) == -1)
	{
		free(word);
		return (NULL);
	}
	return (word);
}
