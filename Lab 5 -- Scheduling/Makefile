scheduler: main.o Task.o SchedulingAlgorithmRMA.o SchedulingAlgorithmEDF.o SchedulingAlgorithmLLF.o Schedule.o Logger.o
	g++ -g main.o Task.o SchedulingAlgorithmRMA.o SchedulingAlgorithmEDF.o SchedulingAlgorithmLLF.o Schedule.o Logger.o -o scheduler.exe

main.o: main.cpp Task.h SchedulingAlgorithm.h SchedulingAlgorithmRMA.h SchedulingAlgorithmEDF.h SchedulingAlgorithmLLF.h Schedule.h Logger.h
	g++ -g -c main.cpp -o main.o

Task.o: Task.cpp Task.h Logger.h
	g++ -g -c Task.cpp -o Task.o

SchedulingAlgorithmLLF.o: SchedulingAlgorithmLLF.cpp SchedulingAlgorithmLLF.h Task.h SchedulingAlgorithm.h
	g++ -g -c SchedulingAlgorithmLLF.cpp -o SchedulingAlgorithmLLF.o

SchedulingAlgorithmEDF.o: SchedulingAlgorithmEDF.cpp SchedulingAlgorithmEDF.h Task.h SchedulingAlgorithm.h
	g++ -g -c SchedulingAlgorithmEDF.cpp -o SchedulingAlgorithmEDF.o

SchedulingAlgorithmRMA.o: SchedulingAlgorithmRMA.cpp SchedulingAlgorithmRMA.h Task.h SchedulingAlgorithm.h
	g++ -g -c SchedulingAlgorithmRMA.cpp -o SchedulingAlgorithmRMA.o

Schedule.o: Schedule.cpp Schedule.h Task.h SchedulingAlgorithm.h Logger.h
	g++ -g -c Schedule.cpp -o Schedule.o

Logger.o: Logger.cpp Logger.h Task.h Schedule.h SchedulingAlgorithm.h
	g++ -g -c Logger.cpp -o Logger.o