#include <stdint.h>

static constexpr uint8_t electrical_connection_parameter_description_request[] = R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
            {
              "device": "d:_n:Bosch_myHP-12345678"
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
          "addressDestination": [
            {
              "device": "d:_i:Demo_HEMS-123456789"
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
          "msgCounter": 7
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
                "electricalConnectionParameterDescriptionListData": []
              }
            ]
          ]
        }
      ]
    }
  ]
})";
