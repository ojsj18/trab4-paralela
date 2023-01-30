#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "chrono.c"

void verificaVetores( long ping[], long pong[], int ni )
{
   static int twice = 0;
   int ping_ok = 1;
   int pong_ok = 1;
   int i, rank;
      
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );   
   
   if( twice == 0 ) {
  
      if (rank == 0) {
      
          for( i=0; i<ni; i++ ) {
            if( ping[i] != i+1 ) { ping_ok = 0; break; }
            if( pong[i] != 0   ) { pong_ok = 0; break; }
          }
          if( !ping_ok )
             fprintf(stderr, 
               "--------- rank 0, initial value of ping[%d] = %ld (wrong!)\n", i, ping[i] );
          if( !pong_ok )
             fprintf(stderr, 
               "--------- rank 0, initial value of pong[%d] = %ld (wrong!)\n", i, pong[i] );
          if( ping_ok && pong_ok )
             fprintf(stderr, 
               "--------- rank 0, initial value of ping and pong are OK\n" );

      } else if (rank == 1) {
      
          for( i=0; i<ni; i++ ) {
            if( ping[i] != 0      ) { ping_ok = 0; break; }
            if( pong[i] != i+ni+1 ) { pong_ok = 0; break; }
          }
          if( !ping_ok )
             fprintf(stderr, 
               "--------- rank 1, initial value of ping[%d] = %ld (wrong!)\n", i, ping[i] );
          if( !pong_ok )
             fprintf(stderr, 
               "--------- rank 1, initial value of pong[%d] = %ld (wrong!)\n", i, pong[i] );
          if( ping_ok && pong_ok )
             fprintf(stderr, 
               "--------- rank 1, initial values of ping and pong are OK\n" );
      }          
   }   // end twice == 0
   
   if( twice == 1 ) {
  
          for( i=0; i<ni; i++ ) {
            if( ping[i] != i+1      ) { ping_ok = 0; break; }
            if( pong[i] != i+ni+1   ) { pong_ok = 0; break; }
          }
          if( !ping_ok )
             fprintf(stderr, 
               "--------- rank %d, FINAL value of ping[%d] = %ld (wrong!)\n", rank, i, ping[i] );
          if( !pong_ok )
             fprintf(stderr, 
               "--------- rank %d, FINAL value of pong[%d] = %ld (wrong!)\n", rank, i, pong[i] );
          if( ping_ok && pong_ok )
             fprintf(stderr, 
               "--------- rank %d, FINAL values of ping and pong are OK\n", rank );

   }  // end twice == 1
   
   ++twice;
   if( twice > 2 )
      fprintf(stderr, 
               "--------- rank %d, verificaVetores CALLED more than 2 times!!!\n", rank );     
}          

int main(int argc, char* argv[]) {

  chronometer_t mpiTime;

  int size, rank;
  long elements;
  long nMsg;
  long tMsg;
  long ni;


  long msgS;
  long msgR;
  int bloqueante = 0;

  MPI_Status stat;

  MPI_Init(&argc, &argv) ;
  MPI_Comm_size (MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(size > 2){
    printf("só são permitidos 2 processos \n");
    exit(0);
  }

  if (argc < 3)
	{
		printf("usage: %s <nmsg> <tmsg> <Optional: bl|nbl>\n",argv[0]);
		exit(0);
	}
	else
	{
    
		nMsg = atoi(argv[1]);
    if (rank == 0) 
  	  printf("<nmsg>: %ld \n",nMsg);

    bloqueante = 1;
    if(argc == 4)
       bloqueante = strcmp(argv[3], "-nbl");

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
  long *pong = (long *) malloc( ni*sizeof(long) );

	MPI_Request reqs[2];
	MPI_Status status[2];

  if(size > 2){
    exit(0);
  }
  
  if (rank == 0)
      for(int i=0;i<ni;i++) {
        ping[i] = i+1;
        pong[i] = 0;
      }  

  else if (rank == 1)
      for(int i=0;i<ni;i++) {
            ping[i] = 0;
            pong[i] = i+ni+1;
      }           
  
  verificaVetores(ping, pong, ni );
  MPI_Barrier(MPI_COMM_WORLD);

  chrono_reset(&mpiTime);
	chrono_start(&mpiTime);

  if(bloqueante == 0){ // se == 0, nbloqueante
      if (rank == 0) 
        printf("NAO BLOQUEANTE\n");
      for(int i=0;i<nMsg/2;i++)
        if (rank == 0) {
          MPI_Isend(ping, ni, MPI_LONG, 1, 2, MPI_COMM_WORLD, &reqs[1]);
          MPI_Irecv(pong, ni, MPI_LONG, 1, 1, MPI_COMM_WORLD, &reqs[0]);

          MPI_Waitall(2, reqs, status);

        } else if (rank == 1) {
          MPI_Irecv(ping, ni, MPI_LONG, 0, 2, MPI_COMM_WORLD, &reqs[1]);
          MPI_Isend(pong, ni, MPI_LONG, 0, 1, MPI_COMM_WORLD, &reqs[0]);

          MPI_Waitall(2, reqs, status);
      }
  } else{     
    if (rank == 0) 
      printf("BLOQUEANTE\n");
    for(int i=0;i<nMsg/2;i++)
        if (rank == 0) {
          MPI_Send(ping, ni, MPI_LONG, 1, 1, MPI_COMM_WORLD);
          MPI_Recv(pong, ni, MPI_LONG, 1, 1, MPI_COMM_WORLD, &status[0]);

        } else if (rank == 1) {
          MPI_Send(pong, ni, MPI_LONG, 0, 1, MPI_COMM_WORLD);
          MPI_Recv(ping, ni, MPI_LONG, 0, 1, MPI_COMM_WORLD, &status[0]);
        }
  }

  MPI_Barrier( MPI_COMM_WORLD );

  chrono_stop(&mpiTime);

  for(int i=0; i<ni;i++)
    printf("%ld ", ping[i]);
  printf("\n");
  for(int i=0; i<ni;i++)
    printf("%ld ", pong[i]);
  
  verificaVetores(ping, pong, ni );

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
