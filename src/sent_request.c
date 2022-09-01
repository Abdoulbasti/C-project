#include "cassini.h"
#include "timing.h"
#include "timing-text-io.h"
#include "sent_request.h"

//***************************************send and receive request for list***********************************************
void list_task_request(uint16_t op_list, char* m_path)
{
  uint big_a =htobe16(op_list);
  int fd, r_write;
  fd=open(m_path, O_WRONLY);
  if(fd==-1)
  {
    perror("error open");
    exit(1);
  }

  r_write=write(fd, &big_a,2);
  if(r_write==-1)
  {
    perror("error write");
    exit(1);
  }

  close(fd);
}


void list_task_replay(char* m_path)
{
  uint32_t L;
  uint16_t op_retype; 
  uint32_t nombre_tache; 
  uint64_t taskid;
  uint64_t minutes;
  uint32_t hours;
  uint32_t argc;
  uint8_t daysofweek;

  char** chaine;

  int l_minutes=sizeof(uint64_t);
  int l_hours=sizeof(uint32_t);
  int l_daysofweek=sizeof(uint8_t);
  int l_argc =sizeof(uint32_t);
  int l_taskid=sizeof(uint64_t);
  int l_L = sizeof(uint32_t);


  char* dest=malloc(1024);
  struct timing* time =malloc(l_minutes+l_hours+l_daysofweek);

  

  int fd;
  fd=open(m_path, O_RDONLY);
  if(fd==-1)
  {
    perror("error open");
    exit(1);
  }

  //Les deux prémier read sont les plus importants : il vont determiner les actions restant.
  read(fd, &op_retype,2);
  read(fd, &nombre_tache,4);
  op_retype=be16toh(op_retype);
  nombre_tache=be32toh(nombre_tache);
  //printf("%x\n",op_retype );
  //printf("%d\n", nombre_tache);

  if(nombre_tache==1)
  {
    read(fd, &taskid, l_taskid);
    read(fd, &minutes, l_minutes);
    read(fd, &hours, l_hours);
    read(fd, &daysofweek, l_daysofweek);
    read(fd, &argc,l_argc);

    taskid = be64toh(taskid);
    minutes =be64toh(minutes);
    hours = be32toh(hours);
    argc = be32toh(argc);
    /*printf("tache :%ld\n", taskid);
    printf("argc :%d\n", argc);
    printf("minutes :%lx\n", minutes);
    printf("hours :%x\n", hours);
    printf("daysofweek :%x\n",daysofweek);*/

    
    time->minutes=minutes;
    time->hours=hours;
    time->daysofweek=daysofweek;
    timing_string_from_timing(dest, time);
    printf("%ld: %s ", taskid, dest);
    //Gestion pour le tableau de string
    for(int j=0; j<argc; j++)
    {
      //Lecture de struct string
      read(fd, &L,l_L);//
      L=be32toh(L);
      //printf("L :%d\n",L);
      chaine =malloc(sizeof(char*)*L);
      chaine[j]=malloc(L);
      //chaine[1]=malloc(6);
      read(fd, chaine[j],L);
      printf("%s ",chaine[j]);
    }
  }
  else
  {
  for(int i=0; i<nombre_tache; i++)  
  {
    printf("\n");
    read(fd, &taskid, l_taskid);
    read(fd, &minutes, l_minutes);
    read(fd, &hours, l_hours);
    read(fd, &daysofweek, l_daysofweek);
    read(fd, &argc,l_argc);

    taskid = be64toh(taskid);
    minutes =be64toh(minutes);
    hours = be32toh(hours);
    argc = be32toh(argc);
    /*printf("tache :%ld\n", taskid);
    printf("argc :%d\n", argc);
    printf("minutes :%lx\n", minutes);
    printf("hours :%x\n", hours);
    printf("daysofweek :%x\n",daysofweek);*/

    
    time->minutes=minutes;
    time->hours=hours;
    time->daysofweek=daysofweek;
    timing_string_from_timing(dest, time);
    printf("%ld: %s ", taskid, dest);
    //Gestion pour le tableau de string
    for(int j=0; j<argc; j++)
    {
      //Lecture de struct string
      read(fd, &L,l_L);//
      L=be32toh(L);
      //printf("L :%d\n",L);
      chaine =malloc(sizeof(char*)*L);
      chaine[j]=malloc(L);
      //chaine[1]=malloc(6);
      read(fd, chaine[j],L);
      printf("%s ",chaine[j]);
  }
  }
  close(fd);
}
}


//***************************************send and receive request for creat***********************************************
//Une fonction chercher_tiret : renvoie 0 s'il le caractère à la position 0 est un -, -1 si non.
int chercher_tiret(char* str)
{
  if(str[0]=='-')
    return 0;
  else 
    return -1;
}

//requete creat(test2)
void creat_task_request(char* path, struct timing* des, uint16_t op_creat, char** ARGV,uint32_t c_position, uint32_t argc)
{
  uint32_t big_hours=htobe32(des->hours);
  uint64_t big_minutes=htobe64(des->minutes);
  uint8_t daysofw= des->daysofweek;
  char* buff_1=malloc(15);//Buffer pour le timing
  op_creat=htobe16(op_creat);

  int fd;
  fd=open(path, O_WRONLY);
  if(fd==-1)
  {
    perror("error open");
    exit(1);
  }
  memcpy(buff_1,&op_creat,2);
  memcpy(buff_1+2,&big_minutes,8);
  memcpy(buff_1+2+8,&big_hours,4);
  memcpy(buff_1+2+8+4,&daysofw,1);

  // écrire le le op_creat et le timing dans le tube d'écriture
  write(fd, buff_1, 15); 

  while(chercher_tiret(ARGV[c_position])==0)
  {
    c_position= c_position+2; 
  }
  uint32_t nbr_arg_commande=argc- c_position;
  nbr_arg_commande=htobe32(nbr_arg_commande);

  write(fd, &nbr_arg_commande, 4);
  
  for(int i=c_position; i<argc; i++) //Parcourire les commandes à exécuter 
  {

    uint32_t taille =strlen(ARGV[i]); //La taille de la chaine de caractère 
    //length_big[i]=htobe32(length[i]);
    uint32_t taille_big =htobe32(taille);
    write(fd, &taille_big, 4);
    write(fd, ARGV[i],taille);

}
  close(fd);
}


//void creat_task_reply(char* PATH, uint16_t c_retype, uint16_t t_id)// reponse de creat (test2)
void creat_task_reply(char* PATH)// reponse de creat (test2)
{
   uint16_t c_retype;
   uint64_t t_id;
   int fd=open(PATH, O_RDONLY);
   if (fd==-1)
   {
    perror("error open ");
    exit(1);
   }

   read(fd, &c_retype,2);
   read(fd, &t_id, 8);
   c_retype=be16toh(c_retype);// Big endian -> endian de la machine
   t_id = be64toh(t_id);

   printf("%ld\n", t_id);// affichage de la tache sur la sortie standard.

   close(fd);
}



//***************************************send and receive request for quit***********************************************

void quit_task_request(uint16_t op_quit,char* path)
{
  uint16_t big_a =htobe16(op_quit);
  int fd, r_write;
  fd=open(path, O_WRONLY);
  if(fd==-1)
  {
    perror("error open");
    exit(1);
  }

  r_write=write(fd, &big_a,2);
  if(r_write==-1)
  {
    perror("error write");
    exit(1);
  }

  close(fd);
}



//***************************************send and receive request for remove***********************************************

void remove_task_request(char* request_path, uint64_t taskid, uint16_t remove)
{
  int l_taskid=sizeof(uint64_t);
  int l_remove=sizeof(uint16_t);
  int l_remove_request =l_remove+l_taskid;
  int fd;
  char* buff=malloc(l_remove+l_taskid);

  fd=open(request_path, O_WRONLY);
  if(fd==-1)
  {
    perror("open error");
    exit(1);
  }

  //Mettre les données en big endian 
  taskid=htobe64(taskid);
  remove=htobe16(remove);

  //conctaenation dans buff
  memcpy(buff, &remove, l_remove);
  memcpy(buff+l_remove, &taskid, l_taskid);

  //ecrire dans resquest_path
  write(fd, buff,l_remove_request);

  close(fd);
}

void remove_task_repaly(char* replay_path)
{
  int fd;
  int l_remove_replay=sizeof(uint16_t);
  //char* buff=malloc(l_remove_replay);
  uint16_t buff;
  fd=open(replay_path,O_RDONLY);
  if(fd==-1)
  {
    perror("open error");
    exit(1);
  }

  read(fd, &buff, l_remove_replay);

  buff=be16toh(buff);

  printf("%x\n", buff);

  close(fd);
}



//***************************************send and receive request for stderror***********************************************

void stderror_task_request(char* request_path, uint16_t stderror,  uint64_t taskid)
{  
  int l_stderror=sizeof(uint16_t);
  int l_taskid=sizeof(uint64_t);
  int l_stderror_request =l_stderror+l_taskid;
  int fd;
  char* buff=malloc(l_stderror+l_taskid);

  fd=open(request_path, O_WRONLY);
  if(fd==-1)
  {
    perror("open error");
    exit(1);
  }

  //Mettre les données en big endian 
  taskid=htobe64(taskid);
  stderror=htobe16(stderror);

  //conctaenation dans buff
  memcpy(buff, &stderror, l_stderror);
  memcpy(buff+l_stderror, &taskid, l_taskid);

  //ecrire dans resquest_path
  write(fd, buff, l_stderror_request);

  close(fd);
}


void stderror_task_replay(char* replay_path)
{
  //struct string* s;
  char* chaine;
  uint32_t longeur; 
  int l_retype=sizeof(uint16_t);
  int l_errcode = sizeof(uint16_t);
  int l_longeur = sizeof(uint32_t);
  uint16_t retype, errcode; 
  int fd;


  fd=open(replay_path, O_RDONLY);
  if (fd==-1)
  {
    perror("open error");
    exit(1);
  }

  read(fd, &retype, l_retype);

  retype=be16toh(retype);
  //printf("%x\n", retype);

  if(retype==0x4f4b) //si c'est ok
  {
    read(fd, &longeur, l_longeur);
    chaine=malloc(longeur);
    read(fd, chaine, longeur);
    printf("%s\n", chaine); 
  }
  else if(retype==0x4552) // s'il y'a erreur
  {
    read(fd, &errcode, l_errcode);
    errcode=be16toh(errcode);

    if(errcode==0x4e46)
    {

    }
    else if(errcode==0x4e52)
    {

    }
  }
  close(fd);
}



//***************************************send and receive request for getstdout***********************************************

void getstdout_task_request(char* request_path, uint64_t taskid, uint16_t remove)
{
  int l_taskid=sizeof(uint64_t);
  int l_remove=sizeof(uint16_t);
  int l_remove_request =l_remove+l_taskid;
  int fd;
  char* buff=malloc(l_remove+l_taskid);

  fd=open(request_path, O_WRONLY);
  if(fd==-1)
  {
    perror("open error");
    exit(1);
  }

  //Mettre les données en big endian 
  taskid=htobe64(taskid);
  remove=htobe16(remove);

  //conctaenation dans buff
  memcpy(buff, &remove, l_remove);
  memcpy(buff+l_remove, &taskid, l_taskid);

  //ecrire dans resquest_path
  write(fd, buff,l_remove_request);

  close(fd);
}

void getstdout_task_replay(char* replay_path)
{
  int fd;
  int l_stdout_replay=sizeof(uint16_t);
  int l_errcode = sizeof(uint16_t);
  int l_taille_replay=sizeof(uint32_t);
  //char* buff=malloc(l_remove_replay);
  uint16_t stdout;
  uint16_t errcode;
  uint32_t taille;

  fd=open(replay_path,O_RDONLY);
  if(fd==-1)
  {
    perror("open error ");
    exit(1);
  }

  read(fd, &stdout,l_stdout_replay);
  stdout=be16toh(stdout);
  if(stdout==0x4f4b)
  {
  read(fd, &taille, l_taille_replay);
  char* chaine =malloc(taille);
  read(fd,chaine, taille);
  //printf("%x\n", stdout);
  printf("%s\n", chaine);
  }
  else if(stdout==0x4552) // s'il y'a erreur
  {
    //printf("%x", stdout);

    read(fd, &errcode, l_errcode);
    errcode=be16toh(errcode);

    if(errcode==0x4e46)
    {
      //printf("%x", errcode);
      exit(1);
    }
    else if(errcode==0x4e52)
    {
      exit(1);
      //printf("%x", errcode);
    }
  }

  close(fd);
}


//***************************************send and receive request for gettimesexitcode***********************************************

void gettimesexitcode_task_replay(char* replay_path)
{
  int fd;
  int l_stdout_replay=sizeof(uint16_t);
  int l_errcode = sizeof(uint16_t);
  //int l_taille_replay=sizeof(uint32_t);
  int l_NBRUNS = sizeof(uint32_t);
  //char* buff=malloc(l_remove_replay);
  uint16_t stdout;
  uint16_t errcode;
  uint32_t NBRUNS;

  fd=open(replay_path,O_RDONLY);
  if(fd==-1)
  {
    perror("open error ");
    exit(1);
  }

  read(fd, &stdout,l_stdout_replay);
  stdout=be16toh(stdout);   

  if(stdout==0x4f4b) //cas ou c'est OK  
  {

  read(fd, &NBRUNS, l_NBRUNS);
  NBRUNS=be32toh(NBRUNS);

  uint16_t EXITCODE[NBRUNS];
  uint64_t TIME[NBRUNS];

  //for(int i=0; i<NBRUNS; i++)
  int i=0;
  struct tm* temps;
  char s1[50];
  char s2[50];
  long int TIME_CAST;
  while(NBRUNS>i)  
  {
     read(fd, &TIME[i], sizeof(uint64_t));
     read(fd, &EXITCODE[i], sizeof(uint16_t));

     TIME[i]= be64toh(TIME[i]);
     EXITCODE[i]= be16toh(EXITCODE[i]);

     TIME_CAST=(long int)TIME[i];// Caster uint64 en long int
     temps=localtime(&TIME_CAST);    

     strftime(s1, 50,"%F",temps);// recuper le temps la date
     strftime(s2, 50,"%X",temps);// Récuperer l'heure
     printf("%s %s %d \n", s1, s2, EXITCODE[i]);
     i++; 
  } 
  }

  else if(stdout==0x4552) // cas ou il y'a erreur
  {
    read(fd, &errcode, l_errcode);
    errcode=be16toh(errcode);
    //printf("%x\n", errcode);
    exit(1);
  }

  close(fd);
}