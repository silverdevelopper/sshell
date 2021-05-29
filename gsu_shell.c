#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
/* Include our own header files */
#include "parser.h"
#include "process.h"

#define VERSION "1.1"
#define PROMPT ">"

int main(int argc, char *argv[])
{

    /* Komut satiri ile ilgili bilgileri tutan struct'i tanimliyoruz. */
    CommandLine cl;

    /* Yukaridaki cl degiskenini gosteren bir gosterici tanimliyoruz. */
    CommandLine *cl_ptr = &cl;

    /* TODO: Kullanicinin home dizinini kaydedin. */

    char *HOME = getenv("HOME");
    while (1)
    {
        /* Ekrana PROMPT ile tanimli komut satiri bekleyicisini yaziyoruz. */
        printf("%s ", PROMPT);

        /* shell_process_line() ile satiri okuyup ayristiriyoruz. Artik cl_ptr
         * ile gosterilen CommandLine yapisindaki komut satirina dair bilgiler
         * var.
         */
        shell_process_line(cl_ptr);

        /* Eger komut satiri okunurken bir hata olduysa, error_code degiskeni
         * ayarlaniyor. shell_print_error() ise struct'taki error_code'un
         * degerine bakarak hata varsa ekrana bunu basiyor. continue ile
         * dongunun basina donuyoruz.
         */
        if (cl_ptr->error_code > 0)
        {
            shell_print_error(cl_ptr);
            printf("ERROR\n");
            continue;
        }

        /* Boru hatti olsun olmasin, cl_ptr->first_argv icinde bir komut yaziyor
         * olmali. Eger NULL ise kullanici hicbir sey yazmadan Enter'a basip
         * gecti demektir.
         */
        if (cl_ptr->first_argv != NULL)
        {
            /* Once gomulu komut mu degil mi diye bakiyoruz. Eger
             * komut gomuluyse, boru hatti olup olmamasiyla ilgilenmiyoruz,
             * dogrudan calistiriyoruz.
             */
            if (strcmp(cl_ptr->first_argv[0], "version") == 0)
            {
                printf("Shell version: %s\n", VERSION);
                shell_free_args(cl_ptr);
            }

            else if (strcmp(cl_ptr->first_argv[0], "exit") == 0)
            {
                shell_free_args(cl_ptr);
                exit(EXIT_SUCCESS);
            }

            else if (strcmp(cl_ptr->first_argv[0], "cd") == 0)
            {
                int res = 0;
                if (cl_ptr->first_argv[1] != NULL)
                {
                    printf("Changing directory to: %s", cl_ptr->first_argv[1]);
                    res = chdir(cl_ptr->first_argv[1]);
                }
                else
                {
                    printf("Going to Home directory!");
                    res = chdir(HOME);
                }
                if (res < 0)
                {
                    printf("Path not found!");
                    perror("gsu_shell");
                }
                /* TODO: chdir fonksiyonunu uygun sekilde calistirip donus degerini alin. Fonksiyon icine gidilecek
                dizin bilgisi yazilmaldir. Dizin bilgisi parametre olarak yok ise basta alinan ev dizinine gidilmelidir.*/

                /* TODO: Donus degeri 0'dan kucuk ise ekrana hata mesaji basalim. */

                /* TODO: cl_ptr icin tutulan bellek alanini yukaridaki orneklerdeki gibi sisteme geri verelim */
                shell_free_args(cl_ptr);
            }

            /* Komut satiri gomulu komutla baslamiyorsa, normal
             * prosedur isleyecek: Komut(lar) yeni yaratilan cocuk
             * surec(ler) tarafindan exec edilecek.
             *
             * Ayrintilar icin PDF'e bakin!
             */
            /*
            else if(strcmp(cl_ptr->first_argv[0]," ") == 0 || strcmp(cl_ptr->first_argv[0],"\n")){
                continue;
            }*/
            else
            {
                /* Cocuk sureclerin PID'lerini tutmak icin. */
                pid_t first_child, second_child;

                /* 2. Asamaya dair geri kalan her sey bu bolgeye yazilacak. 
            * Burada tasarim daha cok sizden beklenmektedir.*/

                /* TODO: Ilk olarak pipe icin dosya tanimlayicilarini tutacak bir dizi tanimlayin.
            * Eger komut satirinda pipe girilmis ise uygun sekilde boru hattini olusturun.
                    */

                int pipe_fd1[2],pipe_fd2[2];
                // int pipe_fd2[2];

                /* TODO: Bu noktada ise cocuk surec yaratmaya baslayacagiz. Eger pipe varsa parent surec iki 
            * cocuk surec yaratmali ve cocuk surecler icinde dup2 fonksiyonu uygun sekilde kullanilmalidir. 
            * Pipe yoksa da bir cocuk surec yaratilmalidir. Pipe olsa da olmasa da cl_ptr->first_argv degeri 
            * birinci cocuk tarafindan shell_exec_cmd ile calistirilmalidir. 
            * Pipe varsa cl_ptr->second_argv degeri ikinci cocuk  tarafindan calistirilmalidir. 
                    * Boru hattinin uclarinin kapatilmasi, cocuk surec veya sureclerin
                    * geri donus degerlerinin alinmasi ve verimli bellek kullanimi gibi islemler de uygulanmalidir.
            */

                int child_retval1 = -1, child_retval2 = -1;
                int fstat, sstat;
                char concat_str[4096], buffer[4096], buffer2[4096];
                int file_desc;
                int num_read;
                FILE *fptr, *fptr2;
                if (cl_ptr->has_pipe)
                {
                    if (pipe(pipe_fd1) == -1 || pipe(pipe_fd2) == -1)
                    {
                        perror("Can't create pipe!");
                        shell_free_args(cl_ptr);
                        exit(EXIT_FAILURE);
                    }
                }
                int fres, sres;
                switch ((first_child = fork()))
                {
                case -1:
                    perror("Fork Failed");
                    shell_free_args(cl_ptr);
                    break;
                case 0: // firt_child process
                    if (cl_ptr->has_pipe)
                    {

                        file_desc = open("output.txt", O_WRONLY);
                        if (file_desc < 0)
                        {
                            perror("Error opening the file\n");
                            shell_free_args(cl_ptr);
                            exit(EXIT_FAILURE);
                            break;
                        }
                        dup2(file_desc, STDOUT_FILENO);
                        fres = shell_exec_cmd(cl_ptr->first_argv);
                        close(file_desc);

                        
                        close(pipe_fd2[0]);
                        char txt[] = "output.txt|\0";
                        write(pipe_fd2[1], txt, sizeof(txt)+1);
                        //close(pipe_fd1[0]);
                        //close(pipe_fd1[1]);
                        close(pipe_fd2[1]);
                        shell_free_args(cl_ptr);
                    }
                    else
                    {
                        //no pipe first child run
                        fres = shell_exec_cmd(cl_ptr->first_argv);
                    }
                    shell_free_args(cl_ptr);
                    exit(fres);
                    break;
                default: // parent process

                    waitpid(first_child, &fstat, 0);
                    if (WIFEXITED(fstat))
                    {
                        child_retval1 = WEXITSTATUS(fstat);
                        printf("Exit status of the first child was %d\n",
                               child_retval1);
                    }
                    if (child_retval1 == 1)
                        continue;
                    break;
                }
                // parent goes here
                if (cl_ptr->has_pipe)
                {
                    //printf("Pipe var \n");
                    //close(pipe_fd2[1]);
                    //read(pipe_fd2[0], buffer,11);
                    //close(pipe_fd2[0]);
                    //close(pipe_fd1[1]);
                    //printf("pipe buffer: %s\n", buffer);
                    
                    write(pipe_fd1[1], "output.txt\0",11);
                    //close(pipe_fd1[1]);
                    switch ((second_child = fork()))
                    {
                    case -1:
                        perror("Fork Failed\n");
                        break;
                    case 0: // second_child process
                        
                        num_read = read(pipe_fd1[0], buffer2, 11);
                        if(num_read<0)
                        {
                            perror("Error");
                            exit(EXIT_FAILURE);
                        }
                        
             
                        int fd2 = open(buffer2, O_RDONLY);
                        dup2(fd2, STDIN_FILENO);

                        sres = shell_exec_cmd(cl_ptr->second_argv);
                        close(fd2);
                        //close(pipe_fd1[0]);
                        close(pipe_fd1[1]);
                        
                        exit(sres);
                        break;
                    }
                    //pipe var parent
                    waitpid(second_child, &sstat, 0);
                    if (WIFEXITED(sstat))
                    {
                        child_retval2 = WEXITSTATUS(sstat);
                        printf("Exit status of the second_child was %d\n",
                               child_retval2);
                    }
                    //shell_free_args(cl_ptr);
                }
                shell_free_args(cl_ptr);
            } /* else */
        }     /* if (cl_ptr) */
    }         /* while(1) */

    return 0;
}
