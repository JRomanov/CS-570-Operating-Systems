#James Lee (RedID: 820655947)
#James Romanov (RedID: 820821671)

pagetable: pagetable.o pgFunctions.o byu_tracereader.o output_mode_helpers.o
	gcc -o pagetable pagetable.o pgFunctions.o byu_tracereader.o output_mode_helpers.o

pagetable.o: pagetable.c
	gcc -c pagetable.c

byu_tracereader.o: byu_tracereader.c
	gcc -c byu_tracereader.c

output_mode_helpers.o: output_mode_helpers.c
	gcc -c output_mode_helpers.c

clean:
	rm *.o
	rm pagetable