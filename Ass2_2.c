#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<signal.h>


#define BufferSize 10

typedef struct
	{
  int add;
  int take;
  int buffer[BufferSize];
	}product;


product proBuff = {0,0,{0}};
unsigned int proId = 1;

pthread_mutex_t mutex;
sem_t full;
sem_t empty;


void *Gen(void *id)
{
  while(1)
    {
      //wait till there is an empty place in buffer for product
      //sem_wait(&empty);
      //wait for the critical area access
      pthread_mutex_lock(&mutex);
      //check if queue is full
      if(!(((proBuff.add + 1) % BufferSize ) == proBuff.take))
	{
	  //Add the product in buffer
	  proBuff.buffer[proBuff.add] = proId++;
	  printf("Product# %d  Produced by Producer# 1 \n",proBuff.buffer[proBuff.add],(long*)id);
	  //Increment the index for add
	  proBuff.add = (proBuff.add + 1) % BufferSize;
	}
      else
	{
	  printf("no place to enter \n",(long*)id);
	}
      //release the critical area access
      pthread_mutex_unlock(&mutex);
      //Post the produced item
      sem_post(&full);
      sleep(1);
    }
}


void *Take(void *id)
{
  while(1) 
    {

      sem_wait(&full);
      //wait for the critical area access
      pthread_mutex_lock(&mutex);
      if(!(proBuff.add == proBuff.take))
	{
	  printf("Product# %d taken  by Consumer# %d \n",proBuff.buffer[proBuff.take],(long*)id);
	  //take the product
	  proBuff.buffer[proBuff.take] = 0;
	  //Increment the index for take
	  proBuff.take = (proBuff.take + 1) % BufferSize ;
	}
      else
	{
	  printf("Nothing is avaiable when accessed by Consumer# %d \n",(long*)id);
	}
      //release the critical area access
      pthread_mutex_unlock(&mutex);
      //Post the empty place
      sem_post(&empty);
      sleep(1);
    }
}


int main(int argc, char *argv[])
{
  unsigned int buyers = 6;
  unsigned int providers = 1;
  pthread_t pro[providers];
  pthread_t* bu;

  //Init Threads
  int t1 = pthread_mutex_init(&mutex, NULL);
  int t2 = sem_init(&full, 0, 0);
  //int t3 = sem_init(&empty, 0, BufferSize);

  //Notify if failed
  if(t1!=0||t2!=0)
    printf("Intialization Error");

  bu = malloc(buyers*sizeof(pthread_t));

  long i;
  //Create threads for Producer
  for(i=0;i<providers;i++)
    pthread_create(&pro[i], NULL, Gen, (void*) i);

  //Create threads for buyer
  for(i=0;i<buyers;i++)
    pthread_create(&bu[i], NULL, Take, (void*) i);

  //Wait for Gen and Take threads to finish
  for(i=0;i<providers;i++)
    pthread_join(pro[i], NULL);
  for(i=0;i<buyers;i++)
    pthread_join(bu[i], NULL);

  //Destroy mutex and semaphores
  pthread_mutex_destroy(&mutex);
  sem_destroy(&full);
  //sem_destroy(&empty);
  return 0;
}
