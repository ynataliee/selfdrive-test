/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Natalie Yam
* Student ID:: 920698945
* Github-Name:: ynataliee
* Project:: Assignment 4 - Follow in Line but Stop
*
* File:: followLine.c
*
* Description:: This program uses threads to read sensor data 
* from the TCRT5000 Line Sensor and the IR Avoidance Sensor.
**************************************************************/

#include <stdlib.h> //exit()
#include <stdio.h>
#include <pigpio.h>
#include <signal.h> // for stopping the program with ctrl c
#include <pthread.h>


#define OBSTACLE_SENSOR 18 //BCM pin 18 on gpio
#define LINE_SENSOR 23 //BCM pin 23 on gpio

// used to track the states of the sensors
volatile int lineSensorVal;
volatile int obstacleSensorVal;
// for the while loops in the read sensor function that is being threaded
int isDone = 1;

// used to cleanly terminate the program when the user enters ctrl c
void handler(int signo){
	// ending the while loop for the threaded function
	isDone = 0;
	time_sleep(1);
	printf("\nStopping the sensor from reading\n");
	// releasing gpio resources 
	gpioTerminate();
	exit(0);
}

// function to be threaded 
void * readSensor(void* args){
	// void pointers cannot be dereferenced, cast to int pointer 
	int * pinNumber = (int*)(args);
        while(isDone){
		if(*pinNumber == OBSTACLE_SENSOR){
			// update sensor state values 
                	if(gpioRead(*pinNumber) == 0){
                        	obstacleSensorVal = 0;
                	}
                	else{
                        	obstacleSensorVal = 1;
                	}
		}
		else{
			// update sensor state values 
			if(gpioRead(*pinNumber) == 0){
				lineSensorVal = 0;
                        }
                        else{
                                lineSensorVal = 1;
                        }
		}
        }
	// to eliminate no return on non-void function warning when compiling
	return (void*)(1);
}

int main(int argc, char*argv[]){
	int linePin = LINE_SENSOR;
	int obstaclePin = OBSTACLE_SENSOR;
	// prepare the gpio pins to be used 
        if(gpioInitialise()< 0){
                fprintf(stderr, "pigpio initialization failed\n");
                return 1;
        }

	// setting obstacle sensor pin to input mode
	if(gpioSetMode(OBSTACLE_SENSOR, PI_INPUT)!= 0){
		printf("could not set echo pin\n");
		gpioTerminate();
		return 1;
	} 

	// setting line sensor pin to input mode
	if(gpioSetMode(LINE_SENSOR, PI_INPUT)!= 0){
                printf("could not set echo pin\n");
                gpioTerminate();
                return 1;
        }

	// setting up callback handler function for ctrl c
	signal(SIGINT, handler);

	// to hold thread ids 
	pthread_t lineSensor, obstacleSensor;

	// creating thread that monitors line sensor
	int iret1 = pthread_create( &lineSensor, NULL, &readSensor, (void*)(&linePin));
    	if(iret1 != 0){
        	printf("failed to create line sensor thread\n");
    	}

	// creating thread that monitors obstacle sensor 
    	int iret2 = pthread_create( &obstacleSensor, NULL, &readSensor, (void*)(&obstaclePin));
	if(iret2 != 0){
        	printf("failed to create obstacle sensor thread\n");
    	}

	// continuously check the values of the sensors and display 
	// if there are obstacles and if sensor is on the line
	while(1){
		if(lineSensorVal == 1){
			printf("OFF the line\n");
		}
		else{
			printf("ON the line\n");
		}
		if(obstacleSensorVal == 1){
			printf("NO obstacle\n");
		}
		else{
			printf("OBSTACLE\n");
		}
		time_sleep(1);
	}

	gpioTerminate();
	// wait for threads to finish 
	pthread_join(lineSensor, NULL);
        pthread_join(obstacleSensor, NULL);
	return 0;
}
