#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include "chrono.c"
      

void myBCAST(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm){

    int size, rank, pos;

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    int source, target;

    int limite = log2(size);

    for (int i = 0; i < limite; i++)
    {
        MPI_Status status;
        pos = pow(2, i);
        target = (rank+pos);
        source = (rank-pos);

        if (rank < pos && target < size)
        {
          MPI_Send(buffer, count, datatype, target, 0, comm);
        }
        else if (rank >= pos && source >= root)
        {
          MPI_Recv(buffer, count, datatype, source, 0, comm, &status);
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

  if (rank == 0)
    for(int i=0;i<ni;i++) {
        ping[i] = i+1;
      }    
  
  MPI_Barrier(MPI_COMM_WORLD);

  chrono_reset(&mpiTime);
	chrono_start(&mpiTime);

  if (rank == 0){
    int nTrocas = (nMsg/nProcessos);

    for(int i=0;i < nTrocas;i++){
      myBCAST(ping, ni, MPI_LONG, 0, MPI_COMM_WORLD);
    }

  }

  MPI_Barrier( MPI_COMM_WORLD );

  chrono_stop(&mpiTime);
  
  //printf("Rank: %d, init : %ld, final: %ld \n", rank, ping[0], ping[ni]);

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
