# MCP79411-RTCC 操作於 APP-All-MCU-2023 並顯示於 OLED 上 - 使用 AVR128DA48 MCU
**使用 APP-All MCU 2023 MCU 搭配 APP-LCD-1.8 擴充板上的 MCP79411 來完成 RTCC 功能請下載 AVR128DA48_OLED_RTCC.X.zip 來使用這一個範例程式**

在 APP-LCD-1.8 這片擴充卡上，有一個 RTCC 的晶片 MCP79411。本範例提供 MCP79411 的 Library 來展示如何使用 AVR128DA48 MCU 來完成 MCP79411 的操作。APP-All MCU2023 留有一個與 Arduino UNO 相容的擴充介面，以及一個與 mikroE 相容的擴充介面，就是要用來擴充在 APP-All MCU2023 上原先為安置的外接周邊。I2C 的範例並不難建，但是針對像 MCP79411 這種 RTCC 的專用 IC 比較少。所以我們使用這一個範例，一方面展現 Melody 的 I2C Host Driver 的用法，並且也讓使用者多一個支援 MCP79411 操作的 Library。這個範例的完成方式如下 :
-  使用 MCC Classic 有支援 MCP79411 的條件下，以 PIC18F 系列為 MCU 建立專案、產生 MCP79411 的操作程式庫
-  修改由 MCC Classic 得到的 MCP79411 操作程式，主要是針對 I2C 的部分，讓程式可以被 MCC Melody 使用。修改後的 Library 在 MCC Melody 具有很好的相容性
    -  MCP79411.c
    -  MCP79411.h
-  這個 MCP79411 的 Library 使用時是透過 time.h 裡面的標準 C Library 來操作設定、讀取及資料結構分拆的工作，使用者可以看看這些 C Library
    -  localtime()
    -  mktime()

![image](https://github.com/CalvinHoMicrochip/MCP79411-RTCC-with-APP-All-MCU-2023-OLED/assets/20454551/f952e117-2b98-4d21-b899-8ee432175bc0)

![image](https://github.com/CalvinHoMicrochip/MCP79411-RTCC-with-APP-All-MCU-2023-OLED/assets/20454551/42daa5f5-6f29-4581-9955-9c5e2d4a0099)

![image](https://github.com/CalvinHoMicrochip/MCP79411-RTCC-with-APP-All-MCU-2023-OLED/assets/20454551/8767e1e4-11e7-4455-a041-573097e59f71)

![image](https://github.com/CalvinHoMicrochip/MCP79411-RTCC-with-APP-All-MCU-2023/assets/20454551/735472e3-2586-43af-b98e-1a0788fc22cd) 

**APP-LCD-1.8 的背面圖，MCP79411 置於背面。當然 ... CR-2032 的水銀電池要裝上才可以在斷電時 Keep 住時間並且繼續計時喔。**

![image](https://github.com/CalvinHoMicrochip/MCP79411-RTCC-with-APP-All-MCU-2023/assets/20454551/58df3191-0f66-47de-a579-ff30c9d22ced)


**MCC Melody Project Builder 中的設定以及 I/O 的設定 - 也請參考 APP-LCD-1.8 的線路圖**


![image](https://github.com/CalvinHoMicrochip/MCP79411-RTCC-with-APP-All-MCU-2023/assets/20454551/357247db-513e-4de1-9e95-3b4c80ab8830)

**MCC Content Manager 中用於此範例程式的各 Module 的版本**

![image](https://github.com/CalvinHoMicrochip/MCP79411-RTCC-with-APP-All-MCU-2023/assets/20454551/dcecfb63-9923-427a-97a3-984912d2dcee)






