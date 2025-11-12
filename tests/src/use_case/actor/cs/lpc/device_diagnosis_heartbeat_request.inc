#include <stdint.h>

static constexpr uint8_t device_diagnosis_heartbeat_request[] = R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
            {
              "device": "d:_n:Demo_ControlBox-123456789"
            },
            {
              "entity": [
                1
              ]
            },
            {
              "feature": 1
            }
          ]
        },
        {
          "addressDestination": [
            {
              "device": "d:_n:Demo_HEMS-123456789"
            },
            {
              "entity": [
                1
              ]
            },
            {
              "feature": 4
            }
          ]
        },
        {
          "msgCounter": 12
        },
        {
          "cmdClassifier": "read"
        }
      ]
    },
    {
      "payload": [
        {
          "cmd": [
            [
              {
                "deviceDiagnosisHeartbeatData": []
              }
            ]
          ]
        }
      ]
    }
  ]
})";
