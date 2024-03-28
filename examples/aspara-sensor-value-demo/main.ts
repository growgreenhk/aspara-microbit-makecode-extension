function get_date_time () {
    date_time = asparaSmartGrower.getDatetime()
    Year = date_time[1] * 100 + date_time[0]
    month = date_time[2]
    day = date_time[3]
    hour = date_time[4]
    minute = date_time[5]
    second = date_time[6]
}
function initialization () {
    Year = 0
    month = 0
    day = 0
    hour = 0
    minute = 0
    second = 0
}
bluetooth.onBluetoothConnected(function () {
    basic.showIcon(IconNames.Happy)
    initialization()
})
bluetooth.onBluetoothDisconnected(function () {
    basic.showIcon(IconNames.Sad)
})
input.onButtonPressed(Button.A, function () {
    asparaSmartGrower.beep(DURATION.short)
    set_LED_lights(100, 0, 50)
})
function toggle_indicators () {
    if (second % 2 == 0) {
        asparaSmartGrower.setIndicator(INDICATOR_TYPE.indicator_1, ON_OFF.off)
    } else {
        asparaSmartGrower.setIndicator(INDICATOR_TYPE.indicator_1, ON_OFF.on)
    }
}
input.onButtonPressed(Button.AB, function () {
    asparaSmartGrower.beep(DURATION.short)
    toggle_pump()
})
input.onButtonPressed(Button.B, function () {
    asparaSmartGrower.beep(DURATION.short)
    set_LED_lights(0, 100, 50)
})
input.onGesture(Gesture.Shake, function () {
    asparaSmartGrower.beep(DURATION.long)
})
function toggle_pump () {
    if (asparaSmartGrower.pumpState() == 0) {
        asparaSmartGrower.setPump(ON_OFF.on)
    } else {
        asparaSmartGrower.setPump(ON_OFF.off)
    }
}
function get_data () {
    serial.writeValue("temperature", asparaSmartGrower.temperature())
    serial.writeValue("light intensity", asparaSmartGrower.lightsensor())
}
function print_date_time () {
    serial.writeLine("***************************************************************")
    serial.writeLine("")
    serial.writeString("" + convertToText(Year) + "-" + convertToText(month) + "-" + convertToText(day) + "            " + convertToText(hour) + ":" + convertToText(minute) + ":" + convertToText(second))
    serial.writeLine("")
    serial.writeLine("***************************************************************")
}
function set_LED_lights (red: number, blue: number, white: number) {
    asparaSmartGrower.setLEDlight(LED_TYPE.red, red)
    asparaSmartGrower.setLEDlight(LED_TYPE.blue, blue)
    asparaSmartGrower.setLEDlight(LED_TYPE.white, white)
}
let second = 0
let minute = 0
let hour = 0
let day = 0
let month = 0
let Year = 0
let date_time: Buffer = null
basic.showIcon(IconNames.Heart)
asparaSmartGrower.startAsparaSmartGrowerService(22582)
loops.everyInterval(1000, function () {
    get_date_time()
    toggle_indicators()
    get_data()
    if (asparaSmartGrower.lightsensor() < 20) {
        set_LED_lights(0, 0, 100)
    }
    if (asparaSmartGrower.lightsensor() > 1000) {
        set_LED_lights(0, 0, 0)
    }
    if (second % 5 == 0) {
        print_date_time()
    }
})
