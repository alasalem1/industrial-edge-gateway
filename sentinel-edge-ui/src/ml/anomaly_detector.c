#include "anomaly_detector.h"
#include "isolation_forest_model.h"

#include <math.h>
#include <stddef.h>

#define SENTINEL_AI_EULER_GAMMA 0.5772156649015329

static double average_path_length(int n)
{
    if (n <= 1)
        return 0.0;

    if (n == 2)
        return 1.0;

    return 2.0 * (log((double)(n - 1)) +
                  SENTINEL_AI_EULER_GAMMA)
           - 2.0 * (double)(n - 1) / (double)n;
}

static int tree_path_length(int tree_index,
                            const float input[SENTINEL_AI_FEATURE_COUNT],
                            double *path_length)
{
    int start = sentinel_ai_tree_offsets[tree_index];
    int end = sentinel_ai_tree_offsets[tree_index + 1];
    int node = start;
    int depth = 0;

    while (node >= start && node < end) {
        int feature = sentinel_ai_features[node];

        /* Negative feature index identifies a leaf. */
        if (feature < 0) {
            int leaf_samples = sentinel_ai_samples[node];
            *path_length = (double)depth +
                           average_path_length(leaf_samples);
            return 0;
        }

        if (feature >= SENTINEL_AI_FEATURE_COUNT)
            return -1;

        if (input[feature] <=
            sentinel_ai_thresholds[node]) {
            node = start + sentinel_ai_left[node];
        } else {
            node = start + sentinel_ai_right[node];
        }

        depth++;
    }

    return -1;
}

int sentinel_ai_predict(const double features[SENTINEL_AI_FEATURE_COUNT],
                        double *anomaly_score)
{
    float input[SENTINEL_AI_FEATURE_COUNT];
    double total_path_length = 0.0;
    double normalization;
    int i;

    if (features == NULL || anomaly_score == NULL)
        return -1;

    for (i = 0; i < SENTINEL_AI_FEATURE_COUNT; i++) {
        if (!isfinite(features[i]))
            return -1;

        input[i] = (float)features[i];

        if (!isfinite(input[i]))
            return -1;
    }

    normalization = average_path_length(SENTINEL_AI_MAX_SAMPLES);

    if (normalization <= 0.0)
        return -1;

    for (i = 0; i < SENTINEL_AI_TREE_COUNT; i++) {
        double path_length;

        if (tree_path_length(i, input, &path_length) != 0)
            return -1;

        total_path_length += path_length;
    }

    *anomaly_score = -pow(
        2.0,
        -(total_path_length /
          ((double)SENTINEL_AI_TREE_COUNT * normalization))
    );

    return (*anomaly_score < SENTINEL_AI_THRESHOLD) ? 1 : 0;
}
