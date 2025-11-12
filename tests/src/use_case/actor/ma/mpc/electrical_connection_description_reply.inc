#include <stdint.h>

static constexpr uint8_t electrical_connection_description_reply[] = R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
            {
              "device": "d:_n:NIBE_06613320300999"
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
              "device": "d:_n:OpenEEBUS_123456789"
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
          "msgCounter": 8
        },
        {
          "msgCounterReference": 6
        },
        {
          "cmdClassifier": "reply"
        }
      ]
    },
    {
      "payload": [
        {
          "cmd": [
            [
              {
                "electricalConnectionDescriptionListData": [
                  {
                    "electricalConnectionDescriptionData": [
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "powerSupplyType": "ac"
                        },
                        {
                          "positiveEnergyDirection": "consume"
                        }
                      ]
                    ]
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
