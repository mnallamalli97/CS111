#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <mraa.h>
#include <mraa/aio.h>
#include <mraa/i2c.h>
#include <signal.h>
#include <poll.h>

// **** MRAA variables **** //
mraa_aio_context temp_sensor;
mraa_gpio_context button;
#define THERMISTOR 4275

// **** global files, counters, and flags **** //
int opt;
char temp_scale = 'F';
int period_v = 1;
FILE* log_v = 0;
int out_time_temp = 1;
int log_v_flag = 0;
double temp_val;
struct pollfd pollfds;

// **** time variables **** //
struct timeval start;
struct tm* curr_time;
time_t next_time = 0;


double recordtemp(){
	int recorded_val = mraa_aio_read(temp_sensor);
	int thermistor = THERMISTOR;
	double converted_temp = (double)1023.0 / ((double) recorded_val) - (double)1.0;
	converted_temp = converted_temp * 100000.0;
	double celsius = 1.0 / (log(converted_temp/100000.0)/thermistor + 1/298.15) - 273.15;

	if(temp_scale == 'F'){
		return ((celsius*(9/5)) + 32);

	}else{
		return celsius;
	}
}

void turnoff(){
	curr_time = localtime(&start.tv_sec);
	if (log_v > 0)
	{
		fprintf(log_v, "%02d:%02d:%02d SHUTDOWN\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp_val );
		fprintf(stdout, "%02d:%02d:%02d SHUTDOWN\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp_val );

	}else{
		fprintf(stdout, "%02d:%02d:%02d SHUTDOWN\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp_val );
	}
	
	exit(0);
}


void analyze_buff(char* buff){
	int len_of_buff = strlen(buff);
	//add null byte 
	buff[len_of_buff-1] = '\0';
	
	//return pointer to beginning of period or log in the buffer
	char* analyze_period = strstr(buff, "PERIOD=");
	char* analyze_log = strstr(buff, "LOG");

	int equal_scalef = strcmp(buff, "SCALE=F");
	if (equal_scalef == 0)
	{
		if (log_v > 0)
		{
			fprintf(log_v, "SCALE=F\n");
			//fflush(buff);
		}
		temp_scale = 'F';
	}

	int equal_scalec = strcmp(buff, "SCALE=C");
	if (equal_scalec == 0)
	{
		if (log_v > 0)
		{
			fprintf(log_v, "SCALE=C\n");
			//fflush(buff);
		}
		temp_scale = 'C';
	}

	int equal_stop = strcmp(buff, "STOP");
	if (equal_stop == 0)
	{
		if (log_v > 0)
		{
			fprintf(log_v, "STOP\n");
		}
		//fflush(buff);
		out_time_temp = 0;
	}

	int equal_start = strcmp(buff, "START");
	if (equal_start == 0)
	{
		if (log_v > 0)
		{
			fprintf(log_v, "START\n");
			//fflush(buff);
		}
		out_time_temp = 1;
	}

	int equal_off = strcmp(buff, "OFF");
	if (equal_off == 0)
	{
		if (log_v > 0)
		{
			fprintf(log_v, "OFF\n");
			//fflush(buff);
		}
		turnoff();
	}


	int len = strlen(buff);
	if (len > 4 && buff[0] == 'L')
	{
		if(log_v_flag == 0){
			return;
		}else{
			fputs(buff, log_v);
		}
	}else if(len > 7 && buff[0] == 'P'){
		int new_period = strlen(buff) - 1;
		for(int i = 7; i < new_period; i++){
			if(!isdigit(buff[i])){
				return 0;
			}
		}
		int ans = atoi(&buff[7]);

		if(ans <= 0){
			if (log_v_flag == 1)
			{
				fprintf(log_v, "%s\n", buff );
				fprintf(log_v, "cannot recog. arg \n");
			}
			fflush(log_v);
			exit(1);
		}else{
			if (log_v_flag == 1)
			{
				fprintf(log_v, "%s\n", buff );
			}
			fflush(log_v);
			period_v = ans;
		}
	}
}

int main(int argc, char** argv){
	static struct option long_options[] = 
	{
		{"period", 1, 0, 'p'},
		{"scale", 1, 0, 's'},
		{"log", 1, 0, 'l'},
		{0,0,0,0}
	};

	while((opt = getopt_long(argc, argv, "", long_options, 0)) != -1){
		switch(opt){
			case 'p':
				period_v = atoi(optarg);
				break;
			case 's':

				if((optarg[0] != 'F' && optarg[0] != 'C') || strlen(optarg) != 1){
					fprintf(stderr, "The optarg is not valid\n" );
					exit(1);
				}
				temp_scale = optarg[0];
				break;

			case 'l':
				log_v = fopen(optarg, "w+");
				log_v_flag = 1;
				if (log_v == NULL)
				{
					fprintf(stderr, "cannot write to log file\n" );
					exit(1);
				}
				break;

			default: 
				fprintf(stderr, "unregonized argument\n");
				exit(1);
		}
	}

	//once the arguments are recorded, now can start the process of init I/O and pollinh
	//polling
	
	pollfds.fd = STDIN_FILENO;
	pollfds.events = POLLIN;

	//I/O
	temp_sensor = mraa_aio_init(1);
	button = mraa_gpio_init(60);
	mraa_gpio_dir(button, MRAA_GPIO_IN);
	//what function to call if button pressed
	mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &turnoff, NULL);

	char buff[128];

	for(;;){
		gettimeofday(&start, 0);
		//for every period, get the temp and time, print it to either file or console depening on arg
		if (start.tv_sec >= next_time && out_time_temp == 1)
		{
			temp_val = recordtemp();
			curr_time = localtime(&start.tv_sec);
			if(log_v_flag == 1){
				fprintf(log_v, "%02d:%02d:%02d %.1f\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp_val );
				fprintf(stdout, "%02d:%02d:%02d %.1f\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp_val );
			}else{
				fprintf(stdout, "%02d:%02d:%02d %.1f\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp_val );
			}
			
			next_time = start.tv_sec + period_v;
		}

		//if there is an input while the program is running. 
		int ret = poll(&pollfds, 1, 0);
		if (ret)
		{
			fgets(buff, 1024, stdin);
			analyze_buff(buff);
		}

	}

	//close the api call for the button and sensor .
	mraa_aio_close(temp_sensor);
	mraa_gpio_close(button);

	//free(buff);
	exit(0);
}