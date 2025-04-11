#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2;

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if (pid1 == -1) {
        perror("fork başarısız");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) { // Birinci çocuk: ls çıktısını ilk pipe'a yazar
        close(pipe1[0]);  // Okuma ucunu kapat
        dup2(pipe1[1], STDOUT_FILENO);  // stdout'u ilk pipe'a yönlendir
        close(pipe1[1]);
        execlp("ls", "ls", (char *)NULL);
        perror("execlp başarısız");
        exit(EXIT_FAILURE);
    }

    pid2 = fork();
    if (pid2 == -1) {
        perror("fork başarısız");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) { // İkinci çocuk: ilk pipe'dan okur, ikinci pipe'a yazar
        close(pipe1[1]);  // İlk pipe'ın yazma ucunu kapat
        close(pipe2[0]);  // İkinci pipe'ın okuma ucunu kapat
        dup2(pipe1[0], STDIN_FILENO);  // stdin'i ilk pipe'a yönlendir
        dup2(pipe2[1], STDOUT_FILENO);  // stdout'u ikinci pipe'a yönlendir
        close(pipe1[0]);
        close(pipe2[1]);
        execlp("grep", "grep", ".c", (char *)NULL);
        perror("execlp başarısız");
        exit(EXIT_FAILURE);
    }

    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[1]);  // Ebeveyn ikinci pipe'ın yazma ucunu kapat
    dup2(pipe2[0], STDIN_FILENO);  // stdin'i ikinci pipe'a yönlendir
    close(pipe2[0]);

    execlp("wc", "wc", "-l", (char *)NULL);
    perror("execlp başarısız");
    exit(EXIT_FAILURE);
}
