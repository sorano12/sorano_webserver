.PHONY:run clean
run:main
main:main.cpp	echo.h	processpoll.h
	g++	-o	main   	main.cpp
clean:
	rm	-f	*.o	main
