var five = require("johnny-five");
var Raspi = require("raspi-io");
var temp = require("node-dht-sensor");
var tsl2591 = require("tsl2591");
var Gpio = require('onoff').Gpio;
var sleep = require('sleep');

var board = new five.Board({    //Initialize with J5-Rpio interface
    io: new Raspi()
});

board.on("ready", function () {
    var status = 1; // turn to 0 when initialized

    //pins Initialize
    var RLED = new five.Led("P1-37");
    var GLED = new five.Led("P1-35");

    var ABTN = new five.Button("P1-13");
    var LBTN = new five.Button("P1-16");
    var RBTN = new five.Button("P1-18");

    var piezo = new Gpio(18, 'out');
    var BLED = new Gpio(6, 'out');
    var light = new tsl2591({ device: "/dev/i2c-1" }); //i2c Initialize


    //on/off reversed
    RLED.on();
    GLED.on();
    BLED.writeSync(1);

    //toggle status switch
    ABTN.on("down", function () { //active button pushed

        if (status == 0) {
            status = 1;
            console.log("activated", status);
            GLED.off(); //on/off reversed
        }
        else if (status == 1) {
            status = 0;
            console.log("de-activated", status);
            GLED.on();  //on/off reversed
        }
    });

    LBTN.on("down", function () { //left button : beep 1sec
        console.log("LBTN");
        if (status == 1) {
            for (var i = 0; i < 300; i++) {
                piezo.writeSync(1);
                sleep.usleep(1000);
                piezo.writeSync(0);
                sleep.usleep(1000);
            };
        }
    });

    RBTN.on("down", function () { // right button : beep 2 sec
        console.log("RBTN");
        if (status == 1) {
            for (var i = 0; i < 300; i++) {
                piezo.writeSync(1);
                sleep.usleep(1000);
                piezo.writeSync(0);
                sleep.usleep(1000);
            };
            for (var i = 0; i < 300; i++) {
                piezo.writeSync(1);
                sleep.usleep(700);
                piezo.writeSync(0);
                sleep.usleep(700);
            };
        }
    });

    showLum();  //light sensor output(LED)
    showTmp();  //temperature sensor output(console)
});

light.init({ AGAIN: 0, ATIME: 1 }, function (err) { //light sensor Initialize
    if (err) {
        console.log(err);
        process.exit(-2);
    } else {
        console.log("Device ready");
    }
});

function showLum() { //light sensor output(LED)
    light.readLuminosity(function (err, data) {
        if (err) { console.log(err); }
        else {
            if (data.ir < 3) { //handle LED with onoff module, normal : 10 ~ 20
                BLED.writeSync(0);
            }
            else {
                BLED.writeSync(1);
            };
        }
    });
    setTimeout(showLum, 100); // delay 0.1sec
}

function showTmp() { //temp sensor output(console)
    temp.read(22, 17, function (err, temperature, humidity) {
        if (!err) {
            console.log("temp :" + temperature.toFixed(1) + "C,  " + "humidity:  " + humidity.toFixed(1) + "%");
        }  //directly read
    });
    setTimeout(showTmp, 5000); // delay 5sec
}
