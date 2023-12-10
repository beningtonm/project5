.PHONY: littleOldWoman dining_philosophers clean
#r=littleOldWoman
r=dining_philosophers1

run:

	@echo "|----------------------------------|\n|  Compiling $r...|\n|----------------------------------|\n\n"
	@make $r
	@echo "\nDone\n|----------------------------------|\n|----------------------------------|\n|  Running $r...  |\n|----------------------------------|\n|----------------------------------|\n\n"
	@./$r 5
	@echo "\n\nDone"


littleOldWoman:
	gcc -o littleOldWoman littleOldWoman.c -Wall -pthread 

dining_philosophers1:
	gcc -o dining_philosophers1 dining_philosophers1.c -Wall -pthread
	./dining_philosophers1 5

clean:
	@echo "Cleaning up..."
	rm -f littleOldWoman dining_philosophers err.txt


# This doesnt work unfortunately, ran out of time to fix