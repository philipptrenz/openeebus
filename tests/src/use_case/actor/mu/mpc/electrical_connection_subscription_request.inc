#include <stdint.h>

static constexpr uint8_t electrical_connection_subscription_request[] = R"({
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
                0
              ]
            },
            {
              "feature": 0
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
                0
              ]
            },
            {
              "feature": 0
            }
          ]
        },
        {
          "msgCounter": 5
        },
        {
          "cmdClassifier": "call"
        },
        {
          "ackRequest": true
        }
      ]
    },
    {
      "payload": [
        {
          "cmd": [
            [
              {
                "nodeManagementSubscriptionRequestCall": [
                  {
                    "subscriptionRequest": [
                      {
                        "clientAddress": [
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
                        "serverAddress": [
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
                        "serverFeatureType": "ElectricalConnection"
                      }
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
