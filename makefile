objs: global lipmodel lowpass lowpass trombone tube

global: ./src/global.cpp
	g++ ./src/global.cpp -c -o ./out/obj/global.o

lipmodel: ./src/lipmodel.cpp
	g++ ./src/lipmodel.cpp -c -o ./out/obj/lipmodel.o

lowpass: ./src/lowpass.cpp
	g++ ./src/lowpass.cpp -c -o ./out/obj/lowpass.o

trombone: ./src/trombone.cpp
	g++ ./src/trombone.cpp -c -o ./out/obj/trombone.o

tube: ./src/tube.cpp
	g++ ./src/tube.cpp -c -o ./out/obj/tube.o

clean:
	rm -r -f ./out/