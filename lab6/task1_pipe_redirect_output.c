#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    pid_t pid;

    // Pipe oluştur
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork başarısız");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Çocuk: pipe'tan okur
        close(pipefd[1]);  // Yazma ucunu kapat
        dup2(pipefd[0], STDIN_FILENO);  // stdin'i pipe'ın okuma ucuna yönlendir
        close(pipefd[0]);
        execlp("wc", "wc", "-l", (char *)NULL);
        perror("execlp başarısız");
        exit(EXIT_FAILURE);
    } else { // Ebeveyn: pipe'a yazar
        close(pipefd[0]);  // Okuma ucunu kapat
        dup2(pipefd[1], STDOUT_FILENO);  // stdout'u pipe'ın yazma ucuna yönlendir
        close(pipefd[1]);
        execlp("ls", "ls", "-l", (char *)NULL);
        perror("execlp başarısız");
        exit(EXIT_FAILURE);
    }

    wait(NULL);  // Ebeveynin bitmesini bekleyin
    return 0;
}
