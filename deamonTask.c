#define PROC_FILE_PATH ~/workspace/
static int32_t deamonTrytoLockfile(int fd, int lock)
{
    struct flock fl;

    fl.l_type   = (lock == 1) ? F_WRLCK : F_UNLCK;
    fl.l_start  = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len    = 0;

    return (fcntl(fd, F_SETLK, &fl));
}

static int8_t deamonGetRunningState(const char* filename)
{
    int  fd;

    if (filename == NULL) {                                                    
        return -1;
    }

    fd = open(filename, O_RDWR, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));

    if (fd < 0) {
        return 0;
    }

    if (deamonTrytoLockfile(fd, 1) == -1) {
        close(fd);
        return 2;
    } else {
        deamonTrytoLockfile(fd, 0);
        close(fd);
        return 1;
    }
}

static int8_t deamonDetector(const char *procname)
{
    int8_t  result, state;
    char filename[100];

    result = 0;

    sprintf(filename, "/var/run/%s.pid", procname);

    state = deamonGetRunningState(filename);

    switch (state)
    {
        case 0:
            result = -1;
            break;

        case 1:
            result = start_proc_by_name(procname);
            break;

        case 2:
            result = 0;
            break;

        default:
            break;
    }

    return result;
}

int8_t deamonBootProcessor(const char* procname)
{
    pid_t pid, child_pid;
    char  filename[100];

    sprintf(filename, "%s%s", PROC_FILE_PATH, procname);

    child_pid = 0;

    if (access(filename, X_OK | F_OK) != 0) {
        return 0;
    }

    pid = fork();

    if (pid < 0) {
        return 0;
    } else if (pid == 0) {
        if (execl(filename, procname, (char *)NULL) != -1) {
            return 1;
        } else {
            return 0;
        }
    } else {
        child_pid = pid;
    }

    return (int)child_pid;
}

static void *thread_client_hdl(void *pdata)
{
    int result;

    pdata = pdata;

    sleep(10);

    for (;;) {
        printf("time to check thread_client...\n");

        result = deamonDetector("httpClent");
        if (result == -1) {
            printf("thread_client never exist...\n");
        } else if (result == 0) {
            printf("thread_client running ok...\n");
        } else {
            printf("thread_client has gone! but restarted...\n");
        }
        sleep(10);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    int       client_para;
    char     *p, *process_name;
    pthread_t thread_client;

    process_name = argv[0];

    p = process_name + strlen(process_name);

    while (*p != '/' && p != process_name) {
        p--;
    }

    if (*p == '/') {
        process_name = p + 1;
    }

    printf("\"%s\" starting...\n", process_name);

    client_para  = 0x01;

    if (pthread_create(&thread_client, NULL, thread_client_hdl, &client_para) != 0) {
        printf("create thread_client failed!\n");
        return 1;
    }

    if (start_proc_by_name(PROC_NAME_CLIENT) == 0) {
        printf("start thread_client failed!\n");
        return 1;
    }

    for (;;) {
        sleep(60);
        printf("i am still alive...\n");
    }

    return 0;
}


