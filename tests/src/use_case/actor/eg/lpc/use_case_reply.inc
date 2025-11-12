static constexpr char use_case_reply[] = R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
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
          "addressDestination": [
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
          "msgCounter": 8
        },
        {
          "msgCounterReference": 4
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
                "nodeManagementUseCaseData": [
                  {
                    "useCaseInformation": [
                      [
                        {
                          "address": [
                            {
                              "device": "d:_n:Demo_HEMS-123456789"
                            },
                            {
                              "entity": [
                                1
                              ]
                            }
                          ]
                        },
                        {
                          "actor": "ControllableSystem"
                        },
                        {
                          "useCaseSupport": [
                            [
                              {
                                "useCaseName": "limitationOfPowerConsumption"
                              },
                              {
                                "useCaseVersion": "1.0.0"
                              },
                              {
                                "useCaseAvailable": true
                              },
                              {
                                "scenarioSupport": [
                                  1,2,3,4
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "release"
                              }
                            ],
                            [
                              {
                                "useCaseName": "limitationOfPowerProduction"
                              },
                              {
                                "useCaseVersion": "1.0.0"
                              },
                              {
                                "useCaseAvailable": true
                              },
                              {
                                "scenarioSupport": [
                                  1,2,3,4
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "release"
                              }
                            ]
                          ]
                        }
                      ],
                      [
                        {
                          "address": [
                            {
                              "device": "d:_n:Demo_HEMS-123456789"
                            },
                            {
                              "entity": [
                                1
                              ]
                            }
                          ]
                        },
                        {
                          "actor": "EnergyGuard"
                        },
                        {
                          "useCaseSupport": [
                            [
                              {
                                "useCaseName": "limitationOfPowerConsumption"
                              },
                              {
                                "useCaseVersion": "1.0.0"
                              },
                              {
                                "useCaseAvailable": true
                              },
                              {
                                "scenarioSupport": [
                                  1,2,3,4
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "release"
                              }
                            ],
                            [
                              {
                                "useCaseName": "limitationOfPowerProduction"
                              },
                              {
                                "useCaseVersion": "1.0.0"
                              },
                              {
                                "useCaseAvailable": true
                              },
                              {
                                "scenarioSupport": [
                                  1,2,3,4
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "release"
                              }
                            ]
                          ]
                        }
                      ],
                      [
                        {
                          "address": [
                            {
                              "device": "d:_n:Demo_HEMS-123456789"
                            },
                            {
                              "entity": [
                                1
                              ]
                            }
                          ]
                        },
                        {
                          "actor": "MonitoringAppliance"
                        },
                        {
                          "useCaseSupport": [
                            [
                              {
                                "useCaseName": "monitoringOfGridConnectionPoint"
                              },
                              {
                                "useCaseVersion": "1.0.0"
                              },
                              {
                                "useCaseAvailable": true
                              },
                              {
                                "scenarioSupport": [
                                  1,2,3,4,5,6,7
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "release"
                              }
                            ]
                          ]
                        }
                      ],
                      [
                        {
                          "address": [
                            {
                              "device": "d:_n:Demo_HEMS-123456789"
                            },
                            {
                              "entity": [
                                1
                              ]
                            }
                          ]
                        },
                        {
                          "actor": "CEM"
                        },
                        {
                          "useCaseSupport": [
                            [
                              {
                                "useCaseName": "visualizationOfAggregatedBatteryData"
                              },
                              {
                                "useCaseVersion": "1.0.1"
                              },
                              {
                                "useCaseAvailable": true
                              },
                              {
                                "scenarioSupport": [
                                  1,2,3,4
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "RC1"
                              }
                            ],
                            [
                              {
                                "useCaseName": "visualizationOfAggregatedPhotovoltaicData"
                              },
                              {
                                "useCaseVersion": "1.0.1"
                              },
                              {
                                "useCaseAvailable": true
                              },
                              {
                                "scenarioSupport": [
                                  1,2,3
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "RC1"
                              }
                            ]
                          ]
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
