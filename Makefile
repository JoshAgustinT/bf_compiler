hanoi:
	g++ -O3 bf.cpp -o jbf
	./jbf benches/hanoi.b
	@echo "linking.."
	gcc bf.s 
	./a.out
mandel:
	g++ -O3 bf.cpp -o jbf
	./jbf benches/mandel.b
	@echo "linking.."
	gcc bf.s 
	./a.out

input:
	g++ bf.cpp
	./a.out benches/input.b
	@echo "linking.."
	gcc bf.s 
	./a.out

simple:
	g++ bf.cpp
	./a.out benches/simple.b
	@echo "linking.."
	gcc bf.s 
	./a.out
test:
	g++ -O3 bf.cpp -o jbf
	./jbf benches/hello.b 
	gcc bf.s 
	./a.out
	./jbf benches/deadcodetest.b 
	gcc bf.s 
	./a.out
	./jbf benches/mandel.b 
	gcc bf.s 
	./a.out
	./jbf benches/bench.b 
	gcc bf.s 
	./a.out
	./jbf benches/loopremove.b 
	gcc bf.s 
	./a.out
	./jbf benches/twinkle.b 
	gcc bf.s 
	./a.out
	./jbf benches/simple.b 
	gcc bf.s 
	./a.out
	./jbf benches/hanoi.b 
	gcc bf.s 
	./a.out
	./jbf benches/bottles.b 
	gcc bf.s 
	./a.out
	./jbf benches/long.b 
	gcc bf.s
	@echo -------------starting long.b test, dont panic---------
	./a.out
	./jbf benches/serptri.b 
	gcc bf.s 
	./a.out
	@echo "All tests completed."
	@echo



compile:
	g++ -O3 bf.cpp -o jbf
