/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import hiAppEvent from "@ohos.hiAppEvent"

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('HiAppEventJsTest', function () {
    beforeAll(function() {
        /*
         * @tc.setup: setup invoked before all test cases
         */
        console.info('HiAppEventJsTest beforeAll called')
    })

    afterAll(function() {
        /*
         * @tc.teardown: teardown invoked after all test cases
         */
        console.info('HiAppEventJsTest afterAll called')
    })

    beforeEach(function() {
        /*
         * @tc.setup: setup invoked before each test case
         */
        console.info('HiAppEventJsTest beforeEach called')
    })

    afterEach(function() {
        /*
         * @tc.teardown: teardown invoked after each test case
         */
        console.info('HiAppEventJsTest afterEach called')
    })

    /**
     * @tc.name: HiAppEventJsTest001
     * @tc.desc: Test the write interface using callback.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest001', 0, async function (done) {
        console.info('HiAppEventJsTest001 start');
        var result = 0;
        hiAppEvent.write("base_test1", hiAppEvent.EventType.BEHAVIOR,
            {
                "key_int":100,
                "key_string":"strValue",
                "key_bool":true,
                "key_float": 30949.374,
                "key_int_arr": [1, 2, 3],
                "key_string_arr": ["a", "b", "c"],
                "key_float_arr": [1.1, 2.2, 3.0],
                "key_bool_arr": [true, false, true]
            },
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest1 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest1 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(0)
                done()
            }
        );
        console.info('HiAppEventJsTest001 end');
    });

    /**
     * @tc.name: HiAppEventJsTest002
     * @tc.desc: Test the write interface using promise.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest002', 0, async function (done) {
        console.info('HiAppEventJsTest002 start');
        var result = 0;
        hiAppEvent.write("base_test2", hiAppEvent.EventType.FAULT,
            {
                "key_int":100,
                "key_string":"strValue",
                "key_bool":true,
                "key_float": 30949.374,
                "key_int_arr": [1, 2, 3],
                "key_string_arr": ["a", "b", "c"],
                "key_float_arr": [1.1, 2.2, 3.0],
                "key_bool_arr": [true, false, true]
            }
        ).then(
            (value) => {
                console.info('HiAppEvent_Napi baseTest2 suc.code=' + value);
                result = value;
                done()
            }
        ).catch(
            (err) => {
                console.error('HiAppEvent_Napi baseTest2 err.code=' + err.code);
                result = err.code;
                done()
            }
        );
        expect(result).assertEqual(0)
        console.info('HiAppEventJsTest002 end');
    });

    /**
     * @tc.name: HiAppEventJsTest003
     * @tc.desc: Error code 1 is returned when the event has an invalid key name.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest003', 0, async function (done) {
        console.info('HiAppEventJsTest003 start');
        var result = 0;
        hiAppEvent.write("base_test3", hiAppEvent.EventType.STATISTIC,
            {
                "**":"ha",
                "key_int":1,
                "HH22":"ha",
                "key_str":"str",
                "":"empty",
                "aa_":"underscore"
            },
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest3 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest3 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(1)
                done()
            }
        );
        console.info('HiAppEventJsTest003 end');
    });

    /**
     * @tc.name: HiAppEventJsTest004
     * @tc.desc: Error code 2 is returned when the event has an invalid key type.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest004', 0, async function (done) {
        console.info('HiAppEventJsTest004 start');
        var result = 0;
        hiAppEvent.write("base_test4", hiAppEvent.EventType.SECURITY,
            {
                [null]: 1,
                [{}]:2,
                [[1,2,3]]:3,
                "key_str":"str",
            },
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest4 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest4 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(2)
                done()
            }
        );
        console.info('HiAppEventJsTest004 end');
    });

    /**
     * @tc.name: HiAppEventJsTest005
     * @tc.desc: Error code 3 is returned when the event has an invalid value type.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest005', 0, async function (done) {
        console.info('HiAppEventJsTest005 start');
        var result = 0;
        hiAppEvent.write("base_test5", hiAppEvent.EventType.STATISTIC,
            {
                key_1_invalid:{},
                key_2_invalid:null,
                "key_str":"str",
            },
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest5 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest5 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(3)
                done()
            }
        );
        console.info('HiAppEventJsTest005 end');
    });

    /**
     * @tc.name: HiAppEventJsTest006
     * @tc.desc: Error code 4 is returned when the event has an invalid string length.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest006', 0, async function (done) {
        console.info('HiAppEventJsTest006 start');
        var longStr = "a".repeat(8*1024)
        var invalidStr = "a".repeat(8*1024 + 1)
        var result = 0;
        hiAppEvent.write("base_test6", hiAppEvent.EventType.STATISTIC,
            {
                key_long: longStr,
                key_i_long: invalidStr,
                key_long_arr: ["ha", longStr],
                key_i_long_arr: ["ha", invalidStr],
                "key_str": "str",
            },
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest6 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest6 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(4)
                done()
            }
        );
        console.info('HiAppEventJsTest006 end');
    });

    /**
     * @tc.name: HiAppEventJsTest007
     * @tc.desc: Error code 5 is returned when the event has too many params.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest007', 0, async function (done) {
        console.info('HiAppEventJsTest007 start');
        var result = 0;
        let keyValues = {}
        for (var i = 1; i <= 33; i++) {
            keyValues["key" + i] = "value" + i;
        }
        hiAppEvent.write("base_test7", hiAppEvent.EventType.STATISTIC, keyValues,
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest7 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest7 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(5)
                done()
            }
        );
        console.info('HiAppEventJsTest007 end');
    });

    /**
     * @tc.name: HiAppEventJsTest008
     * @tc.desc: Error code 6 is returned when there is an array with too many elements.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest008', 0, async function (done) {
        console.info('HiAppEventJsTest008 start');
        var result = 0;
        let longArr = new Array(100).fill(1);
        let iLongArr = new Array(101).fill("a");
        hiAppEvent.write("base_test8", hiAppEvent.EventType.STATISTIC,
            {
                key_long_arr: longArr,
                key_i_long_arr: iLongArr,
                "key_str": "str",
            },
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest8 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest8 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(6)
                done()
            }
        );
        console.info('HiAppEventJsTest008 end');
    });

    /**
     * @tc.name: HiAppEventJsTest009
     * @tc.desc: Error code 7 is returned when there is an array with inconsistent or illegal parameter types.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest009', 0, async function (done) {
        console.info('HiAppEventJsTest009 start');
        var result = 0;
        hiAppEvent.write("base_test9", hiAppEvent.EventType.STATISTIC,
            {
                key_arr_null: [null, null],
                key_arr_obj: [{}],
                key_arr_not_same1:[true, "ha"],
                key_arr_not_same2:[123, "ha"],
                "key_str": "str",
            },
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest9 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest9 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(7)
                done()
            }
        );
        console.info('HiAppEventJsTest009 end');
    });

    /**
     * @tc.name: HiAppEventJsTest010
     * @tc.desc: Error code -1 is returned when the event has invalid event name.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest010', 0, async function (done) {
        console.info('HiAppEventJsTest010 start');
        var result = 0;
        hiAppEvent.write("verify_test_1.**1", hiAppEvent.EventType.STATISTIC, {},
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest10_1 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest10_1 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(-1)
                done()
            }
        );

        hiAppEvent.write("VVV", hiAppEvent.EventType.STATISTIC, {},
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest10_2 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest10_2 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(-1)
                done()
            }
        );

        hiAppEvent.write("", hiAppEvent.EventType.STATISTIC, {},
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest10_3 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest10_3 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(-1)
                done()
            }
        );
        console.info('HiAppEventJsTest010 end');
    });

    /**
     * @tc.name: HiAppEventJsTest011
     * @tc.desc: Error code -2 is returned when the event has invalid eventName type, eventType type, keyValues type.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest011', 0, async function (done) {
        console.info('HiAppEventJsTest011 start');
        var result = 0;
        hiAppEvent.write(null, hiAppEvent.EventType.STATISTIC, {},
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest11_1 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest11_1 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(-2)
                done()
            }
        );

        hiAppEvent.write(123, hiAppEvent.EventType.STATISTIC, {},
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest11_2 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest11_2 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(-2)
                done()
            }
        );

        hiAppEvent.write("base_test11", "invalid Type", {},
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest11_3 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest11_3 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(-2)
                done()
            }
        );

        hiAppEvent.write("base_test11", hiAppEvent.EventType.STATISTIC, null,
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest11_4 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest11_4 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(-2)
                done()
            }
        );
        console.info('HiAppEventJsTest011 end');
    });

    /**
     * @tc.name: HiAppEventJsTest012
     * @tc.desc: Test preset events and preset parameters.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest012', 0, async function (done) {
        console.info('HiAppEventJsTest012 start');
        var result = 0;
        hiAppEvent.write(hiAppEvent.Event.USER_LOGIN, hiAppEvent.EventType.FAULT,
            {
                [hiAppEvent.Param.USER_ID]:"123456"
            },
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest12_1 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest12_1 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(0)
                done()
            }
        );

        hiAppEvent.write(hiAppEvent.Event.USER_LOGOUT, hiAppEvent.EventType.STATISTIC,
            {
                [hiAppEvent.Param.USER_ID]:"123456"
            },
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest12_2 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest12_2 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(0)
                done()
            }
        );

        hiAppEvent.write(hiAppEvent.Event.DISTRIBUTED_SERVICE_START, hiAppEvent.EventType.SECURITY,
            {
                [hiAppEvent.Param.DISTRIBUTED_SERVICE_NAME]:"test_service",
                [hiAppEvent.Param.DISTRIBUTED_SERVICE_INSTANCE_ID]:"123",
            },
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest12_3 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest12_3 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(0)
                done()
            }
        );
        console.info('HiAppEventJsTest012 end');
    });

    /**
     * @tc.name: HiAppEventJsTest013
     * @tc.desc: Error code -99 is returned when the logging function is disabled.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest013', 0, async function (done) {
        console.info('HiAppEventJsTest013 start');
        var result = 0;
        hiAppEvent.configure({
            disable: true
        })
        hiAppEvent.write("base_test13", hiAppEvent.EventType.SECURITY,
            {
                "key_str": "str",
            },
            (err ,value) => {
                if (err) {
                    console.error('HiAppEvent_Napi baseTest13 err.code=' + err.code);
                    result = err.code;
                } else {
                    console.info('HiAppEvent_Napi baseTest13 suc.code=' + value);
                    result = value;
                }
                expect(result).assertEqual(-99)
                done()
            }
        );
        console.info('HiAppEventJsTest013 end');
    });

    /**
     * @tc.name: HiAppEventJsTest014
     * @tc.desc: Correctly configure the event logging function.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest014', 0, function () {
        console.info('HiAppEventJsTest014 start');
        var result = false;

        result = hiAppEvent.configure({
            disable: true,
            maxStorage: "100m"
        });
        expect(result).assertTrue()

        result = hiAppEvent.configure({
            disable: false,
            maxStorage: "10G"
        });
        expect(result).assertTrue()

        console.info('HiAppEventJsTest014 end');
    });

    /**
     * @tc.name: HiAppEventJsTest015
     * @tc.desc: Incorrectly configure the event logging function.
     * @tc.type: FUNC
     */
    it('HiAppEventJsTest015', 0, function () {
        console.info('HiAppEventJsTest015 start');
        var result = true;

        result = hiAppEvent.configure({
            disable: false,
            maxStorage: "xxx"
        })
        expect(result).assertFalse()

        result = hiAppEvent.configure(null)
        expect(result).assertFalse()

        result = hiAppEvent.configure({
            disable: null,
            maxStorage: {}
        })
        expect(result).assertFalse()

        console.info('HiAppEventJsTest015 end');
    });
});