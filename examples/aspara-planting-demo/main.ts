function toggle_indicator () {
    if (second % 2 == 1) {
        asparaSmartGrower.setIndicator(INDICATOR_TYPE.indicator_2, ON_OFF.on)
    } else {
        asparaSmartGrower.setIndicator(INDICATOR_TYPE.indicator_2, ON_OFF.off)
    }
}
function in_planting_period () {
    if (hour >= 9 && hour < 18) {
        return 1
    } else if (hour == 18) {
        if (minute >= 0 && minute < 30) {
            return 1
        } else {
            return 0
        }
    } else {
        return 0
    }
}
bluetooth.onBluetoothConnected(function () {
    basic.showIcon(IconNames.Happy)
    Initialization()
})
bluetooth.onBluetoothDisconnected(function () {
    basic.showIcon(IconNames.Sad)
})
input.onButtonPressed(Button.A, function () {
    asparaSmartGrower.beep(DURATION.short)
    asparaSmartGrower.setLEDlight(LED_TYPE.red, 0)
    asparaSmartGrower.setLEDlight(LED_TYPE.blue, 0)
    asparaSmartGrower.setLEDlight(LED_TYPE.white, 100)
})
function get_datetime () {
    date_time = asparaSmartGrower.getDatetime()
    Year = date_time[1] * 100 + date_time[0]
    month = date_time[2]
    day = date_time[3]
    hour = date_time[4]
    minute = date_time[5]
    second = date_time[6]
}
function water_program () {
    if (minute % 15 < 1) {
        asparaSmartGrower.setPump(ON_OFF.on)
    } else {
        asparaSmartGrower.setPump(ON_OFF.off)
    }
}
function Initialization () {
    Year = 0
    month = 0
    day = 0
    hour = 0
    minute = 0
    second = 0
}
input.onButtonPressed(Button.B, function () {
    asparaSmartGrower.beep(DURATION.long)
    asparaSmartGrower.setLEDlight(LED_TYPE.red, 0)
    asparaSmartGrower.setLEDlight(LED_TYPE.blue, 0)
    asparaSmartGrower.setLEDlight(LED_TYPE.white, 0)
})
function get_data () {
    serial.writeValue("temperature", asparaSmartGrower.temperature())
    serial.writeValue("nutrient", asparaSmartGrower.nutrient())
    serial.writeValue("light intensity", asparaSmartGrower.lightsensor())
}
function light_program () {
    if (in_planting_period() == 1) {
        asparaSmartGrower.setLEDlight(LED_TYPE.red, 100)
        asparaSmartGrower.setLEDlight(LED_TYPE.blue, 100)
        asparaSmartGrower.setLEDlight(LED_TYPE.white, 0)
    } else {
        asparaSmartGrower.setLEDlight(LED_TYPE.red, 0)
        asparaSmartGrower.setLEDlight(LED_TYPE.blue, 0)
        asparaSmartGrower.setLEDlight(LED_TYPE.white, 0)
    }
}
let day = 0
let month = 0
let Year = 0
let date_time: Buffer = null
let minute = 0
let hour = 0
let second = 0
basic.showIcon(IconNames.TShirt)
Initialization()
asparaSmartGrower.startAsparaSmartGrowerService(22582)
loops.everyInterval(1000, function () {
    get_datetime()
    light_program()
    water_program()
    toggle_indicator()
    get_data()
})
