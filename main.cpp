
#include "monitor.h"
#include <stdio.h>

/* wielkość buforów */
#define MAX 5


class MyMonitor : Monitor{

private:
	int buf[MAX];
	int count;
  int epoka;

	Condition full, empty;

public:
  /*Konstruktor klasy Monitor*/
	MyMonitor();
  /*włożenie elementu o zadanej wartości do bufora*/
	void MyEnter(int value, int bufor, char nazwa);
  /*wyjęcie elementu z bufora w sposób zgodny z trwającą epoką*/
	void MyRemove(int bufor, char nazwa);

	void WypiszBufor();
  /*metody sortowania bąbelkowego - by elementy w buforze były poza kolejnościa FIFO
   ustawione priorytetami */
	void swap(int *xp, int *yp);
	void bubbleSort(int arr[], int n);

};

/******************************************************************************/

MyMonitor::MyMonitor(){
	count = 0;
	for(int i = 0; i<MAX; i++){
		buf[i]=0;
	}
  epoka=1;
}

/******************************************************************************/

void MyMonitor::WypiszBufor(){
	printf("Bufor: ");
	for(int i = 0; i<MAX; i++){
		printf(" %d ",buf[i]);
	}
}

/******************************************************************************/


void MyMonitor::MyEnter(int value, int bufor,char nazwa){

	enter();


	if(count == MAX){
		wait(full);
	}

	int i;
	/* włożenie elementu na pierwsze wolne miejsce */
	for( i = 0; i<MAX; i++){
		if(buf[i]==0){
			buf[i] = value;
			break;
		}
	}
	printf("Producent %c : dodany element w buforze: %d ! na miejscu %d wartość: %d\n",nazwa,bufor,i,value);
	WypiszBufor();
  printf("\n\n");

	count++;

	if(count == 1) signal(empty);
	leave();
}


/******************************************************************************/

void MyMonitor::MyRemove(int bufor, char nazwa){

	enter();

	int value=0;

	if(count==0){
		wait(empty);
	}

	int i=0;

	bubbleSort(this->buf, MAX);

  /*pobieranie pierwszego elementu*/
	value=buf[i];
	buf[i]=0;
  count--;

/*działanie programu, gdy jest epoka podstawowa*/
  if(epoka==1){
    printf("Epoka 1: pobrany 1 element\nKonsument %c : pobrany element z bufora: %d ! Na miejscu %d wartość: %d\n",nazwa,bufor,i,value);
	   WypiszBufor();
	   printf("\n\n");

	   bubbleSort(this->buf, MAX);
  }
  
  /*działanie programu, gdy jest epoka specjalna - konsument pobiera wszystkie pozostałe elementy bufora*/
  else{
    printf("Epoka 2 : pobrane wszystkie elementy znajdujące się w buforze\nKonsument %c : pobrane elementy z bufora : %d\n %d",nazwa,bufor,value);
    for(i=1;  i<MAX && buf[i]!=0; i++){
      printf(" %d ",buf[i]);
      buf[i]=0;
      count --;
    }
    printf(" z miejsc: od 0 do %d\n",i-1);
    WypiszBufor();
    printf("\n\n");
  } 
 
  epoka=value;
  
	if(count==MAX-1) {
		signal(full);
	}

	leave();

}

/******************************************************************************/

void MyMonitor::swap(int *xp, int *yp){ 
    int temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
} 
  
/******************************************************************************/
/*sortowanie bąbelkowe */
void MyMonitor::bubbleSort(int arr[], int n){ 
   int i, j; 
   for (i = 0; i < n; i++)          
       for (j = 0; j < n-i-1; j++)  
           if (arr[j] < arr[j+1]) 
              swap(&arr[j], &arr[j+1]); 
} 


/******************************************************************************/
/******************************************************************************/

/* deklaracja trzech monitorów */
MyMonitor first;
MyMonitor second;
MyMonitor third;


/******************************************************************************/

/*funkcja inicjująca działanie producenta*/

void * producer (void * args){
	char n=*((char *)args);

	while(true){
		int value;

		 /*losowanie wartości*/
    	int i, t;
    	time_t tt;
    	t = time(&tt);
    	srand(t);         // za zarodek wstawiamy pobrany czas w sekundach

    	i=rand()%3;       // przedział losowania: <0,2>
    	if(i==0)          //zostanie wstawiony element o wyższym priorytecie, gdy zostanie wylosowane 0
    	  value = 2;
    	else value = 1;

      i=(int)n;
      i=i%3;

    	if(i==0 && value==1) {         
    	   first.MyEnter(value,i+1,n);
    	 }
    	if (i==1 && value ==1) {
    		second.MyEnter(value,i+1,n);
    	}
    	if(i==2 && value ==1) {
    		third.MyEnter(value,i+1,n);
    	}

      /*włożenie elementu do wszytkich trzech buforów - element priorytetowy*/
      if(value==2){

          first.MyEnter(value,1,n);
          second.MyEnter(value,2,n);
          third.MyEnter(value,3,n);
      }
    	sleep(rand()%5+3);

	}
}

/******************************************************************************/

/*funkcja inicjująca działanie konsumenta*/
void * consumer (void *args){

	char n=*((char *)args);
	while(true){
		int value;

    	int i;

      i=(int)n;
      i=i%3;

    	if(i==0) {

    	  first.MyRemove(i+1,n);
    	}
    	if (i==1) {
    	 second.MyRemove(i+1,n);
    	}
    	if(i==2) {
    	  third.MyRemove(i+1,n);
    	}
    	sleep(rand()%5+3);
	}


}

/******************************************************************************/


int main() {
  pthread_t prodA, consA, prodB,consB, prodC,consC,prodD,consD,prodE,consE,prodF,consF,prodG,consG;
  char a='A';
  char b='B';  
  char c='C';
  char d='D';
  char e='E';
  char f='F';
  char g='G';

  pthread_create(&prodA,NULL,producer, &a);
  pthread_create(&consA,NULL,consumer, &a);
  pthread_create(&prodB,NULL,producer, &b);
  pthread_create(&consB,NULL,consumer, &b);  
  pthread_create(&prodC,NULL,producer, &c);
  pthread_create(&consC,NULL,consumer, &c);
  pthread_create(&prodD,NULL,producer, &d);
  pthread_create(&consD,NULL,producer, &d);
  pthread_create(&prodE,NULL,producer, &e);
  pthread_create(&consE,NULL,producer, &e);
  pthread_create(&prodF,NULL,producer, &f);
  pthread_create(&consF,NULL,producer, &f);
  pthread_create(&prodG,NULL,producer, &g);
  pthread_create(&consG,NULL,producer, &g); 



  pthread_join(prodA,NULL);
  pthread_join(consA,NULL);
  pthread_join(prodB,NULL);  
  pthread_join(consB,NULL);
  pthread_join(prodC,NULL);
  pthread_join(consC,NULL);
  pthread_join(prodD,NULL);
  pthread_join(consD,NULL);
  pthread_join(prodE,NULL);
  pthread_join(consE,NULL);
  pthread_join(prodF,NULL);
  pthread_join(consF,NULL);
  pthread_join(prodG,NULL);
  pthread_join(consG,NULL);  


  return 0;

}
