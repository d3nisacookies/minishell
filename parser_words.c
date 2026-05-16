#include "minishell.h"

static int	skip_quoted(char *s, int i)
{
	char	quote;

	quote = s[i++];
	while (s[i] && s[i] != quote)
		i++;
	if (!s[i])
		return (-1);
	return (i + 1);
}

int	parser_count_args(char *s)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		parser_skip_spaces(s, &i);
		if (!s[i])
			break ;
		count++;
		while (s[i] && !parser_is_space(s[i]))
		{
			if (s[i] == '\'' || s[i] == '"')
				i = skip_quoted(s, i);
			else
				i++;
			if (i < 0)
				return (-1);
		}
	}
	return (count);
}

static int	get_word_len(char *s, int i)
{
	int	len;
	int	next;

	len = 0;
	while (s[i] && !parser_is_space(s[i]))
	{
		if (s[i] == '\'' || s[i] == '"')
		{
			next = skip_quoted(s, i);
			if (next < 0)
				return (-1);
			len += next - i - 2;
			i = next;
		}
		else
		{
			len++;
			i++;
		}
	}
	return (len);
}

static int	fill_word(char *s, int *i, char *word)
{
	int		j;
	char	c;

	j = 0;
	while (s[*i] && !parser_is_space(s[*i]))
	{
		if (s[*i] == '\'' || s[*i] == '"')
		{
			c = s[(*i)++];
			while (s[*i] && s[*i] != c)
				word[j++] = s[(*i)++];
			if (!s[*i])
				return (-1);
			(*i)++;
		}
		else
			word[j++] = s[(*i)++];
	}
	word[j] = '\0';
	return (0);
}

char	*parser_extract_word(char *s, int *i)
{
	char	*word;
	int		len;

	len = get_word_len(s, *i);
	if (len < 0)
		return (NULL);
	word = malloc(sizeof(char) * (len + 1));
	if (!word)
		return (NULL);
	if (fill_word(s, i, word) == -1)
	{
		free(word);
		return (NULL);
	}
	return (word);
}
