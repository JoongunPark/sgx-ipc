make clean
make SGX_MODE=HW

for i in {2..1000}
do
	./app
done
