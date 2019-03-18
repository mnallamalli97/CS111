#!/bin/sh	

	
	#list test cases

	./lab2_list --threads=1 --iterations=100 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
	
	./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv

	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv


	./lab2_list --threads=1 --iterations=100 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv
	
	./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv

	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2b_list.csv


	./lab2_list --threads=1 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv

	./lab2_list --threads=16 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv



	./lab2_list --threads=1 --iterations=1 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=4 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=8 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=16 --yield=id --lists=4 --sync=m>> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=4 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=8 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=16 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=4 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=8 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=16 --yield=id --lists=4 --sync=m >> lab2b_list.csv

	./lab2_list --threads=16 --iterations=1 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=4 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=8 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=16 --yield=id --lists=4 --sync=m >> lab2b_list.csv


	./lab2_list --threads=1 --iterations=1 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=4 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=8 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=16 --yield=id --lists=4 --sync=s>> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=4 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=8 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=16 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=4 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=8 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=16 --yield=id --lists=4 --sync=s >> lab2b_list.csv

	./lab2_list --threads=16 --iterations=1 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=4 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=8 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=16 --yield=id --lists=4 --sync=s >> lab2b_list.csv


	./lab2_list --threads=1 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv

	./lab2_list --threads=4 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv


	./lab2_list --threads=1 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv

	./lab2_list --threads=4 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv


