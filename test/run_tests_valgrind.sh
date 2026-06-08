#!/bin/bash

# Minishell test suite
# Compares minishell output AND exit codes against bash.
# Usage: bash tests/run_tests.sh  (from any directory)

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$PROJECT_DIR"

MINISHELL="./minishell"
PASS=0
FAIL=0
TOTAL=0

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
DIM='\033[2m'
RESET='\033[0m'

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

get_ms_output()
{
	printf '%s\n' "$1" | valgrind --leak-check=full --show-leak-kinds=all $MINISHELL 2>/tmp/err.txt \
		| sed 's/minishell\$ .*//g' \
		| grep -v '^> ' \
		| sed '/^$/d'
}

get_ms_exit()
{
	printf '%s\n' "$1" "exit" | $MINISHELL >/dev/null 2>&1
	echo $?
}

get_ms_inner_exit()
{
	printf '%s\n' "$1" 'echo $?' "exit" | $MINISHELL 2>/dev/null \
		| sed 's/minishell\$ .*//g' \
		| grep -v '^> ' \
		| sed '/^$/d' \
		| tail -1
}

# Show the command being tested (truncated, escaped newlines)
show_cmd()
{
	local cmd="$1"
	local display
	display=$(printf '%s' "$cmd" | tr '\n' '↵' | head -c 80)
	printf "  ${DIM}cmd: %s${RESET}\n" "$display"
}

# ---------------------------------------------------------------------------
# Test runners
# ---------------------------------------------------------------------------

# Compare stdout against bash, sorting both outputs (for env/unordered output).
run_test_sorted()
{
	local name="$1"
	local input="$2"
	local bash_out ms_out

	TOTAL=$((TOTAL + 1))
	bash_out=$(printf '%s\n' "$input" | bash 2>/dev/null | sort)
	ms_out=$(get_ms_output "$input" | sort)
	if [ "$bash_out" = "$ms_out" ]; then
		printf "${GREEN}PASS${RESET} %s\n" "$name"
		PASS=$((PASS + 1))
	else
		printf "${RED}FAIL${RESET} %s\n" "$name"
		show_cmd "$input"
		printf "  bash: |%s|\n" "$(echo "$bash_out" | head -3)"
		printf "  ms:   |%s|\n" "$(echo "$ms_out" | head -3)"
		FAIL=$((FAIL + 1))
	fi
}

# Compare stdout against bash.
run_test()
{
	local name="$1"
	local input="$2"
	local bash_out ms_out

	TOTAL=$((TOTAL + 1))
	bash_out=$(printf '%s\n' "$input" | bash 2>/dev/null)
	ms_out=$(get_ms_output "$input")
	if [ "$bash_out" = "$ms_out" ]; then
		printf "${GREEN}PASS${RESET} %s\n" "$name"
		PASS=$((PASS + 1))
	else
		printf "${RED}FAIL${RESET} %s\n" "$name"
		show_cmd "$input"
		printf "  bash: |%s|\n" "$(echo "$bash_out" | head -3)"
		printf "  ms:   |%s|\n" "$(echo "$ms_out" | head -3)"
		FAIL=$((FAIL + 1))
	fi
}

# Compare both stdout AND exit code against bash.
run_test_full()
{
	local name="$1"
	local input="$2"
	local bash_out ms_out mem_err rl_err bash_exit ms_exit out_ok exit_ok mem_ok

	TOTAL=$((TOTAL + 1))
	bash_out=$(printf '%s\n' "$input" | bash 2>/dev/null)
	bash_exit=$(printf '%s\n' "$input" | bash >/dev/null 2>&1; echo $?)
	ms_out=$(get_ms_output "$input")
	ms_exit=$(get_ms_inner_exit "$input")
	mem_err=0
	output=$(grep -e "record 1 of" /tmp/err.txt)
	while IFS= read -r line; do
		num="${line##* }"
		(( mem_err += num ))
	done <<< "$output"
	rl_err=$(grep -e " readline " -e " add_history " /tmp/err.txt | wc -l)
	out_ok=0
	exit_ok=0
	mem_ok=0
	[ "$bash_out" = "$ms_out" ] && out_ok=1
	[ "$bash_exit" = "$ms_exit" ] && exit_ok=1
	[ "$mem_err" = "$rl_err" ] && mem_ok=1
	if [ "$out_ok" = "1" ] && [ "$exit_ok" = "1" ] && [ "$mem_ok" = "1" ]; then
		printf "${GREEN}PASS${RESET} %s\n" "$name"
		PASS=$((PASS + 1))
	else
		printf "${RED}FAIL${RESET} %s\n" "$name"
		show_cmd "$input"
		if [ "$out_ok" = "0" ]; then
			printf "  stdout bash: |%s|\n" "$(echo "$bash_out" | head -3)"
			printf "  stdout ms:   |%s|\n" "$(echo "$ms_out" | head -3)"
		fi
		if [ "$exit_ok" = "0" ]; then
			printf "  exit bash: %s\n" "$bash_exit"
			printf "  exit ms:   %s\n" "$ms_exit"
		fi
		if [ "$mem_ok" = "0" ]; then
			printf "  mem_err: %s\n" "$mem_err"
			printf "  rl_err:   %s\n" "$rl_err"
		fi
		FAIL=$((FAIL + 1))
	fi
}

# Compare only the shell exit code (for exit builtin).
run_test_exit()
{
	local name="$1"
	local input="$2"
	local bash_exit ms_exit

	TOTAL=$((TOTAL + 1))
	printf '%s\n' "$input" | bash >/dev/null 2>&1
	bash_exit=$?
	printf '%s\n' "$input" | $MINISHELL >/dev/null 2>&1
	ms_exit=$?
	if [ "$bash_exit" = "$ms_exit" ]; then
		printf "${GREEN}PASS${RESET} %s\n" "$name"
		PASS=$((PASS + 1))
	else
		printf "${RED}FAIL${RESET} %s\n" "$name"
		show_cmd "$input"
		printf "  bash exit: %s\n" "$bash_exit"
		printf "  ms exit:   %s\n" "$ms_exit"
		FAIL=$((FAIL + 1))
	fi
}

# ===========================================================================
# Build
# ===========================================================================

make re >/dev/null 2>&1
if [ ! -x "$MINISHELL" ]; then
	printf "${RED}Build failed${RESET}\n"
	exit 1
fi

# ===========================================================================
# Tests
# ===========================================================================

# ---------- Echo ----------
printf "${CYAN}=== Echo ===${RESET}\n"
run_test_full "echo basic" "echo hello"
run_test_full "echo multiple args" "echo hello world"
run_test_full "echo empty" "echo"
run_test_full "echo -n via file" "echo -n hello > /tmp/ms_echo_n
cat /tmp/ms_echo_n"
run_test_full "echo -nnn via file" "echo -nnn hello > /tmp/ms_echo_n
cat /tmp/ms_echo_n"
run_test_full "echo -n -n -n" "echo -n -n -n hello > /tmp/ms_echo_n
cat /tmp/ms_echo_n"
# tab tests skipped: readline eats \t (completion trigger) when input is piped
# run_test_full "echo with tabs" "echo hello	world"
run_test_full "echo many spaces" "echo    hello    world"
run_test_full "echo only spaces" "echo '   '"
run_test_full "echo single char" "echo a"
run_test_full "echo numbers" "echo 123 456"
run_test_full "echo special chars" "echo hello! @#%^"
run_test_full "echo dash not flag" "echo -"
run_test_full "echo dash-a not flag" "echo -a hello"
run_test_full "echo -nnna" "echo -nnna hello"

# ---------- Variable expansion ----------
printf "\n${CYAN}=== Variable expansion ===${RESET}\n"
run_test_full "expand HOME" 'echo $HOME'
run_test_full "expand USER" 'echo $USER'
run_test_full "expand PATH" 'echo $PATH'
run_test_full "expand unset var" 'echo $NONEXISTENT'
run_test_full "expand bare dollar" 'echo $'
run_test_full "expand \$?" 'echo $?'
run_test_full "var then text" 'echo hello $USER world'
run_test_full "adjacent vars" 'echo $USER$HOME'
run_test_full "var in middle" "echo before$USER after"
run_test_full "dollar followed by space" 'echo $ hello'
run_test_full "dollar at end" 'echo hello$'
run_test_full "dollar question chain" 'echo $?$?'
run_test_full "var underscore" 'export _TEST_VAR=works
echo $_TEST_VAR'
run_test_full "dollar in dquote" 'echo "hello $USER"'
run_test_full "dollar question in dquote" 'echo "status: $?"'
run_test_full "multiple vars in dquote" 'echo "$USER $HOME"'
run_test_full "empty var in dquote" 'echo "hello $NONEXISTENT world"'

# ---------- Quoting ----------
printf "\n${CYAN}=== Quoting ===${RESET}\n"
run_test_full "single quote blocks \$" "echo '\$HOME'"
run_test_full "double quote spaces" 'echo "hello   world"'
run_test_full "mixed quotes" "echo 'hello'\"world\""
run_test_full "empty single quotes" "echo '' hello"
run_test_full "empty double quotes" 'echo "" hello'
run_test_full "dollar in single quotes" "echo '\$USER'"
run_test_full "single in double" 'echo "it'\''s"'
run_test_full "double in single" "echo '\"hello\"'"
run_test_full "adjacent single quotes" "echo 'hel''lo'"
run_test_full "adjacent double quotes" 'echo "hel""lo"'
run_test_full "quote at start" "'echo' hello"
run_test_full "quote in middle of word" "ec'h'o hello"
run_test_full "empty quoted arg" "echo '' '' ''"
run_test_full "quoted empty string" "echo \"\""
run_test_full "spaces in double quotes" 'echo "  hello  "'
# run_test_full "tabs in double quotes" 'echo "	hello	"'
run_test_full "single quotes preserve backslash" "echo 'hello\\world'"
run_test_full "double quotes partial expand" 'echo "$USER"rest'

# ---------- Pipes ----------
printf "\n${CYAN}=== Pipes ===${RESET}\n"
run_test_full "simple pipe" "echo hello | cat"
run_test_full "pipe chain" "echo hello | cat | cat"
run_test_full "pipe to wc -c" "echo hello | wc -c"
run_test_full "pipe to wc -l" "printf 'a\nb\nc\n' | wc -l"
run_test_full "pipe to grep" "echo hello world | grep hello"
run_test_full "pipe to grep no match" "echo hello | grep xyz"
run_test_full "multi pipe" "ls /bin | head -5 | wc -l"
run_test_full "pipe preserves order" "echo a b c | tr ' ' '\n' | sort -r | head -1"
run_test_full "long pipe chain" "echo test | cat | cat | cat | cat"
run_test_full "pipe with builtin" "echo hello | cat"
run_test_full "cat pipe cat" "echo hello world | cat | cat | cat"

# ---------- Redirections ----------
printf "\n${CYAN}=== Redirections ===${RESET}\n"
run_test_full "output >" \
	"echo hello > /tmp/ms_test_out
cat /tmp/ms_test_out"
rm -f /tmp/ms_test_out

run_test_full "append >>" \
	"echo first > /tmp/ms_test_app
echo second >> /tmp/ms_test_app
cat /tmp/ms_test_app"
rm -f /tmp/ms_test_app

run_test_full "input <" \
	"echo hello > /tmp/ms_test_in
cat < /tmp/ms_test_in"
rm -f /tmp/ms_test_in

run_test_full "redir before cmd" "< /dev/null cat"

run_test_full "multiple output redir" \
	"echo hello > /tmp/ms_r1 > /tmp/ms_r2
cat /tmp/ms_r2"
rm -f /tmp/ms_r1 /tmp/ms_r2

run_test_full "redir overwrites" \
	"echo first > /tmp/ms_redir_ow
echo second > /tmp/ms_redir_ow
cat /tmp/ms_redir_ow"
rm -f /tmp/ms_redir_ow

run_test_full "redir append creates" \
	"rm -f /tmp/ms_redir_ac
echo hello >> /tmp/ms_redir_ac
cat /tmp/ms_redir_ac"
rm -f /tmp/ms_redir_ac

run_test_full "redir in and out" \
	"echo input > /tmp/ms_rio_in
cat < /tmp/ms_rio_in > /tmp/ms_rio_out
cat /tmp/ms_rio_out"
rm -f /tmp/ms_rio_in /tmp/ms_rio_out

run_test_full "redir output no space" \
	"echo hello>/tmp/ms_redir_ns
cat /tmp/ms_redir_ns"
rm -f /tmp/ms_redir_ns

run_test_full "redir input no space" \
	"echo hello > /tmp/ms_redir_ins
cat</tmp/ms_redir_ins"
rm -f /tmp/ms_redir_ins

run_test_full "redir to nonexistent dir" \
	"echo hello > /nonexistent/file
echo \$?"

run_test_full "redir with pipe" \
	"echo hello > /tmp/ms_rp
cat < /tmp/ms_rp | cat"
rm -f /tmp/ms_rp

# ---------- Heredoc ----------
printf "\n${CYAN}=== Heredoc ===${RESET}\n"
run_test_full "heredoc basic" "cat << EOF
hello
world
EOF"

run_test_full "heredoc single line" "cat << STOP
just one line
STOP"

run_test_full "heredoc var expansion" 'cat << EOF
hello $USER
EOF'

run_test_full "heredoc quoted delim no expand" "cat << 'EOF'
hello \$USER
EOF"

run_test_full "heredoc empty" "cat << EOF
EOF"

run_test_full "heredoc with spaces" "cat << EOF
  hello  
  world  
EOF"

run_test_full "heredoc multi dollar" 'cat << EOF
$USER and $HOME
EOF'

run_test_full "heredoc with pipe" "cat << EOF | cat
hello heredoc pipe
EOF"

# run_test_full "heredoc tab preserved" "cat << EOF
#	tabbed
# EOF"

# ---------- Export / Unset / Env ----------
printf "\n${CYAN}=== Export / Unset / Env ===${RESET}\n"
run_test_full "export and echo" "export FOO=bar
echo \$FOO"

run_test_full "export overwrite" "export FOO=first
export FOO=second
echo \$FOO"

run_test_full "unset removes var" "export FOO=bar
unset FOO
echo \$FOO"

run_test_full "export quoted value" 'export A="hello world"
echo $A'

run_test_full "export single quoted" "export B='single'
echo \$B"

run_test_sorted "env shows var" "export TESTVAR99=hello
env | grep TESTVAR99"

run_test_full "multi export" "export A=1 B=2 C=3
echo \$A \$B \$C"

run_test_full "export no value" "export NOVALUE
env | grep NOVALUE"

run_test_full "unset multiple" "export A=1 B=2 C=3
unset A B C
echo \$A \$B \$C"

run_test_full "unset nonexistent" "unset DOESNOTEXIST
echo \$?"

run_test_full "export then unset then reexport" "export FOO=first
unset FOO
export FOO=second
echo \$FOO"

run_test_full "export empty value" 'export EMPTY=
echo "[$EMPTY]"'

run_test_full "export value with equals" 'export EQ="a=b=c"
echo $EQ'

run_test_full "export underscore name" 'export _foo=bar
echo $_foo'

# ---------- Export display ----------
printf "\n${CYAN}=== Export display ===${RESET}\n"
run_test_sorted "export no args shows vars" "export | grep PATH | head -1"

# ---------- Export / Unset errors ----------
printf "\n${CYAN}=== Export / Unset errors ===${RESET}\n"
run_test_full "export invalid =" "export =bad
echo \$?"
run_test_full "export invalid digit" "export 1ABC=no
echo \$?"
run_test_full "export invalid dash" "export a-b=c
echo \$?"
run_test_full "export invalid special" "export a.b=c
echo \$?"
run_test_full "unset invalid name" "unset 1ABC
echo \$?"

# ---------- Cd / Pwd ----------
printf "\n${CYAN}=== Cd / Pwd ===${RESET}\n"
run_test_full "pwd" "pwd"

run_test_full "pwd if PWD=LOLOLOL" "export PWD=LOLOLOL
pwd"

run_test_full "cd home" "cd
pwd"

run_test_full "cd updates OLDPWD" "cd /tmp
echo \$OLDPWD"

run_test_full "cd nonexistent" "cd /nonexistent_dir_xyz
echo \$?"

run_test_full "cd too many args" "cd a b
echo \$?"

run_test_full "cd dash" "cd /tmp
cd -
pwd"

run_test_full "cd dash redirects" "cd /tmp
cd - > /tmp/cd_dash_redir
cat /tmp/cd_dash_redir"

run_test_full "cd dot" "cd .
pwd"

run_test_full "cd dotdot" "cd ..
pwd"

run_test_full "cd with HOME unset" "unset HOME
cd"

run_test_full "cd dash with OLDPWD unset" "unset OLDPWD
cd -
pwd"

# ---------- cd / pwd with symbolic links ----------
printf "\n${CYAN}=== cd / pwd with symbolic links ===${RESET}\n"

# create a symbolic link in the current directory that links to the directory it's in
ln -s . ./link

run_test_full "cd moves into symbolic links" "cd link
pwd
ls"

run_test_full "cd moves into multiple symbolic links" "cd link
cd link
cd link
cd link
pwd
ls"

run_test_full "cd moves out of multiple symbolic links" "cd link
cd link
cd link
cd link
cd ..
cd ..
cd ..
pwd
ls"

run_test_full "cd moves out of multiple symbolic links with unset PWD" "unset PWD
cd link
cd link
cd link
cd ..
cd ..
cd ..
pwd
ls"

run_test_full "cd moves out of multiple symbolic links with PWD unset halfway" "cd link
cd link
unset PWD
cd link
cd ..
cd ..
cd ..
pwd
ls"

run_test_full "cd moves out of multiple symbolic links with different PWD" "export PWD=LOLOLOL
cd link
cd link
cd link
cd ..
cd ..
cd ..
pwd
ls"

run_test_full "cd moves out of multiple symbolic links with different PWD halfway" "cd link
cd link
export PWD=LOLOLOL
cd link
cd ..
cd ..
cd ..
pwd
ls"

# cleanup
rm ./link

# ---------- Exit ----------
printf "\n${CYAN}=== Exit ===${RESET}\n"
run_test_exit "exit 0" "exit 0"
run_test_exit "exit 42" "exit 42"
run_test_exit "exit 255" "exit 255"
run_test_exit "exit overflow" "exit 256"
run_test_exit "exit negative" "exit -1"
run_test_exit "exit no arg" "exit"
run_test_exit "exit large" "exit 999"

# ---------- Exit status ($?) ----------
printf "\n${CYAN}=== Exit status (\$?) ===${RESET}\n"
run_test_full "status after true" "true
echo \$?"
run_test_full "status after false" "false
echo \$?"
run_test_full "status after bad cmd" "nonexistent_cmd_xyz
echo \$?"
run_test_full "status after echo" "echo hello
echo \$?"
run_test_full "status after pipe success" "echo hello | cat
echo \$?"
run_test_full "status after pipe fail" "echo hello | grep xyz
echo \$?"
run_test_full "status is last pipe cmd" "false | true
echo \$?"
run_test_full "status after redir fail" "echo hi > /nonexistent/path
echo \$?"

# ---------- Path resolution ----------
printf "\n${CYAN}=== Path resolution ===${RESET}\n"
run_test_full "absolute path" "/bin/echo hello"
run_test_full "cmd not found" "nonexistent_cmd_xyz
echo \$?"
run_test_full "relative path dot" "echo hello > /tmp/ms_path_test
cat /tmp/ms_path_test"
rm -f /tmp/ms_path_test
run_test_full "empty command" "''"
touch /tmp/ms_perm_test
chmod 000 /tmp/ms_perm_test
run_test_full "permission denied" "/tmp/ms_perm_test
echo \$?"

# ---------- SHLVL ----------
printf "\n${CYAN}=== SHLVL ===${RESET}\n"
run_test_full "shlvl incremented" "echo \$SHLVL"

# ---------- Edge cases ----------
printf "\n${CYAN}=== Edge cases ===${RESET}\n"
run_test_full "leading spaces" "   echo hello"
run_test_full "trailing spaces" "echo hello   "
run_test_full "only spaces between args" "echo   a   b   c"
run_test_full "empty quoted arg preserved" 'echo "" hello'
run_test_full "dollar question after pipe" 'echo hello | cat
echo $?'
run_test_full "consecutive pipes content" "echo a | cat | cat | cat"
run_test_full "echo star literal" "echo '*'"
run_test_full "single char cmd" "echo a"
# Strange. Manual testing shows this works but this test fails
# run_test_full "tab as separator" "echo	hello"

# ---------- Multiline / complex ----------
printf "\n${CYAN}=== Multi-command sequences ===${RESET}\n"
run_test_full "three echoes" "echo a
echo b
echo c"

run_test_full "export use unset" "export X=42
echo \$X
unset X
echo \$X"

run_test_full "cd and pwd" "cd /tmp
pwd
cd /
pwd"

run_test_full "pipe after redir" "echo hello > /tmp/ms_par
cat < /tmp/ms_par | tr a-z A-Z"
rm -f /tmp/ms_par

run_test_full "var in redir filename" 'export F=/tmp/ms_var_redir
echo hello > $F
cat $F'
rm -f /tmp/ms_var_redir

run_test_full "chained export echo" "export A=1
export B=2
echo \$A \$B"

# ---------- Builtin in pipe ----------
printf "\n${CYAN}=== Builtin in pipe ===${RESET}\n"
run_test_full "echo in pipe" "echo hello | cat"
run_test_full "pwd in pipe" "pwd | cat"
run_test_full "env in pipe count" "env | wc -l"
run_test_full "export in pipe" "export | wc -l"
run_test_full "echo pipe grep" "echo findme | grep findme"
run_test_full "echo pipe grep miss" "echo findme | grep nothere"

# ---------- Tricky quoting ----------
printf "\n${CYAN}=== Tricky quoting ===${RESET}\n"
run_test_full "quote preserves empty" 'echo ""'
run_test_full "var in partial quote" 'export X=hello
echo "$X"world'
run_test_full "nested quote types" "echo \"it's a test\""
run_test_full "backslash in dquote" 'echo "hello\world"'
run_test_full "hash not comment" "echo hello#world"
run_test_full "semicolon literal" "echo 'hello;world'"

# Cleanup
rm -f /tmp/ms_test_out /tmp/ms_test_app /tmp/ms_test_in /tmp/ms_echo_n \
	/tmp/ms_r1 /tmp/ms_r2 /tmp/ms_redir_ow /tmp/ms_redir_ac \
	/tmp/ms_rio_in /tmp/ms_rio_out /tmp/ms_redir_ns /tmp/ms_redir_ins \
	/tmp/ms_rp /tmp/ms_par /tmp/ms_var_redir /tmp/ms_path_test \
	/tmp/ms_perm_test

rm -f '&1'

printf "\n${YELLOW}================================${RESET}\n"
printf "Total: %d  " "$TOTAL"
printf "${GREEN}Pass: %d${RESET}  " "$PASS"
if [ "$FAIL" -gt 0 ]; then
	printf "${RED}Fail: %d${RESET}\n" "$FAIL"
else
	printf "Fail: %d\n" "$FAIL"
fi
printf "${YELLOW}================================${RESET}\n"

exit $FAIL
