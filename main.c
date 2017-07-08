#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
/**
    Declararea functiilor pentru comenzile shell
    */
int ms_cd(char **args);
int ms_help(char **args);
int ms_exit(char **args);
/**
    Lista comenzilor, urmate de functiile corespondente
    */
char *builtin_str[] = {"cd", "help", "exit"};
int (*builtin_func[]) (char **)= {&ms_cd, &ms_help, &ms_exit};
int ms_num_builtins(){
    return sizeof(builtin_str)/sizeof(char*);
}
/**
    Implementarea functiilor
    @brif Comanda cd: change directory
    @param Lista argumentelor : args[0] = cd, args[1]=directory
    @return 1 pentru continuarea executiei.
    */
int ms_cd(char **args){
    if(args[1]==NULL){
        fprintf(stderr, "MhcrnlShell asteapta argumentul \"cd\" \n");
    } else {
        if (chdir(args[1])!= 0){
            perror("MhcrnlShell");
        }
    }
    return 1;
}
/**
    @brief Comanda : print help
    @Param : lista argumentelor
    @return: 1 pentru continuarea executiei
    */
int ms_help(char **args){
    int i;
    printf("MhcrnlShell\n");
    printf("Scrieti numele programului apoi lista argumentelor si enter.\n");
    printf("Urmatoarele functii sunt incluse:\n");
    for(i=0; i< ms_num_builtins();i++){
        printf("%s\n", builtin_str[i]);
    }
    printf("Utilizati comanda man pentru info despre alte prog.\n");
    return 1;
}
/**
    @brief Comanda exit
    @param Lista argumentelor
    @return : 0 petru incheierea executiei
    */
int ms_exit(char **args){ return 0; }
/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int ms_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("MhcrnlShell");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("MhcrnlShell");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}
/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int ms_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < ms_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return ms_launch(args);
}
#define LSH_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *ms_read_line(void)
{
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "MhcrnlShell: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **ms_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "MhcrnlShell: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}
/**
   @brief Loop getting input and executing it.
 */
void ms_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = ms_read_line();
    args = ms_split_line(line);
    status = ms_execute(args);

    free(line);
    free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main()
{
    printf("Hello world! Din ms_shell!\n");
    ms_loop();
    return EXIT_SUCCESS;
}
