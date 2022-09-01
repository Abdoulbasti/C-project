#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char const *argv[])
{
	char repertoire[] = "/tmp/abdoul/saturnd/pipes/";
	char tube_requet[] ="/tmp/abdoul/saturnd/pipes/saturnd-request-pipe";
	char tube_replay[] ="/tmp/abdoul/saturnd/pipes/saturnd-reply-pipe";
	//mkdir(repertoire, 0755);
	if (mkdir("/tmp/abdoul/saturnd/pipes/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)==-1)
	{
		perror("erreur de création de la repertoire ");
		//rmdir(tube_requet);
		//rmdir("/home/abdoul/Bureau/pipes");
		exit(1);
	}


	/*int r_requet, r_replay;
	int fd_requet, fd_replay;
	
	//r_requet= mkfifo(tube_requet, 0777);
	if (mkfifo(tube_requet, 0777)==-1)
		{
			perror("erreur de création de la tube de requette ");
			unlink(tube_requet);
			exit(1);
		}

	//r_replay= mkfifo(tube_replay, 0777);
	if (mkfifo(tube_replay, 0777)==-1)
		{
			perror("erreur de création de la tube de reponse"); 
			unlink(tube_replay);//supression de la tube si elle est créer
			exit(1);
		}

	fd_requet=open(tube_requet, O_WRONLY); //tube de requête
	if (fd_requet==-1)
		{perror("erreur d'ouverture de la tube de requette"); exit(1);}

	fd_replay=open(tube_replay, O_RDONLY);// tube de lecture /*fd_replay=open(tube_replay, O_RDONLY | O_CREAT);*/
	/*if(fd_replay==-1)
		{perror("erreur d'ouverture de la tube de reponse"); exit(1);}*/


	return 0;
}