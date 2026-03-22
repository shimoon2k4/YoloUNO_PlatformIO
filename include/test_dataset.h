#ifndef TEST_DATASET_H
#define TEST_DATASET_H

// 50 Comprehensive Test Samples for TinyML Accuracy Evaluation
// Format: {Temperature, Humidity, Expected_Label}
// Expected_Label: 0 = Normal, 1 = Anomaly
// Assuming Normal bounds roughly: Temp 20-35C, Humi 40-70% (based on standard comfort zones)

const int TEST_SAMPLES_COUNT = 50;

const float EVALUATION_DATASET[50][3] = {
    // --- STANDARD NORMAL CASES (Label 0) ---
    {25.0, 50.0, 0}, {22.0, 45.0, 0}, {28.0, 60.0, 0}, {30.0, 65.0, 0}, {24.0, 55.0, 0},
    {26.5, 48.0, 0}, {21.0, 42.0, 0}, {29.0, 68.0, 0}, {33.0, 60.0, 0}, {27.0, 50.0, 0},
    {23.5, 58.0, 0}, {31.0, 62.0, 0}, {20.5, 45.0, 0}, {34.0, 69.0, 0}, {28.5, 53.0, 0},
    {25.5, 47.0, 0}, {22.5, 52.0, 0}, {32.0, 64.0, 0}, {23.0, 41.0, 0}, {29.5, 66.0, 0},
    
    // --- EDGE NORMAL CASES (Label 0) ---
    {20.1, 40.1, 0}, {34.9, 69.9, 0}, {20.0, 70.0, 0}, {35.0, 40.0, 0}, {28.0, 40.5, 0},
    
    // --- ANOMALY CASES : High Temperature (Label 1) ---
    {40.0, 50.0, 1}, {45.0, 60.0, 1}, {38.0, 45.0, 1}, {50.0, 80.0, 1}, {55.0, 20.0, 1},
    
    // --- ANOMALY CASES : Low Temperature (Label 1) ---
    {10.0, 50.0, 1}, {5.0, 60.0, 1}, {15.0, 40.0, 1}, {0.0, 80.0, 1}, {-5.0, 30.0, 1},
    
    // --- ANOMALY CASES : High Humidity (Label 1) ---
    {25.0, 85.0, 1}, {28.0, 95.0, 1}, {22.0, 75.0, 1}, {30.0, 90.0, 1}, {24.0, 100.0, 1},
    
    // --- ANOMALY CASES : Low Humidity (Label 1) ---
    {25.0, 20.0, 1}, {28.0, 10.0, 1}, {22.0, 35.0, 1}, {30.0, 15.0, 1}, {24.0, 5.0, 1},
    
    // --- ANOMALY CASES : Extreme Combinations (Label 1) ---
    {45.0, 95.0, 1}, {-10.0, 10.0, 1}, {50.0, 5.0, 1}, {0.0, 90.0, 1}, {60.0, 100.0, 1}
};

#endif
