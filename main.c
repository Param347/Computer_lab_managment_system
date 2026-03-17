#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX 5

// ---------------- GLOBAL VARIABLES ----------------
int available_systems = MAX;
int print_queue[100], front = -1, rear = -1;
int file_in_use = 0;

pthread_mutex_t lock;

// ---------------- LOGIN SYSTEM ----------------
int login()
{
    char username[20], password[20];
    char u[20], p[20];
    FILE *fp;
    int found = 0;

    printf("\nEnter Username: ");
    scanf("%s", username);

    printf("Enter Password: ");
    scanf("%s", password);

    fp = fopen("users.txt", "r");

    if(fp == NULL)
    {
        printf("User file not found!\n");
        return 0;
    }

    while(fscanf(fp,"%s %s",u,p)!=EOF)
    {
        if(strcmp(username,u)==0 && strcmp(password,p)==0)
        {
            found = 1;
            break;
        }
    }

    fclose(fp);

    if(found)
    {
        printf("Login Successful\n");
        return 1;
    }
    else
    {
        printf("Invalid Login\n");
        return 0;
    }
}

// ---------------- SYSTEM RESOURCE ----------------
void request_system()
{
    pthread_mutex_lock(&lock);

    if(available_systems > 0)
    {
        available_systems--;
        printf("System Allocated. Available: %d\n", available_systems);
    }
    else
    {
        printf("All systems busy. User waiting...\n");
    }

    pthread_mutex_unlock(&lock);
}

void release_system()
{
    pthread_mutex_lock(&lock);

    if(available_systems < MAX)
    {
        available_systems++;
        printf("System Released. Available: %d\n", available_systems);
    }

    pthread_mutex_unlock(&lock);
}

// ---------------- PRINT QUEUE ----------------
void add_print_job(int id)
{
    pthread_mutex_lock(&lock);

    if(rear == 99)
    {
        printf("Print Queue Full\n");
    }
    else
    {
        if(front == -1)
            front = 0;

        print_queue[++rear] = id;

        printf("User %d added print job\n", id);
    }

    pthread_mutex_unlock(&lock);
}

void process_print_job()
{
    pthread_mutex_lock(&lock);

    if(front == -1 || front > rear)
    {
        printf("No print jobs\n");
    }
    else
    {
        printf("Processing print job of User %d\n", print_queue[front]);
        front++;
    }

    pthread_mutex_unlock(&lock);
}

// ---------------- SHOW PRINT QUEUE ----------------
void show_print_queue()
{
    pthread_mutex_lock(&lock);

    if(front == -1 || front > rear)
    {
        printf("Print Queue Empty\n");
    }
    else
    {
        printf("Print Queue: ");

        for(int i = front; i <= rear; i++)
        {
            printf("User%d ", print_queue[i]);
        }

        printf("\n");
    }

    pthread_mutex_unlock(&lock);
}

// ---------------- SHARED FILE ----------------
void access_file()
{
    pthread_mutex_lock(&lock);

    if(file_in_use == 0)
    {
        file_in_use = 1;
        printf("File accessed\n");
    }
    else
    {
        printf("File busy\n");
    }

    pthread_mutex_unlock(&lock);
}

void release_file()
{
    pthread_mutex_lock(&lock);

    file_in_use = 0;

    printf("File released\n");

    pthread_mutex_unlock(&lock);
}

// ---------------- DEADLOCK CHECK ----------------
void check_deadlock()
{
    if(available_systems == 0 && file_in_use == 1 && front <= rear)
    {
        printf("Deadlock Risk Detected\n");
    }
    else
    {
        printf("No Deadlock\n");
    }
}

// ---------------- MULTI USER SIMULATION ----------------
void* user_sim(void* arg)
{
    int id = *(int*)arg;

    printf("\n[User %d started]\n", id);

    printf("User %d requesting system...\n", id);

    request_system();

    sleep(1);

    add_print_job(id);

    sleep(1);

    access_file();

    sleep(1);

    release_file();

    process_print_job();

    release_system();

    printf("[User %d finished]\n", id);

    return NULL;
}

void run_multi_user()
{
    pthread_t t[3];
    int ids[3];

    for(int i = 0; i < 3; i++)
    {
        ids[i] = i + 1;

        pthread_create(&t[i], NULL, user_sim, &ids[i]);
    }

    for(int i = 0; i < 3; i++)
    {
        pthread_join(t[i], NULL);
    }
}

// ---------------- MAIN FUNCTION ----------------
int main()
{
    int choice, job_id = 1;

    pthread_mutex_init(&lock, NULL);

    while(!login());

    while(1)
    {
        printf("\n====== MENU ======\n");
        printf("1. Request Computer\n");
        printf("2. Release Computer\n");
        printf("3. Add Print Job\n");
        printf("4. Process Print Job\n");
        printf("5. Access Shared File\n");
        printf("6. Release File\n");
        printf("7. Check Deadlock\n");
        printf("8. Run Multi-User Simulation\n");
        printf("9. Show Print Queue\n");
        printf("10. Exit\n");

        printf("Enter choice: ");
        scanf("%d",&choice);

        switch(choice)
        {
            case 1:
                request_system();
                break;

            case 2:
                release_system();
                break;

            case 3:
                add_print_job(job_id++);
                break;

            case 4:
                process_print_job();
                break;

            case 5:
                access_file();
                break;

            case 6:
                release_file();
                break;

            case 7:
                check_deadlock();
                break;

            case 8:
                run_multi_user();
                break;

            case 9:
                show_print_queue();
                break;

            case 10:
                return 0;

            default:
                printf("Invalid choice\n");
        }
    }
}
