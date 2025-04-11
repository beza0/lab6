#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
    int fd;
    pid_t pid;

    // Geçici dosyayı oluşturun
    fd = open("temp.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (fd == -1) {
        perror("Dosya açılamadı");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork başarısız");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Çocuk: yazıcı
        dup2(fd, STDOUT_FILENO);  // stdout'u temp.txt'ye yönlendir
        close(fd);
        execlp("ls", "ls", "-l", (char *)NULL);
        perror("execlp başarısız");
        exit(EXIT_FAILURE);
    } else { // Ebeveyn: okuyucu
        wait(NULL); // Yazıcı işleminin bitmesini bekleyin

        pid = fork();
        if (pid == -1) {
            perror("fork başarısız");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Çocuk: okuyucu
            fd = open("temp.txt", O_RDONLY);
            if (fd == -1) {
                perror("Dosya açılamadı");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);  // stdin'i temp.txt'ye yönlendir
            close(fd);
            execlp("wc", "wc", "-l", (char *)NULL);
            perror("execlp başarısız");
            exit(EXIT_FAILURE);
        }
    }

    wait(NULL);  // Okuyucu işleminin bitmesini bekleyin
    remove("temp.txt");  // Geçici dosyayı silin
    return 0;
}
