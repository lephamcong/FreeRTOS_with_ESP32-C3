#include <Arduino.h>

// Khai báo hai biến QueueHandle_t. Cả hai hàng đợi được thêm vào cùng một tập hợp hàng đợi.
static QueueHandle_t xQueue1 = NULL, xQueue2 = NULL;

// Khai báo một biến QueueSetHandle_t. Đây là tập hợp hàng đợi
// mà hai hàng đợi được thêm vào.
static QueueSetHandle_t xQueueSet = NULL;

void vSenderTask1(void *pvParameters);
void vSenderTask2(void *pvParameters);
void vReceiverTask(void *pvParameters);

void setup() {
    Serial.begin(9600);
    // Tạo hai hàng đợi, cả hai đều gửi con trỏ ký tự.
    xQueue1 = xQueueCreate(1, sizeof(char *));
    xQueue2 = xQueueCreate(1, sizeof(char *));

    // Tạo tập hợp hàng đợi. Hai hàng đợi sẽ được thêm vào tập hợp.
    xQueueSet = xQueueCreateSet(1 * 2);

    // Thêm hai hàng đợi vào tập hợp.
    xQueueAddToSet(xQueue1, xQueueSet);
    xQueueAddToSet(xQueue2, xQueueSet);

    // Tạo các tác vụ gửi đến hàng đợi.
    xTaskCreate(vSenderTask1, "Sender1", 1000, NULL, 1, NULL);
    xTaskCreate(vSenderTask2, "Sender2", 1000, NULL, 1, NULL);

    // Tạo tác vụ nhận từ tập hợp hàng đợi để xác định hàng đợi nào chứa dữ liệu.
    xTaskCreate(vReceiverTask, "Receiver", 1000, NULL, 2, NULL);

    // Bắt đầu bộ lập lịch để các tác vụ đã tạo bắt đầu thực thi.
    //vTaskStartScheduler();
    for(;;);
}

void loop() {
    // Hàm loop không bao giờ được thực thi vì vTaskStartScheduler() sẽ không trở về.
}

void vSenderTask1(void *pvParameters) {
    const TickType_t xBlockTime = pdMS_TO_TICKS(100);
    const char *const pcMessage = "Message from vSenderTask1\r\n";

    for (;;) {
        // Chờ 100ms.
        vTaskDelay(xBlockTime);
        // Gửi chuỗi này đến xQueue1. Không cần sử dụng thời gian chờ, 
        // vì hàng đợi chỉ có thể chứa một mục.
        xQueueSend(xQueue1, &pcMessage, 0);
    }
}

void vSenderTask2(void *pvParameters) {
    const TickType_t xBlockTime = pdMS_TO_TICKS(200);
    const char *const pcMessage = "Message from vSenderTask2\r\n";

    for (;;) {
        // Chờ 200ms.
        vTaskDelay(xBlockTime);
        // Gửi chuỗi này đến xQueue2. Không cần sử dụng thời gian chờ.
        xQueueSend(xQueue2, &pcMessage, 0);
    }
}

void vReceiverTask(void *pvParameters) {
    QueueHandle_t xQueueThatContainsData;
    char *pcReceivedString;

    for (;;) {
        // Chờ trên tập hợp hàng đợi để chờ một trong các hàng đợi chứa dữ liệu.
        xQueueThatContainsData = (QueueHandle_t)xQueueSelectFromSet(xQueueSet, portMAX_DELAY);

        // Đọc từ hàng đợi.
        xQueueReceive(xQueueThatContainsData, &pcReceivedString, 0);

        // In chuỗi nhận được từ hàng đợi.
        Serial.print(pcReceivedString);
    }
}
