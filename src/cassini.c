#include "cassini.h"
#include "timing.h"
#include "sent_request.h"
#include "timing-text-io.h"

/*struct string
{
  uint32_t L;
  char* chaine[];
};

struct commandline
{
  uint32_t ARGC;
  struct string ARGV[];
};*/


const char usage_info[] = "\
   usage: cassini [OPTIONS] -l -> list all tasks\n\
      or: cassini [OPTIONS]    -> same\n\
      or: cassini [OPTIONS] -q -> terminate the daemon\n\
      or: cassini [OPTIONS] -c [-m MINUTES] [-H HOURS] [-d DAYSOFWEEK] COMMAND_NAME [ARG_1] ... [ARG_N]\n\
          -> add a new task and print its TASKID\n\
             format & semantics of the \"timing\" fields defined here:\n\
             https://pubs.opengroup.org/onlinepubs/9699919799/utilities/crontab.html\n\
             default value for each field is \"*\"\n\
      or: cassini [OPTIONS] -r TASKID -> remove a task\n\
      or: cassini [OPTIONS] -x TASKID -> get info (time + exit code) on all the past runs of a task\n\
      or: cassini [OPTIONS] -o TASKID -> get the standard output of the last run of a task\n\
      or: cassini [OPTIONS] -e TASKID -> get the standard error\n\
      or: cassini -h -> display this message\n\
\n\
   options:\n\
     -p PIPES_DIR -> look for the pipes in PIPES_DIR (default: /tmp/<USERNAME>/saturnd/pipes)\n\
";

int main(int argc, char * argv[]) {
  errno = 0;
  
  char * minutes_str = "*";
  char * hours_str = "*";
  char * daysofweek_str = "*";
  char * pipes_directory = NULL;

  uint16_t operation = CLIENT_REQUEST_LIST_TASKS;
  uint64_t taskid;

  struct timing* dest=malloc(sizeof(uint16_t)+sizeof(uint32_t)+sizeof(uint64_t));//malloc(13);
  int r_string_to_timing; 

  char* pipes_path="./run/pipes/saturnd-request-pipe";
  char* replay_path="./run/pipes/saturnd-reply-pipe";
  //char* ARG1, ARG2;
  int creat_position; //creat_position2;
  //uint32_t n_commande;

  int opt;
  char * strtoull_endp;
  while ((opt = getopt(argc, argv, "hlcqm:H:d:p:r:x:o:e:")) != -1) {
    switch (opt) {
    case 'm':
      minutes_str = optarg;//exemple de l'enoncé : ./cassini -c -m 0-2,5,10  ///minutes_str="0-2,5,10"
      break;
    case 'H':   
      hours_str = optarg; //./cassini -c -m 0-2,5,10 -H heure ///hours_str="heure"
      break;
    case 'd':
      daysofweek_str = optarg;
      break;
    case 'p':
      pipes_directory = strdup(optarg);//  ./cassini -p './run/pipes' -l    /// pipes_directory= "'./run/pipes'"
      if (pipes_directory == NULL) goto error;
      break;
    case 'l':
      operation = CLIENT_REQUEST_LIST_TASKS;
      break;
    case 'c':
      operation = CLIENT_REQUEST_CREATE_TASK;
      creat_position= optind;
      //ARG1=argv[optind];
      //ARG2=argv[optind+1];
      //printf("%d\n",optind);
      
      break;
    case 'q':
      operation = CLIENT_REQUEST_TERMINATE;
      break;
    case 'r':
      operation = CLIENT_REQUEST_REMOVE_TASK;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'x':
      operation = CLIENT_REQUEST_GET_TIMES_AND_EXITCODES;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'o':
      operation = CLIENT_REQUEST_GET_STDOUT;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'e':
      operation = CLIENT_REQUEST_GET_STDERR;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'h':
      printf("%s", usage_info);
      return 0; 
    case '?':
      fprintf(stderr, "%s", usage_info);
      goto error;
    }
  }

  // --------
  // | TODO |
  // --------
  
  switch(operation)
  {
    case CLIENT_REQUEST_LIST_TASKS :
    list_task_request(CLIENT_REQUEST_LIST_TASKS, pipes_path);//(test1)
    list_task_replay(replay_path);//// -l -> 0: * * * echo test-1(pour le test3)
    //printf("%d \t %d \n",list_retype, nbr_tache);
    break; 

    case CLIENT_REQUEST_CREATE_TASK : //(test3)
    //n_commande = argc-creat_position;
    //Vérifier si la récuperation du temps s'est exécuter correctement.
    r_string_to_timing=timing_from_strings(dest, minutes_str, hours_str, daysofweek_str);
    if(r_string_to_timing==-1)
    {
      printf("error de transformation du string_time en timing\n");
    }

    creat_task_request(pipes_path, dest, CLIENT_REQUEST_CREATE_TASK, argv,creat_position, argc);
    creat_task_reply(replay_path);
    break;

    case CLIENT_REQUEST_TERMINATE :
    quit_task_request(CLIENT_REQUEST_TERMINATE,pipes_path);//test16 is passed
    break;

    case CLIENT_REQUEST_REMOVE_TASK :
    
    remove_task_request(pipes_path, taskid,CLIENT_REQUEST_REMOVE_TASK);// test9 is passed
    //void remove_task_repaly(char* replay_path)
    break;

    case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES :
    getstdout_task_request(pipes_path,taskid, CLIENT_REQUEST_GET_TIMES_AND_EXITCODES);/*getstdout_task_request fonction aussi pour gettimesexitcode_task_replay*/
    gettimesexitcode_task_replay(replay_path);
    break;

    case CLIENT_REQUEST_GET_STDOUT: // test12
    getstdout_task_request(pipes_path,taskid, CLIENT_REQUEST_GET_STDOUT);
    //stderror_task_replay(replay_path);
    getstdout_task_replay(replay_path);
    break;

    case CLIENT_REQUEST_GET_STDERR :
    stderror_task_request( pipes_path,CLIENT_REQUEST_GET_STDERR,taskid);
    stderror_task_replay(replay_path);

    break;

    default :
    break;
  }

  return EXIT_SUCCESS;

 error:
  if (errno != 0) perror("main");
  free(pipes_directory);
  pipes_directory = NULL;
  return EXIT_FAILURE;
}