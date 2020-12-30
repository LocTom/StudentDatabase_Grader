#include <unistd.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h> 
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>


void concatstr(const char* src, char dest[])
{
    int i, j = 0;
    for (i = 0; dest[i] != '\0'; i++);

    while (src[j] != '\0') {
        dest[i] = src[j];
        i++;
        j++;
    }

    dest[i] = '\0';
}

void copystr(char* a, char dest[])
{
    int i = 0;
    while (a[i] != '\0')
    {
        dest[i] = a[i];
        i++;
    }

    dest[i] = '\0';
}


int main(int argc, char* argv[])
{
     char* prog_out = "./prog_out.txt";
    char path0[250]; /* paths from config*/
    char path1[250]; /* paths from config*/
    char path2[250]; /* paths from config*/
    char fstudents[250];
    DIR* dir;
    struct dirent* myDirent;

    char* list[250];
    int i;
    for (i=0; i < 5; i++) {
        list[i] = NULL;
    }

    //gets locations from config
    int dis = open(argv[1], O_RDONLY);
    if (dis < 0)
    {
        perror("File could not be opened\n");
        exit(-1);
    }

    int keyboard_fd = dup(0);//Store keyboard address
    if (keyboard_fd < 0)
    {
        perror("dup failed!\n");
        exit(-1);
    }

    dup2(dis, 0);
    if (dis < 0)
    {
        perror("dup failed!\n");
        exit(-1);
    }

    scanf("%s", path0); //path0= students folder
    scanf("%s", path1); //path1= test_input.qa
    scanf("%s", path2); //path2= output.qa
    dup2(keyboard_fd, 0);
    if (keyboard_fd< 0)
    {
        perror("dup failed!\n");
        exit(-1);
    }

    int ch=close(dis);
    if (ch < 0)
    {
        perror("File not closed");
        exit(-1);
    }


    int results = open("results.csv", O_CREAT | O_WRONLY |O_TRUNC, 0666);
    if (results < 0)
    {
        perror("File could not be opened\n");
        exit(-1);
    }

    int out = open(path2, O_RDONLY);
    if (out < 0)
    {
        perror("File could not be opened\n");
        exit(-1);
    }

   

    //here we will run the main and output it to program_output.txt then go out of this kid and to parent
    dir = opendir(path0);
    if (dir == NULL)
    {
        perror("File not opened");
        exit(-1);
    }
    readdir(dir);
    readdir(dir);
    myDirent = readdir(dir); //gets metadata of first file/folder

    while (myDirent != NULL)//here need student loop
    {
        int progoutput;
        if ((progoutput = open(prog_out, O_CREAT | O_WRONLY | O_TRUNC, 0666)) < 0) // create file and check if opened
        {
            exit(-1);
        }
		
		int in = open(path1, O_RDONLY);
		if (in < 0)
		{
			perror("File could not be opened\n");
			exit(-1);
		}
    
    
        copystr(path0, fstudents);

        pid_t pid;
        int stat, res1, res2;
        if ((pid = fork()) == -1)
        {
            perror("Fork didnt happen");
            exit(-1);
        }
        else if (pid != 0)
        {
            wait(&stat); //father is wating for son to exit
        }
        else
        {
            int inner = dup2(in, 0);
            if (inner < 0)
            {
                perror("dup failed!\n");
                exit(-1);
            }

           int pout= dup2(progoutput, 1);
           if (pout < 0)
           {
               perror("dup failed!\n");
               exit(-1);
           }
            
           //list[0] = fstudents;
            //copystr(path0, list[0]);
            concatstr("/", fstudents);
            concatstr(myDirent->d_name, fstudents);
            concatstr("/main.out", fstudents);
            list[0]="main.out\0";
			list[1]=NULL;
            res1 = execvp(fstudents, list);
            if(res1<0) 
            {
				exit(-1);
			}
        }

        //second fork
        if ((pid = fork()) == -1)
        {
            perror("Fork didnt happen");
            exit(-1);
        }
        else if (pid != 0)
        {
			fflush(stdin);
			fflush(stdout);
            wait(&stat); //father is wating for son to exit
        }
        else
        {
			list[0]="./comp.out";
			list[1]=prog_out;
			list[2]=path2;
			list[3]=NULL;
			
            /*copystr("./comp.out", list[0]);
            concatstr(prog_out, list[1]);
            copystr("./output.qa", list[2]); //this is the expected output
*/
            res2 = execvp("./comp.out", (char**) list); //the list now has three arguments
            if(res2<0) 
            {
				perror("comp didnt happen");
				exit(-1);
			}
            
        }


        int ans = WEXITSTATUS(stat);
        //switching screen to prooutput
        int screen_fd = dup(1);//Store screen address
        int res= dup2(results, 1);
            if (res < 0)
            {
            perror("dup failed!\n");
            exit(-1);
             }

        if (ans == 2)
        {
            printf( "%s,100\n", myDirent->d_name);
        }
        else printf("%s,0\n", myDirent->d_name);
        fflush(stdout);
		
        //bringing back screen
        int scrfd =dup2(screen_fd, 1);
        if (scrfd < 0)
        {
            perror("dup failed!\n");
            exit(-1);
        }

        ch = close(progoutput);
        if (ch < 0)
        {
            perror("File not closed");
            exit(-1);
        }
        
        
		ch=close(in);
    if (ch < 0)
    {
        perror("File not closed");
        exit(-1);
    }
        myDirent = readdir(dir); //gets metadata of file/folder

    }
    ch=close(results);
    if (ch < 0)
    {
        perror("File not closed");
        exit(-1);
    }
    
    ch=close(out);
    if (ch < 0)
    {
        perror("File not closed");
        exit(-1);
    }
    ch=closedir(dir);		/* free allocated structures */
    if (ch < 0)
    {
        perror("File not closed");
        exit(-1);
    }

	return(0);
}

//have a folder of stuendts name now need to open it and run thier main.out with test input.qa










