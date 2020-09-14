// Thanks to nyx for the AQI code

#include <math.h> //for round()

#include "aqi.h"

static const float pm25_cuts[7] = {12, 35.4, 55.4, 150.4, 250.4, 350.4, 500.4};
static const float pm10_cuts[7] = {54, 154, 254, 354, 424, 504, 604};
static const float aqi_cuts[7] = {50, 100, 150, 200, 300, 400, 500};

unsigned aqi25(float pm25_conc){
	unsigned i = 0;
	float aqi, i_hi, i_lo, c_hi, c_lo;
	while (i < 7){
		if (pm25_conc < pm25_cuts[i]){
			i_hi = aqi_cuts[i];
			i_lo = i == 0 ? 0 : aqi_cuts[i-1] + 1;
			c_hi = pm25_cuts[i];
			c_lo = i == 0 ? 0 : pm25_cuts[i-1] + 0.1;
			aqi = (i_hi - i_lo) / (c_hi - c_lo) * (pm25_conc - c_lo) + (i_lo);
			return round(aqi);
		}
		i++;
	}
	return 500;
}

unsigned aqi10(float pm10_conc){
	unsigned i = 0;
	float aqi, i_hi, i_lo, c_hi, c_lo;
	while (i < 7){
		if (pm10_conc < pm10_cuts[i]){
			i_hi = aqi_cuts[i];
			i_lo = i == 0 ? 0 : aqi_cuts[i-1] + 1;
			c_hi = pm10_cuts[i];
			c_lo = i == 0 ? 0 : pm10_cuts[i-1] + 1;
			aqi = (i_hi - i_lo) / (c_hi - c_lo) * (pm10_conc - c_lo) + (i_lo);
			return round(aqi);
		}
		i++;
	}
	return 500;
}

unsigned aqi(float pm25_conc, float pm10_conc){
	float aqi25_val = aqi25(pm25_conc);
	float aqi10_val = aqi10(pm10_conc);
	return aqi25_val > aqi10_val ? aqi25_val : aqi10_val;
}
