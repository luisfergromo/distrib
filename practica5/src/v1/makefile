EXECS=MPI
MPICC?=mpicc

all: ${EXECS}

MPI: MPI.cpp
	${MPICC} -o MPI MPI.cpp

clean:
	rm -f ${EXECS}
