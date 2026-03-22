#include "tinyml.h"
#include "test_dataset.h"

// Globals, for the convenience of one-shot setup.
namespace
{
    tflite::ErrorReporter *error_reporter = nullptr;
    const tflite::Model *model = nullptr;
    tflite::MicroInterpreter *interpreter = nullptr;
    TfLiteTensor *input = nullptr;
    TfLiteTensor *output = nullptr;
    constexpr int kTensorArenaSize = 8 * 1024; // Adjust size based on your model
    uint8_t tensor_arena[kTensorArenaSize];
} // namespace

void setupTinyML()
{
    if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
        Serial.println("TensorFlow Lite Init....");
        xSemaphoreGive(xSerialMutex);
    }
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    model = tflite::GetModel(dht_anomaly_model_tflite); // g_model_data is from model_data.h
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        error_reporter->Report("Model provided is schema version %d, not equal to supported version %d.",
                               model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;

    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        error_reporter->Report("AllocateTensors() failed");
        return;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);
    if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
        Serial.println("TensorFlow Lite Micro initialized on ESP32.");
        xSemaphoreGive(xSerialMutex);
    }
}

void evaluate_accuracy() {
    if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
        Serial.println("\n=======================================================");
        Serial.println("MODEL ACCURACY EVALUATION STARTED");
        Serial.println("=======================================================");
        xSemaphoreGive(xSerialMutex);
    }

    // Synthetic Test Dataset moved to include/test_dataset.h
    // Contains 50 edge, standard, and extreme cases for rigorous evaluations.

    int correct_predictions = 0;
    int true_positives = 0;   // Predicted Anomaly & Actual Anomaly
    int false_positives = 0;  // Predicted Anomaly & Actual Normal
    int false_negatives = 0;  // Predicted Normal & Actual Anomaly

    for (int i = 0; i < TEST_SAMPLES_COUNT; i++) {
        input->data.f[0] = EVALUATION_DATASET[i][0];
        input->data.f[1] = EVALUATION_DATASET[i][1];

        // Run inference
        if (interpreter->Invoke() != kTfLiteOk) {
            if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
                Serial.println("Invoke failed during test!");
                xSemaphoreGive(xSerialMutex);
            }
            continue;
        }

        float result = output->data.f[0];
        int predicted_label = (result > 0.5) ? 1 : 0;
        int actual_label = (int)EVALUATION_DATASET[i][2];

        // Comparision
        if (predicted_label == actual_label) {
            correct_predictions++;
        }
        
        if (predicted_label == 1 && actual_label == 1) true_positives++;
        if (predicted_label == 1 && actual_label == 0) false_positives++;
        if (predicted_label == 0 && actual_label == 1) false_negatives++;

        if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
            Serial.printf("Test %02d | T: %.1f, H: %.1f | Score: %.3f | Actual: %d, Predicted: %d\r\n", 
                i+1, EVALUATION_DATASET[i][0], EVALUATION_DATASET[i][1], result, actual_label, predicted_label);
            xSemaphoreGive(xSerialMutex);
        }
        
        vTaskDelay(pdMS_TO_TICKS(50)); // Small delay to prevent Serial buffer overflow
    }

    // Calculate Confusion Matrix
    float accuracy = ((float)correct_predictions / (float)TEST_SAMPLES_COUNT) * 100.0;
    float precision = 0.0;
    if (true_positives + false_positives > 0) {
        precision = ((float)true_positives / (true_positives + false_positives)) * 100.0;
    }
    float recall = 0.0;
    if (true_positives + false_negatives > 0) {
        recall = ((float)true_positives / (true_positives + false_negatives)) * 100.0;
    }

    if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
        Serial.println("\n---------------- EVALUATION RESULTS ----------------");
        Serial.printf("Total samples: %d | Correctly predicted: %d/%d\r\n", TEST_SAMPLES_COUNT, correct_predictions, TEST_SAMPLES_COUNT);
        Serial.printf("Accuracy  : %.2f %%\r\n", accuracy);
        Serial.printf("Precision : %.2f %%\r\n", precision);
        Serial.printf("Recall    : %.2f %%\r\n", recall);
        Serial.println("=======================================================\n");
        Serial.println("Proceeding to real-time sensor monitoring (Task 5)...\n");
        xSemaphoreGive(xSerialMutex);
    }
}


void tiny_ml_task(void *pvParameters)
{
    SystemData *data = (SystemData *)pvParameters;
    setupTinyML();

    // RUN EVALUATION SCRIPT ON STARTUP
    evaluate_accuracy(); 

    while (1)
    {
        // Fetch input sensor data safely via SystemData
        float temp = 0.0, humi = 0.0;
        if (xSemaphoreTake(data->xMutex, portMAX_DELAY)) {
            temp = data->temperature;
            humi = data->humidity;
            xSemaphoreGive(data->xMutex);
        }
        
        input->data.f[0] = temp;
        input->data.f[1] = humi;

        // Run inference
        TfLiteStatus invoke_status = interpreter->Invoke();
        if (invoke_status != kTfLiteOk)
        {
            error_reporter->Report("Invoke failed");
            return;
        }

        // Fetch output result
        float result = output->data.f[0];
        
        // Print raw data and inference result concurrently
        if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
            Serial.printf("[TinyML] Temp: %.1fC | Humi: %.1f%%  -->  Anomaly Score: %.3f\r\n", temp, humi, result);
            xSemaphoreGive(xSerialMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}