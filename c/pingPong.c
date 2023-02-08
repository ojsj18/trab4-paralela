#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include "chrono.c"
      
#include <assert.h>

#define USE_MPI_Bcast 1  // do NOT change
#define USE_my_Bcast 2   // do NOT change
//choose either BCAST_TYPE in the defines bellow
//#define BCAST_TYPE USE_MPI_Bcast
#define BCAST_TYPE USE_my_Bcast

const int SEED = 100;

void my_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm){

    int size, rank, pos;

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    int source, target;

    int limite = ceil(log2(size));
    int rank_r = (rank - root + size) % size;

    for (int i = 0; i < limite; i++)
    {
        MPI_Status status;
        pos = pow(2, i);

        target = (rank+pos) % size;
        source = (root + rank_r-pos) %size;

        if (target < size && rank < pos)
        {
          MPI_Send(buffer, count, datatype, target, 0, comm);
        }
        if(i >= floor(log2(rank)) && rank >= pos)
        {
          MPI_Recv(buffer, count, datatype, source, 0, comm, &status);
        }
    } 
}

void verifica_my_Bcast( void *buffer, int count, MPI_Datatype datatype,
                        int root, MPI_Comm comm )
{
    int comm_size;
    int my_rank;
    
    MPI_Comm_size( comm, &comm_size );
    MPI_Comm_rank( comm, &my_rank );
    static long *buff = (long *) calloc( count*comm_size, sizeof(long) );
    
    
    // preenche a faixa do raiz com alguma coisa (apenas no raiz)
    if( my_rank == root )
       for( int i=0; i<count; i++ )
          buff[ i ] = i+SEED;
    
    #if BCAST_TYPE == USE_MPI_Bcast
       MPI_Bcast( buff, count, datatype, root, comm );
    #elif BCAST_TYPE == USE_my_Bcast
       my_Bcast( buff, count, datatype, root, comm );
    #else
       assert( BCAST_TYPE == USE_MPI_Bcast || BCAST_TYPE == USE_my_Bcast );
    #endif   
	   
    
    // cada nodo verifica se sua faixa recebeu adequadamente o conteudo
    int ok=1;
    int i;
    for( i=0; i<count; i++ )
       if( buff[ i ] != i+SEED ) {
          ok = 0;
          break;
       }
    // imprime na tela se OK ou nao
    if( ok )
        fprintf( stderr, "MY BCAST VERIF: node %d received ok\n", my_rank );
    else
        fprintf( stderr, "MY BCAST VERIF: node %d NOT ok! local position: %d contains %ld\n",
                           my_rank, i, buff[i] );

   free(buff);      
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
  
  if (rank != 0)
    for(int i=0;i<ni;i++) {
        ping[i] = 0;
      }  

  MPI_Barrier(MPI_COMM_WORLD);

  chrono_reset(&mpiTime);
	chrono_start(&mpiTime);


    for(int i=0;i < nMsg;i++){
      my_Bcast(ping, ni, MPI_LONG, 0, MPI_COMM_WORLD);
    }


  MPI_Barrier( MPI_COMM_WORLD );

  chrono_stop(&mpiTime);

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
		printf("Latencia: %lf us p/ broadcast\n", (total_time_in_micro / nMsg));
		double MBPS = ((double)(nMsg*tMsg) / ((double)total_time_in_seconds*1000*1000));
		printf("Throughput: %lf MB/s\n", MBPS);
	}

  printf("Rank: %d, init : %ld, final: %ld \n", rank, ping[0], ping[ni-1]);
  verifica_my_Bcast( ping, ni, MPI_LONG, 0, MPI_COMM_WORLD );
  MPI_Finalize();
}
