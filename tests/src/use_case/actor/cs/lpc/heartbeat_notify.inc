#include <stdint.h>

static constexpr uint8_t heartbeat_notify[] = R"({
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
              "feature": 5
            }
          ]
        },
        {
          "addressDestination": [
            {
              "device": "d:_i:Demo_HP234567890"
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
          "msgCounter": 54
        },
        {
          "cmdClassifier": "notify"
        }
      ]
    },
    {
      "payload": [
        {
          "cmd": [
            [
              {
                "deviceDiagnosisHeartbeatData": [
                  {
                    "timestamp": "2025-03-03T16:39:59Z"
                  },
                  {
                    "heartbeatCounter": 42
                  },
                  {
                    "heartbeatTimeout": "PT1M"
                  }
                ]
              }
            ]
          ]
        }
      ]
    }
  ]
})";
