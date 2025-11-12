#include <stdint.h>

static constexpr uint8_t node_management_subscription_request[] = R"({
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
              "device": "d:_n:OpenEEBUS_123456789"
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
          "msgCounter": 3
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
                            "device": "d:_n:NIBE_06613320300999"
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
                        "serverAddress": [
                          {
                            "device": "d:_n:OpenEEBUS_123456789"
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
                        "serverFeatureType": "NodeManagement"
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
