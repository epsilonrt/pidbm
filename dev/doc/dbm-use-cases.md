# PiDuino Database management use cases

## List

    list manufacturer [name_like/id] <-- Checked

    list arch [name_like/id] <-- Checked
    list soc_family [name_like/id] <-- Checked

    list soc [name_like/id] <-- Checked
    
    list board_family [name_like/id] <-- Checked
    list board_model [name_like/id] <-- Checked

    list board [board_name_like/board_id] <-- Checked
    list board revision [revision_value] <-- Checked
    list board tag [tag_value] <-- Checked

    list gpio [name_like/id] <-- Checked
    list gpio pin [gpio_name_like/gpio_id] [-M <pin_mode>] <-- Checked

    list connector_family [name_like/id] <-- Checked

    list connector [name_like/id] <-- Checked
    list connector gpio gpio_name_like/gpio_id <-- Checked
    list connector board board_name_like/board_id <-- Checked

    list pin [pin_name_like/pin_id] [-M <pin_mode>] <-- Checked

    list pin gpio [pin_name_like/pin_id] [-M <pin_mode>] <-- Checked
    list pin net [pin_name_like/pin_id] [-M <pin_mode>] <-- Checked
    list pin audio [pin_name_like/pin_id] [-M <pin_mode>] <-- Checked
    list pin power [pin_name_like/pin_id] [-M <pin_mode>] <-- Checked
    list pin usb [pin_name_like/pin_id] [-M <pin_mode>] <-- Checked
    list pin video [pin_name_like/pin_id] [-M <pin_mode>] <-- Checked
    list pin nc [pin_name_like/pin_id] [-M <pin_mode>] <-- Checked
    
    list pin gpio soc [soc_name_like/soc_id] [-M <pin_mode>] <-- Checked
    list pin net soc [soc_name_like/soc_id] [-M <pin_mode>] <-- Checked
    list pin audio soc [soc_name_like/soc_id] [-M <pin_mode>] <-- Checked
    list pin power soc [soc_name_like/soc_id] [-M <pin_mode>] <-- Checked
    list pin usb soc [soc_name_like/soc_id] [-M <pin_mode>] <-- Checked
    list pin video soc [soc_name_like/soc_id] [-M <pin_mode>] <-- Checked
    list pin nc soc [soc_name_like/soc_id] [-M <pin_mode>] <-- Checked

    list pin soc 3 [-M <pin_mode>] <-- Checked
    list pin soc H5 [-M <pin_mode>] <-- Checked

## Add

    add manufacturer <-- Checked 
    0      name
    add manufacturer "Sony Japan"

    add connector <-- Checked
    0      name
    1      connector_family  (id/name)
    2      rows
    add connector "j2-j1" h2x  16
    add connector inner h1x  7

    add gpio  <-- Checked
    0     name        
    1     board_family_id
    add gpio nanopiduo2  1

    add con2gpio <-- Checked
    0     gpio (id/name)         
    1     #connector    
    2     connector_id
    add con2gpio 10           1             17
    add con2gpio orangepione  1             test

    add pin <-- Checked
    0     type 
    1     input_mode_name 
    2     [soc_pin_num]
    3     [sys_pin_num]
    add pin video "CVBS"

    add name2pin <-- Checked
    0     pin_id 
    1     mode (id/name)
    2     name 
    3+2*n [mode] 
    4+2*n [name]
    add name2pin 98 alt0 UART1TX alt5 SPI0MISO

    add pin2soc <-- Checked
    0      soc   (id/name)
    1      pin_id/pin_name # name is for pin_mode = 'input'
    add pin2soc 3 98
    add pin2soc 3 # <<< interactif

    add pin2con <-- Checked
    0          connector   (id/name)
    1          row
    2          [col]
    2/3        pin_id/pin_name # name is for pin_mode = 'input'
    add pin2con 17  1 1 2
    add pin2con 17 # <<< interactif

    add pin2gpio <-- Checked
    0      gpio   (id/name)
    1      ino_pin_num # must be a unsigned integer
    2      pin_id/pin_name # name is for pin_mode = 'input'
    add pin2gpio 3 1 GPIOA0
    add pin2gpio 3 1 # <<< interactif, ino_pin_num for the first pin number to request

    add board_model <-- Checked 
    0     name             
    1     board_family (id/name) 
    2     soc (id/name)
    add board_model "RaspberryPi 4B" 0             5
    add board_model "RaspberryPi 4B" nanopi        H3

    add board <-- Checked  
    0          name
    1          board_model   (id/name)
    2          gpio          (id/name)
    3          manufacturer  (id/name)
    4          default_i2c_id
    5          default_spi_id
    6          default_uart_id
               [-rREVISION] [-tTAG] [-mRAM] [-pPCB_REV]

    add board "RaspberryPi 4B (0xA03111)" 23 3 1 1 0 0 -r0xa03111 -m1024 -p"1.1"

## Copy

    cp soc  <-- Checked
    0     [name_like/id] 
    1     new_name

    cp connector 
    0     [name_like/id] 
    1     new_name
    

## Modify

    mod board_family id/name new_name new_i2c_syspath new_spi_syspath new_uart_syspath
    mod board_model id/name new_name new_board_family_id new_soc_id
    mod board id/name new_name new_board_model_id new_gpio_id new_manufacturer_id new_ram new_pcb_revision new_default_i2c_id new_default_spi_id new_default_uart_id
    mod gpio id/name new_name new_board_family_id
    mod connector id/name new_name new_rows new_connector_family_id
    mod manufacturer id/name new_name
    mod pin_type id/name new_name
    mod pin_mode id/name new_name
    mod pin_name id/name new_name

## Remove 

    rm board [name_like/id]
    rm board_model [name_like/id]
    rm gpio [name_like/id]
    rm connector [name_like/id]
    rm manufacturer [name_like/id]
    rm pin_name [name_like/id]
