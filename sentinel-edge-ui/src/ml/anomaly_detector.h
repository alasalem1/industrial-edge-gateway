#ifndef SENTINEL_ANOMALY_DETECTOR_H
#define SENTINEL_ANOMALY_DETECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#define SENTINEL_AI_FEATURE_COUNT 4

/*
 * Features:
 * 0: temperature (C)
 * 1: pressure (bar)
 * 2: vibration (mm/s)
 * 3: motor speed (RPM)
 *
 * Returns:
 *  1 = anomaly
 *  0 = normal
 * -1 = invalid input or inference error
 */
int sentinel_ai_predict(const double features[SENTINEL_AI_FEATURE_COUNT],
                        double *anomaly_score);

#ifdef __cplusplus
}
#endif

#endif
