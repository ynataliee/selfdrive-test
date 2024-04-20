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

// right on bcm 26
// middle on bcm 13
// left on bcm 6

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

// NEW
#define RIGHT 26 //BCM pin
#define MIDDLE 13 //BCM pin
#define LEFT 6 //BCM pin

// NEW 
volatile int rightVal;
volatile int middleVal;
volatile int leftVal;

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
		if(*pinNumber == RIGHT){
			// update sensor state values 
                	if(gpioRead(*pinNumber) == 0){
                        	rightVal = 0;
                	}
                	else{
                        	rightVal = 1;
                	}
		}
		else if(*pinNumber == LEFT){
			// update sensor state values 
                	if(gpioRead(*pinNumber) == 0){
                        	leftVal = 0;
                	}
                	else{
                        	leftVal = 1;
                	}
		}
		else if(*pinNumber == MIDDLE){
			// update sensor state values 
                	if(gpioRead(*pinNumber) == 0){
                        	middleVal = 0;
                	}
                	else{
                        	middleVal = 1;
                	}
		}
			
        }
	// to eliminate no return on non-void function warning when compiling
	return (void*)(1);
}

int main(int argc, char*argv[]){
	int linePin = LINE_SENSOR;
	int obstaclePin = OBSTACLE_SENSOR;

	int rightPin = RIGHT;
	int leftPin = LEFT;
	int middlePin = MIDDLE;

	// prepare the gpio pins to be used 
        if(gpioInitialise()< 0){
                fprintf(stderr, "pigpio initialization failed\n");
                return 1;
        }

	// setting right sensor pin to input mode
	if(gpioSetMode(RIGHT, PI_INPUT)!= 0){
		printf("could not set echo pin\n");
		gpioTerminate();
		return 1;
	} 

	// setting left line sensor pin to input mode
	if(gpioSetMode(LEFT, PI_INPUT)!= 0){
                printf("could not set echo pin\n");
                gpioTerminate();
                return 1;
        }

	// setting middle sensor pin to input mode
	if(gpioSetMode(MIDDLE, PI_INPUT)!= 0){
                printf("could not set echo pin\n");
                gpioTerminate();
                return 1;
        }

	// setting up callback handler function for ctrl c
	signal(SIGINT, handler);

	// to hold thread ids 
	pthread_t rightSensor, leftSensor, middleSensor;

	// creating thread that monitors line sensor
	int iret1 = pthread_create( &rightSensor, NULL, &readSensor, (void*)(&rightPin));
    	if(iret1 != 0){
        	printf("failed to create line sensor thread\n");
    	}

	// creating thread that monitors obstacle sensor 
    	int iret2 = pthread_create( &leftSensor, NULL, &readSensor, (void*)(&leftPin));
	if(iret2 != 0){
        	printf("failed to create obstacle sensor thread\n");
    	}

	// creating thread that monitors obstacle sensor 
    	int iret3 = pthread_create( &middleSensor, NULL, &readSensor, (void*)(&middlePin));
	if(iret3 != 0){
        	printf("failed to create obstacle sensor thread\n");
    	}

	// continuously check the values of the sensors and display 
	// if there are obstacles and if sensor is on the line
	while(1){
		if(rightVal == 1){
			printf("OFF the line\n");
		}
		else{
			printf("ON the line\n");
		}
		if(leftVal == 1){
			printf("OFF the line\n");
		}
		else{
			printf("ON the line\n");
		}
		if(middleVal == 1){
			printf("OFF the line\n");
		}
		else{
			printf("ON the linen");
		}
		time_sleep(1);
	}

	gpioTerminate();
	// wait for threads to finish 
	pthread_join(rightSensor, NULL);
        pthread_join(leftSensor, NULL);
	pthread_join(middleSensor, NULL);
	
	return 0;
}
