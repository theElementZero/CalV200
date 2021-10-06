/*
 * algo.h
 *
 *  Created on: Feb 14, 2018
 *      Author: Sushant
 */

#ifndef ALGO_H_
#define ALGO_H_

float computeSunrise(unsigned int sunrise);
unsigned int get_yday(ST_DATE date);

int dayofweek(ST_DATE *date);

extern float timeRise;
#endif /* ALGO_H_ */
