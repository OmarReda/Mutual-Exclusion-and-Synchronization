#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <limits.h>
#define MAX 10

sem_t mutex;
sem_t counter;
sem_t ReadBuffer;
sem_t FullBuffer;
sem_t EmptyBuffer;

int Count = 0;

int queue[MAX];
int size  = 0;
int rear  = MAX - 1;
int front = 0;

int enqueue(int data)
{
    if (isFull())
    {
        return 0;
    }
    rear = (rear + 1) % MAX;
    size++;
    queue[rear] = data;
    return 1;
}

int dequeue()
{
    int data = INT_MIN;
    if (isEmpty())
    {
        return INT_MIN;
    }
    data = queue[front];
    front = (front + 1) % MAX;
    size--;

    return data;
}

int isFull()
{
    return (size == MAX);
}

int isEmpty()
{
    return (size == 0);
}

int getFront()
{
    return (isEmpty())
            ? INT_MIN
            : queue[front];
}

int getRear()
{
    return (isEmpty())
            ? INT_MIN
            : queue[rear];
}

//void* thread(void* arg)
//{
//    //wait
//    sem_wait(&mutex);
//    printf("\nAna d5lt ya Basha..\n");
//
//    //critical section
//    sleep(4);
//
//    //signal
//    printf("\nAna tl3t ya Basha...\n");
//    sem_post(&mutex);
//}

void* CounterThreadFunction(int ID)
{
    while(1)
    {
        sleep(rand()%5);
        printf("\nCounter Thread %d: Recieved a Message..\n",ID);
        printf("\nCounter Thread %d: waiting to Write..\n",ID);

        sem_wait(&counter);

        Count++;
        printf("\nCounter Thread %d: Adding to counter, Counter value is %d..\n",ID,Count);

        sem_post(&counter);

    }
}

void* CollectorThreadFunction()
{
    while(1)
    {
        sleep(rand()%5);

        if(isEmpty()){
            printf("\nCollector Thread: Buffer is Empty..\n");
        }

        sem_wait(&EmptyBuffer);
        sem_wait(&ReadBuffer);

        printf("\nCollector Thread: Reading from buffer in position %d..\n",front);
        dequeue();

        sem_post(&ReadBuffer);
        sem_post(&FullBuffer);
    }
}

void* MonitorThreadFunction()
{
    while(1)
    {
        sleep(rand()%5);
        printf("\nMonitor Thread: Waiting to Read ..\n");

        sem_wait(&counter);

        printf("\nMonitor Thread: Read Counter of Value %d..\n",Count);

        sem_post(&counter);

        if(isFull()){
            printf("\nMonitor Thread: Buffer is Full..\n");
        }

        sem_wait(&FullBuffer);
        sem_wait(&ReadBuffer);

        enqueue(Count);
        printf("\nMonitor Thread: Writing to Buffer Position %d..\n",rear);
        Count = 0;

        sem_post(&ReadBuffer);
        sem_post(&EmptyBuffer);
    }
}

int main()
{
    int Num = 5, i = 0;

    sem_init(&mutex, 0, 1);
    sem_init(&counter, 0, 1);
    sem_init(&ReadBuffer, 0, 1);
    sem_init(&EmptyBuffer, 0, 0);
    sem_init(&FullBuffer, 0, 10);

    pthread_t CounterThreads[Num];

    for (i=0; i<Num; i++)
    {
        int * ThreadID = (int*) malloc(sizeof(int));
        ThreadID = i;
        pthread_create(&CounterThreads[i], NULL, CounterThreadFunction, ThreadID);
    }

    pthread_t MonitorThread;
    pthread_t CollectorThread;

    pthread_create(&MonitorThread, NULL, MonitorThreadFunction, NULL);
    pthread_create(&CollectorThread, NULL, CollectorThreadFunction, NULL);


    for (i=0; i<Num; i++)
    {
        pthread_join(CounterThreads[i], NULL);
    }

    pthread_join(MonitorThread,NULL);
    pthread_join(CollectorThread,NULL);

    sem_destroy(&counter);
    sem_destroy(&ReadBuffer);
    sem_destroy(&FullBuffer);
    sem_destroy(&EmptyBuffer);

//    pthread_t t1,t2;
//    pthread_create(&t1,NULL,thread,NULL);
//    sleep(2);
//    pthread_create(&t2,NULL,thread,NULL);
//    pthread_join(t1,NULL);
//    pthread_join(t2,NULL);
//    sem_destroy(&mutex);

    return 0;
}
