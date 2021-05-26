#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Include our own header files */
#include "parser.h"
#include "process.h"

#define VERSION "1.1"
#define PROMPT  ">"

int main(int argc, char *argv[]) {

    /* Komut satiri ile ilgili bilgileri tutan struct'i tanimliyoruz. */
    CommandLine cl;

    /* Yukaridaki cl degiskenini gosteren bir gosterici tanimliyoruz. */
    CommandLine *cl_ptr = &cl;

    /* TODO: Kullanicinin home dizinini kaydedin. */
    

    while (1) {
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
        if (cl_ptr->error_code > 0) {
            shell_print_error(cl_ptr);
            continue;
        }

        /* Boru hatti olsun olmasin, cl_ptr->first_argv icinde bir komut yaziyor
         * olmali. Eger NULL ise kullanici hicbir sey yazmadan Enter'a basip
         * gecti demektir.
         */
        if (cl_ptr->first_argv != NULL) {
            /* Once gomulu komut mu degil mi diye bakiyoruz. Eger
             * komut gomuluyse, boru hatti olup olmamasiyla ilgilenmiyoruz,
             * dogrudan calistiriyoruz.
             */
            if (strcmp(cl_ptr->first_argv[0], "version") == 0) {
                printf("Shell version: %s\n", VERSION);
                shell_free_args(cl_ptr);
            }

            else if (strcmp(cl_ptr->first_argv[0], "exit") == 0) {
                shell_free_args(cl_ptr);
                exit(EXIT_SUCCESS);
            }

            else if (strcmp(cl_ptr->first_argv[0], "cd") == 0) {
                /* TODO: chdir fonksiyonunu uygun sekilde calistirip donus degerini alin. Fonksiyon icine gidilecek
                dizin bilgisi yazilmaldir. Dizin bilgisi parametre olarak yok ise basta alinan ev dizinine gidilmelidir.*/
                
                /* TODO: Donus degeri 0'dan kucuk ise ekrana hata mesaji basalim. */
                
                /* TODO: cl_ptr icin tutulan bellek alanini yukaridaki orneklerdeki gibi sisteme geri verelim */
                
            }

            /* Komut satiri gomulu komutla baslamiyorsa, normal
             * prosedur isleyecek: Komut(lar) yeni yaratilan cocuk
             * surec(ler) tarafindan exec edilecek.
             *
             * Ayrintilar icin PDF'e bakin!
             */
            else {
                /* Cocuk sureclerin PID'lerini tutmak icin. */
                pid_t first_child, second_child;
                

		/* 2. Asamaya dair geri kalan her sey bu bolgeye yazilacak. 
		 * Burada tasarim daha cok sizden beklenmektedir.*/

                /* TODO: Ilk olarak pipe icin dosya tanimlayicilarini tutacak bir dizi tanimlayin.
		 * Eger komut satirinda pipe girilmis ise uygun sekilde boru hattini olusturun.
                 */

                /* TODO: Bu noktada ise cocuk surec yaratmaya baslayacagiz. Eger pipe varsa parent surec iki 
		 * cocuk surec yaratmali ve cocuk surecler icinde dup2 fonksiyonu uygun sekilde kullanilmalidir. 
		 * Pipe yoksa da bir cocuk surec yaratilmalidir. Pipe olsa da olmasa da cl_ptr->first_argv degeri 
		 * birinci cocuk tarafindan shell_exec_cmd ile calistirilmalidir. 
		 * Pipe varsa cl_ptr->second_argv degeri ikinci cocuk  tarafindan calistirilmalidir. 
                 * Boru hattinin uclarinin kapatilmasi, cocuk surec veya sureclerin
                 * geri donus degerlerinin alinmasi ve verimli bellek kullanimi gibi islemler de uygulanmalidir.
 		 */


            } /* else */
        } /* if (cl_ptr) */
    } /* while(1) */

    return 0;
}
