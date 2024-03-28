bluetooth.onBluetoothConnected(function () {
    basic.showIcon(IconNames.Happy)
})
bluetooth.onBluetoothDisconnected(function () {
    basic.showIcon(IconNames.Sad)
})
input.onButtonPressed(Button.A, function () {
    asparaSmartGrower.beep(DURATION.short)
    asparaSmartGrower.setLEDlight(LED_TYPE.red, 100)
    asparaSmartGrower.setLEDlight(LED_TYPE.blue, 0)
    asparaSmartGrower.setLEDlight(LED_TYPE.white, 0)
})
input.onButtonPressed(Button.AB, function () {
    asparaSmartGrower.beep(DURATION.short)
    asparaSmartGrower.setLEDlight(LED_TYPE.white, 100)
    asparaSmartGrower.setLEDlight(LED_TYPE.red, 0)
    asparaSmartGrower.setLEDlight(LED_TYPE.blue, 0)
})
input.onButtonPressed(Button.B, function () {
    asparaSmartGrower.beep(DURATION.short)
    asparaSmartGrower.setLEDlight(LED_TYPE.blue, 100)
    asparaSmartGrower.setLEDlight(LED_TYPE.red, 0)
    asparaSmartGrower.setLEDlight(LED_TYPE.white, 0)
})
input.onGesture(Gesture.Shake, function () {
    asparaSmartGrower.beep(DURATION.short)
    if (asparaSmartGrower.pumpState() == 0) {
        asparaSmartGrower.setPump(ON_OFF.on)
    } else {
        asparaSmartGrower.setPump(ON_OFF.off)
    }
    asparaSmartGrower.setLEDlight(LED_TYPE.red, 0)
    asparaSmartGrower.setLEDlight(LED_TYPE.blue, 0)
    asparaSmartGrower.setLEDlight(LED_TYPE.white, 0)
})
basic.showIcon(IconNames.TShirt)
asparaSmartGrower.startAsparaSmartGrowerService(22582)
basic.forever(function () {
	
})
