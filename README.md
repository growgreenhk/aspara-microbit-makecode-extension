# aspara micro:bit MakeCode Extension
This is the micro:bit MakeCode Extension for controlling the aspara Smart Grower by [Growgreen Limited](https://www.grow-green.com/)

<img src="./pngs/stylist.png" alt="image" width="300" height="auto"><img src="./pngs/microbit.png" alt="image" width="200" height="auto">

## How to add aspara micro:bit MakeCode Extension to your MakeCode project

* Create/Open a MakeCode project using the micro:bit MakeCode Editor at https://makecode.microbit.org
* In the web editor, click on <B>"Extensions"</B> to add extensions to the project

    <img src="./pngs/proj_ext_b4.png" width=114 height=245>

* Enter "<I><B>https://github.com/growgreenhk/aspara-microbit-makecode-extension</B></I>" and search

    <img src="./pngs/proj_ext_search.png" width=878 height=284>

* Select the <B>"aspara-smart-grower"</B> from the search results.

    <img src= "./pngs/proj_ext_result.png" width=146 height=161>

* Select <B>"Remove extension(s) and add aspara-smart-grower"</B> if a pop-up appears with the message <B>"Some extensions will be removed"</B>

    <img src="./pngs/proj_ext_popup.png" width=361 height=119>

* <B>"aspara Smart Grower"</B> will show up in the editor and be ready to use.

    <img src="./pngs/proj_ext_complete.png" width=121 height=101 >

## How to use the extension
### On start - assign an unique microbit ID for the project
* Add <B>"start aspara service with 5 characters ID [A-Z][0-9]"</B> from the aspara Smart Grower extension to the block <B>"on start"</B>
* Enter an ID for this project.
* E.g.

    <img src="./pngs/proj_microbit_id.png" width=579 height=177/>
### Control blocks
* Set LED (red/blue/white) lights intensity (0 - 100)%

    <img src="./pngs/block_set_led_lights.png" width=327 height=65/>

* Set front panel indicators (8 indicators) on/off

    <img src="./pngs/block_set_indicators.png" width=279 height=71/>

* Set pump on/off

    <img src="./pngs/block_set_pump.png" width=165 height=61/>
    
* Play a short/long beep sound

    <img src="./pngs/block_make_beep.png" width=206 height=62/>

### Value blocks
* (red/blue/white) LED intensity (0 - 100)%

    <img src="./pngs/block_data_led_light_intensity.png" width=249 height=51/>

* Indicators (1 - 8) state (on / off)

    <img src="./pngs/block_data_indicator_state.png" width=235 height=52/>

* Key pressed count (key 1 - 8)

    <img src="./pngs/block_data_key_pressed_count.png" width=273 height=40/>

* Pump state (on / off)

    <img src="./pngs/block_data_pump_state.png" width=195 height=48/>

* Room temperature (°C) from aspara Wireless Planting Sensor

    <img src="./pngs/block_data_temperature.png" width=171 height=41/>
    
* Relative humidity (%) from aspara Wireless Planting Sensor

    <img src="./pngs/block_data_humidity.png" width=208 height=40/>

* Light intensity (Lux) from aspara Wireless Planting Sensor

    <img src="./pngs/block_data_light_intensity.png" width=212 height=41/>

* Nutrient level (μS/cm) from aspara Wireless Planting Sensor.

    <img src="./pngs/block_data_nutrient_level.png" width=217 height=40/>

* Battery level from aspara Wireless Planting Sensor

    <img src="./pngs/block_data_battery_level.png" width=175 height=44/>

* Real time clock (year, month, day, hour, minute, second)

    <img src="./pngs/block_data_datetime.png" width=449 height=40/>

## Setup
* Assign an unique micro:bit ID for this project.
* Download and Run the project.
* Use the aspara STEM app to pair the aspara Smart Grower and aspara Wireless Planting Sensor with the micro:bit.

## Sample micro:bit MakeCode Projects using aspara Smart Grower Extension
#### A very simple project to show the use of the control blocks
* <I><B>https://makecode.microbit.org/S43232-26131-18874-50968</B></I>
#### A simple project to show the use of the aspara Wireless Planting Sensor values
* <I><B>https://makecode.microbit.org/S26309-97267-60499-68678</B></I>
#### A full cycle of planting controlled by the micro:bit
* <I><B>https://makecode.microbit.org/S25655-44721-19185-89153</B></I>

## Compatibility

* Works with <B>micro:bit V2</B> hardware only

## Supported targets
* for PXT/microbit
