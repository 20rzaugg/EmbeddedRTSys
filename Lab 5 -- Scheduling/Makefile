scheduler: main.o Task.o SchedulingAlgorithmRMA.o Schedule.o
	g++ -g main.o Task.o SchedulingAlgorithmRMA.o Schedule.o -o scheduler.exe

main.o: main.cpp Task.h SchedulingAlgorithm.h SchedulingAlgorithmRMA.h Schedule.h
	g++ -g -c main.cpp -o main.o

Task.o: Task.cpp Task.h
	g++ -g -c Task.cpp -o Task.o

SchedulingAlgorithmRMA.o: SchedulingAlgorithmRMA.cpp SchedulingAlgorithmRMA.h Task.h SchedulingAlgorithm.h
	g++ -g -c SchedulingAlgorithmRMA.cpp -o SchedulingAlgorithmRMA.o

Schedule.o: Schedule.cpp Schedule.h Task.h SchedulingAlgorithm.h
	g++ -g -c Schedule.cpp -o Schedule.o