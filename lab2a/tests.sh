#!/bin/sh	

	./lab2_add --threads=1 --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=1 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=1 --iterations=100000 >> lab2_add.csv

	./lab2_add --threads=2 --iterations=100 >> lab2_add.csv
	./lab2_add --threads=2 --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=2 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=2 --iterations=100000 >> lab2_add.csv


	./lab2_add --threads=4 --iterations=100 >> lab2_add.csv
	./lab2_add --threads=4 --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=4 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=4 --iterations=100000 >> lab2_add.csv


	./lab2_add --threads=1 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=1 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=1 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=1 --iterations=100000 --yield >> lab2_add.csv

	./lab2_add --threads=2 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=100000 --yield >> lab2_add.csv

	./lab2_add --threads=4 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=100000 --yield >> lab2_add.csv


	./lab2_add --threads=5 --iterations=100 >> lab2_add.csv
	./lab2_add --threads=5 --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=5 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=5 --iterations=100000 >> lab2_add.csv

	./lab2_add --threads=8 --iterations=100 >> lab2_add.csv
	./lab2_add --threads=8 --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=8 --iterations=100000 >> lab2_add.csv

	
	./lab2_add --threads=10 --iterations=100 >> lab2_add.csv
	./lab2_add --threads=10 --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=10 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=10 --iterations=100000 >> lab2_add.csv

	
	./lab2_add --threads=13 --iterations=100 >> lab2_add.csv
	./lab2_add --threads=13 --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=13 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=13 --iterations=100000 >> lab2_add.csv

	./lab2_add --threads=5 --iterations=30 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=30 --yield >> lab2_add.csv
	./lab2_add --threads=10 --iterations=30 --yield >> lab2_add.csv
	./lab2_add --threads=13 --iterations=30 --yield >> lab2_add.csv



	./lab2_add --threads=5 --iterations=50 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=50 --yield >> lab2_add.csv
	./lab2_add --threads=10 --iterations=50 --yield >> lab2_add.csv
	./lab2_add --threads=13 --iterations=50 --yield >> lab2_add.csv


	./lab2_add --threads=5 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=10 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=13 --iterations=100 --yield >> lab2_add.csv


	./lab2_add --threads=5 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=10 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=13 --iterations=1000 --yield >> lab2_add.csv



	./lab2_add --threads=5 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=10 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=13 --iterations=10000 --yield >> lab2_add.csv


	./lab2_add --threads=5 --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=10 --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=13 --iterations=100000 --yield >> lab2_add.csv


	./lab2_add --threads=5 --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=10 --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=13 --iterations=10000 --sync=m >> lab2_add.csv

	./lab2_add --threads=5 --iterations=10000  --sync=c >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000  --sync=c >> lab2_add.csv
	./lab2_add --threads=10 --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=13 --iterations=10000 --sync=c >> lab2_add.csv


	./lab2_add --threads=5 --iterations=10000   --sync=s >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=10 --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=13 --iterations=10000 --sync=s >> lab2_add.csv




	./lab2_add --threads=5 --iterations=10000 --yield  --sync=m >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 --yield --sync=m >> lab2_add.csv
	./lab2_add --threads=10 --iterations=10000 --yield --sync=m >> lab2_add.csv
	./lab2_add --threads=13 --iterations=10000 --yield --sync=m >> lab2_add.csv

	./lab2_add --threads=5 --iterations=10000 --yield  --sync=c >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 --yield --sync=c >> lab2_add.csv
	./lab2_add --threads=10 --iterations=10000 --yield --sync=c >> lab2_add.csv
	./lab2_add --threads=13 --iterations=10000 --yield --sync=c >> lab2_add.csv


	./lab2_add --threads=5 --iterations=10000 --yield  --sync=s >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 --yield --sync=s >> lab2_add.csv
	./lab2_add --threads=10 --iterations=10000 --yield --sync=s >> lab2_add.csv
	./lab2_add --threads=13 --iterations=10000 --yield --sync=s >> lab2_add.csv






	#list test cases

	./lab2_list --threads=1 --iterations=100 >> lab2_list.csv
	./lab2_list --threads=1 --iterations=1000 >> lab2_list.csv
	./lab2_list --threads=1 --iterations=10000 >> lab2_list.csv
	./lab2_list --threads=1 --iterations=100000 >> lab2_list.csv

	./lab2_list --threads=2 --iterations=100 >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 >> lab2_list.csv
	./lab2_list --threads=2 --iterations=10000 >> lab2_list.csv
	./lab2_list --threads=2 --iterations=100000 >> lab2_list.csv


	./lab2_list --threads=4 --iterations=100 >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 >> lab2_list.csv
	./lab2_list --threads=4 --iterations=10000 >> lab2_list.csv
	./lab2_list --threads=4 --iterations=100000 >> lab2_list.csv


	./lab2_list --threads=1 --iterations=100 --yield=i >> lab2_list.csv
	./lab2_list --threads=1 --iterations=1000 --yield=i >> lab2_list.csv
	./lab2_list --threads=1 --iterations=10000 --yield=i >> lab2_list.csv
	./lab2_list --threads=1 --iterations=100000 --yield=i >> lab2_list.csv

	./lab2_list --threads=2 --iterations=100 --yield=i >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --yield=i >> lab2_list.csv
	./lab2_list --threads=2 --iterations=10000 --yield=i >> lab2_list.csv
	./lab2_list --threads=2 --iterations=100000 --yield=i >> lab2_list.csv

	./lab2_list --threads=4 --iterations=100 --yield=i >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --yield=i >> lab2_list.csv
	./lab2_list --threads=4 --iterations=10000 --yield=i >> lab2_list.csv
	./lab2_list --threads=4 --iterations=100000 --yield=i >> lab2_list.csv


	./lab2_list --threads=1 --iterations=100 --yield=d >> lab2_list.csv
	./lab2_list --threads=1 --iterations=1000 --yield=d >> lab2_list.csv
	./lab2_list --threads=1 --iterations=10000 --yield=d >> lab2_list.csv
	./lab2_list --threads=1 --iterations=100000 --yield=d >> lab2_list.csv

	./lab2_list --threads=2 --iterations=100 --yield=d >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --yield=d >> lab2_list.csv
	./lab2_list --threads=2 --iterations=10000 --yield=d >> lab2_list.csv
	./lab2_list --threads=2 --iterations=100000 --yield=d >> lab2_list.csv

	./lab2_list --threads=4 --iterations=100 --yield=d >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --yield=d >> lab2_list.csv
	./lab2_list --threads=4 --iterations=10000 --yield=d >> lab2_list.csv
	./lab2_list --threads=4 --iterations=100000 --yield=d >> lab2_list.csv


	./lab2_list --threads=1 --iterations=100 --yield=il >> lab2_list.csv
	./lab2_list --threads=1 --iterations=1000 --yield=il >> lab2_list.csv
	./lab2_list --threads=1 --iterations=10000 --yield=il >> lab2_list.csv
	./lab2_list --threads=1 --iterations=100000 --yield=il >> lab2_list.csv

	./lab2_list --threads=2 --iterations=100 --yield=il >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --yield=il >> lab2_list.csv
	./lab2_list --threads=2 --iterations=10000 --yield=il >> lab2_list.csv
	./lab2_list --threads=2 --iterations=100000 --yield=il >> lab2_list.csv

	./lab2_list --threads=4 --iterations=100 --yield=il >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --yield=il >> lab2_list.csv
	./lab2_list --threads=4 --iterations=10000 --yield=il >> lab2_list.csv
	./lab2_list --threads=4 --iterations=100000 --yield=il >> lab2_list.csv



	./lab2_list --threads=5 --iterations=10000 --sync=m >> lab2_list.csv
	./lab2_list --threads=8 --iterations=10000 --sync=m >> lab2_list.csv
	./lab2_list --threads=10 --iterations=10000 --sync=m >> lab2_list.csv
	./lab2_list --threads=13 --iterations=10000 --sync=m >> lab2_list.csv


	./lab2_list --threads=5 --iterations=10000   --sync=s >> lab2_list.csv
	./lab2_list --threads=8 --iterations=10000 --sync=s >> lab2_list.csv
	./lab2_list --threads=10 --iterations=10000 --sync=s >> lab2_list.csv
	./lab2_list --threads=13 --iterations=10000 --sync=s >> lab2_list.csv


	./lab2_list --threads=12 --iterations=1000 --yield=i >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --yield=d >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --yield=il >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --yield=dl >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --yield=i --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --yield=d --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --yield=dl --sync=m >> lab2_list.csv


	./lab2_list --threads=12 --iterations=1000 --yield=i --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --yield=d --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --yield=dl --sync=s >> lab2_list.csv


	./lab2_list --threads=12 --iterations=1000  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000  --sync=s >> lab2_list.csv



	./lab2_list --threads=5 --iterations=10000 --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=8 --iterations=10000 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=10 --iterations=10000 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=13 --iterations=10000 --yield=d --sync=m >> lab2_list.csv


	./lab2_list --threads=5 --iterations=10000 --yield=dl  --sync=s >> lab2_list.csv
	./lab2_list --threads=8 --iterations=10000 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=10 --iterations=10000 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=13 --iterations=10000 --yield=d --sync=s >> lab2_list.csv





