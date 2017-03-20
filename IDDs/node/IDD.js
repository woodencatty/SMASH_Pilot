var five = require("johnny-five");
var Raspi = require("raspi-io");
var board = new five.Board({ //Initialize with J5-Rpio interface
  io: new Raspi()

});


board.on("ready", function () {
  var status = 0;

      //pins Initialize
  var led = new five.Led("P1-37");
  var button = new five.Button("P1-12");

  var x, y, z;  //accel data value

  
  var accelerometer = new five.Accelerometer({ // Accelerometer Initializing
    controller: "LIS3DH",
    // Optionally set the range to one of
    // 2, 4, 8, 16 (±g)
    // Defaults to ±2g
    // range: ...
  });
  console.log("accelerometer READY");
  var testTime;

console.log("Initializing complete");

  button.on("up", function () {// toggle button
    if (status == 0) {
      status = 1;
      console.log("acceletometer ON", status);
      led.on();
      testTime = setInterval(function () { // loop timer
        console.log("     x:", accelerometer.x, "    y: ", accelerometer.y, "     z :", accelerometer.z);
      }, 900);
    }
    else if (status == 1) {
      status = 0;
      console.log("acceletometer OFF", status);
      led.off();
      clearInterval(testTime);// off loop timer
      }
  });


  accelerometer.on("change", function () { // get Accelerometer data
    x = this.x;
    y = this.y;
    z = this.z;
    //    console.log("  pitch        : ", this.pitch);
    //    console.log("  roll         : ", this.roll);
    //    console.log("  acceleration : ", this.acceleration);
    //    console.log("  inclination  : ", this.inclination);
    //    console.log("  orientation  : ", this.orientation);
    //    console.log("--------------------------------------");
  });


});
