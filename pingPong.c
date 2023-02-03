#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include "chrono.c"

void verificaVetores( long ping[], int ni )
{
   static int twice = 0;
   int ping_ok = 1;
   int i, rank;
      
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );   
   
   if( twice == 0 ) {
  
      if (rank == 0) {
      
          for( i=0; i<ni; i++ ) {
            if( ping[i] != i+1 ) { ping_ok = 0; break; }
          }
          if( !ping_ok )
             fprintf(stderr, 
               "--------- rank 0, initial value of ping[%d] = %ld (wrong!)\n", i, ping[i] );
          if( ping_ok)
             fprintf(stderr, 
               "--------- rank 0, initial value of ping is OK\n" );

      } else if (rank == 1) {
      
          for( i=0; i<ni; i++ ) {
            if( ping[i] != 0      ) { ping_ok = 0; break; }
          }
          if( !ping_ok )
             fprintf(stderr, 
               "--------- rank 1, initial value of ping[%d] = %ld (wrong!)\n", i, ping[i] );
          if( ping_ok)
             fprintf(stderr, 
               "--------- rank 1, initial values of ping is OK\n" );
      }          
   }   // end twice == 0
   
   if( twice == 1 ) {
  
          for( i=0; i<ni; i++ ) {
            if( ping[i] != i+1      ) { ping_ok = 0; break; }
          }
          if( !ping_ok )
             fprintf(stderr, 
               "--------- rank %d, FINAL value of ping[%d] = %ld (wrong!)\n", rank, i, ping[i] );
          if( ping_ok)
             fprintf(stderr, 
               "--------- rank %d, FINAL values of ping is OK\n", rank );

   }  // end twice == 1
   
   ++twice;
   if( twice > 2 )
      fprintf(stderr, 
               "--------- rank %d, verificaVetores CALLED more than 2 times!!!\n", rank );     
}          

void myBCAST(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm){

  int size, rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &size);
  MPI_Status status[size];

  int pos, dest, source;

  int i = 0;
  while(i <= size){

    while(i<= (ceil(size/2))){
      pos = pow(2, i);
      dest = rank + pos;
      source = rank - pos;

      if(rank < pos){
        printf("Rank: %d, pos:%d , destino:%d\n", rank, pos, dest);
        MPI_Send(buffer, count, MPI_LONG, dest, 1, MPI_COMM_WORLD);
      }
      else
      {
        MPI_Recv(buffer, count, MPI_LONG, source, 1, MPI_COMM_WORLD, &status[0]);
      }
      
      i = i + pos;
    }

  }

}

int main(int argc, char* argv[]) {

  chronometer_t mpiTime;

  int size, rank;
  long elements;
  long nMsg;
  long tMsg;
  long ni;
  int nProcessos;


  long msgS;
  long msgR;
  int bloqueante = 0;

  MPI_Status stat;

  MPI_Init(&argc, &argv) ;
  MPI_Comm_size (MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (argc < 4)
	{
		printf("usage: %s <nmsg> <tmsg> <nProcess> <Optional: bl|nbl>\n",argv[0]);
		exit(0);
	}
	else
	{
    if (rank == 0){
      printf("<nmsg>: %ld \n",atoi(argv[1]));
  	  printf("<tMsg>: %ld \n",atoi(argv[2]));
      printf("<nProcess>: %d \n",atoi(argv[3]));
      printf("<<Optional: bl|nbl>: %s \n",argv[4]);

    }
		nMsg = atoi(argv[1]);
    nProcessos = atoi(argv[3]);

    bloqueante = 1;

    if(argc == 5)
      bloqueante = strcmp(argv[4], "-nbl");

		if (nMsg%2 != 0)
		{
      if (rank == 0) 
			  printf("<nmsg>: %ld (precisa ser um numero par!!)\n",nMsg);
			exit(0);
		}
    tMsg = atoi(argv[2]);
    if (tMsg%sizeof(long) != 0)
		{
      if (rank == 0) 
			  printf("<tmsg>: %ld (precisa ser um numero multiplo de 8!!)\n",tMsg);
			exit(0);
		}

    ni = tMsg/8;

	}

  long *ping = (long *) malloc( ni*sizeof(long) );

	MPI_Request reqs[2];
	MPI_Status status[2];

  if (rank == 0)
      for(int i=0;i<ni;i++) {
        ping[i] = i+1;
      }  

  else if (rank == 1)
      for(int i=0;i<ni;i++) {
            ping[i] = 0;
      }           
  
  verificaVetores(ping,ni );
  MPI_Barrier(MPI_COMM_WORLD);

  chrono_reset(&mpiTime);
	chrono_start(&mpiTime);

  if(bloqueante == 0){ // se == 0, nbloqueante

      if (rank == 0) 
        printf("NAO BLOQUEANTE\n");

      for(int i=0;i<nMsg/nProcessos;i++)
        myBCAST(ping, ni, MPI_LONG, 0, MPI_COMM_WORLD);

  } else{     
    if (rank == 0) 
      printf("BLOQUEANTE\n");

    for(int i=0;i<nMsg/nProcessos;i++)
        myBCAST(ping, ni, MPI_LONG, 0, MPI_COMM_WORLD);
  }

  MPI_Barrier( MPI_COMM_WORLD );

  chrono_stop(&mpiTime);
  
  verificaVetores(ping, ni );

	if(rank == 0){
		chrono_stop(&mpiTime);
		chrono_reportTime(&mpiTime, "mpiTime");

		// calcular e imprimir a VAZAO (numero de operacoes/s)
		double total_time_in_seconds = (double)chrono_gettotal(&mpiTime) /
									((double)1000 * 1000 * 1000);
	  double total_time_in_micro = (double)chrono_gettotal(&mpiTime) /
									((double)1000);
		printf("total_time_in_seconds: %lf s\n", total_time_in_seconds);
    printf("total_time_in_micro: %lf s\n", total_time_in_micro);
		printf("Latencia: %lf us/nmsg\n", (total_time_in_micro / nMsg)/2);
		double MBPS = ((double)(nMsg*tMsg) / ((double)total_time_in_seconds*1000*1000));
		printf("Throughput: %lf MB/s\n", MBPS);
	}

  MPI_Finalize();
}
