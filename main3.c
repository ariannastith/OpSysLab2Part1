#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

static void run_child_work(void) {
    // Seed RNG uniquely per child using PID and current time
    srandom((unsigned) (time(NULL) ^ getpid()));

    // Random number of iterations in [1, 30]
    int iters = (int)(random() % 30) + 1;

    for (int i = 0; i < iters; i++) {
        printf("Child Pid: %d is going to sleep!\n", getpid());
        fflush(stdout);

        // Random sleep in [1, 10] seconds (avoid 0 so output is visible)
        unsigned int nap = (unsigned int)(random() % 10) + 1;
        sleep(nap);

        printf("Child Pid: %d is awake!\nWhere is my Parent: %d?\n",
               getpid(), getppid());
        fflush(stdout);
    }

    // Done
    exit(0);
}

int main(void) {
    pid_t child1, child2;

    // Parent can seed too (not required for children)
    srandom((unsigned) time(NULL));

    // Fork first child
    child1 = fork();
    if (child1 < 0) {
        perror("fork child1");
        return 1;
    }
    if (child1 == 0) {
        run_child_work(); // never returns
    }

    // Fork second child
    child2 = fork();
    if (child2 < 0) {
        perror("fork child2");
        return 1;
    }
    if (child2 == 0) {
        run_child_work(); // never returns
    }

    // Parent: wait for both children
    int status;
    int remaining = 2;
    while (remaining > 0) {
        pid_t done = wait(&status);
        if (done == -1) {
            if (errno == EINTR) continue; // interrupted; try again
            perror("wait");
            break;
        }

        if (WIFEXITED(status)) {
            printf("Child Pid: %d has completed (exit status %d)\n",
                   done, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child Pid: %d terminated by signal %d\n",
                   done, WTERMSIG(status));
        } else {
            printf("Child Pid: %d ended (unknown status)\n", done);
        }
        fflush(stdout);
        remaining--;
    }

    return 0;
}
