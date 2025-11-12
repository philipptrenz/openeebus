#include <stdint.h>

static constexpr uint8_t load_control_binding_request[] = R"({
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
              "device": "d:_n:Demo_HEMS-123456789"
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
          "msgCounter": 6
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
                "nodeManagementBindingRequestCall": [
                  {
                    "bindingRequest": [
                      {
                        "clientAddress": [
                          {
                            "device": "d:_n:Demo_ControlBox-123456789"
                          },
                          {
                            "entity": [
                              1
                            ]
                          },
                          {
                            "feature": 2
                          }
                        ]
                      },
                      {
                        "serverAddress": [
                          {
                            "device": "d:_n:Demo_HEMS-123456789"
                          },
                          {
                            "entity": [
                              1
                            ]
                          },
                          {
                            "feature": 2
                          }
                        ]
                      },
                      {
                        "serverFeatureType": "LoadControl"
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
