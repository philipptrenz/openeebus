#include <stdint.h>

static constexpr uint8_t measurement_notify_power[] = R"({
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
              "feature": 2
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
              "feature": 2
            }
          ]
        },
        {
          "msgCounter": 14
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
                "function": "measurementListData"
              },
              {
                "filter": [
                  [
                    {
                      "cmdControl": [
                        {
                          "partial": []
                        }
                      ]
                    }
                  ]
                ]
              },
              {
                "measurementListData": [
                  {
                    "measurementData": [
                      [
                        {
                          "measurementId": 1
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 1000
                            },
                            {
                              "scale": 0
                            }
                          ]
                        },
                        {
                          "valueSource": "measuredValue"
                        }
                      ],
                      [
                        {
                          "measurementId": 2
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 1100
                            },
                            {
                              "scale": 0
                            }
                          ]
                        },
                        {
                          "valueSource": "measuredValue"
                        }
                      ],
                      [
                        {
                          "measurementId": 3
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 1200
                            },
                            {
                              "scale": 0
                            }
                          ]
                        },
                        {
                          "valueSource": "measuredValue"
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
